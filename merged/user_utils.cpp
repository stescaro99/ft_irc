#include "standard_libraries.hpp"

void User::memset_buff()
{
	memset(buff, 0, 1024);
	if (state > 2)
	{
		// send(user_fd, Yellow, 5, 0);
		// send(user_fd, "[", 1, 0);
		// send(user_fd, Reset, 4, 0);
		//send(user_fd, user_nickname.c_str(), user_nickname.length(), 0);
		//send(user_fd, " ", 1, 0);
		// send(user_fd, Yellow, 5, 0);
		// send(user_fd, "] ", 2, 0);
		// send(user_fd, Reset, 4, 0);
	}
}
