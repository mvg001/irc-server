/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCClient.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user1 <user1@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 10:45:05 by user1             #+#    #+#             */
/*   Updated: 2026/01/23 12:10:14 by user1            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include <string>

IRCClient::IRCClient() : fd(-1) {}
IRCClient::IRCClient(const IRCClient &other):
    fd(other.fd), 
    nick(other.nick), 
    username(other.username), 
    fullname(other.fullname),
    channelNames(other.channelNames), 
    flags(other.flags) {}

IRCClient &IRCClient::operator=(const IRCClient &other) {
  if (this != &other) {
    this->nick = other.nick;
    this->username = other.username;
    this->fullname = other.fullname;
    this->fd = other.fd;
    this->channelNames = other.channelNames;
    this->flags = other.flags;
  }
  return *this;
}

IRCClient::~IRCClient() {
  nick.clear();
  username.clear();
  fullname.clear();
  fd = -1;
  channelNames.clear();
  flags.clear();
}

const std::string& IRCClient::getNick() const { return nick; }

bool IRCClient::setNick(const std::string& nick) {
  if (!isValidNick(nick)) return false;
  this->nick = nick;
  return true;
}

bool IRCClient::isValidNick(const std::string &nick) {
  if (nick.empty() || nick.length() > MAX_NICK_LENGTH) return false;
  if (!isLetter(nick.at(0))) return false;
  if (!isSpecial(nick.at(0))) return false;
  std::string::const_iterator it;
  for (it = ++(nick.begin()); it != nick.end(); ++it) {
    if (isLetter(*it)) continue;
    if (isDigit(*it)) continue;
    if (isSpecial(*it)) continue;
    if (*it == '-') continue;
    return false;
  }
  return true;
}
const std::string& IRCClient::getUsername() const { return username; }

bool IRCClient::setUsername(const std::string& username) {
  if (!isValidUsername(username)) return false;
  this->username = username;
  return true;
}

bool IRCClient::isValidUsername(const std::string &username) {
  if (username.empty() || username.length() >= MAX_MESSAGE_LENGTH)
    return false;
  std::string::const_iterator it;
  for (it = username.begin(); it != username.end(); ++it) {
      if ((*it & 0x80) || *it == '\0' || *it == '\r' || *it == '\n'
        || *it == '\f' || *it == '\t' || *it == '\v' || *it == ' ')
        return false;
    }
  return true;
}

const std::string& IRCClient::getFullname() const { return fullname; }
bool IRCClient::setFullname(const std::string& fullname) {
  if (!isValidFullname(fullname)) return false;
  this->fullname = fullname;
  return true;
}

bool IRCClient::isValidFullname(const std::string &fullname) {
  if (fullname.empty() || fullname.length() >= MAX_MESSAGE_LENGTH)
    return false;
  std::string::const_iterator it;
  for (it = fullname.begin(); it != fullname.end(); ++it) {
      if (*it == '\0') return false;
    }
  return true;  
}

int IRCClient::getFd() const { return fd; }

bool IRCClient::setFd(int fd) {
  if (fd < 0) return false;
  this->fd = fd;
  return true;
}

bool IRCClient::checkChannel(const std::string& channelName) const {
  return channelNames.count(channelName) != 0;
}

bool IRCClient::addChannel(const std::string& channelName) {
  return channelNames.insert(channelName).second;
}

bool IRCClient::delChannel(const std::string& channelName) {
  return channelNames.erase(channelName) != 0;
}

void IRCClient::clearChannels() {
  channelNames.clear();
}

std::pair<setOfStringsIterator, setOfStringsIterator>
  IRCClient::getChannelIterators() const {
  return std::pair<setOfStringsIterator, setOfStringsIterator>(
    channelNames.begin(), channelNames.end());
}

bool IRCClient::checkFlag(const FtIRCFlag& f) const {
  return flags.count(f) != 0;
}

bool IRCClient::setFlag(const FtIRCFlag& f) {
  if (f == SERVICE_FLAG && flags.count(USER_FLAG) != 0)
    return false;
  if (f == USER_FLAG && flags.count(SERVICE_FLAG) != 0)
    return false;
  if (f == OPERATOR_FLAG && flags.count(USER_FLAG) == 0)
    return false;
  return flags.insert(f).second;
}

bool IRCClient::unsetFlag(const FtIRCFlag& f) {
  return flags.erase(f) != 0;
}

void IRCClient::clearFlags() {
  flags.clear();
}