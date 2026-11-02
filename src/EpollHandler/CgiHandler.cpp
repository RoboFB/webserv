/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 14:45:38 by rgohrig           #+#    #+#             */
/*   Updated: 2026/11/02 12:36:28 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"
#include "logging.hpp"
#include "Signals.hpp"
#include "Methods.hpp"
#include "ConnectionHandler.hpp"
#include "Response.hpp"
#include "Syntax.hpp"

#include <array>
#include <vector>
#include <cstring>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>
#include <signal.h>
#include <fcntl.h>


static std::string cgi_http_header_name(const std::string &name)
{
    std::string result = "HTTP_";

    for (char character : name)
    {
        if (character == '-')
            result += '_';
        else if (character >= 'a' && character <= 'z')
            result += character - ('a' - 'A');
        else
            result += character;
    }

    return result;
}

CgiHandler::CgiHandler(const Server *server, const CloseFd &epoll_fd,
                       ConnectionHandler &connection, Response &response,
                       const Request &cgi_request,
                       const std::filesystem::path &script_path,
                       const std::string &interpreter_path)
    : EpollHandler(server, -1, epoll_fd),
      connection_(connection),
      response_(response),
      request_body_(cgi_request.getBody()),
      request_body_offset_(0),
      input_fd_(int(-1)),
      child_pid_(-1),
      child_input_closed_(false),
      last_activity_(std::chrono::steady_clock::now())
{
    int input_fds[2];
    int output_fds[2];

    if (socketpair(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0, input_fds) < 0)
    {
        LOG(LOG_ERROR, "socketpair input: " +
                       std::string(std::strerror(errno)));
        return;
    }

    if (socketpair(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0, output_fds) < 0)
    {
        close(input_fds[0]);
        close(input_fds[1]);
        LOG(LOG_ERROR, "socketpair output: " +
                       std::string(std::strerror(errno)));
        return;
    }

    if (fcntl(input_fds[0], F_SETFL, O_NONBLOCK) < 0 ||
        fcntl(output_fds[0], F_SETFL, O_NONBLOCK) < 0)
    {
        close(input_fds[0]);
        close(input_fds[1]);
        close(output_fds[0]);
        close(output_fds[1]);
        LOG(LOG_ERROR, "fcntl CGI parent socket: " +
                       std::string(std::strerror(errno)));
        return;
    }

    const pid_t pid = fork();

    if (pid < 0)
    {
        close(input_fds[0]);
        close(input_fds[1]);
        close(output_fds[0]);
        close(output_fds[1]);
        LOG(LOG_ERROR, "fork: " + std::string(std::strerror(errno)));
        return;
    }

    if (pid == 0)
    {
        reset_signal_handling();

        close(input_fds[0]);
        close(output_fds[0]);

        if (dup2(input_fds[1], STDIN_FILENO) < 0 ||
            dup2(output_fds[1], STDOUT_FILENO) < 0)
            _exit(127);

        close(input_fds[1]);
        close(output_fds[1]);

        const std::filesystem::path script_directory =
            script_path.parent_path();

        if (script_directory.empty() ||
            chdir(script_directory.c_str()) < 0)
            _exit(127);

        char *argv[] = {
            const_cast<char *>(interpreter_path.c_str()),
            const_cast<char *>(script_path.c_str()),
            nullptr
        };

        const URITarget target = cgi_request.split_uri_target();
        const HeaderValues *content_type =
            cgi_request.get_headers().find("content-type");

        std::vector<std::string> environment;
        environment.push_back("GATEWAY_INTERFACE=CGI/1.1");
        environment.push_back("REDIRECT_STATUS=200");
        environment.push_back("SERVER_PROTOCOL=HTTP/1.1");
        environment.push_back(
            "REQUEST_METHOD=" + methods_to_string(cgi_request.get_method()));
        environment.push_back("QUERY_STRING=" + target.query);
        environment.push_back(
            "CONTENT_LENGTH=" + std::to_string(cgi_request.getBody().size()));
        environment.push_back("SCRIPT_NAME=" + target.path);
        environment.push_back("SCRIPT_FILENAME=" + script_path.string());

        if (content_type != nullptr && content_type->size() == 1)
            environment.push_back("CONTENT_TYPE=" + content_type->at(0));

        const Header &headers = cgi_request.get_headers();

        for (HeaderMap::const_iterator field = headers.begin();
             field != headers.end(); ++field)
        {
            if (field->first == "content-type" ||
                field->first == "content-length")
                continue;

            std::string value;

            for (const std::string &part : field->second)
            {
                if (!value.empty())
                    value += field->first == "cookie" ? "; " : ", ";

                value += part;
            }

            environment.push_back(
                cgi_http_header_name(field->first) + "=" + value);
        }

        std::vector<char *> envp;
        envp.reserve(environment.size() + 1);

        for (std::string &entry : environment)
            envp.push_back(entry.data());

        envp.push_back(nullptr);

        execve(argv[0], argv, envp.data());
        _exit(127);
    }

    close(input_fds[1]);
    close(output_fds[1]);

    child_pid_ = pid;
    input_fd_ = CloseFd(std::move(input_fds[0]));
    fd_ = CloseFd(std::move(output_fds[0]));

    add_to_epoll();

    struct epoll_event input_event = {};
    input_event.events = EPOLLOUT;
    input_event.data.ptr = this;

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, input_fd_, &input_event) < 0)
        LOG(LOG_ERROR, "epoll_ctl add CGI input: " +
                       std::string(std::strerror(errno)));
}

void CgiHandler::close_cgi_input()
{
    if (child_input_closed_)
        return;

    epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, input_fd_, nullptr);
    input_fd_ = CloseFd(int(-1));
    child_input_closed_ = true;
}

void CgiHandler::write_request_body()
{
    if (child_input_closed_)
        return;

    const std::size_t remaining =
        request_body_.size() - request_body_offset_;

    if (remaining == 0)
    {
        close_cgi_input();
        return;
    }

    const ssize_t bytes_sent =
        send(input_fd_, request_body_.data() + request_body_offset_,
             remaining, MSG_NOSIGNAL);

    if (bytes_sent <= 0)
    {
        fail_cgi();
        return;
    }

    request_body_offset_ += static_cast<std::size_t>(bytes_sent);
    last_activity_ = std::chrono::steady_clock::now();

    if (request_body_offset_ == request_body_.size())
        close_cgi_input();
}

void CgiHandler::read_cgi_output()
{
    std::array<char, 4096> buffer = {};

    const ssize_t bytes_read =
        recv(fd_, buffer.data(), buffer.size(), 0);

    if (bytes_read > 0)
    {
        cgi_output_.append(
            buffer.data(), static_cast<std::size_t>(bytes_read));
        last_activity_ = std::chrono::steady_clock::now();
        return;
    }

    if (bytes_read == 0)
    {
        finish_cgi_response();
        return;
    }

    fail_cgi();
}

void CgiHandler::fail_cgi()
{
    response_.convert_to_error(502);
    connection_.cgi_finished();
    stop_listening();
}

void CgiHandler::on_epoll_event(AllServers &servers, uint32_t events)
{
    (void)servers;

    if (events & EPOLLERR)
    {
        fail_cgi();
        return;
    }

    if (events & EPOLLIN)
        read_cgi_output();

    if (!remove_me_ && events & EPOLLOUT)
        write_request_body();

    if (!remove_me_ && events & (EPOLLRDHUP | EPOLLHUP))
        read_cgi_output();
}

bool CgiHandler::is_timeout() const
{
	return false;
}

void CgiHandler::stop_listening(void)
{
    if (remove_me_)
        return;

    if (child_pid_ > 0)
    {
        kill(child_pid_, SIGKILL);
        child_pid_ = -1;
    }

    if (input_fd_ >= 0)
    {
        epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, input_fd_, nullptr);
        input_fd_ = CloseFd(int(-1));
    }

    remove_from_epoll();
    remove_me_ = true;
}

void CgiHandler::finish_cgi_response()
{
    const std::size_t header_end = cgi_output_.find(LINE LINE);

    if (header_end == std::string::npos || header_end == 0)
    {
        fail_cgi();
        return;
    }

    int status = 200;
    bool has_status = false;
    bool has_location = false;
    std::size_t cursor = 0;

    while (cursor < header_end)
    {
        const std::size_t line_end = cgi_output_.find(LINE, cursor);

        if (line_end == std::string::npos || line_end > header_end)
        {
            fail_cgi();
            return;
        }

        const std::optional<HeaderField> field =
            parse_header_field(
                cgi_output_.substr(cursor, line_end - cursor));

        if (!field)
        {
            fail_cgi();
            return;
        }

        if (field->normalized_name == "status")
        {
            const std::size_t status_end = field->value.find(' ');
            const std::string status_code =
                field->value.substr(0, status_end);

            std::size_t parsed_status = 0;

            if (has_status || status_code.size() != 3 ||
                !parse_decimal_size(status_code, parsed_status) ||
                parsed_status < 100 || parsed_status > 599)
            {
                fail_cgi();
                return;
            }

            status = static_cast<int>(parsed_status);
            has_status = true;
        }
        else if (field->normalized_name != "content-length" &&
                 field->normalized_name != "connection" &&
                 field->normalized_name != "transfer-encoding")
        {
            response_.addHeader(field->name, field->value);

            if (field->normalized_name == "location")
                has_location = true;
        }

        cursor = line_end + 2;
    }

    if (!has_status && has_location)
        status = 302;

    cgi_output_.erase(0, header_end + 4);

    response_.set_status(status);
    response_.setBody(std::move(cgi_output_));

	child_pid_ = -1;
    connection_.cgi_finished();
    stop_listening();
}

bool CgiHandler::is_started() const
{
    return child_pid_ > 0 && fd_ >= 0 && input_fd_ >= 0;
}
