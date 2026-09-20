/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandlers.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 00:50:28 by modiepge          #+#    #+#             */
/*   Updated: 2026/09/21 00:59:22 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"
#include "Server.hpp"

Response	handle_request(const Request& request, const Server& server) {
	if (request.getMethod() == Methods::GET)
		return (handle_get(request, server));
}

Response	handle_get(const Request& request, const Server& server) {
	const std::string file_contents = read_file();

	Response response(200);
	response.setVersion("HTTP/1.1");
	response.addHeader("Content-Type", "text/html");
	response.addHeader("Content-Length", std::to_string(file_contents.size()));
	response.addHeader("Connection", "close");
	response.setBody(file_contents);
	return response;
}