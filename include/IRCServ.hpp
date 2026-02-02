/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvassall <mvassall@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 14:52:51 by user1             #+#    #+#             */
/*   Updated: 2026/01/31 12:16:24 by mvassall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERV_HPP
#define IRCSERV_HPP
#include <map>
#include <set>
#include <string>
#include <sys/epoll.h>
#include "IRCClient.hpp"

class IRCServ {
public:
		IRCServ();

    int & getListeningSocket();
    void setListeningSocket(int socket);
    void setClientPassword(std::string& password);
    bool checkClientPassword(std::string& password) const;
    int & getEpollFd();
    void setEpollFd(int fd);
    const std::map<int, IRCClient>& getClients() const;
    std::map<int, IRCClient>& getClients();
    void setClients(const std::map<int, IRCClient>& newClients);
    struct epoll_event* getEvents();
    const struct epoll_event* getEvents() const;
    void setEvent(int fd, epoll_event event);

private:
    int listening_socket;
    std::string clientPassword;
    int epoll_fd;
    std::map<int, IRCClient> clients;
    struct epoll_event events[16];
		std::set<std::string> nicks;
		std::set<std::string> channels;
};
#endif