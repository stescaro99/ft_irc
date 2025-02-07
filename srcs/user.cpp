#include "standard_libraries.hpp"

void User::create_channel(const std::string &channel, const std::string &password)
{
	std::cout << Blue << get_user_name() << " create channel: " << channel << Reset << std::endl;
	Channel *nc = new Channel(channel, this);

	if (password != "")
	{
		nc->change_password(password);
		std::string pass_msg = ":IRCSERV MODE " + channel + " +k " + password + "\r\n";
		nc->c_send_message(user_name, pass_msg, false);
	}
	server.add_channel(nc);
	user_channels[channel] = nc;
	std::cout << Green << "Channel " << channel << " was create" << Reset << std::endl;
	//set modes -l -i -t
	std::string limit_msg = ":IRCSERV MODE " + channel + " -l\r\n";
	nc->c_send_message(user_name, limit_msg, false);
	std::string invite_msg = ":IRCSERV MODE " + channel + " -i\r\n";
	nc->c_send_message(user_name, invite_msg, false);
	std::string topic_msg = ":IRCSERV MODE " + channel + " -t\r\n";
	nc->c_send_message(user_name, topic_msg, false);
}

void User::join_channel(Channel *channel, const std::string &password)
{
	if (channel->get_mode() && !channel->is_user_invited(user_name))
	{
		std::string error_msg = ":IRCSERV 473 " + user_nickname + " " + channel->get_name() + " :Cannot join channel (+i)\r\n";
		send(user_fd, error_msg.c_str(), error_msg.size(), 0);
		return ;
	}
	else if (password != channel->get_password())
	{
		std::string error_msg = ":IRCSERV 475 " + user_nickname + " " + channel->get_name() + " :Cannot join channel (+k)\r\n";
		send(user_fd, error_msg.c_str(), error_msg.size(), 0);
		std::string wrong_pass = ":IRCSERV 464 " + user_nickname + " " + channel->get_name() + " :Wrong password (+k)\r\n";
		send(user_fd, wrong_pass.c_str(), wrong_pass.size(), 0);
		return ;
	}
	if (channel->get_users_count() + (channel->get_bot() != NULL) == (size_t)channel->get_limit())
	{
		std::string error_msg = ":IRCSERV 471 " + user_nickname + " " + channel->get_name() + " :Cannot join channel (+l)\r\n";
		send(user_fd, error_msg.c_str(), error_msg.size(), 0);
		return ;
	}
	channel->add_user_to_channel(this, password);
	user_channels[channel->get_name()] = channel;
}

void User::leave_channel(const std::string &channel)
{
	if (channel == "")
	{
		for (std::map<std::string, Channel*>::iterator it = user_channels.begin(); it != user_channels.end();it++)
		{
			std::string part_msg = ":" + user_nickname + "!" + user_name + "@" + user_host + " PART :" + it->first + "\r\n";
			(it->second)->c_send_message(user_name, part_msg, true);
			if ((it->second)->get_users_count() == 1)
				server.rem_channel(it->first);
			else
			{
				if ((it->second)->is_user_admin(user_name))
				{
					(it->second)->rem_admin(user_name);
					std::string new_admin = (it->second)->get_new_admin();
					User *new_ad = server.find_user(new_admin);
					std::string admin_msg = ":" + new_ad->get_user_nick() + "!" + new_admin + "@" + new_ad->get_user_host() + " MODE " + it->first + " -o " + user_nickname + "\r\n";
					(it->second)->c_send_message(user_name, admin_msg, false);
				}
				(it->second)->rem_user_from_channel(user_name, false);
			}
		}
		user_channels.clear();
		return ;
	}
	if (user_channels.find(channel) != user_channels.end())
	{
		if (CH->get_users_count() == 1)
			server.rem_channel(channel);
		else
		{
			if (CH->is_user_admin(user_name))
			{
				CH->rem_admin(user_name);
				std::string admin_msg = ":" + user_nickname + "!" + user_name + "@" + user_host + " MODE " + channel + " -o " + user_nickname + "\r\n";
				CH->c_send_message(user_name, admin_msg, false);
			}
			CH->rem_user_from_channel(user_name, false);
		}
		std::cout << Cyan << user_nickname << " left " << channel << Reset << std::endl;
	}
}

std::string get_download_path()
{
	char *path = getenv("HOME");
	std::string download_path = path;
	download_path += "/Downloads/";
	return (download_path);
}

void User::accept_client(int socket_fd, std::vector<std::string> file_info , size_t size)
{
	struct sockaddr_in client_addr;
	socklen_t len = sizeof(client_addr);
	int client_fd = -1;

	while (true)
	{
		client_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &len);
		if (client_fd == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				continue;
			}
			else
			{
				std::cerr << "accept failed: " << std::strerror(errno) << std::endl;
				break;
			}
		}
		int file_fd = open(file_info[0].c_str(), O_RDONLY);
		char buff[size];
		ssize_t n = read(file_fd, buff, size);
		if (n == -1)
		{
			std::cerr << "read failed: " << std::strerror(errno) << std::endl;
			break;
		}
		send(client_fd, buff, n, 0);
		std::string new_file_path = get_download_path() + file_info[0].substr(file_info[0].find_last_of('/') + 1);
		int new_file = open(new_file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
		std::cout << n << std::endl;
		write(new_file, buff, n);
		break;
	}
    close(client_fd);
}
