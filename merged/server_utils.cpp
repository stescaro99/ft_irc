#include "standard_libraries.hpp"

bool Server::signal = false;

void Server::signal_handler(int signum)
{
	(void)signum;
	Server::signal = true;
}

void Server::ser_socket()
{
	struct	sockaddr_in	add;
	struct	pollfd		new_poll;
	add.sin_family = AF_INET;
	add.sin_port = htons(this->port);
	add.sin_addr.s_addr = INADDR_ANY;

	int en = 1;
	if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
		throw(std::runtime_error("failed to set on socket"));
	if(fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1)
		throw(std::runtime_error("failed to set NONBLOCK on socket"));
	if (bind(socket_fd, (struct sockaddr *)&add, sizeof(add)) == -1)
		throw(std::runtime_error("failed to bind socket"));
	if(listen(socket_fd, SOMAXCONN) == -1)
		std::runtime_error("lissen failed");
	
	new_poll.fd = socket_fd;
	new_poll.events = POLLIN;
	new_poll.revents = 0;
	server_poll = new_poll;
}

void Server::server_init()
{
	ser_socket();

	std::cout << "server connected" << std::endl;
	while(Server::signal == false)
	{
		if ((poll(&server_poll, users.size() + 1, -1) == -1) && Server::signal == false)
			throw(std::runtime_error("poll failed"));
		for (size_t i = 0; i < users.size() + 1; i++)
		{
			if (i == 0)
			{
				if (server_poll.revents & POLLIN)
					add_user();
			}
			else
			{
				int j = 0;
				for (std::map<struct pollfd, User *>::iterator it = users.begin(); it != users.end(); it++)
				{
					if (i == ++j && it->first.revents & POLLIN)
					{
						receive_new_data(it->first.fd);
						break;
					}
				}
			}
		}
	}
	close_server();
}

void Server::take_str(std::string *dest, char *src)
{
	*dest = src;
	size_t i = dest->find("\n");
	if (i != std::string::npos)
		*dest = dest->substr(0, i);
	std::fill(src, src + strlen(src), '\0');
}

User *Server::find_user(int fd)
{
	for (std::map<struct pollfd, User *>::iterator i = users.begin(); i != users.end(); i++)
		if (i->second->get_user_fd() == fd)
			return (i->second);
	return (NULL);
}

void Server::print_all(int Usfd,const std::string &mess, const std::string &nick)
{
	for (std::map<struct pollfd, User *>::iterator i = users.begin(); i != users.end(); i++)
	{
		if (i->second->get_user_fd() != Usfd)
		{
			write(i->second->get_user_fd(), nick.c_str(), nick.length());
			write(i->second->get_user_fd(), mess.c_str(), mess.length());
		}
	}
}

void Server::receive_new_data(int fd)
{
	char buff[1024];
	memset(buff, 0, sizeof(buff));
	ssize_t bytes = recv(fd, buff, sizeof(buff) - 1, 0);

	User *i = find_user(fd);
	if (bytes <= 0)
	{
		std::cout << Red << "user " << i->get_user_nick() << " disconnected"  << Reset << std::endl;
		rem_user(i->get_user_name());
		close(fd);
	}
	std::string s;
	take_str(&s, buff);
	switch (i->get_state())
	{
	case 0:
			if (s != password)
			{
				i->increment_tries();
				std::cout << i->get_tries() << password << std::endl;
				write(i->get_user_fd(), "Incorrect password\n", 20);
				if (i->get_tries() == 3)
				{
					write(i->get_user_fd(), "too many tries\n", 15);
					close(i->get_user_fd());
					rem_user(i->get_user_name());
					return ;
				}
				write(i->get_user_fd(), "Insert password\n", 17);
				break ;
			}
			else
			{
				write(i->get_user_fd(), "select a name\n", 15);
				i->increment_state();
			}
		break;
	case 1:
		if (is_user(s))
		{
			write(i->get_user_fd(), "name already taken\nselect a name\n", 34);
			break;
		}
		i->set_user_name(s);
		i->increment_state();
		write(i->get_user_fd(), "select a nickname\n", 19);
		break;
	case 2:
		i->set_user_nick(s);
		i->increment_state();
		break;
	case 3:
		buff[bytes] = '\0';
		std::cout << Yellow << "user <" << i->get_user_nick() << "> data:"  << Reset << buff;
		break;
	}
}
