#include "server.hpp"

void Server::add_user(const std::string &user, const std::string &nick, int ufd)
{
	if (!is_user(user))
	{
		User *new_user = new User(user, nick, *this, ufd);
		users.push_back(new_user);
	}
}

void Server::rem_user(const std::string &user)
{
	if (is_user(user))
	{
		for (std::vector<User*>::iterator it = users.begin(); it != users.end(); it++)
		{
			if ((*it)->get_user_name() == user)
			{
				users.erase(it);
				delete *it;
				break ;
			}
		}
	}
}

void Server::add_channel(Channel *ch)
{
	if (!is_channel(ch->get_name()))
	{
		channels.push_back(ch);
	}
}

void Server::rem_channel(const std::string &channel)
{
	if (is_channel(channel))
	{
		for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); it++)
		{
			if ((*it)->get_name() == channel)
			{
				delete *it;
				channels.erase(it);
				break ;
			}
		}
	}
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

/* void Server::print_message(int fd, std::string nick, const std::string &message)
{
	for (std::vector<User*>::iterator it = users.begin(); it != users.end(); it++)
	{
		if ((*it)->get_user_fd() != fd)
		{
			//write((*it)->get_user_fd(), "\033[2K", 4); ??? guardare se si riesce a salvare la parte iniziale della riga
			write((*it)->get_user_fd(), "\033[1m", 5);
			write((*it)->get_user_fd(), nick.c_str(), nick.size());
			write((*it)->get_user_fd(), ": ", 2);
			write((*it)->get_user_fd(), "\033[0m", 5);
			write((*it)->get_user_fd(), message.c_str(), message.size());
		}
	}
} */