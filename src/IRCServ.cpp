/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user1 <user1@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 14:57:30 by user1             #+#    #+#             */
/*   Updated: 2026/01/28 15:02:33 by user1            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCServ.hpp"

IRCServ::IRCServ() : listening_socket(0), epoll_fd(0) {}
				
// Getter and Setter for listening_socket
int & IRCServ::getListeningSocket() {
		return listening_socket;
}
void IRCServ::setListeningSocket(int socket) {
		listening_socket = socket;
}

// Getter and Setter for epoll_fd
int & IRCServ::getEpollFd() {
		return epoll_fd;
}
void IRCServ::setEpollFd(int fd) {
		epoll_fd = fd;
}

// Getter and Setter for clients
const std::map<int, IRCClient>& IRCServ::getClients() const {
		return clients;
}
std::map<int, IRCClient>& IRCServ::getClients() {
		return clients;
}
void IRCServ::setClients(const std::map<int, IRCClient>& newClients) {
		clients = newClients;
}

// Getter and Setter for events
struct epoll_event* IRCServ::getEvents() {
		return events;
}
const struct epoll_event* IRCServ::getEvents() const {
		return events;
}
void IRCServ::setEvent(int fd, epoll_event event) {
				events[fd] = event;
}
