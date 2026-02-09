/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   answer_prvmsg.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrollon- <jrollon-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/09 17:20:21 by jrollon-          #+#    #+#             */
/*   Updated: 2026/02/09 17:49:13 by jrollon-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCCommand.hpp"
#include "IRCServ.hpp"
#include <sstream>

void	priv_user(IRCMessage& msg, int fd, std::string target){


}

void	priv_channel(IRCMessage& msg, int fd, std::string target){
	
}

void IRCServ::answer_privmsg(IRCMessage & msg, int fd) {
	vectorIteratorPairType params = msg.getParameters();
	size_t num_params = std::distance(params.first, params.second);
	
	if (num_params < 1){
		//si es <1 411 ERR_NORECIPIENT
	}
	if (num_params < 2){
		//si es <2 412 ERR_NOTEXTTOSEND
	}
	
	std::string target = msg.getParam(0);
	
	if (target[0] == '#' || target[0] == '&')
		priv_channel(msg, fd, target);
	else
		priv_user(msg, fd, target);
}

