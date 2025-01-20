#include "server.hpp"

void Server::add_user()
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

void Server::rem_user(const std::string &user)
{
	if (is_user(user))
	{
		for (std::vector<User*>::iterator it = users.begin(); it != users.end(); it++)
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
				delete *it;
				users.erase(it);
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

void Server::do_command(User *user, std::string const &s)
{
	std::string cmd = s.substr(0, s.find(" "));
	printf("cmd: %s\n", cmd.c_str());
	std::string tmp = s.substr(s.find(" ") + 1);
	if (cmd == "/join")
	{
		std::string channel_name = tmp.substr(0, channel_name.find(" "));
		std::string password = tmp.substr(channel_name.size() + 1);
		Channel *ch = find_channel(channel_name);
		if (ch)
			user->join_channel(ch, password);
		else
		{
			//errore canale non esistente
		}
	}
	else if (cmd == "/leave")
	{
		std::string channel_name = tmp;
		Channel *ch = find_channel(channel_name);
		if (ch)
			user->leave_channel(channel_name);
		else
		{
			//errore canale non esistente
		}
	}
	else if (cmd == "/create")
	{
		std::string channel_name = tmp.substr(0, channel_name.find(" "));
		std::string password = tmp.substr(channel_name.size() + 1);

		if (is_channel(channel_name))
		{
			//errore canale gia' esistente
		}
		else
			user->create_channel(channel_name, password);
	}
	else if (cmd == "/delete")
	{
		std::string channel_name = tmp;

		if (is_channel(channel_name))
			user->delete_channel(channel_name);
		else
		{
			//errore canale non esistente
		}
	}
	else if (cmd == "/kick")
	{
		std::string channel_name = tmp.substr(0, channel_name.find(" "));
		std::string user_name = tmp.substr(channel_name.size() + 1);

		if (is_channel(channel_name))
			user->kick_user(user_name, channel_name);
		else
		{
			//errore canale non esistente
		}
	}
	else if (cmd == "/ban")
	{
		std::string channel_name = tmp.substr(0, channel_name.find(" "));
		std::string user_name = tmp.substr(channel_name.size() + 1);

		if (is_channel(channel_name))
			user->ban_user(user_name, channel_name);
		else
		{
			//errore canale non esistente
		}
	}
	else if (cmd == "/invite")
	{
		std::string channel_name = tmp.substr(0, channel_name.find(" "));
		std::string user_name = tmp.substr(channel_name.size() + 1);

		if (is_channel(channel_name))
			user->invite_user(user_name, channel_name);
		else
		{
			//errore canale non esistente
		}
	}
	else if (cmd == "/topic")
	{
		std::string channel_name = tmp.substr(0, channel_name.find(" "));
		std::string topic = tmp.substr(channel_name.size() + 1);

		if (is_channel(channel_name))
			user->change_topic(topic, channel_name);
		else
		{
			//errore canale non esistente
		}
	}
	else if (cmd == "/mode")
	{
		std::string channel_name = tmp.substr(0, channel_name.find(" "));
		std::string m = tmp.substr(channel_name.size() + 1);
		if (m.size() != 1)
		{
			//errore comando non valido
		}
		else
		{
			if (is_channel(channel_name))
				user->change_mode(m[0], channel_name);
			else
			{
				//errore canale non esistente
			}
		}
	}
	else if (cmd == "/password")
	{
		std::string channel_name = tmp.substr(0, channel_name.find(" "));
		std::string password = tmp.substr(channel_name.size() + 1);

		if (is_channel(channel_name))
			user->change_password(password, channel_name);
		else
		{
			//errore canale non esistente
		}
	}
	else if (cmd == "/addadmin")
	{
		std::string channel_name = tmp.substr(0, channel_name.find(" "));
		std::string user_name = tmp.substr(channel_name.size() + 1);

		if (is_channel(channel_name))
			user->add_admin(user_name, channel_name);
		else
		{
			//errore canale non esistente
		}
	}
	else if (cmd == "/unban")
	{
		std::string channel_name = tmp.substr(0, channel_name.find(" "));
		std::string user_name = tmp.substr(channel_name.size() + 1);

		if (is_channel(channel_name))
			user->unban_user(user_name, channel_name);
		else
		{
			//errore canale non esistente
		}
	}
	else if (cmd == "/leaveadmin")
	{
		std::string channel_name = tmp;

		if (is_channel(channel_name))
			user->leave_admin(channel_name);
		else
		{
			//errore canale non esistente
		}
	}
	else if (cmd == "/acceptinvite")
	{
		std::string channel_name = tmp;

		if (is_channel(channel_name))
			user->accept_invite(channel_name);
		else
		{
			//errore canale non esistente
		}
	}
	/* else if (s == "/remmessage")
	{
		//rem_message(); ?????
	} */
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