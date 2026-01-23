/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user1 <user1@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/21 12:37:13 by user1             #+#    #+#             */
/*   Updated: 2026/01/23 12:16:30 by user1            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_IRC_HPP
#define FT_IRC_HPP
#include <set>
#include <string>
#include <utility>
#include <vector>

#define MAX_MESSAGE_LENGTH 512
#define MAX_NICK_LENGTH 9

bool isLetter(char c);
bool isDigit(char c);
bool isSpecial(char c);

typedef enum {
  USER_FLAG,
  SERVICE_FLAG,
  OPERATOR_FLAG,
  REGISTERED_FLAG,
} FtIRCFlag;

typedef std::set<std::string>::const_iterator setOfStringsIterator;

class IRCClient {
public:
  IRCClient();
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
  std::pair<setOfStringsIterator, setOfStringsIterator> getChannelIterators() const;
  
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
  
private:
  int fd;
  std::string nick;
  std::string username;
  std::string fullname;
  std::set<std::string> channelNames;
  std::set<FtIRCFlag> flags;
};

class IRCChannel {
public:
private:
  std::string name;
  std::set<std::string> nicks;
};

/** Commands defined in RFC2812 for client-server messages.
*/
typedef enum {
  UNDEF = 0,
  PASS,
  NICK,
  USER,
  OPER,
  MODE,
  SERVICE,
  QUIT,
  SQUIT,
  JOIN,
  PART,
  TOPIC,
  NAMES,
  LIST,
  INVITE,
  KICK,
  PRIVMSG,
  NOTICE,
  MOTD,
  LUSERS,
  VERSION,
  STATS,
  LINKS,
  TIME,
  CONNECT,
  TRACE,
  ADMIN,
  INFO,
  SERVLIST,
  SQUERY,
  WHO,
  WHOIS,
  WHOWAS,
  KILL,
  PING,
  PONG,
  ERROR,
  AWAY,
  REHASH,
  DIE,
  RESTART,
  SUMMON,
  USERS,
  WALLOPS,
  USERHOST,
  ISON,
} IRCCommands;

class IRCMessage {
public:
  IRCCommands getCommand() const;
private:
  std::string prefix;
  IRCCommands command;
  std::vector<std::string> parameters;
};

/** Numeric reply and error codes
*/
typedef enum {
  RPL_WELCOME         = 001,
  RPL_YOURHOST        = 002,
  RPL_CREATED         = 003,
  RPL_MYINFO          = 004,
  RPL_BOUNCE          = 005,
  RPL_TRACELINK       = 200,
  RPL_TRACECONNECTING = 201,
  RPL_TRACEHANDSHAKE  = 202,
  RPL_TRACEUNKNOWN    = 203,
  RPL_TRACEOPERATOR   = 204,
  RPL_TRACEUSER       = 205,
  RPL_TRACESERVER     = 206,
  RPL_TRACESERVICE    = 207,
  RPL_TRACENEWTYPE    = 208,
  RPL_TRACECLASS      = 209,
  RPL_TRACERECONNECT  = 210,
  RPL_STATSLINKINFO   = 211,
  RPL_STATSCOMMANDS   = 212,
  RPL_STATSCLINE      = 213,
  RPL_STATSNLINE      = 214,
  RPL_STATSILINE      = 215,
  RPL_STATSKLINE      = 216,
  RPL_STATSQLINE      = 217,
  RPL_STATSYLINE      = 218,
  RPL_ENDOFSTATS      = 219,
  RPL_UMODEIS         = 221,
  RPL_SERVICEINFO     = 231,
  RPL_ENDOFSERVICES   = 232,
  RPL_SERVICE         = 233,
  RPL_SERVLIST        = 234,
  RPL_SERVLISTEND     = 235,
  RPL_STATSVLINE      = 240,
  RPL_STATSLLINE      = 241,
  RPL_STATSUPTIME     = 242,
  RPL_STATSOLINE      = 243,
  RPL_STATSHLINE      = 244,
  RPL_STATSSLINE      = 244,
  RPL_STATSPING       = 246,
  RPL_STATSBLINE      = 247,
  RPL_STATSDLINE      = 250,
  RPL_LUSERCLIENT     = 251,
  RPL_LUSEROP         = 252,
  RPL_LUSERUNKNOWN    = 253,
  RPL_LUSERCHANNELS   = 254,
  RPL_LUSERME         = 255,
  RPL_ADMINME         = 256,
  RPL_ADMINLOC1       = 257,
  RPL_ADMINLOC2       = 258,
  RPL_ADMINEMAIL      = 259,
  RPL_TRACELOG        = 261,
  RPL_TRACEEND        = 262,
  RPL_TRYAGAIN        = 263,
  RPL_NONE            = 300,
  RPL_AWAY            = 301,
  RPL_USERHOST        = 302,
  RPL_ISON            = 303,
  RPL_UNAWAY          = 305,
  RPL_NOWAWAY         = 306,
  RPL_WHOISUSER       = 311,
  RPL_WHOISSERVER     = 312,
  RPL_WHOISOPERATOR   = 313,
  RPL_WHOWASUSER      = 314,
  RPL_ENDOFWHO        = 315,
  RPL_WHOISCHANOP     = 316,
  RPL_WHOISIDLE       = 317,
  RPL_ENDOFWHOIS      = 318,
  RPL_WHOISCHANNELS   = 319,
  RPL_LISTSTART       = 321,
  RPL_LIST            = 322,
  RPL_LISTEND         = 323,
  RPL_CHANNELMODEIS   = 324,
  RPL_UNIQOPIS        = 325,
  RPL_NOTOPIC         = 331,
  RPL_TOPIC           = 332,
  RPL_INVITING        = 341,
  RPL_SUMMONING       = 342,
  RPL_INVITELIST      = 346,
  RPL_ENDOFINVITELIST = 347,
  RPL_EXCEPTLIST      = 348,
  RPL_ENDOFEXCEPTLIST = 349,
  RPL_VERSION         = 351,
  RPL_WHOREPLY        = 352,
  RPL_NAMREPLY        = 353,
  RPL_KILLDONE        = 361,
  RPL_CLOSING         = 362,
  RPL_CLOSEEND        = 363,
  RPL_LINKS           = 364,
  RPL_ENDOFLINKS      = 365,
  RPL_ENDOFNAMES      = 366,
  RPL_BANLIST         = 367,
  RPL_ENDOFBANLIST    = 368,
  RPL_ENDOFWHOWAS     = 369,
  RPL_INFO            = 371,
  RPL_MOTD            = 372,
  RPL_INFOSTART       = 373,
  RPL_ENDOFINFO       = 374,
  RPL_MOTDSTART       = 375,
  RPL_ENDOFMOTD       = 376,
  RPL_YOUREOPER       = 381,
  RPL_REHASHING       = 382,
  RPL_YOURESERVICE    = 383,
  RPL_MYPORTIS        = 384,
  RPL_TIME            = 391,
  RPL_USERSSTART      = 392,
  RPL_USERS           = 393,
  RPL_ENDOFUSERS      = 394,
  RPL_NOUSERS         = 395,
  ERR_NOSUCHNICK      = 401,
  ERR_NOSUCHSERVER    = 402,
  ERR_NOSUCHCHANNEL   = 403,
  ERR_CANNOTSENDTOCHAN = 404,
  ERR_TOOMANYCHANNELS = 405,
  ERR_WASNOSUCHNICK   = 406,
  ERR_TOOMANYTARGETS  = 407,
  ERR_NOSUCHSERVICE   = 408,
  ERR_NOORIGIN        = 409,
  ERR_NORECIPIENT     = 411,
  ERR_NOTEXTTOSEND    = 412,
  ERR_NOTOPLEVEL      = 413,
  ERR_WILDTOPLEVEL    = 414,
  ERR_BADMASK = 415,
  ERR_UNKNOWNCOMMAND = 421,
  ERR_NOMOTD = 422,
  ERR_NOADMININFO = 423,
  ERR_FILEERROR = 424,
  ERR_NONICKNAMEGIVEN = 431,
  ERR_ERRONEUSNICKNAME = 432,
  ERR_NICKNAMEINUSE = 433,
  ERR_NICKCOLLISION = 436,
  ERR_UNAVAILRESOURCE = 437,
  ERR_USERNOTINCHANNEL = 441,
  ERR_NOTONCHANNEL = 442,
  ERR_USERONCHANNEL = 443,
  ERR_NOLOGIN = 444,
  ERR_SUMMONDISABLED = 445,
  ERR_USERSDISABLED = 446,
  ERR_NOTREGISTERED = 451,
  ERR_NEEDMOREPARAMS = 461,
  ERR_ALREADYREGISTRED = 462,
  ERR_NOPERMFORHOST = 463,
  ERR_PASSWDMISMATCH = 464,
  ERR_YOUREBANNEDCREEP = 465,
  ERR_YOUWILLBEBANNED = 466,
  ERR_KEYSET = 467,
  ERR_CHANNELISFULL = 471,
  ERR_UNKNOWNMODE = 472,
  ERR_INVITEONLYCHAN = 473,
  ERR_BANNEDFROMCHAN = 474,
  ERR_BADCHANNELKEY = 475,
  ERR_BADCHANMASK = 476,
  ERR_NOCHANMODES = 477,
  ERR_BANLISTFULL = 478,
  ERR_NOPRIVILEGES = 481,
  ERR_CHANOPRIVSNEEDED = 482,
  ERR_CANTKILLSERVER = 483,
  ERR_RESTRICTED = 484,
  ERR_UNIQOPPRIVSNEEDED = 485,
  ERR_NOOPERHOST = 491,
  ERR_NOSERVICEHOST = 492,
  ERR_UMODEUNKNOWNFLAG = 501,
  ERR_USERSDONTMATCH = 502,
} IRCReplyCode;
#endif // FT_IRC_HPP
