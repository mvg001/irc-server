/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   misc.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user1 <user1@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 14:22:07 by user1             #+#    #+#             */
/*   Updated: 2026/01/26 12:22:06 by user1            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include <sstream>

bool isLetter(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

bool isDigit(char c) {
  return ('0' <= c && c <= '9');
}

// [ ] { } \ ^ _ ` |
bool isSpecial(char c) {
  return (0x5B <= c && c <= 0x60) || (0x7B <= c && c <= 0x7D);
}

std::string ft_itoa(int n) {
  std::ostringstream os;
  os << n;
  return os.str();    
}