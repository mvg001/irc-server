/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCClient.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user1 <user1@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 10:45:05 by user1             #+#    #+#             */
/*   Updated: 2026/01/22 12:06:16 by user1            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

IRCClient::IRCClient() : fd(-1) {}
IRCClient::IRCClient(const IRCClient &other):
    nick(other.nick), 
    name(other.name), 
    fullname(other.fullname),
    fd(other.fd), 
    channelNames(other.channelNames), 
    flags(other.flags) {}

IRCClient &IRCClient::operator=(const IRCClient &other) {
  if (this != &other) {
    this->nick = other.nick;
    this->name = other.name;
    this->fullname = other.fullname;
    this->fd = other.fd;
    this->channelNames = other.channelNames;
    this->flags = other.flags;
  }
  return *this;
}

IRCClient::~IRCClient() {
  nick.clear();
  name.clear();
  fullname.clear();
  fd = -1;
  channelNames.clear();
  flags.clear();
}

const std::string& IRCClient::getNick() const { return nick; }

const std::string& IRCClient::getName() const { return name; }

const std::string& IRCClient::getFullname() const { return fullname; }

int IRCClient::getFd() const { return fd; }

bool IRCClient::checkChannel(const std::string& channelName) const {
  return channelNames.count(channelName) == 1;
}

bool IRCClient::addChannel(const std::string& channelName) {
  return channelNames.insert(channelName).second;
}

bool IRCClient::delChannel(const std::string& channelName) {
  return channelNames.erase(channelName) == 1;
}

void IRCClient::clearChannels() {
  channelNames.clear();
}

bool IRCClient::checkFlag(const FtIRCFlag& f) const {
  return flags.count(f) == 1;
}

bool IRCClient::setFlag(const FtIRCFlag& f) {
  return flags.insert(f).second;
}

bool IRCClient::unsetFlag(const FtIRCFlag& f) {
  return flags.erase(f) == 1;
}

void IRCClient::clearFlags() {
  flags.clear();
}