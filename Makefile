NAME = webserv

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic

SRCS := DarwinWorker.cpp KQueue.cpp ListeningSocket.cpp ListeningSocketsBlock.cpp RequestHandler.cpp main.cpp

OBJS := $(SRCS:%.cpp=%.o)

.PHONY: clean fclean re all

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I. -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
