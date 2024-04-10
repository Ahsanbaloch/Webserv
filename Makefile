NAME = webserv

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic -fsanitize=address -g

SRCS_DIR = ./srcs/

SRCS := $(addprefix $(SRCS_DIR),ListeningSocket.cpp ListeningSocketsBlock.cpp RequestHandler.cpp RequestHeader.cpp AResponse.cpp GETResponse.cpp DELETEResponse.cpp ERRORResponse.cpp ConnectionHandler.cpp RequestBody.cpp MULTIPARTBody.cpp PLAINBody.cpp URLENCODEDBody.cpp main.cpp config/config_pars_utils.cpp config/config_pars.cpp config/exception_parsing.cpp)

ifeq ($(shell uname -s), Darwin)
    SRCS += $(addprefix $(SRCS_DIR),DarwinWorker.cpp KQueue.cpp)
endif

ifeq ($(shell uname -s), Linux)
    SRCS += $(addprefix $(SRCS_DIR),LinuxWorker.cpp EPoll.cpp)
endif

OBJS_DIR = ./objs/

OBJS := $(SRCS:$(SRCS_DIR)%.cpp=$(OBJS_DIR)%.o)

.PHONY: clean fclean re all

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

$(OBJS_DIR)%.o: $(SRCS_DIR)%.cpp
	mkdir -p $(OBJS_DIR)
	mkdir -p $(OBJS_DIR)config/
	$(CXX) $(CXXFLAGS) -I./includes -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
