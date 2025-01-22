#include "standard_libraries.hpp"

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

std::string User::set_user_host(int fd) const
{
	if (fd == -1)
		return ("unknown");
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);
	if (getpeername(fd, (struct sockaddr*)&addr, &addr_len) == 0)
	{
		char host[NI_MAXHOST];
		if (getnameinfo((struct sockaddr*)&addr, addr_len, host, sizeof(host), NULL, 0, NI_NAMEREQD) == 0)
			return (host);
		else
			return (inet_ntoa(addr.sin_addr));
	}
	return ("unknown");
}