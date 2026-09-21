/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandlers.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 00:50:28 by modiepge          #+#    #+#             */
/*   Updated: 2026/09/21 02:18:16 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"
#include "Server.hpp"

static std::string path_without_query(const Request& request) {
	const std::string target = request.getTarget().string();
	const std::size_t query_start = target.find('?');
	if (query_start == std::string::npos)
		return (target);
	return (target.substr(0, query_start));
}

Response make_error_response(int status) {
	const	std::string body = std::to_string(status) + " " + Response(status).getReason() + "\n";
	Response response(status);

	response.setVersion("HTTP/1.1");
	response.addHeader("Content-Type", "text/plain; charset=utf-8");
	response.addHeader("Content-Length", std::to_string(body.size()));
	response.addHeader("Connection", "close");
	response.setBody(body);
	return (response);
}

Response	handle_get(const Request& request, const Server& server) {
	const std::string path = path_without_query(request);
	const Location* location = server.find_location(path);
	if (location == nullptr)
		return (make_error_response(404));

	const std::string body = "Matched location: " + location->location_path + "\n";
	// const std::string file_contents = read_file();

	Response response(200);
	response.setVersion("HTTP/1.1");
	response.addHeader("Content-Type", "text/plain; charset=utf-8");
	response.addHeader("Content-Length", std::to_string(body.size()));
	response.addHeader("Connection", "close");
	response.setBody(body);
	return response;
}

Response	handle_request(const Request& request, const Server& server) {
	if (request.getMethod() == Methods::GET)
		return (handle_get(request, server));

	Response response(501);
	response.setVersion("HTTPS/1.1");
	response.addHeader("Content-Length", "0");
	response.addHeader("Connection", "close");
	return response;
}
