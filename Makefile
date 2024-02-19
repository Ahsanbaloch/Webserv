NAME = webserv

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic -fsanitize=address -g

SRCS_DIR = ./srcs/

SRCS := $(addprefix $(SRCS_DIR),DarwinWorker.cpp KQueue.cpp ListeningSocket.cpp ListeningSocketsBlock.cpp RequestHandler.cpp main.cpp)

OBJS_DIR = ./objs/

OBJS := $(SRCS:$(SRCS_DIR)%.cpp=$(OBJS_DIR)%.o)

.PHONY: clean fclean re all

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

$(OBJS_DIR)%.o: $(SRCS_DIR)%.cpp
	mkdir -p $(OBJS_DIR)
	$(CXX) $(CXXFLAGS) -I. -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
