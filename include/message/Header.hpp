/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: modiepge <modiepge@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 15:54:52 by modiepge          #+#    #+#             */
/*   Updated: 2026/10/30 17:18:17 by modiepge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"
#include <cstddef>
#include <string>
#include <unistd.h>

// using HeaderPair = std::pair<std::string, std::vector<std::string>>;
// using Values = std::vector<std::string>;

using HeaderMap = std::vector<std::string, std::vector<std::string>>;

class Header
{
	public:
	Header(const Header &) = default;
	Header &operator=(const Header &) = default;

	Header(Header &&) = default;
	Header &operator=(Header &&) = default;

	Header() = default;
	~Header() = default;
	
	bool add(const std::string &line); // add a header
	std::vector<std::string> find(const std::string &search); // find the value(s) for a given header
	std::string serialize(); // turn searchable headers back into string for sending
	char *const *environment(); // turn headers into environment for cgi
	size_t	size(); // how long is this header block?
	bool	is_chunked(); // content-length = false, transfer-encoding = true?
	
	private:
		HeaderMap data;
		std::string serialize(std::string prefix = ""); // turn searchable headers back into string for sending
}
// execve(const char *path, char *const *argv, char *const *envp);