#include "server.hpp"

void Server::add_user()
{
	struct sockaddr_in	usadd;
	struct pollfd		new_poll;
	socklen_t			len = sizeof(usadd);
	int incofd = accept(socket_fd, (sockaddr *)&(usadd), &len);
	User *user = new User(*this, incofd, inet_ntoa(usadd.sin_addr));
	if (incofd == -1)
	{
		delete user;
		return;
	}
	std::string welcome = server_name + " INFO :Hello, welcome to the server\r\n";
	if (send(incofd, welcome.c_str(), welcome.length(), 0) != 45)
			throw(std::runtime_error("failed to send welcome message"));
	new_poll.fd = incofd;
	new_poll.events = POLLIN;
	new_poll.revents = 0;
	if (fcntl(incofd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cout << "user fcntl failed" << std::endl;
		return;
	}
	users.push_back(user);
	fds.push_back(new_poll);
	send(user->get_user_fd(), "IRCSERV Insert password\r\n", 26, 0);
	std::cout  << Cyan << "client <" << incofd << "> connected to ip: " << user->get_priv_ip() << Reset << std::endl;
}

void Server::rem_user(const std::string &user)
{
	if (is_user(user))
	{
		for (std::vector<User*>::iterator it = users.begin(); it != users.end(); it++)
		{
			if ((*it)->get_user_name() == user)
			{
				int fd = (*it)->get_user_fd();
				(*it)->leave_channel("");
				for (std::vector<struct pollfd>::iterator i = fds.begin(); i != fds.end(); i++)
				{
					if (i->fd == fd)
					{
						close(fd);
						fds.erase(i);
						break ;
					}
				}
				delete *it;
				users.erase(it);
				break ;
			}
		}
	}
	else if (is_bot(user))
	{
		for (std::vector<Bot*>::iterator it = bots.begin(); it != bots.end(); it++)
		{
			if ((*it)->get_user_name() == user)
			{
				int fd = (*it)->get_user_fd();
				for (std::vector<struct pollfd>::iterator i = fds.begin(); i != fds.end(); i++)
				{
					if (i->fd == fd)
					{
						close(fd);
						fds.erase(i);
						break ;
					}
				}
				fd = (*it)->get_bot_fd();
				close(fd);
				delete *it;
				bots.erase(it);
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
				if ((*it)->get_bot_name() != "")
				{
					Bot *bot = find_bot((*it)->get_bot_name());
					if ((*it)->get_bot())
						(*it)->bot_kick(bot);
					rem_user(bot->get_user_name());
				}
				delete *it;
				channels.erase(it);
				break ;
			}
		}
	}
}
