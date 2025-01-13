#include "server.hpp"

void Server::add_user(std::string user, std::string nick, std::string password)
{
	if (!is_user(user))
	{
		User new_user(user, nick, password);
		users.push_back(&new_user);
	}
}

void Server::rem_user(std::string user)
{
	if (is_user(user))
	{
		for (std::vector<User*>::iterator it = users.begin(); it != users.end(); it++)
		{
			if ((*it)->get_user_name() == user)
			{
				users.erase(it);
				break ;
			}
		}
	}
}

void Server::add_channel(Channel *ch)
{
	if (!is_channel((*ch).get_name()))
	{
		channels.push_back(ch);
	}
}

void Server::rem_channel(std::string channel)
{
	if (is_channel(channel))
	{
		for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); it++)
		{
			if ((*it)->get_name() == channel)
			{
				channels.erase(it);
				break ;
			}
		}
	}
}

bool Server::is_user(std::string user) const
{
	for (std::vector<User*>::const_iterator it = users.begin(); it != users.end(); it++)
	{
		if ((*it)->get_user_name() == user)
			return (true);
	}
	return (false);
}

bool Server::is_channel(std::string channel) const
{
	for (std::vector<Channel*>::const_iterator it = channels.begin(); it != channels.end(); it++)
	{
		if ((*it)->get_name() == channel)
			return (true);
	}
	return (false);
}