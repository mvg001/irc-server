#include <arpa/inet.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <limits>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <vector>

using std::cerr;
using std::endl;
using std::string;

/** Parse a string to int value using re /' '*[-]d+/
 * @param {const std::string} input
 * @returns {std::pair<int,bool>} .second == false if conversion has failed
 * .first the int value parsed
 */
std::pair<int, bool> ft_atoi(const std::string& str)
{
    bool isNegative = false;
    int digitCount = 0;
    long l = 0;
    std::string::const_iterator it;
    for (it = str.begin(); it != str.end(); ++it) {
        if (*it == ' ')
            continue;
        if (('0' <= *it && *it <= '9')) {
            digitCount++;
            l = (l * 10) + (*it - '0');
            if (l > std::numeric_limits<int>::max())
                return std::pair<int, bool>(0, false);
            continue;
        }
        break;
    }
    if (digitCount == 0)
        return std::pair<int, bool>(0, false);
    if (isNegative)
        l *= -1;
    return std::pair<int, bool>(static_cast<int>(l), true);
}

std::vector<string> readPhrases(const char* filename) {
  std::vector<string> lines;
  if (filename == NULL) return lines;
  std::ifstream is;
  is.open(filename);
  if (!is.is_open()) {
    cerr << "Error: could not open '" << filename << '\'' << endl;
    return lines;
  }
  string line;
  while (std::getline(is, line)) {
    if (line.empty()) continue;
    lines.push_back(line);
  }
  is.close();
  return lines;
}

int main(int ac, char* av[]) {
  if (ac != 6) {
    cerr << "Usage: " << av[0] <<" server port pass nick lines-file" << endl;
    return 1;
  }
  char *serverName = av[1];
  std::pair<int,bool> result = ft_atoi(av[2]);
  if (!result.second || result.first > std::numeric_limits<unsigned short>::max()
    || result.first < 1024) {
    cerr << "Invalid port" << endl;
    return 2;
  }
  int port = result.first;
  char *pass = av[3];
  char *nick = av[4];
  std::vector<string> phrases = readPhrases(av[5]);
  if (phrases.empty()) {
    cerr << "Fatal: no phrases found" << endl;
    return 3;
  }
  cerr << "Read " << phrases.size() << " phrases" << endl;
  srand(time(0));
  struct sockaddr_in serv_addr;

  // Create socket
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    cerr << "Socket creation error" << endl;
    return 4;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);

  // Convert IPv4 and IPv6 addresses from text to binary
  if (inet_pton(AF_INET, serverName, &serv_addr.sin_addr) <= 0) {
    cerr << "Invalid address/ Address not supported" << endl;
    return 5;
  }

  // Connect to server
  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    cerr << "Connection Failed" << endl;
    return 6;
  }
  // Send data
  string msg = "pass "; msg.append(pass); msg.append("\r\n");
  msg.append("nick "); msg.append(nick); msg.append("\r\n");
  msg.append("user "); msg.append(nick); msg.append(" 0 * :Konfucius\r\n");
  if (send(sock, msg.c_str(), msg.length(), MSG_NOSIGNAL) < 0) {
    close(sock);
    cerr << "send error login" << endl;
    return 7;
  }
  FILE* file = fdopen(sock, "r+");
  if (!file) {
      cerr << "fdopen" << endl;
      close(sock);
      return 8;
  }
  char* buf = NULL;
  size_t lineLen = 0;
  bool keepGoing = true;
  while (keepGoing && getline(&buf, &lineLen, file) >= 0) {
    string line = buf;
    free(buf);
    buf = NULL;
    // :bbb!BBB@127.0.0.1 PRIVMSG aaa :hello
    if (line.find(" PRIVMSG ") == string::npos) {
      line.clear(); continue;
    }
    if (line.find("SHUTDOWN") != string::npos) {
      string quitMsg = "quit :bye bye bye bye bye bye bye bye\r\n";
      send(sock,quitMsg.c_str(),quitMsg.length(),MSG_NOSIGNAL);
      shutdown(sock,SHUT_WR);
      keepGoing = false;
      continue;
    }
    size_t startPos = line.find_first_of(':');
    size_t endPos = line.find_first_of('!');
    if (startPos == string::npos || endPos == string::npos || startPos >= endPos) {
      line.clear(); continue;
    }
    string nick = line.substr(startPos+1,endPos-startPos-1);
    string reply = "privmsg ";
    reply.append(nick);
    reply.append(" :");
    reply.append(phrases[rand() % phrases.size()]);
    reply.append("\r\n");
    if (send(sock,reply.c_str(),reply.length(),MSG_NOSIGNAL) < 0) {
      keepGoing = false;
    }
  }
  free(buf);
  buf = NULL;
  while (getline(&buf, &lineLen, file) > 0) {
    cerr << buf;
    free(buf);
    buf = NULL;
  }
  free(buf);
  // Close socket
  shutdown(sock, SHUT_RD);
  fclose(file);
  close(sock);
  return 0;
}

