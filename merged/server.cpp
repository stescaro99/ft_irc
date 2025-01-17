#include "standard_libraries.hpp"

void Server::add_user()
{
	struct sockaddr_in	usadd;
	struct pollfd		new_poll;
	socklen_t			len = sizeof(usadd);
	User *user = new User(*this, accept(socket_fd, (sockaddr *)&(usadd), &len));

	int incofd = user->get_user_fd();
	if (incofd == -1)
	{
		std::cout << "accept failed" << std::endl;
		delete user;
		return;
	}

	new_poll.fd = incofd;
	new_poll.events = POLLIN;
	new_poll.revents = 0;

	user->set_user_IPadd(inet_ntoa((usadd.sin_addr)));
	
	if (fcntl(incofd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cout << "user fcntl failed" << std::endl;
		return;
	}

	users[new_poll] = user;

	write(incofd, "Insert password\n", 17);	
	std::cout  << Green << "client <" << incofd << "> is connect" << Reset << std::endl;
}

void Server::rem_user(const std::string &user)
{
	if (is_user(user))
	{
		for (std::map<struct pollfd, User*>::iterator it = users.begin(); it != users.end(); it++)
		{
			if (it->second->get_user_name() == user)
			{
				users.erase(it);
				delete it->second;
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
	for (std::map<struct pollfd, User*>::const_iterator it = users.begin(); it != users.end(); it++)
	{
		if (it->second->get_user_name() == user)
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

void Server::close_server()
{
	for (std::map<struct pollfd, User *>::iterator i = users.begin(); i != users.end(); i++)
		rem_user(i->second->get_user_nick());
	if (socket_fd != 1)
	{
		std::cout << Magenta << "close server" << Reset << std::endl;
		close(socket_fd);
	}
}