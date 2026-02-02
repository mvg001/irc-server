/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marcoga2 <marcoga2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 14:57:30 by user1             #+#    #+#             */
/*   Updated: 2026/02/02 18:57:52 by marcoga2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCServ.hpp"

IRCServ::IRCServ() : listening_socket(0), epoll_fd(0) {}

IRCServ::~IRCServ()
{
	close(epoll_fd);
	close(listening_socket);
}

IRCServ::IRCServ(int listening_port) : listening_socket(0), epoll_fd(0)
{
		listening_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (listening_socket == -1)
		throw std::runtime_error(std::string("socket: ")
		+ strerror(errno));

	int opt = 1;
	if (setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR,
		&opt, sizeof(opt)) == -1)
		throw std::runtime_error(std::string("Server constructor: ")
		+ strerror(errno));

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(listening_port);

	if (bind(listening_socket, (struct sockaddr *)&server_addr,
		sizeof(server_addr)) == -1)
		throw std::runtime_error(std::string("Bind: ")
		+ strerror(errno));

	if (listen(listening_socket, 10) == -1)
		throw std::runtime_error(std::string("Listen: ")
		+ strerror(errno));

	if (fcntl(listening_socket, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error(std::string("Fcntl: ")
		+ strerror(errno));

	this->setEpollFd(epoll_create1(EPOLL_CLOEXEC));
	if (epoll_fd == -1)
		throw std::runtime_error(std::string("Epoll_create1: ")
		+ strerror(errno));

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = listening_socket;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listening_socket, &ev) == -1)
		close(epoll_fd);

	std::cout << "🎧 Servidor escuchando en puerto " << listening_port
	<< "..." << std::endl;
	return;
}






int IRCServ::getListeningSocket() const{
		return listening_socket;
}
void IRCServ::setListeningSocket(int socket) {
		listening_socket = socket;
}
void IRCServ::setClientPassword(std::string& password) {
	clientPassword = password;
}
bool IRCServ::checkClientPassword(std::string& password) const {
	return password == clientPassword;
}
int IRCServ::getEpollFd() const{
		return epoll_fd;
}
void IRCServ::setEpollFd(int fd) {
		epoll_fd = fd;
}
const std::map<int, IRCClient>& IRCServ::getClients() const {
		return clients;
}
std::map<int, IRCClient>& IRCServ::getClients() {
		return clients;
}
void IRCServ::setClients(const std::map<int, IRCClient>& newClients) {
		clients = newClients;
}
struct epoll_event* IRCServ::getEvents() {
		return events;
}
const struct epoll_event* IRCServ::getEvents() const {
		return events;
}
void IRCServ::setEvent(int fd, epoll_event event) {
				events[fd] = event;
}





void IRCServ::run()
{
	while (true)
	{
		int ready = epoll_wait(epoll_fd, events, 16, -1);
		if (ready == -1)
		{
			if (errno == EINTR)
				continue;
			throw std::runtime_error(std::string("Epoll_create1: ")
			+ strerror(errno));
		}
		for (int i = 0; i < ready; ++i)
		{
			int fd = events[i].data.fd;
			if (fd == listening_socket)
				accept_new_connection();
			else
			{
				if (read_from_client(clients[fd]))
					return close_client(fd);
				process_client_buffer(fd);
			}
		}
	}
}

void IRCServ::close_client(int fd)
{
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1)
		throw std::runtime_error(std::string("epoll_ctl delete client: ")
		+ strerror(errno));
	close(fd);
	clients.erase(fd);

	std::cout << "Cliente" << fd << "desconectado" << std::endl;
}

// Acepta una nueva conexión entrante en listening_socket.
// - Configura el socket cliente en non-blocking.
// - Registra el cliente en epoll con EPOLLIN | EPOLLET.
// - Inicializa el buffer del cliente en client_buffers.
// Devuelve 0 en éxito, -1 en error (y no registra el cliente).
void IRCServ::accept_new_connection()
{
	int fd = accept(listening_socket, NULL, NULL);
	if (fd == -1)
		throw std::runtime_error(std::string("accept: ")
		+ strerror(errno));

	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
		close(fd);
		throw std::runtime_error(std::string("set nonblocking client: ")
		+ strerror(errno));
	}

	struct epoll_event client_ev;
	client_ev.events = EPOLLIN | EPOLLET;
	client_ev.data.fd = fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &client_ev) == -1){
		close(fd);
		throw std::runtime_error(std::string("epoll_ctl add client: ")
		+ strerror(errno));
	}

	clients[fd] = IRCClient(fd); // crear cliente
	std::cout << "Nueva conexión: fd=" << fd << std::endl;
}

// Lee todo lo disponible en modo non-blocking (EPOLLET) desde fd.
// - Añade los bytes leídos al buffer del cliente en client_buffers.
// - Si no está cerrado devuelve false.
bool IRCServ::read_from_client(IRCClient & client)
{
	char tmp[4096];

	while (true)
	{
		ssize_t n = recv(client.getFd(), tmp, sizeof(tmp), 0);
		if (n > 0)
			client.addToBuffer(tmp, n);

		if (client.getBuffer().size() > 4000) {
			std::cerr << "Client flooding, disconnecting..." << std::endl;
			close_client(client.getFd());
			return true;
		}
		else if (n == 0)
			return true;
		else
		{
			// el hecho de que errno haya sido llenado con estos numeros
			//    por recv() significa que llegamos al final del buffer
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return false;
			else if (errno == EINTR)
				continue;
			else
				throw std::runtime_error(std::string("recv: ")
				+ strerror(errno));
		}
	}
}

void IRCServ::process_client_buffer(int fd)
{
	std::string &buf = clients[fd].getBuffer();

	while (true)
	{
		size_t pos = buf.find("\r\n");
		bool found_crlf = true;
		if (pos == std::string::npos) {
			pos = buf.find('\n');
			found_crlf = false;
		}
		if (pos == std::string::npos)
			break;
		if (pos > 510)
			pos = 510;
		// Remover la parte consumida
		std::string message = buf.substr(0, pos);
		if (found_crlf)
			buf.erase(0, pos + 2);
		else
			buf.erase(0, pos + 1);
		// PARSEO
		try {
			IRCMessage ircMsg = IRCMessage::parse(message);
			std::cout << fd << ": " << ircMsg.toString() << std::endl;
		} catch (...) {
			std::cerr << fd << ": Unable to parse message => " << message << std::endl;
		}
	}
}

