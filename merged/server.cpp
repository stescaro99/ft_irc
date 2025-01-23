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
	std::string welcome = server_name + " INFO :Hello, welcome to the server\r\n";
	if (send(incofd, welcome.c_str(), welcome.length(), 0) != 47)
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

void Server::do_command(short cmd, User *user, std::vector<std::string> const &v)
{
	std::vector<std::string> channels;
	std::vector<std::string> passwords;
	Channel *ch;

	switch (cmd)
	{
		case 1:
		{
			if (v.size() < 2 || v.size() > 3)
			{
				// errore comando non valido
				return;
			}
			split(v[1], ",", channels);
			if (v.size() > 2)
				split(v[2], ",", passwords);
			while (channels.size() != passwords.size())
				passwords.push_back("");
			for (size_t i = 0; i < channels.size(); i++)
			{
				if (channels[i][0] != '#' && channels[i][0] != '&')
				{
					// errore canale non valido
					continue;
				}
				if (is_channel(channels[i]) && find_channel(channels[i])->is_user_inside(user->get_user_name()))
					continue;
				if (is_channel(channels[i]))
					user->join_channel(find_channel(channels[i]), passwords[i]);
				else
					user->create_channel(channels[i], passwords[i]);
				ch = find_channel(channels[i]);
				if (ch && ch->is_user_inside(user->get_user_name()))
					send_join_message(ch, user);
			}
			break;
		}
		case 2:
		{
			if (v.size() < 2)
			{
				// errore comando non valido
				return;
			}
			split(v[1], ",", channels);
			for (size_t i = 0; i < channels.size(); i++)
			{
				if (channels[i][0] != '#' && channels[i][0] != '&')
				{
					// errore canale non valido
					continue;
				}
				ch = find_channel(channels[i]);
				if (ch && ch->is_user_inside(user->get_user_name()))
				{
					send_part_message(ch, user);
					user->leave_channel(channels[i]);
				}
			}
			break;
		}
	}
}


