/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IpAddress.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/04 14:53:37 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/04 18:33:00 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <array>
#include <cstdint>

/*
		struct sockaddr_in6 {
			sa_family_t     sin6_family;       AF_INET6
			in_port_t       sin6_port;         Port number
			uint32_t        sin6_flowinfo;     IPv6 flow info
			struct in6_addr sin6_addr;         IPv6 address
			uint32_t        sin6_scope_id;     Set of interfaces for a scope
		};

			struct in6_addr {
			uint8_t   s6_addr[16];
		};


				struct sockaddr_in {
			sa_family_t     sin_family;       AF_INET
			in_port_t       sin_port;         Port number
			struct in_addr  sin_addr;         IPv4 address
		};

		struct in_addr {
			in_addr_t s_addr;
		};

		typedef uint32_t in_addr_t;
		typedef uint16_t in_port_t;
*/

class IpAddress
{
  public:
	IpAddress() = delete;
	
	IpAddress(uint32_t address_ipv4, uint16_t port) : 
		is_ipv6_(false),
		ipv4_(address_ipv4), 
		port_(port), 
		ipv6_words_() {};

	IpAddress(const std::array<uint16_t, 8> &address_ipv6, uint16_t port) : 
		is_ipv6_(true), 
		ipv4_(), 
		port_(port), 
		ipv6_words_(address_ipv6) {};

	bool isIpv6() const
	{
		return (is_ipv6_);
	}
	bool isIpv4() const
	{
		return (!is_ipv6_);
	}

	uint32_t ipv4() const
	{
		return (ipv4_);
	}

	const std::array<uint16_t, 8> &ipv6() const
	{
		return (ipv6_words_);
	}

	uint16_t port() const
	{
		return (port_);
	}

  private:
	bool is_ipv6_;
	uint32_t ipv4_;
	uint16_t port_;
	std::array<uint16_t, 8> ipv6_words_;
};