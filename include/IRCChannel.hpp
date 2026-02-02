/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCChannel.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marcoga2 <marcoga2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 15:10:45 by user1             #+#    #+#             */
/*   Updated: 2026/02/02 16:48:28 by marcoga2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCCHANNEL_HPP
#define IRCCHANNEL_HPP
#include <string>
#include <set>
class IRCChannel {
public:
  IRCChannel();
  IRCChannel(const IRCChannel& other);
  IRCChannel& operator=(const IRCChannel& other);
  virtual ~IRCChannel();


  const std::string& getName() const;
  bool setName(const std::string& nick);
  bool checkUser(const std::string& channelName) const;
  bool addUser(const std::string& channelName);
  bool delUser(const std::string& channelName);
  void clearUsers();
	
  void sendMessageThrough();

  /** Generates a text view of the object
  * @returns {std::string} with the contents of the object */
  std::string toString() const;
private:
  std::string name;
	std::set<std::string> nicks;
};
#endif
