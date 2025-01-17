#include "standard_libraries.hpp"

void Server::clear_user(int fd)
{
	for (size_t i = 0; i < fds.size(); i++)
	{
		if (fds[i].fd == fd)
		{
			fds.erase(fds.begin() + i);
			break;
		}
	}
	for (size_t i = 0; i < users.size(); i++)
	{
		if (users[i]->get_user_fd() == fd)
		{
			users.erase(users.begin() + i);
			break;
		}
	}
}

bool Server::Signal = false;

void Server::signal_handler(int signum)
{
	(void)signum;
	Server::Signal = true;
}

void Server::close_fds()
{
	for (size_t i = 0; i < users.size(); i++)
	{
		std::cout << Red << "User <" << users[i]->get_user_name() << "> disconnected" << Reset << std::endl;
		close(users[i]->get_user_fd());
	}
	if (socket_fd != 1)
	{
		std::cout << Magenta << "close server" << Reset << std::endl;
		close(socket_fd);
	}
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
	fds.push_back(new_poll);

}

void Server::server_init()
{
	ser_socket();

	std::cout << "server connected" << std::endl;
	while(Server::Signal == false)
	{
		if ((poll(&fds[0],fds.size(), -1) == -1) && Server::Signal == false)
			throw(std::runtime_error("poll failed"));
		for (size_t i = 0; i < fds.size(); i++)
		{
			if (fds[i].revents & POLLIN)
			{
				if (fds[i].fd == socket_fd)
					accept_new_client();
				else
					receive_new_data(fds[i].fd);
			}
		}
	}
	close_fds();
}

void Server::take_str(std::string *dest, char *src)
{
	*dest = src;
	size_t i = dest->find("\n");
	if (i != std::string::npos)
		*dest = dest->substr(0, i);
	//std::cout << "a" <<std::endl;
	std::fill(src, src + strlen(src), '\0');
}

void Server::accept_new_client()
{
	std::string name, UsPassword, nick;
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

	users.push_back(user);
	fds.push_back(new_poll);

	write(incofd, "Insert password\n", 17);	
	std::cout  << Green << "client <" << incofd << "> is connect" << Reset << std::endl;
}

User *Server::find_user(int fd)
{
	for (std::vector<User *>::iterator i = users.begin(); i != users.end(); i++)
		if ((*i)->get_user_fd() == fd)
			return(*i);
	return (NULL);
}

void Server::print_all(int Usfd,const std::string &mess, const std::string &nick)
{
	for (std::vector<User *>::iterator i = users.begin(); i != users.end(); i++)
	{
		if ((*i)->get_user_fd() != Usfd)
		{
			write((*i)->get_user_fd(), nick.c_str(), nick.length());
			write((*i)->get_user_fd(), mess.c_str(), mess.length());
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
		clear_user(fd);
		close(fd);
	}
	std::string UsPassword, name, nick;
	switch (i->get_state())
	{
	case 0:
			take_str(&UsPassword, buff);
			if (UsPassword != password)
			{
				i->increment_tries();
				std::cout << i->get_tries() << password << std::endl;
				write(i->get_user_fd(), "Incorrect password\n", 20);
				if (i->get_tries() == 3)
				{
					write(i->get_user_fd(), "too many tries\n", 15);
					close(i->get_user_fd());
					clear_user(fd);
					return ;
				}
				write(i->get_user_fd(), "Insert password\n", 17);
				break ;
			}
			else
			{
				std::cout << "giusto" << std::endl;
				write(i->get_user_fd(), "select a name\n", 15);
				i->increment_state();
			}
		break;
	case 1:
		take_str(&name, buff);
		i->set_user_name(name);
		i->increment_state();
		write(i->get_user_fd(), "select a nickname\n", 19);
		break;
	case 2:
		take_str(&nick, buff);
		i->set_user_nick(nick);
		i->increment_state();
		break;
	case 3:
		buff[bytes] = '\0';
		std::cout << Yellow << "user <" << i->get_user_nick() << "> data:"  << Reset << buff;
		break;
	}
}
