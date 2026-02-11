/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   answer_join.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marcoga2 <marcoga2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 12:42:05 by mvassall          #+#    #+#             */
/*   Updated: 2026/02/10 15:29:05 by marcoga2         ###   ########.fr       */
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

// static void createNewChannel() {
  
// }
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
  IRCClient& client = clients[fd];
  size_t nParams = msg.getParametersSize();
  if (nParams == 0) { // not enough parameters
    queue_and_send(fd,
      genErrNeedMoreParams(server_name, client.getNick()));
    return;
  }
  string param0 = msg.getParam(0);
  if (nParams == 1) {
    if (param0 == "0") { // user leaves all channels
      join0(client, fd);
      return;
    }
    // a list of channels
    vector<string> channelNames = split(param0, ",");
    if (channelNames.empty()) {
      queue_and_send(fd,
      genErrNeedMoreParams(server_name, client.getNick()));
      return;      
    }
    vector<string> channelKeys;
    if (nParams == 2) channelKeys = split(msg.getParam(1),",");
    for (size_t i=0; i < channelNames.size(); ++i) {
      string channelName = channelNames[i];
      if (!IRCChannel::isValidName(channelName)) {
        queue_and_send(
          fd, 
          genNoSuchChannel(server_name, 
          client.getNick(),
          channelName)
        );
        continue;
      }
      ft_toLower(channelName);
      string key = i < channelKeys.size() ? channelKeys[i] : "";
      IRCChannel ircChannel;
      if (channels.find(channelName) != channels.end()) {
        ircChannel = channels[channelName];
      } else {
        ircChannel.setName(channelName);
      }
      std::ostringstream msg;
      string subscribedNick;
      UserMode subscribedUserMode;
      PairUserMapIterators pairIterators = ircChannel.getUsersIterators();
      switch (ircChannel.addUser(client.getNick(),USER_ONLY,key)) {
      case ADD_USER_OK:
        channels[channelName] = ircChannel;
        client.addChannel(channelName);
        // :n01!~user1@lenovo-i5 JOIN :#test
        msg.clear();
        msg << ':' << client.getNick() 
          << '!' << client.getUsername()
          << '@' << client.getHost()
          << " JOIN :" << channelName << "\r\n";
        queue_and_send(client.getFd(), msg.str());
        msg.clear();
        // :ngircd.none.net 332 n02 #test :Channel usage # RPL_TOPIC
        msg << ':' << server_name << " 332 " << client.getNick()
          << " :" << ircChannel.getTopic() << "\r\n";
        queue_and_send(client.getFd(), msg.str());
        for (UserMapIterator kvIt = pairIterators.first;
              kvIt != pairIterators.second; ++kvIt) {
          subscribedNick = kvIt->first;
          if (subscribedNick != client.getNick() 
            && nicks.find(subscribedNick) != nicks.end()) {
            int fd = nicks[subscribedNick];
            if (clients.find(fd) != clients.end()) {
              IRCClient mClient = clients[fd];
              msg.clear(); // :japo!~javier@lenovo-i5 JOIN :#tst
              msg << ':' << mClient.getNick() << '!' << mClient.getUsername()
                << '@' << mClient.getHost() << " JOIN :" 
                << channelName << "\r\n";
                queue_and_send(client.getFd(), msg.str());
            }
          }
          subscribedUserMode = kvIt->second;
          msg.clear();
          //:ngircd.none.net 353 n02 = #test :n02 @n01    #  RPL_NAMREPLY
          msg << ':' << server_name << " 353 " << client.getNick()
            << " = " << channelName << ':';
          if (subscribedUserMode == CHANNEL_OPERATOR) msg << '@';
          msg << subscribedNick << "\r\n";
          queue_and_send(client.getFd(), msg.str());
        }        
        msg.clear();
        //:ngircd.none.net 366 n02 #test :End of NAMES list # RPL_ENDOFNAMES
        msg << server_name << " 366 " << client.getNick()
          << ' ' << channelName << " :End of NAMES list\r\n";
        queue_and_send(client.getFd(), msg.str());
        break;
      case INVITE_ONLY:
        // :ngircd.none.net 473 japo #test :Cannot join channel (+i) -- Invited users only
        msg.clear();
        msg << server_name << " 473 " << client.getNick() << ' ' << channelName
          << " :Cannot join channel (+i) -- Invited users only\r\n";
        queue_and_send(client.getFd(), msg.str());
        break;
      case TOPIC:
        break;
      case KEY:
        // :ngircd.none.net 475 japo #test :Cannot join channel (+k) -- Wrong channel key
        msg.clear();
        msg << server_name << " 475 " << client.getNick() << ' ' << channelName
          << " :Cannot join channel (+k) -- Wrong channel key\r\n";
        queue_and_send(client.getFd(), msg.str());        
        break;
      case USER_LIMIT:
        // :ngircd.none.net 471 japo #test :Cannot join channel (+l) -- Channel is full, try later
        msg.clear();
        msg << server_name << " 471 " << client.getNick() << ' ' << channelName
          << " :Cannot join channel (+l) -- Channel is full, try later\r\n";
        queue_and_send(client.getFd(), msg.str());         
        break;
      }
    }
  }
  
}

