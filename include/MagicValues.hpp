/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MagicValues.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 16:11:39 by rgohrig           #+#    #+#             */
/*   Updated: 2026/08/28 13:01:53 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define BODY_SIZE_FACTOR 1024 // ein bytes steep
#define BODY_SIZE_B 1 // byte
#define BODY_SIZE_K (BODY_SIZE_FACTOR * BODY_SIZE_B) // kibibyte
#define BODY_SIZE_M (BODY_SIZE_FACTOR * BODY_SIZE_K) // mebibyte
#define BODY_SIZE_G (BODY_SIZE_FACTOR * BODY_SIZE_M) // gibibyte
#define BODY_SIZE_DEFAULT (1 * BODY_SIZE_M) // 1MB

#define MAX_CONFIG_FILE_SIZE (1 * BODY_SIZE_G) // 1GB
#define MAX_TOKEN_LENGTH (1000) // 1000 characters

#define MAX_EVENTS 10
#define EPOLL_TIMEOUT -1 // -1 = infinite wait todo: set to better value