/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCClient.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marcoga2 <marcoga2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 12:36:20 by user1             #+#    #+#             */
/*   Updated: 2026/02/02 20:10:08 by marcoga2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCCLIENT_HPP
#define IRCCLIENT_HPP
#include <set>
#include <string>
typedef enum {
  PASS_FLAG,        // after PASS command
  NICK_FLAG,        // after NICK command
  USER_FLAG,        // after USER command
  SERVICE_FLAG,     // after SERVICE command
  OPERATOR_FLAG,    // after OPERATOR command
  REGISTERED_FLAG,  // after PASS, NICK, (USER || SERVICE) commands
} FtIRCFlag;

const std::string FtIRCFlagToString(FtIRCFlag flag);
typedef std::set<std::string>::const_iterator setOfStringsIterator;
typedef std::pair<setOfStringsIterator, setOfStringsIterator> pairIterators;

class IRCClient {
public:
  IRCClient();
  IRCClient(int fd);
  IRCClient(const IRCClient& other);
  IRCClient& operator=(const IRCClient& other);
  virtual ~IRCClient();
  
  /** Getter for nick name
  * @returns {const std::string&} nick value
  */
  const std::string& getNick() const;

  /** Setter for nick name
  * @param {const std::string} nick - new nick name for the client
  * @returns {bool} operation successfully performed
  */
  bool setNick(const std::string& nick);

  /** Verify string is valid according to the rule defined in RFC2812 2.3.1:
  *   nickname = ( letter / special ) *8( letter / digit / special / "-" )
  * @param {const std::string} nick - nick name to check
  * @returns {bool} nick has a valid format
  */
  static bool isValidNick(const std::string& nick);

  /** Getter for username
  * @returns {const std::string&} username value
  */
  const std::string& getUsername() const;
  
  /** Setter for username
  * @param {const std::string} username - new username for the client
  * @returns {bool} operation successfully performed
  */  
  bool setUsername(const std::string& username);
  
  /** Verify string is valid according RFC2812 2.3.1:
  * user =  1*( %x01-09 / %x0B-0C / %x0E-1F / %x21-3F / %x41-FF )
  * which means 1 to 9 chars, any 7-bit US_ASCII character, 
  * except NUL, CR, LF, FF, h/v TABs, and " "
  * @param {const std::string&} username to be checked
  * @returns {bool} username has a valid format
  */
  static bool isValidUsername(const std::string& username);

  /** Getter for fullname
  * @returns {const std::string&} fullname value
  */
  const std::string& getFullname() const;

  /** Setter for fullname
  * @param {const std::string} fullname - new value for the client
  * @returns {bool} operation successfully performed
  */  
  bool setFullname(const std::string& fullname);

  /** Verify string is valid, no strict rule in RFC2812, only in item 3.1.3
  * it is allowed explicitly space character ' '.
  * We are accepting anything except empty string, larger than MAX_MESSAGE_LENGTH
  * or it has a NUL '\0' char.
  * @param {const std::string&} fullname to be checked
  * @returns {bool} fullname has a valid format
  */
  static bool isValidFullname(const std::string& fullname);

  /** Getter for file descriptor associated to this client
  * @returns {int} fd value
  */
  int getFd() const;
  
  /** Setter for file descriptor
  * @param {int} fd - value cannot be < 0
  * @returns {bool} operation successfully performed
  */ 
  bool setFd(int fd);
  
  /** Verifies if the client belongs to a channel given its name
  * @param {const std::string&} channelName to check
  * @returns {bool} user has joined the channel
  */
  bool checkChannel(const std::string& channelName) const;

  /** Add a new channel joined by this client.
  * @param {const std::string&} channelName joined
  * @returns {bool} returns false if already joined the channel
  */
  bool addChannel(const std::string& channelName);

  /** Delete a channel from the set of joined ones.
  * @param {const std::string&} channelName to delete
  * @returns {bool} returns false if channel was not joined
  */
  bool delChannel(const std::string& channelName);

  /** Clear all channels
  */
  void clearChannels();
  
  /** Getter for const iterators (begin, end) of the set of joined channels
  * @returns {std::pair<setOfStringsIterator,setOfStringsIterator>}
  * setOfStringsIterator is defined as std::set<std::string>::const_iterator
  */
  pairIterators getChannelIterators() const;
  
  /** Verifies whether a flag is set for this client
  * @param {const FtIRCFlag&} f - flag to be checked
  * @returns {bool} flag is set
  */
  bool checkFlag(const FtIRCFlag& f) const;
  
  /** Set a client flag, avoiding inconsistent combinations
  * @param {const FtIRCFlag&} f - flag to be set
  * @returns {bool} operation successfully performed
  */
  bool setFlag(const FtIRCFlag& f);

  /** Unset a client flag
  * @param {const FtIRCFlag&} f - flag to be unset
  * @returns {bool} return false if the flag was not set */
  bool unsetFlag(const FtIRCFlag& f);
  
  /** Clear all client flags */
  void clearFlags();
  
  /** Generates a text view of the object
  * @returns {std::string} with the contents of the object */
  std::string toString() const;

  /** Edit Ibuffer */
  void addToIbuffer(const std::string & s);
  void addToIbuffer(const char * s, size_t n);

  /** Set Ibuffer */
  void setIbuffer(const std::string & s);

  /** Get Ibuffer */
  std::string & getIbuffer();

	/** Edit Obuffer */
  void addToObuffer(const std::string & s);
  void addToObuffer(const char * s, size_t n);

  /** Set Obuffer */
  void setObuffer(const std::string & s);

  /** Get Obuffer */
  std::string & getObuffer();

private:
  int fd;
  std::string nick;
  std::string username;
  std::string fullname;
  std::set<std::string> channelNames;
  std::set<FtIRCFlag> flags;
  std::string Ibuffer;
  std::string Obuffer;
};

#endif
