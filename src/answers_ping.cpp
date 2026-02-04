/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   answers_ping.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrollon- <jrollon-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/03 16:02:52 by jrollon-          #+#    #+#             */
/*   Updated: 2026/02/04 14:40:05 by jrollon-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCServ.hpp"

void IRCServ::answer_ping(IRCMessage & msg, int fd) {

	std::string ans;
			
	vectorIteratorPairType params = msg.getParameters();
	if (params.first == params.second){ //no hay parametros
		if (clients.count(fd)){
			std::string nick = clients[fd].getNick();
			if (nick.empty())
				nick = "*"; //se usa como placeholder cuando no hay nick asociado
			ans = ":" + server_name + " 461 " + nick + " PING :Not enough parameters\r\n";
			queue_and_send(fd, ans);
		}
		return ;	
	}
	ans = std::string(":") + server_name + " PONG " + server_name + " :" + *(params.first) + "\r\n"; 
	queue_and_send(fd, ans);
}
