/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AddrInfoPtr.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 16:21:35 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/18 16:25:11 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <memory>
#include <netdb.h>  //IWYU pragma: private

struct AddrInfoDeleter
{
    void operator()(addrinfo* p) const {
		if (p) freeaddrinfo(p);
    }
};

using AddrInfoPtr = std::unique_ptr<addrinfo, AddrInfoDeleter>;