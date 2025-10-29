NAME = ircserv

CC = c++

CFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS =	sources/main.cpp \
		sources/Server.cpp \
		sources/ft_irc.cpp \
		sources/Client.cpp \
		sources/Message.cpp \
		sources/Channel.cpp \
		commands/pass.cpp \
		commands/user.cpp \
		commands/nick.cpp \
		commands/quit.cpp \
		commands/who.cpp \
		commands/mode.cpp \
		commands/topic.cpp \
		commands/invite.cpp \
		commands/kick.cpp \
		commands/part.cpp \
		commands/privmsg.cpp \
		commands/join.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
		$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

%.o: %.cpp
		$(CC) $(CFLAGS) -c $< -o $@

clean:
		rm -rf $(OBJS)

fclean: clean
		rm -rf $(NAME)

re: fclean all

.PHONY: all, clean, fclean, re
