/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marcoga2 <marcoga2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 15:17:48 by user1             #+#    #+#             */
/*   Updated: 2026/02/02 19:08:39 by marcoga2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCServ.hpp"
#include "utils.hpp"
// #include "IRCChannel.hpp"
// #include "IRCCommand.hpp"






int main(int ac, char **av)
{
	if (ac != 3) {
		std::cerr << "Usage: ircserver <port> <client_password>" << std::endl;
		return 1;
	}
	std::pair<int, bool> pairPort = ft_atoi(av[1]);
	if (!pairPort.second) {
		std::cerr << "Invalid server port" << std::endl;
		return 1;
	}

	IRCServ server;
	try
		{server = IRCServ(pairPort.first);}
	catch(std::runtime_error &e)
		{std::cerr << e.what() << std::endl; return;}

	try
		{server.run();}
	catch(std::runtime_error &e)
		{std::cerr << e.what() << std::endl; return;}

	return 0;
}
