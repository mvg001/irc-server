/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCMessage.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user1 <user1@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 11:55:03 by user1             #+#    #+#             */
/*   Updated: 2026/01/28 15:16:29 by user1            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <stdexcept>
#include <string>
#include <vector>

#include "IRCMessage.hpp"
#include "utils.hpp"

IRCMessage::IRCMessage(const std::string prefix, const IRCCommand command, 
  std::vector<std::string> params):
  prefix(prefix), command(command), parameters(params) {}

static void checkBasics(std::string& str) {
  if (str.empty()) 
    throw std::invalid_argument("empty message");
  if (str.length() >= MAX_MESSAGE_LENGTH)
    throw std::invalid_argument("message too large");
  if (str.find('\0') != std::string::npos)
    throw std::invalid_argument("message contains null char");
  std::string::size_type crlfPos = str.rfind("\r\n");
  if (crlfPos != std::string::npos) { // found crlf
    if (str.length() - crlfPos == 2) { // found crlf at end of string
      str.erase(crlfPos); // erase "\r\n" at the end
    } else { // found crlf not at the end of string
      throw std::invalid_argument("found more than one crlf");
    }
    if (str.find("\r\n") != std::string::npos) { // found crlf in the middle
      throw std::invalid_argument("found crlf in the middle");
    }
  }
}

IRCMessage IRCMessage::parse(std::string& str) {
  checkBasics(str);
  std::string prefixStr;
  std::string cmdStr;
  std::string param;
  std::vector<std::string> params;
  std::string::const_iterator it = str.begin();
  int state = 1;
  while (state > 0) {
    switch (state) {
    case 1: 
      if (*it == ' ') state = 1;
      else if (*it == ':') state = 2;
      else if (ft_isLetter(*it)) {
        cmdStr.append(1, *it);
        state = 4;
      } else if (ft_isDigit(*it)) {
        cmdStr.append(1, *it);
        state = 5;
      } else state = -1;
      break; // state == 1
    case 2:
      if (*it == ' ') state = 3;
      else if (it == str.end()) state = -1;
      else prefixStr.append(1, *it);
      break; // state == 2
    case 3:
      if (*it == ' ') state = 3;
      else if (ft_isLetter(*it)) {
        cmdStr.append(1, *it);
        state = 4;
      } else if (ft_isDigit(*it)) {
        cmdStr.append(1, *it);
        state = 5;
      } else state = -1;
      break; // state == 3
    case 4:
      if (ft_isLetter(*it)) cmdStr.append(1, *it);
      else if (*it == ' ') {
        param.clear();
        state = 6;
      }
      else if (it == str.end()) state = 0;
      else state = -1;
      break; // state == 4
    case 5:
      if (ft_isDigit(*it)) cmdStr.append(1, *it);
      else if (*it == ' ') {
        param.clear();
        state = 6;
      }
      else if (it == str.end()) state = 0;
      else state = -1;
      break; // state == 5
    case 6:
      if (*it == ' ') state = 6;
      else if (*it == ':') {
        param.clear();
        state = 8; 
      }
      else if (it == str.end()) {
        params.push_back(param.substr());
        state = 0;
      }
      else {
        param.append(1, *it);
        state = 7;
      }
      break; // state == 6
    case 7:
      if (it == str.end()) {
        params.push_back(param.substr());
        state = 0;
      } else if (*it == ' ') {
        params.push_back(param.substr());
        param.clear();
        state = 6;
      }
      else param.append(1, *it);
      break; // state == 7
    case 8:
      if (it == str.end()) {
        params.push_back(param.substr());
        state = 0;
      } else param.append(1, *it);
    }
    ++it;
  }
  if (state < 0) throw std::invalid_argument("invalid syntax");
  IRCCommand cmd = stringToIRCCommand(cmdStr);
  if (cmd == UNDEFINED) throw std::invalid_argument("unknown command");
  return IRCMessage(prefixStr, cmd, params);
}

IRCMessage::~IRCMessage() {}

