/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvassall <mvassall@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 15:17:48 by user1             #+#    #+#             */
/*   Updated: 2026/01/31 12:16:40 by mvassall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <climits>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <map>
#include <sys/socket.h>
#include <unistd.h>
#include "IRCServ.hpp"
#include "IRCClient.hpp"
#include "utils.hpp"
//#include "IRCMessage.hpp"
// #include "IRCChannel.hpp"
// #include "IRCCommand.hpp"


void close_client(int fd, int epoll_fd, std::map<int, IRCClient> &clients)
{
	// Quitar del epoll (ignorar errores)
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1)
		perror("epoll_ctl DEL client");

	close(fd);
	clients.erase(fd);

	printf("Cliente %d desconectado\n", fd);
}

// Acepta una nueva conexión entrante en listening_socket.
// - Configura el socket cliente en non-blocking.
// - Registra el cliente en epoll con EPOLLIN | EPOLLET.
// - Inicializa el buffer del cliente en client_buffers.
// Devuelve 0 en éxito, -1 en error (y no registra el cliente).
int accept_new_connection(int listening_socket, int epoll_fd, std::map<int, IRCClient> &clients)
{
	int fd = accept(listening_socket, NULL, NULL);
	if (fd == -1)
		return (perror("accept"), -1);

	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		return (perror("set_nonblocking client"), close(fd), -1);

	struct epoll_event client_ev;
	client_ev.events = EPOLLIN | EPOLLET;
	client_ev.data.fd = fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &client_ev) == -1)
		return (perror("epoll_ctl ADD client"), close(fd), -1);

	clients[fd] = IRCClient(fd); // crea un cliente
	printf("Nueva conexión: fd=%d\n", fd);
	return 0;
}

// Lee todo lo disponible en modo non-blocking (EPOLLET) desde fd.
// - Añade los bytes leídos al buffer del cliente en client_buffers.
// - Implementa límites de buffer para protección anti-DoS.
// - Si detecta que el cliente cerró la conexión o se produce error no recuperable,
//   devuelve true (cliente cerrado) para que el llamador haga la limpieza.
// - Si no está cerrado devuelve false.
// Parámetros:
// - fd: descriptor del cliente
// - epoll_fd: descriptor epoll (solo para mensajes de error/limpieza opcional)
// - client_buffers: mapa fd -> buffer
// - max_buffer_size: tamaño máximo permitido del buffer por cliente
bool read_from_client(IRCClient & client)
{
	char tmp[4096];
	bool closed = false;

	while (true)
	{
		ssize_t n = recv(client.getFd(), tmp, sizeof(tmp), 0);
		if (n > 0)
			client.addToBuffer(tmp, n);

		else if (n == 0)
		{
			// cliente cerró conexión
			closed = true;
			break;
		}
		else
		{
			// n < 0, leemos el tipo de error
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break;
			else if (errno == EINTR)
				continue;
			else
			{
				perror("recv");
				closed = true;
				break;
			}
		}
	}
	return closed;
}

void process_client_buffer(int fd, std::map<int, IRCClient> &clients)
{
	std::string &buf = clients[fd].getBuffer();

	while (true)
	{
		size_t pos = buf.find("\r\n");
		bool found_crlf = true;
		if (pos == std::string::npos)
		{
			// Si no hay CRLF, buscar solo LF
			pos = buf.find('\n');
			found_crlf = false;
		}
		if (pos == std::string::npos)
			break;

		if (pos > 510)
			pos = 510;

		std::string message = buf.substr(0, pos);

		// Remover la parte consumida (incluyendo terminadores)
		if (found_crlf)
			buf.erase(0, pos + 2);
		else
			buf.erase(0, pos + 1);

		//
		//
		//
		// PARSEO
		//
		//
		printf("Socket %d dice: %s%s%s\n", fd, GREEN_TEXT, message.c_str(), RESET_COLOR);
		//
		//
		//
	}
}

void handle_line(int fd, int listening_socket, int epoll_fd, std::map<int, IRCClient> &clients)
{
	if (fd == listening_socket)
		accept_new_connection(listening_socket, epoll_fd, clients);

	else
	{
		// Evento en un cliente existente: leer todo lo disponible
		if (read_from_client(clients[fd]))
			return close_client(fd, epoll_fd, clients);

		process_client_buffer(fd, clients);
	}
}


//  1. Crea un socket TCP para escuchar conexiones entrantes.
//  2. Activa la opción SO_REUSEADDR para permitir reutilizar la dirección local 
//		si el socket se cierra y reabre rápidamente.
//  3. Inicializa y configura la estructura de dirección `server_addr` para bindear
//		el socket al puerto `server_number` (en cualquier interfaz de red disponible).
//  4. Asocia el socket a la dirección y puerto especificados usando `bind()`.
//  5. Establece el socket en modo escucha (listen) con una cola máxima de 10 conexiones pendientes.
//  6. Configura el socket en modo no bloqueante usando fcntl y O_NONBLOCK.
//  7. Crea una instancia epoll para gestionar eventos de I/O asíncronos.
//  8. Añade el socket de escucha a la instancia epoll para monitorizar eventos de entrada
//		(conexiones nuevas).
int setup_server(IRCServ &server, int listeningPort)
{
	server.getListeningSocket() = socket(AF_INET, SOCK_STREAM, 0);
	if (server.getListeningSocket() == -1)
		return (perror("socket"), -1);

	int opt = 1;
	if (setsockopt(server.getListeningSocket(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		return (close(server.getListeningSocket()), perror("setup_server"), -1);

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(listeningPort);

	if (bind(server.getListeningSocket(), (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
		return (close(server.getListeningSocket()), perror("setup_server"), -1);

	if (listen(server.getListeningSocket(), 10) == -1)
		return (close(server.getListeningSocket()), perror("setup_server"), -1);

	if (fcntl(server.getListeningSocket(), F_SETFL, O_NONBLOCK) == -1)
		return (close(server.getListeningSocket()), perror("setup_server"), -1);

	server.getEpollFd() = epoll_create1(EPOLL_CLOEXEC);
	if (server.getEpollFd() == -1)
		return (close(server.getListeningSocket()), perror("setup_server"), -1);

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = server.getListeningSocket();
	if (epoll_ctl(server.getEpollFd(), EPOLL_CTL_ADD, server.getListeningSocket(), &ev) == -1)
		close(server.getEpollFd());

	
	std::cout << "🎧 Servidor escuchando en puerto " << listeningPort << "..." << std::endl;
	return 0;
}

int main(int ac, char **av)
{
	if (ac != 3) {
		std::cerr << "Usage: ircserver <port> <client_password>" << std::endl;
		return (1);
	}
	IRCServ server = IRCServ();
	std::pair<int, bool> pairPort = ft_atoi(av[1]);
	if (!pairPort.second) {
		std::cerr << "Invalid server port" << std::endl;
		return 1;
	}
	if (setup_server(server, pairPort.first) == -1)
		return 1;

	while (true)
	{
		int ready = epoll_wait(server.getEpollFd(), server.getEvents(), 16, -1);
		if (ready == -1)
		{
			if (errno == EINTR)
				continue;
			perror("epoll_wait");
			break;
		}

		for (int i = 0; i < ready; ++i)
		{
			int fd = server.getEvents()[i].data.fd;
			handle_line(fd, server.getListeningSocket(), server.getEpollFd(), server.getClients());
		}
	}

	close(server.getEpollFd());
	close(server.getListeningSocket());
	return 0;
}
