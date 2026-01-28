/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCMessage.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user1 <user1@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 12:44:00 by user1             #+#    #+#             */
/*   Updated: 2026/01/28 15:07:56 by user1            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCMESSAGE_HPP
#define IRCMESSAGE_HPP

#include <string>
#include <vector>
#include "IRCCommand.hpp"

#define MAX_MESSAGE_LENGTH 512
#define MAX_NICK_LENGTH 9

class IRCMessage {
public:
  IRCMessage(const std::string prefix, const IRCCommand command, std::vector<std::string> params);
  static IRCMessage parse(std::string& str);
  virtual ~IRCMessage();
  IRCMessage(const IRCMessage& other);
  IRCMessage operator=(const IRCMessage& other);
  const std::string& getPrefix();
  std::pair<const std::string::const_iterator,
    const std::string::const_iterator> getParameters();
  IRCCommand getCommand() const;
  std::string toString() const;
private:
  std::string prefix;
  IRCCommand command;
  std::vector<std::string> parameters;
};
#endif