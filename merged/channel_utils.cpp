#include "standard_libraries.hpp"

bool Channel::is_user_invited(const std::string &user) const
{
	for (std::vector<std::string>::const_iterator it = ch_invited.begin(); it != ch_invited.end(); it++)
	{
		if (*it == user)
			return (true);
	}
	return (false);
}

bool Channel::is_user_admin(const std::string &user) const
{
	for (size_t i = 0; i < ch_admin.size(); i++)
	{
		if (ch_admin[i] == user)
			return (true);
	}
	return (false);
}

bool Channel::is_user_inside(const std::string &user) const
{
	for (std::map<std::string, User*>::const_iterator it = ch_users.begin(); it != ch_users.end(); it++)
	{
		if (it->first == user)
			return (true);
	}
	return (false);
}
