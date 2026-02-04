#include "IRCServ.hpp"

// void IRCServ::answer_kick(IRCMessage & msg, int fd) {}

// void IRCServ::answer_invite(IRCMessage & msg, int fd) {}

// void IRCServ::answer_topic(IRCMessage & msg, int fd) {}

// void IRCServ::answer_mode(IRCMessage & msg, int fd) {}

// void IRCServ::answer_quit(IRCMessage & msg, int fd) {}

// void IRCServ::answer_join(IRCMessage & msg, int fd) {}

// void IRCServ::answer_part(IRCMessage & msg, int fd) {}

// void IRCServ::answer_privmsg(IRCMessage & msg, int fd) {}

// void IRCServ::answer_notice(IRCMessage & msg, int fd) {}



// void IRCServ::answer_pong(IRCMessage & msg, int fd) {}


void IRCServ::answer_pass(IRCMessage & msg, int fd)
{
	std::ostringstream recv_pass;
	vectorIteratorPairType range = msg.getParameters();
	for (vector<string>::const_iterator it = range.first; it != range.second; ++it)
		recv_pass << (it == range.first ? "" : " ") << *it;

	if (recv_pass.str().empty())
	{
		queue_and_send(fd, "461 * PASS :Not enough parameters");
	}
	if (recv_pass.str() != clientPassword)
	{
		queue_and_send(fd, "464 * :Password incorrect");
	}
}

void IRCServ::answer_nick(IRCMessage & msg, int fd)
{

}

void IRCServ::answer_user(IRCMessage & msg, int fd)
{

}


void IRCServ::queue_and_send(int fd, std::string data)
{
    clients[fd].addToObuffer(data);

    std::string &buffer = clients[fd].getObuffer();
    
    ssize_t sent = send(fd, buffer.c_str(), buffer.size(), MSG_NOSIGNAL);
    if (sent > 0)
        buffer.erase(0, sent);

    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN | EPOLLET;
    if (!buffer.empty())
        ev.events |= EPOLLOUT;

    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev);
}
