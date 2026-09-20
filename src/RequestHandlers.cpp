/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandlers.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 00:50:28 by modiepge          #+#    #+#             */
/*   Updated: 2026/09/21 01:47:49 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"
#include "Server.hpp"


Response	handle_get(const Request& request, const Server& server) {
	(void)request;
	(void)server;

	const std::string file_contents = "GET parsed successfully\n";
	// const std::string file_contents = read_file();

	Response response(200);
	response.setVersion("HTTP/1.1");
	response.addHeader("Content-Type", "text/plain; charset=utf-8");
	response.addHeader("Content-Length", std::to_string(file_contents.size()));
	response.addHeader("Connection", "close");
	response.setBody(file_contents);
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