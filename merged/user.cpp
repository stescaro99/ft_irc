#include "user.hpp"

void User::change_my_nickname(const std::string &nickname)
{
	if (server.is_nick(nickname))
		return ;
	user_nickname = nickname;
}

void User::create_channel(const std::string &channel, const std::string &password)
{
	std::cout << Blue << get_user_name() << " create channel: " << channel << Reset << std::endl;
	Channel *nc = new Channel(channel, this);

	if (password != "")
	{
		nc->change_password(password);
		nc->change_mode('p');
	}
	server.add_channel(nc);
	user_channels[channel] = nc;
	std::cout << Green << "Channel " << channel << " was create" << Reset << std::endl;
}

void User::delete_channel(const std::string &channel)
{
	if (user_channels.find(channel) != user_channels.end()
		&& CH->get_admins_count() == 1
		&& CH->is_user_admin(this->user_name))
	{
		user_channels.erase(channel);
		server.rem_channel(channel);
	}
	std::cout << Magenta << "Channel " << channel << " was delete" << Reset << std::endl;
}

void User::join_channel(Channel *channel, const std::string &password)
{
	if (channel->is_user_inside(this->user_name))
		return ;
	if (channel->get_mode() && !channel->is_user_invited(this->user_name))
	{
		std::string error_msg = ":IRCSERV 473 " + user_nickname + " " + channel->get_name() + " :Cannot join channel (+i)\r\n";
		send(this->user_fd, error_msg.c_str(), error_msg.size(), 0);
		return ;
	}
	if (password != channel->get_password())
	{
		std::string error_msg = ":IRCSERV 475 " + user_nickname + " " + channel->get_name() + " :Cannot join channel (+k)\r\n";
		send(this->user_fd, error_msg.c_str(), error_msg.size(), 0);
		return ;
	}
	if (channel->get_users_count() == (size_t)channel->get_limit())
	{
		std::string error_msg = ":IRCSERV 471 " + user_nickname + " " + channel->get_name() + " :Cannot join channel (+l)\r\n";
		send(this->user_fd, error_msg.c_str(), error_msg.size(), 0);
		return ;
	}
	channel->add_user_to_channel(this, password);
	user_channels.insert(std::pair<std::string, Channel*>(channel->get_name(), channel));
}

void User::leave_channel(const std::string &channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		if (CH->get_users_count() == 1)
			server.rem_channel(channel);
		else
		{
			if (CH->is_user_admin(this->user_name))
			{
				CH->rem_admin(this->user_name);
			}
			CH->rem_user_from_channel(this->user_name, false);
		}
		user_channels.erase(channel);
		std::cout << Cyan << get_user_name() << " leave the channel" << Reset << std::endl;
	}
}

void User::kick_user(const std::string &user, const std::string &channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		if (CH->is_user_admin(user))
			return ;
		else if (CH->is_user_admin(this->user_name))
			CH->rem_user_from_channel(user, true);
	}
	std::cout << Yellow << get_user_name() << Magenta << " kick " << user << Reset << std::endl;
}

void User::invite_user(const std::string &user, const std::string &channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		if (!CH->get_mode())
			return ;
		if (CH->is_user_admin(this->user_name))
			CH->invite_user(user);
	}
}

void User::leave_admin(const std::string &channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		if (CH->get_users_count() == 1)
			return ;
		if (CH->is_user_admin(this->user_name))
			CH->rem_admin(this->user_name);
	}
	std::cout << Magenta << get_user_name() <<  " isn`t admin anymore"  << Reset << std::endl;
}

void User::accept_invite(const std::string &channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		if (CH->is_user_invited(this->user_name))
		{
			CH->add_user_to_channel(this, "");
			user_channels.insert(std::pair<std::string, Channel*>(channel, CH));
		}
	}
	std::cout  << Green  << get_user_name()<<  " accept the invite!" << Reset << std::endl;
}
