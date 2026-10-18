/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandlers.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 00:50:21 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/18 22:39:08 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Message.hpp"

class Server;
class Location;

Response handle_request(const Request& request, const Server& server);

Response handle_get(
	const Request& request,
	const Location& location,
	const std::string& request_path);

Response handle_post(
	const Request& request,
	const Location& location,
	const std::string& request_path);

Response handle_delete(
	const Request& request,
	const Location& location,
	const std::string& request_path);

Response make_error_response(int status);
