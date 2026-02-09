/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   answer_pong.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrollon- <jrollon-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/09 11:15:06 by jrollon-          #+#    #+#             */
/*   Updated: 2026/02/09 11:16:20 by jrollon-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCServ.hpp"

void IRCServ::answer_pong(IRCMessage & msg, int fd) {
	(void)msg;
	if (clients.count(fd)){
		clients[fd].updateLastActivity();
	}
}
