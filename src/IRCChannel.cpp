/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCChannel.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvassall <mvassall@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 15:12:14 by user1             #+#    #+#             */
/*   Updated: 2026/02/05 16:45:52 by mvassall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCChannel.hpp"
#include "utils.hpp"
#include <sstream>
#include <stdexcept>
#include <utility>

IRCChannel::IRCChannel(): name("invalid") {}

IRCChannel::IRCChannel(const std::string& name) {
  if (!isValidName(name))
    throw std::invalid_argument("invalid channel name");
  this->name = name;
  ft_toLower(this->name);
}

IRCChannel::IRCChannel(const IRCChannel& other):
  name(other.name), nicks(other.nicks) {}

  IRCChannel& IRCChannel::operator=(const IRCChannel& other) {
  if (this != &other) {
    name = other.name;
    nicks = other.nicks;
  }
  return *this;
}
    
IRCChannel::~IRCChannel() {}

bool IRCChannel::isValidName(const std::string &name) {
  // maxlength 50 chars
  if (name.length() > MAX_NAME_LENGTH) return false;
  // must start with any of "#&+!"
  static const std::string VALID_PREFIX = "#&+!";  // TODO safe_channels
  if (VALID_PREFIX.find_first_of(name[0]) == std::string::npos)
    return false;
  // must not have any char " ,:\x07"
  static const std::string INVALID_CHARS = " ,:\x07";
  for (std::string::const_iterator it = INVALID_CHARS.begin();
      it != INVALID_CHARS.end(); ++it)
    if (name.find_first_of(*it) != std::string::npos)
      return false;
  return true;
}

const std::string& IRCChannel::getName() const {
  return name;
}

bool IRCChannel::setName(const std::string& name) {
  if (!isValidName(name))
    return false;
  this->name = name;
  ft_toLower(this->name);
  return true;
}

bool IRCChannel::checkUser(const std::string& user) const {
  return nicks.count(user) != 0;
}

bool IRCChannel::addUser(const std::string& nick) {
  return nicks.insert(nick).second;
}

bool IRCChannel::delUser(const std::string& nick) {
  return nicks.erase(nick) != 0;
}

void IRCChannel::clearUsers() {
  nicks.clear();
}

pairIterators IRCChannel::getChannelIterators() const {
  return pairIterators(nicks.begin(), nicks.end());
}

void IRCChannel::sendMessageThrough(IRCMessage& msg) const {
  // TODO
  (void)msg;
}

  /** Generates a text view of the object
  * @returns {std::string} with the contents of the object */
std::string IRCChannel::toString() const {
  std::ostringstream buf;
  buf << "name=\"" << name << "\", nicks=[";
  for (setOfStringsIterator it = nicks.begin(); it != nicks.end(); ++it) {
    if (it != nicks.begin()) buf << ", ";
    buf << '"' << *it << '"';
  }
  buf << "]";
  return buf.str();
}