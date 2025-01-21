#include "standard_libraries.hpp"

void User::memset_buff()
{
	memset(buff, 0, 1024);
	if (state > 2)
	{
		write(user_fd, Yellow, 5);
		write(user_fd, "[", 1);
		write(user_fd, Reset, 4);
		write(user_fd, user_nickname.c_str(), user_nickname.length());
		write(user_fd, Yellow, 5);
		write(user_fd, "] ", 2);
		write(user_fd, Reset, 4);
	}
}
