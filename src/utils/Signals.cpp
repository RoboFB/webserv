/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Signals.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgohrig <rgohrig@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 12:09:58 by rgohrig           #+#    #+#             */
/*   Updated: 2026/10/27 14:14:25 by rgohrig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Signals.hpp"
#include "logging.hpp"
#include <atomic>
#include <csignal>

std::atomic_bool g_stop_server(false);

static void handle_sig_int(int signal)
{
	(void)signal;
	g_stop_server.store(true);
}

static void handle_sig_pipe(int signal)
{
	(void)signal;
	LOG(LOG_ERROR, "SIGPIPE signal ignored");
}

void init_signal_handling()
{
	std::signal(SIGPIPE, handle_sig_pipe);
	std::signal(SIGINT, handle_sig_int);
}

void reset_signal_handling()
{
	std::signal(SIGPIPE, SIG_DFL);
	std::signal(SIGINT, SIG_DFL);
}
