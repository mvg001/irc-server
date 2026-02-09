/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCMessage.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvassall <mvassall@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 12:44:00 by user1             #+#    #+#             */
/*   Updated: 2026/02/08 17:15:26 by mvassall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCMESSAGE_HPP
#define IRCMESSAGE_HPP

#include <string>
#include <vector>
#include "IRCCommand.hpp"

#define MAX_MESSAGE_LENGTH 512
#define MAX_NICK_LENGTH 9

using std::string;
using std::vector;
using std::pair;

typedef pair<vector<string>::const_iterator, vector<string>::const_iterator> vectorIteratorPairType;
class IRCMessage {
public:
	IRCMessage();
  IRCMessage(const string prefix, const IRCCommand command, vector<string> params);
  static IRCMessage parse(string& str);
  virtual ~IRCMessage();
  IRCMessage(const IRCMessage& other);
  IRCMessage& operator=(const IRCMessage& other);
  const string& getPrefix() const;
  vectorIteratorPairType getParameters() const;
  vector<string>::size_type getParametersSize() const;
  const string& getParam(size_t n) const;
  IRCCommand getCommand() const;
  const string ircMessage() const;
  string toString() const;
private:
  string prefix;
  IRCCommand command;
  vector<string> parameters;
};
#endif
