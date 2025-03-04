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
		if (users[i])
			close(users[i]->get_user_fd());
	}
	for (size_t i = 0; i < bots.size(); i++)
	{
		if (bots[i])
			close(bots[i]->get_user_fd());
	}
	if (socket_fd != 1)
		close(socket_fd);
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
	std::fill(src, src + strlen(src), '\0');
}

void Server::konversations(short i, std::string &s)
{
	if (s.size() < 5 + (i == 0))
		return ;
	std::string tmp = s.substr(0, 5 + (i == 0));
	switch (i)
	{
		case 0:
		{
			if (tmp == "PASS :")
				s = s.substr(6);
			else if (tmp == "CAP LS" && password.substr(0, 6) != "CAP LS")
				s = "\127";
			break ;
		}
		case 1:
		{
			if (tmp == "NICK ")
				s = s.substr(5);
			break ;
		}
		case 2:
		{
			if (tmp == "USER " && s.find(":") != std::string::npos)
				s = s.substr(s.find(":") + 1);
			break ;
		}
	}
}

void Server::receive_new_data(int fd)
{
	User *i = find_user(fd);
	if (!i)
		return ;
	i->memset_buff();
	ssize_t bytes = recv(fd, i->get_buff(), 1024, 0);
	if (bytes <= 0)
	{
		std::cout << Red << "user " << i->get_user_nick() << " disconnected" << Reset << std::endl;
		quit(i);
		return ;
	}
	std::string s;
	switch (i->get_state())
	{
		case 0:
			take_str(&s, i->get_buff());
			konversations(i->get_state(), s);
			if (s.empty())
				break;
			if (s != password && s != "\127")
			{
				i->increment_tries();
				send(i->get_user_fd(), "IRCSERV Incorrect password\r\n", 28, 0);
				if (i->get_tries() == 3)
				{
					send(i->get_user_fd(), "IRCSERV too many tries\r\n", 24, 0);
					rem_user(i->get_user_name());
					return ;
				}
				send(i->get_user_fd(), "IRCSERV Insert password\r\n", 25, 0);
				break ;
			}
			else if (s == "\127")
				break ;
			else
			{
				send(i->get_user_fd(), "IRCSERV select a nickname\r\n", 27, 0);
				i->increment_state();
			}
			break;
		case 1:
			take_str(&s, i->get_buff());
			konversations(i->get_state(), s);
			if (is_nick(s) || s.substr(0, 3) == BOT_NAME)
			{
				send(i->get_user_fd(), "IRCSERV name already taken\nIRCSERV select a nickname\r\n", 54, 0);
				break;
			}
			if (s[0] == '#' || s[0] == '&' || s.find(" ") != std::string::npos)
			{
				send(i->get_user_fd(), "IRCSERV invalid nickname\nIRCSERV select a nickname\r\n", 52, 0);
				break;
			}
			i->set_user_nick(s);
			i->increment_state();
			send(i->get_user_fd(), "IRCSERV select a name\r\n", 23, 0);
			break;
		case 2:
			take_str(&s, i->get_buff());
			konversations(i->get_state(), s);
			if (is_user(s) || s.substr(0, 3) == BOT_NAME || s.find(" ") != std::string::npos)
			{
				send(i->get_user_fd(), "IRCSERV name already taken\nIRCSERV select a name\r\n", 50, 0);
				break;
			}
			i->set_user_name(s);
			i->increment_state();
			s.clear();
			s = "INFO: " + i->get_user_nick() + "!" + i->get_user_name() + "@" + i->set_user_host(i->get_user_fd()) + " Welcome in the server\r\n";
			std::cout << Green << "user " << i->get_user_nick() << " connected" << Reset << std::endl;
			send(i->get_user_fd(), s.c_str(), s.size(), 0);
			break;
		case 3:
			take_str(&s, i->get_buff());
			if (s.empty())
				break;
			std::vector<std::string> v;
			split(s, " ", v);
			short cmd = is_command(v[0]);
			if (cmd)
			{
				std::cout << Yellow << "<" << i->get_user_nick() << "> " << Reset << s << std::endl;
				do_command(cmd, i, v);
				break;
			}
			print_all(fd, s, i->get_user_nick());
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

bool Server::is_bot(const std::string &nick) const
{
	for (std::vector<Bot*>::const_iterator it = bots.begin(); it != bots.end(); it++)
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
		else
			v.push_back("");
		s.erase(0, start + delim.length());
	}
	if (!s.empty())
		v.push_back(s);
}

std::vector<std::string> Server::split_mode(const std::string &s)
{
	std::vector<std::string> v;
	size_t start = 0;
	std::string tmp;

	if (s[0] != '+' && s[0] != '-')
	{
		v.clear();
		return (v);
	}
	while (start < s.size())
	{
		size_t end = start + 1;
		while (end < s.size() && s[end] != '+' && s[end] != '-')
			end++;
		if (end - start < 2)
		{
			v.clear();
			return (v);
		}
		tmp = s.substr(start, end - start);
		start = end;
		v.push_back(tmp);
	}
	return (v);
}

short Server::is_command(const std::string &s) const
{
	std::string cmd = "";
	for (size_t i = 0; i < s.size(); i++)
	{
		if (s[i] >= 'a' && s[i] <= 'z')
			cmd += (s[i] - 32);
		else
			cmd += s[i];
	}
	if (cmd == "JOIN")
		return (1);
	else if (cmd == "PART")
		return (2);
	else if (cmd == "MODE")
		return (3);
	else if (cmd == "TOPIC")
		return (4);
	else if (cmd == "KICK")
		return (5);
	else if (cmd == "INVITE")
		return (6);
	else if (cmd == "PRIVMSG")
		return (7);
	else if (cmd == "QUIT")
		return (8);
	return (0);
}

bool Server::parameters_check(std::vector<std::string> const &flags, std::vector<std::string> const &admin, short limit, std::string password, User *user, Channel *ch)
{
	if (flags.size() == 0)
		return (false);
	short operator_flag = 0;
	short password_flag = 0;
	short limit_flag = 0;

	for (size_t i = 0; i < flags.size(); i++)
	{
		bool plus = (flags[i][0] == '+');
		for (size_t j = 1; j < flags[i].size(); j++)
		{
			char m = flags[i][j];
			if (m == 'o')
				operator_flag++;
			else if (m == 'k' && plus)
				password_flag = 1;
			else if (m == 'l' && plus)
				limit_flag = 1;
		}
	}
	bool exit = false;
	if ((limit != 0) + (password != "") + admin.size() != (size_t)(operator_flag + password_flag + limit_flag))
	{
		std::string error_msg = ":IRCSERV 461 " + user->get_user_nick() + " MODE :Not enough parameters\r\n";
		send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
		exit = true;
	}
	if (limit_flag && (size_t)limit < ch->get_users_count())
	{
		std::string error_msg = ":IRCSERV 472 " + user->get_user_nick() + " MODE :Channel limit exceeded\r\n";
		send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
		exit = true;
	}
	if (password_flag && password == "")
	{
		std::string error_msg = ":IRCSERV 472 " + user->get_user_nick() + " MODE :Password required\r\n";
		send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
		exit = true;
	}
	if (operator_flag && admin.size() == 0)
	{
		std::string error_msg = ":IRCSERV 502 " + user->get_user_nick() + " MODE :Missing new operator name\r\n";
		send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
		exit = true;
	}
	return (exit);
}

std::string Server::get_channels_list() const
{
	std::string ret = "";
	for (std::vector<Channel*>::const_iterator it = channels.begin(); it != channels.end(); it++)
		ret += (*it)->get_name() + " - ";
	ret = ret.substr(0, ret.size() - 3);
	return (ret);
}

std::string Server::get_users_list() const
{
	std::string ret = "";
	for (std::vector<User*>::const_iterator it = users.begin(); it != users.end(); it++)
		ret += (*it)->get_user_nick() + " - ";
	ret = ret.substr(0, ret.size() - 3);
	return (ret);
}

bool Server::file_check(const std::string &file, size_t size)
{
	std::string path = env[0].substr(4);
	if (env[1].substr(0, 4) == "PWD=")
		path = env[1].substr(4);
	if (file[0] == '/' && path[path.size() - 1] == '/')
		path += file.substr(1);
	else if (file[0] == '/' || path[path.size() - 1] == '/')
		path += file;
	else
		path += "/" + file;
	int fd = open(path.c_str(), O_RDONLY);
	if (fd == -1)
		return (false);
	struct stat st;
	fstat(fd, &st);
	if (st.st_size != (off_t)size)
	{
		close(fd);
		return (false);
	}
	close(fd);
	return (true);
}