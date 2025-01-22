
file = channel_utils.cpp channel.cpp const_and_destr.cpp getter.cpp main.cpp messages.cpp server_utils.cpp server.cpp setter.cpp user_utils.cpp user.cpp
NAME =  irc

cc = c++
flag = -Wall -Wextra -Werror -std=c++98 -g
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

.PHONY: all clean fclean re