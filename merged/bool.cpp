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


bool Server::is_user(const std::string &user) const
{
	for (std::vector<User*>::const_iterator it = users.begin(); it != users.end(); it++)
	{
		if ((*it)->get_user_name() == user)
			return (true);
	}
	return (false);
}

bool Server::is_channel(const std::string &channel) const
{
	for (std::vector<Channel*>::const_iterator it = channels.begin(); it != channels.end(); it++)
	{
		if ((*it)->get_name() == channel)
			return (true);
	}
	return (false);
}

bool Server::is_command(User *user, std::string const &s) const
{
	std::string cmd = s.substr(0, s.find(" "));
	if (cmd == "/join" || cmd == "/leave" || cmd == "/create" || cmd == "/delete" || cmd == "/kick" || cmd == "/ban" || cmd == "/invite" || cmd == "/topic" || cmd == "/mode" || cmd == "/password" || cmd == "/addadmin" || cmd == "/unban" || cmd == "/remmessage" || cmd == "/leaveadmin" || cmd == "/acceptinvite")
		return (true);
	return (false);
}

bool User::no_new_line()
{
	for (size_t i = 0; i < sizeof(buff); i++)
	{
		if (buff[i] == '\n')
			return (false);
	}
	return (true);
}