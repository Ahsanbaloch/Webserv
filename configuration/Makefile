# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ahsalam <ahsalam@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/12/24 17:37:52 by ahsalam           #+#    #+#              #
#    Updated: 2024/03/14 13:07:30 by ahsalam          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

Name	=	webserv

CXX		=	c++

CXXFLAGS	=	 -std=c++98

#-Wall -Wextra -Werror

SRC = $(wildcard *.cpp)

OBJDIR = ./obj/

OBJ = $(addprefix $(OBJDIR), $(SRC:.cpp=.o))


all: $(Name)

$(Name): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(Name) $(OBJ)

$(OBJDIR)%.o: %.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(Name)

re: fclean all

.PHONY: all clean fclean re


