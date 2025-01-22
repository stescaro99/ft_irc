#include "user.hpp"

void User::change_my_nickname(const std::string &nickname)
{
	if (server.is_nick(nickname))
		return ;
	user_nickname = nickname;
}

void User::create_channel(const std::string &channel, const std::string &password)
{
	printf("%screate channel%s\n", Blue, Reset);
	Channel *nc = new Channel(channel, this);

	if (password != "")
	{
		nc->change_password(password);
		nc->change_mode('p');
	}
	server.add_channel(nc);
	user_channels[channel] = nc;
	printf("%schannel created%s\n", Green, Reset);
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
	if (!channel || channel->is_user_inside(this->user_name))
	{
		std::cout << Red << "User already in channel" << Reset << std::endl;
	}
	std::cout << Blue << get_user_nick() << " has join in channel : " << channel->get_name() << Reset <<std::endl;
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

void User::ban_user(const std::string &user, const std::string &channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		if (CH->is_user_admin(user))
			return ;
		else if (CH->is_user_admin(this->user_name))
			CH->ban_user(user);
	}
	std::cout << Yellow << get_user_name() << Magenta << " ban " << user << Reset << std::endl;
}

void User::invite_user(const std::string &user, const std::string &channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		if (CH->get_mode() != 'i')
			return ;
		if (CH->is_user_admin(this->user_name))
			CH->invite_user(user);
	}
}

void User::change_topic(const std::string &topic, const std::string &channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		if (CH->is_user_admin(this->user_name))
			CH->change_topic(topic);
	}
}

void User::change_mode(char mode, const std::string &channel)
{
	if (mode != 'i' && mode != 'p' && mode != 'o')  // altre?
		return ;
	if (user_channels.find(channel) != user_channels.end())
	{
		if (CH->is_user_admin(this->user_name))
			CH->change_mode(mode);
	}
}

void User::change_password(const std::string &password, const std::string &channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		if (CH->is_user_admin(this->user_name))
			CH->change_password(password);
	}
}

void User::add_admin(const std::string &user, const std::string &channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		if (CH->is_user_admin(this->user_name) && !CH->is_user_admin(user))
			CH->add_admin(user);
	}
	std::cout << Yellow << get_user_name() << Green << " ad " << user << " to admin club!" << Reset << std::endl;
}

void User::unban_user(const std::string &user, const std::string &channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		if (CH->is_user_admin(this->user_name))
			CH->unban_user(user);
	}
	std::cout << Yellow << get_user_name() << Green << " unban " << user << Reset << std::endl;
}

void User::rem_message(const std::string &channel, const std::string &message)
{
	// non so se si puo' fare
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
