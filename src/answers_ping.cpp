/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   answers_ping.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrollon- <jrollon-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/03 16:02:52 by jrollon-          #+#    #+#             */
/*   Updated: 2026/02/03 16:18:24 by jrollon-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCServ.hpp"

void IRCServ::answer_ping(IRCMessage & msg, int fd) {

	std::string ans;
	std::cout << ans << std::endl;
	queue_and_send(fd, ans);
}
