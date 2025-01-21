#include "server.hpp"

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

	users.push_back(user);
	fds.push_back(new_poll);

	send(incofd, "Insert password\n", 17, MSG_DONTWAIT);	
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
/* 
void Server::do_command(User *user, std::string const &s) // da rivedere
{
	std::string cmd = s.substr(0, s.find(" "));
	std::string channel_name;

	if (s.find(" ") == std::string::npos)
	{
		printf("comando non valido\n");
		return ;
	}
	std::string tmp = s.substr(s.find(" ") + 1);
	if (cmd == "/join")
	{
		channel_name = tmp.substr(0, channel_name.find(" "));
		std::string password;
		if (channel_name.find(" ") == std::string::npos || channel_name.find(" ") + 1 == std::string::npos)
			password = "";
		else
			password = tmp.substr(tmp.find(" ") + 1);
		
		if (is_channel(channel_name))
			user->join_channel(find_channel(channel_name), password);
		else
		{
			user->create_channel(channel_name, password);
		}
	}
	else if (cmd == "/leave")
	{
		channel_name = tmp;

		if (is_channel(channel_name))
			user->leave_channel(channel_name);
		else
		{
			//errore canale non esistente
		}
	}
	else if (cmd == "/create")
	{
		channel_name = tmp.substr(0, channel_name.find(" "));
		std::string password;
		if (channel_name.find(" ") == std::string::npos || channel_name.find(" ") + 1 == std::string::npos)
			password = "";
		else
			password = tmp.substr(channel_name.find(" ") + 1);

		if (is_channel(channel_name))
		{
			//errore canale gia' esistente
			return ;
		}
		else
			user->create_channel(channel_name, password);
	}
	else if (cmd == "/delete")
	{
		channel_name = tmp;

		if (is_channel(channel_name))
			user->delete_channel(channel_name);
		else
		{
			//errore canale non esistente
		}
	}
	else if (cmd == "/kick")
	{
		channel_name = tmp.substr(0, channel_name.find(" "));
		if (channel_name.find(" ") == std::string::npos || channel_name.find(" ") + 1 == std::string::npos)
		{
			//errore comando non valido
			return ;
		}
		std::string nick = tmp.substr(tmp.find(" ") + 1);

		if (is_channel(channel_name))
			user->kick_user(convert_to_username(nick), channel_name);
		else
		{
			//errore canale non esistente
		}
	}
	else if (cmd == "/ban")
	{
		channel_name = tmp.substr(0, channel_name.find(" "));
		std::string nick = tmp.substr(tmp.find(" ") + 1);

		if (is_channel(channel_name))
			user->ban_user(convert_to_username(nick), channel_name);
		else
		{
			//errore canale non esistente
		}
	}
	else if (cmd == "/invite")
	{
		channel_name = tmp.substr(0, channel_name.find(" "));
		std::string nick = tmp.substr(tmp.find(" ") + 1);

		if (is_channel(channel_name))
			user->invite_user(convert_to_username(nick), channel_name);
		else
		{
			//errore canale non esistente
		}
	}
	else if (cmd == "/topic")
	{
		channel_name = tmp.substr(0, channel_name.find(" "));
		std::string topic = tmp.substr(tmp.find(" ") + 1);

		if (is_channel(channel_name))
			user->change_topic(topic, channel_name);
		else
		{
			//errore canale non esistente
		}
	}
	else if (cmd == "/mode")
	{
		channel_name = tmp.substr(0, channel_name.find(" "));
		std::string m = tmp.substr(tmp.find(" ") + 1);
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
		channel_name = tmp.substr(0, channel_name.find(" "));
		std::string password = tmp.substr(tmp.find(" ") + 1);

		if (is_channel(channel_name))
			user->change_password(password, channel_name);
		else
		{
			//errore canale non esistente
		}
	}
	else if (cmd == "/addadmin")
	{
		channel_name = tmp.substr(0, channel_name.find(" "));
		std::string nick = tmp.substr(tmp.find(" ") + 1);

		if (is_channel(channel_name))
			user->add_admin(convert_to_username(nick), channel_name);
		else
		{
			//errore canale non esistente
		}
	}
	else if (cmd == "/unban")
	{
		channel_name = tmp.substr(0, channel_name.find(" "));
		if (channel_name.find(" ") == std::string::npos || channel_name.find(" ") + 1 == std::string::npos)
		{
			//errore comando non valido
			return ;
		}
		std::string nick = tmp.substr(tmp.find(" ") + 1);

		if (is_channel(channel_name))
			user->unban_user(convert_to_username(nick), channel_name);
		else
		{
			//errore canale non esistente
		}
	}
	else if (cmd == "/leaveadmin")
	{
		channel_name = tmp;

		if (is_channel(channel_name))
			user->leave_admin(channel_name);
		else
		{
			//errore canale non esistente
		}
	}
	else if (cmd == "/acceptinvite")
	{
		channel_name = tmp;

		if (is_channel(channel_name))
			user->accept_invite(channel_name);
		else
		{
			//errore canale non esistente
		}
	}
	//  else if (s == "/remmessage")
	// {
	// 	//rem_message(); ?????
	// } 
} */

void Server::do_command(User *user, std::string const &s)
{
	std::string tmp = s.substr(0, s.find("#") + 1);
	std::string channel_name = tmp.substr(0, tmp.find(" "));
	std::string password = tmp.substr(tmp.find(" ") + 1);

	if (is_channel(channel_name))
		user->join_channel(find_channel(channel_name), password);
	else
		user->create_channel(channel_name, password);
}
