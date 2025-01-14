#include "user.hpp"

void User::change_my_nickname(const std::string &nickname)
{
	user_nickname = nickname;
}

void User::change_my_password(const std::string &password)
{
	user_password = password;
}

void User::create_channel(const std::string &channel, const std::string &password)
{
	if (server->is_channel(channel))
		return ;
	Channel *nc = new Channel(channel, this);

	if (password != "")
	{
		nc->change_password(password);
		nc->change_mode('p');
	}
	user_channels.insert(std::pair<std::string, Channel*>(channel, nc));
	server->add_channel(nc);
}

void User::delete_channel(const std::string &channel)
{
	if (user_channels.find(channel) != user_channels.end()
		&& CH->get_admins_count() == 1
		&& CH->is_user_admin(this->user_name))
	{
		user_channels.erase(channel);
		server->rem_channel(channel);
	}
}

void User::join_channel(Channel *channel, const std::string &password)
{
	if (server->is_channel(channel->get_name()))
	{
		size_t n = channel->get_users_count();
		channel->add_user_to_channel(this, password);
		if (n != channel->get_users_count())
			user_channels.insert(std::pair<std::string, Channel*>(channel->get_name(), channel));
	}
	else
		create_channel(channel->get_name(), password);
}

void User::leave_channel(const std::string &channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		if (CH->get_users_count() == 1)
			server->rem_channel(channel);
		else
		{
			if (CH->is_user_admin(this->user_name))
			{
				CH->rem_admin(this->user_name);
			}
			CH->rem_user_from_channel(this->user_name, false);
		}
		user_channels.erase(channel);
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
}

void User::unban_user(const std::string &user, const std::string &channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		if (CH->is_user_admin(this->user_name))
			CH->unban_user(user);
	}
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
}
