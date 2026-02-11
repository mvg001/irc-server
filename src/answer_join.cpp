/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   answer_join.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marcoga2 <marcoga2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 12:42:05 by mvassall          #+#    #+#             */
/*   Updated: 2026/02/11 11:22:47 by marcoga2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCClient.hpp"
#include "IRCCommand.hpp"
#include "IRCMessage.hpp"
#include "IRCServ.hpp"
#include "IRCChannel.hpp"
#include "utils.hpp"
#include <sstream>
#include <string>


static void join0(const IRCClient& client, int fd) {
  (void)client;
  (void)fd;
/*   pairIterators clientChIts = client.getChannelIterators();
  std::ostringstream oss; // append all channel names separated with ','
  for (setOfStringsIterator chIt = clientChIts.first;
    chIt != clientChIts.second; ++chIt) {
    if (chIt != clientChIts.first) oss << ',';
    oss << *chIt;
  }
  vector<string> params;
  params.push_back(oss.str());
  IRCMessage msg("", CMD_PART, params);
  answer_part(msg, fd); */
  return;   
}

/*
join lalala
:ngircd.none.net 403 n01 lalala :No such channel
*/
static string genNoSuchChannel(const string& servername,
  const string& nick, const string& channelName) {
  std::ostringstream oss;
  oss << ':' << servername 
    << ' ' << IRCCommandtoString(ERR_NOSUCHCHANNEL)
    << ' ' << nick
    << ' ' << channelName
    << " :No such channel\r\n";
  return oss.str();
}

/* Creating a new channel
>> join #test
:n01!~user1@lenovo-i5 JOIN :#test
:ngircd.none.net 353 n01 = #test :@n01
:ngircd.none.net 366 n01 #test :End of NAMES list
*/

/* Joining an already existing channel
>> join #test

:n02!~user2@lenovo-i5 JOIN :#test
:ngircd.none.net 332 n02 #test :Channel usage # RPL_TOPIC
:ngircd.none.net 333 n02 #test n01 1770644364 # Undef in RFC2812
:ngircd.none.net 353 n02 = #test :n02 @n01    #  RPL_NAMREPLY
:ngircd.none.net 366 n02 #test :End of NAMES list # RPL_ENDOFNAMES
*/
//  
void  IRCServ::answer_join(IRCMessage& msg, int fd) {
  if (msg.getCommand() != CMD_JOIN || clients.find(fd) == clients.end())
    return;
  IRCClient& newJoinClient = clients[fd];
  size_t nParams = msg.getParametersSize();
  if (nParams == 0 || nParams > 2) { // Syntax error
    queue_and_send(fd,
      genSyntaxError(server_name, newJoinClient.getNick(),"JOIN"));
    return;
  }
  if (nParams == 1 && msg.getParam(0) == "0") {
      join0(newJoinClient, fd);
      return;
  }
  // a list of channels
  vector<string> channelNames = split(msg.getParam(0), ",");
  if (channelNames.empty()) {
    queue_and_send(fd,
    genSyntaxError(server_name, newJoinClient.getNick(),"JOIN"));
    return;      
  }
  vector<string> channelKeys;
  if (nParams == 2) channelKeys = split(msg.getParam(1),",");
  for (size_t i=0; i < channelNames.size(); ++i) {
    string channelName = channelNames[i];
    ft_toLower(channelName);
    if (!IRCChannel::isValidName(channelName)) {
      queue_and_send(
        fd, 
        genNoSuchChannel(server_name, 
        newJoinClient.getNick(),
        channelName)
      );
      continue;
    }
    string key = (i < channelKeys.size()) ? channelKeys[i] : "";
    IRCChannel ircChannel;
    if (channels.find(channelName) != channels.end()) {
      ircChannel = channels[channelName];
    } else {
      ircChannel.setName(channelName);
      channels[channelName] = ircChannel;
      newJoinClient.addChannel(channelName);
    }
    std::ostringstream buf;
    string subscribedNick;
    // UserMode subscribedUserMode;
    PairUserMapIterators pairIterators = ircChannel.getUsersIterators();
    switch (ircChannel.addUser(newJoinClient.getNick(),USER_ONLY,key)) {
    case ADD_USER_OK:
    // 1. Confirmar al propio cliente que ha entrado
    buf.clear();
    buf << ':' << newJoinClient.getNick() << '!' << newJoinClient.getUsername()
        << '@' << newJoinClient.getHost() << " JOIN :" << channelName << "\r\n";
    queue_and_send(newJoinClient.getFd(), buf.str());

    // 2. Notificar a TODOS los demás usuarios del canal que este cliente ha entrado
    // (Aquí deberías iterar sobre los otros usuarios y enviarles el JOIN del nuevo)

    // 3. Enviar el Topic (332) - CORREGIDO
    buf.clear();
    buf << ':' << server_name << " 332 " << newJoinClient.getNick()
        << " " << channelName << " :" << ircChannel.getTopic() << "\r\n"; // Añadido channelName
    queue_and_send(newJoinClient.getFd(), buf.str());

    // 4. Enviar lista de nombres (353) - CORREGIDO
    buf.clear();
    buf << ':' << server_name << " 353 " << newJoinClient.getNick() << " = " << channelName << " :"; // Espacio antes de ':'
    
    for (UserMapIterator kvIt = pairIterators.first; kvIt != pairIterators.second; ++kvIt) {
        if (kvIt != pairIterators.first) buf << " "; // Espacio entre nicks
        if (kvIt->second == CHANNEL_OPERATOR) buf << '@';
        buf << kvIt->first;
    }
    buf << "\r\n";
    queue_and_send(newJoinClient.getFd(), buf.str());

    // 5. Fin de lista de nombres (366) - CORREGIDO
    buf.clear();
    buf << ':' << server_name << " 366 " << newJoinClient.getNick()
        << " " << channelName << " :End of /NAMES list\r\n"; // Añadido ':' inicial
    queue_and_send(newJoinClient.getFd(), buf.str());
    break;
    case INVITE_ONLY:
      // :ngircd.none.net 473 japo #test :Cannot join channel (+i) -- Invited users only
      buf.clear();
      buf << server_name << " 473 " << newJoinClient.getNick() << ' ' << channelName
        << " :Cannot join channel (+i) -- Invited users only\r\n";
      queue_and_send(newJoinClient.getFd(), buf.str());
      break;
    case TOPIC:
      break;
    case KEY:
      // :ngircd.none.net 475 japo #test :Cannot join channel (+k) -- Wrong channel key
      buf.clear();
      buf << server_name << " 475 " << newJoinClient.getNick() << ' ' << channelName
        << " :Cannot join channel (+k) -- Wrong channel key\r\n";
      queue_and_send(newJoinClient.getFd(), buf.str());        
      break;
    case USER_LIMIT:
      // :ngircd.none.net 471 japo #test :Cannot join channel (+l) -- Channel is full, try later
      buf.clear();
      buf << server_name << " 471 " << newJoinClient.getNick() << ' ' << channelName
        << " :Cannot join channel (+l) -- Channel is full, try later\r\n";
      queue_and_send(newJoinClient.getFd(), buf.str());         
      break;
    }
  }
  }
  

