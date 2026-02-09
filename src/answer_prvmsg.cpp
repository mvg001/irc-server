/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   answer_prvmsg.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrollon- <jrollon-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/09 17:20:21 by jrollon-          #+#    #+#             */
/*   Updated: 2026/02/09 22:25:39 by jrollon-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCCommand.hpp"
#include "IRCServ.hpp"
#include <sstream>

void	to_user(std::vector<std::string>& msg, int fd, std::string & target, IRCServ& server){
	const std::map<int, IRCClient>&						clients	= server.getClients(); //have all the clients
	const std::map<const std::string, int>&		nicks		= server.getNicks();
	std::ostringstream												reply;
	
	
	//1. search if there is an fd for the Nick passed (target).
	std::map<const std::string, int>::const_iterator it_target = nicks.find(target);
	
	//2. search for the creator of the msg to obtain his nick/user/host for the prefix.
	std::map<int, IRCClient>::const_iterator it_sender = clients.find(fd);
    if (it_sender == clients.end())
        return;    
  const IRCClient &sender = it_sender->second;

	//3. construct the reply msg format: :nick!user@host PRIVMSG target :message.
	if (it_target != nicks.end()) {
    int target_fd = it_target->second;
		
		//the prefix
		reply << ":" << sender.getNick() << "!" << sender.getUsername() << "@" << sender.getHost() << " PRIVMSG " << target << " :";

		//now the msg construction
		std::vector<std::string>::const_iterator it_msg = msg.begin();
		if (it_msg != msg.end())
			it_msg++; //jump over the target to find the real msg
		
		for (;it_msg != msg.end(); ++it_msg){
			reply << *it_msg;
			if (it_msg + 1 != msg.end())
				reply << " "; //add separator between words.
		}
		
		//end
		reply << "\r\n";
		server.queue_and_send(target_fd, reply.str());
	}
	
	//nick is not valid.
	else{
			std::string my_nick = sender.getNick();
			if (my_nick.empty())
				my_nick = "*";
			reply << ":" << server.getServerName() << " 401 " << my_nick << " " << target << " :No such nick\r\n";
			server.queue_and_send(fd, reply.str());
	}
}


void	to_channel(std::vector<std::string>& msg, int fd, std::string & target, IRCServ& server){
	
}

void IRCServ::answer_privmsg(IRCMessage & msg, int fd) {
	std::ostringstream reply;
	size_t num_params = msg.getParametersSize();
	
	if (num_params < 1){ //411 ERR_NORECIPIENT
		reply << ":" << server_name << IRCCommandtoString(ERR_NORECIPIENT) << " PRIVMSG :Not enough parameters\r\n";
		queue_and_send(fd, reply.str());
		return ; 
	}
	if (num_params < 2){ //412 ERR_NOTEXTTOSEND
		reply << ":" << server_name << IRCCommandtoString(ERR_NOTEXTTOSEND) << " PRIVMSG :No text to send\r\n";
		queue_and_send(fd, reply.str());
		return ; 
	}
	
	if (clients.count(fd)){

	std::string 							target = msg.getParam(0);
	std::vector<std::string> 	parameters = msg.getParamsVector();
	if (target[0] == '#' || target[0] == '&')
		to_channel(parameters, fd, target, *this);
	else
		to_user(parameters, fd, target, *this);
	}
}

