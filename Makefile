
file = srcs/channel_utils.cpp srcs/channel.cpp srcs/const_and_destr.cpp srcs/getter.cpp srcs/main.cpp srcs/messages.cpp srcs/server_utils.cpp srcs/server.cpp srcs/setter.cpp srcs/user.cpp srcs/do_command.cpp srcs/bot.cpp srcs/bot_commands.cpp
NAME = ircserv

cc = c++
flag = -Wall -Wextra -Werror -std=c++98 -g -I./includes
fileobj = $(file:.cpp=.o)
all : $(NAME)

.cpp.o:
	$(cc) $(flag) -c $< -o $@
$(NAME) : $(fileobj)
			$(cc) $(flag) $(fileobj) -o $(NAME)

clean:
			rm -f $(fileobj)
fclean: clean
			rm -f $(NAME)
re: fclean all

fast: re
	rm -f $(fileobj)

.PHONY: all clean fclean re
