/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   answers_ping.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrollon- <jrollon-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/03 16:02:52 by jrollon-          #+#    #+#             */
/*   Updated: 2026/02/03 17:45:40 by jrollon-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCServ.hpp"

void IRCServ::answer_ping(IRCMessage & msg, int fd) {

	std::string ans;

	vectorIteratorPairType params = msg.getParameters();
	ans = std::string("PONG ") + "irc.42server.local" + " :" + *(params.first) + "\r\n"; 
	
	queue_and_send(fd, ans);
}
