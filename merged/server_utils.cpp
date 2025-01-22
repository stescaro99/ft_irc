#include "standard_libraries.hpp"

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
					add_user();
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

void Server::print_all(int Usfd,const std::string &mess, const std::string &nick)
{
	if (Usfd != fds[0].fd)  // stampa anche sul server: tenere???
	{
		std::cout << Yellow << "[" << Reset << nick;
		std::cout << Yellow << "] " << Reset << mess << std::endl;
	}
	for (std::vector<User *>::iterator i = users.begin(); i != users.end(); i++)
	{
		if ((*i)->get_user_fd() != Usfd)
		{
			// send((*i)->get_user_fd(), Yellow , 6, MSG_DONTWAIT);
			// send((*i)->get_user_fd(), "[", 1, MSG_DONTWAIT);
			// send((*i)->get_user_fd(), Reset, 4, MSG_DONTWAIT);
			send((*i)->get_user_fd(), nick.c_str(), nick.length(), MSG_DONTWAIT);
			// send((*i)->get_user_fd(), Yellow, 6, MSG_DONTWAIT);
			// send((*i)->get_user_fd(), "] ", 2, MSG_DONTWAIT);
			send((*i)->get_user_fd(), " ", 1, MSG_DONTWAIT);  //tmp
			// send((*i)->get_user_fd(), Reset, 4, MSG_DONTWAIT);
			send((*i)->get_user_fd(), mess.c_str(), mess.length(), MSG_DONTWAIT);
			// send((*i)->get_user_fd(), "\n", 1, MSG_DONTWAIT);
		}
	}
}

void Server::konversations(short i, std::string &s)
{
	std::string tmp;
	switch (i)
	{
		case 0:
		{
			tmp = s.substr(0, 6);
			if (tmp == "PASS :")
				s = s.substr(6);
			break ;
		}
		case 1:
		{
			tmp = s.substr(0, 5);
			if (tmp == "NICK ")
				s = s.substr(5);
			break ;
		}
		case 2:
		{
			tmp = s.substr(0, 5);
			if (tmp == "USER " && s.find(":") != std::string::npos)
				s = s.substr(s.find(":") + 1);
			break ;
		}
	}
}

void Server::receive_new_data(int fd)
{
	User *i = find_user(fd);
	i->memset_buff();
	ssize_t bytes = recv(fd, i->get_buff(), 1024, 0);
	if (bytes <= 0)
	{
		std::cout << Red << "user " << i->get_user_nick() << " disconnected"  << Reset << std::endl;
		rem_user(i->get_user_name());
		close(fd);
	}
	std::string s;

	switch (i->get_state())
	{
	case 0:
			take_str(&s, i->get_buff());
			konversations(i->get_state(), s);
			if (s != password)
			{
				i->increment_tries();
				send(i->get_user_fd(), "Incorrect password\n", 20, MSG_DONTWAIT);
				if (i->get_tries() == 3)
				{
					send(i->get_user_fd(), "too many tries\n", 15, MSG_DONTWAIT);
					close(i->get_user_fd());
					rem_user(i->get_user_name());
					return ;
				}
				send(i->get_user_fd(), "Insert password\n", 17, MSG_DONTWAIT);
				break ;
			}
			else
			{
				send(i->get_user_fd(), "select a nickname\n", 19, MSG_DONTWAIT);
				i->increment_state();
			}
		break;
	case 1:
		take_str(&s, i->get_buff());
		konversations(i->get_state(), s);
		if (is_nick(s))
		{
			send(i->get_user_fd(), "name already taken\nselect a nickname\n", 34, MSG_DONTWAIT);
			break;
		}
		i->set_user_nick(s);
		i->increment_state();
		send(i->get_user_fd(), "select a name\n", 15, MSG_DONTWAIT);
	case 2:
		take_str(&s, i->get_buff());
		konversations(i->get_state(), s);
		if (is_user(s))
		{
			send(i->get_user_fd(), "name already taken\nselect a name\n", 34, MSG_DONTWAIT);
			break;
		}
		i->set_user_name(s);
		i->increment_state();
		//send(i->get_user_fd(), "\033[2J\033[H", 8, MSG_DONTWAIT);
		break;
	case 3:
		take_str(&s, i->get_buff());
		std::vector<std::string> v;
		split(s, " ", v);
		short cmd = is_command(v[0]);
		if (cmd)
		{
			do_command(cmd, i, v);
			break;
		}
		print_all(fd, s, i->get_user_nick()); //add channel?
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

bool Server::is_nick(const std::string &nick) const
{
	for (std::vector<User*>::const_iterator it = users.begin(); it != users.end(); it++)
	{
		if ((*it)->get_user_nick() == nick)
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

std::string Server::convert_to_username(std::string const &nick) const
{
	for (std::vector<User*>::const_iterator it = users.begin(); it != users.end(); it++)
	{
		if ((*it)->get_user_nick() == nick)
			return ((*it)->get_user_name());
	}
	if (is_user(nick))
		return (nick);
	return ("");
}

void Server::split(std::string s, const std::string &delim, std::vector<std::string> &v)
{
	size_t start = 0;
	std::string tmp;

	while ((start = s.find(delim)) != std::string::npos)
	{
		if(s.find(":") == 0)
			break ;
		tmp = s.substr(0, start);
		if (tmp.size() > 0)
			v.push_back(tmp);
		s.erase(0, start + delim.length());
	}
	if (!s.empty())
		v.push_back(s);
}

short Server::is_command(const std::string &s) const
{
	if (s == "JOIN")
		return (1);
	return (0);
}