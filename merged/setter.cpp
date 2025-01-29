#include "standard_libraries.hpp"

void User::set_user_name(const std::string &name)
{
	user_name = name;
}

void User::set_user_nick(const std::string &nick)
{
	user_nickname = nick;
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

void Server::set_mode_utility(std::vector<std::string> const &v, std::vector<std::string> &users, short &limit, std::string &password, size_t &n, const std::string &flags)
{
	bool plus = (flags[0] == '+');

	n++;
	for (size_t i = 1; i < flags.length(); i++)
	{
		if (flags[i] == 'l' && plus)
		{
			if (n >= v.size())
			{
				limit = 0;
				return ;
			}
			std::stringstream ss(v[n]);
			ss >> limit;
			if (ss.fail())
			{
				limit = 0;
				return ;
			}
			n++;
		}
		else if (flags[i] == 'k' && plus)
		{
			if (n >= v.size())
			{
				password = "";
				return ;
			}
			password = v[n];
			n++;
		}
		else if (flags[i] == 'o')
		{
			if (n >= v.size())
			{
				users.clear();
				return ;
			}
			std::vector<std::string> tmp;
			split(v[n], ",", tmp);
			for (size_t j = 0; j < tmp.size(); j++)
				users.push_back(convert_to_username(tmp[j]));
			n++;
		}
	}
}

void Channel::set_topic_admin(bool only_admin)
{
	topic_only_admin = only_admin;
}