/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandlers.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/21 00:50:21 by modiepge          #+#    #+#             */
/*   Updated: 2026/09/21 01:55:06 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Message.hpp"

class	Server;

Response handle_request(const Request& request, const Server& server);
Response handle_get(const Request& request, const Server& server);
Response make_error_response(int status);
