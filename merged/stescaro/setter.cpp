#include "standard_libraries.hpp"

void User::set_user_fd(int fd)
{
	user_fd = fd;
}

void User::set_user_name(const std::string &name)
{
	user_name = name;
}

void User::set_user_nick(const std::string &nick)
{
	user_nickname = nick;
}

void User::set_user_IPadd(const std::string &ipadd)
{
	IPadd = ipadd;
}

void User::set_user_state(short st)
{
	state = st;
}

void User::increment_tries()
{
	pass_tries++;
}

void User::increment_state()
{
	state++;
}