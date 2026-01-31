/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvassall <mvassall@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 15:05:14 by user1             #+#    #+#             */
/*   Updated: 2026/01/31 12:16:30 by mvassall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

#define GREEN_TEXT "\033[1;32m"
#define RESET_COLOR "\033[0m"

bool ft_isLetter(char c);
bool ft_isDigit(char c);
bool ft_isSpecial(char c);
std::string ft_itoa(int n);
std::pair<int, bool> ft_atoi(const std::string& str);
void ft_toUpper(std::string& str);
const std::string& getLocalHostname();
#endif