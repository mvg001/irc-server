# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: user1 <user1@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/12/11 11:05:02 by user1             #+#    #+#              #
#    Updated: 2026/01/21 12:43:15 by user1            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXX := c++
CXXFLAGS := -Wall -Werror -Wextra -std=c++98 -g3 -fsanitize=address
#CXXFLAGS := -Wall -Werror -Wextra -std=c++98
SRC_DIR := ./src
INCLUDE_DIR := ./include
BUILD_DIR := ./build
RM := /bin/rm -f

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
#SRCS := main.cpp 
INCLUDES := $(shell find $(INCLUDE_DIR) -name '*.hpp')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

NAME = PmergeMe

.PHONY: all clean fclean re

all: $(NAME)

$(OBJS): $(INCLUDES)

$(BUILD_DIR)/%.cpp.o: %.cpp $(INCLUDES)
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) -I$(INCLUDE_DIR) -o $@ -c $<

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

clean:
	$(RM) $(OBJS) 

fclean: clean
	$(RM) $(NAME)

re: fclean all
