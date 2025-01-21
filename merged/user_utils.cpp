#include "standard_libraries.hpp"

void User::memset_buff()
{
	memset(buff, 0, 1024);
	if (state > 2)
	{
		// send(user_fd, Yellow, 5, MSG_DONTWAIT);
		// send(user_fd, "[", 1, MSG_DONTWAIT);
		// send(user_fd, Reset, 4, MSG_DONTWAIT);
		send(user_fd, user_nickname.c_str(), user_nickname.length(), MSG_DONTWAIT);
		send(user_fd, " ", 1, MSG_DONTWAIT);
		// send(user_fd, Yellow, 5, MSG_DONTWAIT);
		// send(user_fd, "] ", 2, MSG_DONTWAIT);
		// send(user_fd, Reset, 4, MSG_DONTWAIT);
	}
}
