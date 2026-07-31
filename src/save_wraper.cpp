/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   save_wraper.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/28 15:51:53 by rgohrig           #+#    #+#             */
/*   Updated: 2026/07/30 16:22:08 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "save_wraper.hpp"


# include <netdb.h>
#include <iostream>

void save_getaddrinfo(void)
{
	int status;
	
	struct addrinfo hints{};
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	struct addrinfo *servinfo = NULL;
	
	
	std::cout << servinfo << std::endl;
	if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) != 0) {
		fprintf(stderr, "gai error: %s\n", gai_strerror(status));
		exit(1);
	}
}


void save_socket(void)
{
	int sock = 0;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("socket");
		exit(1);
	}
}

void save_bind(void)
{
	int sock = 0;
	struct sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(3490);

	if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		exit(1);
	}
}

void save_listen(void)
{
	int sock = 0;
	if (listen(sock, 5) < 0) {
		perror("listen");
		exit(1);
	}
}
