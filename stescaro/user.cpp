#include "user.hpp"

void User::change_my_nickname(std::string nickname)
{
	user_nickname = nickname;
}

void User::change_my_password(std::string password)
{
	user_password = password;
}

void User::create_channel(std::string channel, std::string password)
{
	if ((*server).is_channel(channel))
		return ;
	Channel new_channel(channel, this);

	if (password != "")
	{
		new_channel.change_password(password);
		new_channel.change_mode('p');
	}
	user_channels.insert(std::pair<std::string, Channel*>(channel, &new_channel));
	(*server).add_channel(&new_channel);
}

void User::delete_channel(std::string channel)
{
	if (user_channels.find(channel) != user_channels.end()
		&& (*CH).get_admins_count() == 1
		&& (*CH).is_user_admin(this->user_name))
	{
		user_channels.erase(channel);
		(*server).rem_channel(channel);
	}
}

void User::join_channel(Channel *channel, std::string password)
{
	if ((*server).is_channel((*channel).get_name()))
	{
		size_t n = (*channel).get_users_count();
		(*channel).add_user_to_channel(this, password);
		if (n != (*channel).get_users_count())
			user_channels.insert(std::pair<std::string, Channel*>((*channel).get_name(), channel));
	}
	else
		create_channel((*channel).get_name(), password);
}

void User::leave_channel(std::string channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		if ((*CH).get_users_count() == 1)
			user_channels.erase(channel);
		else
		{
			if ((*CH).is_user_admin(this->user_name) && (*CH).get_admins_count() == 1)
			{
				(*CH).rem_admin(this->user_name);
			}
			(*CH).rem_user_from_channel(this->user_name, 0);
		}
		user_channels.erase(channel);
	}
}

void User::kick_user(std::string user, std::string channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		if ((*CH).is_user_admin(user))
			return ;
		else if ((*CH).is_user_admin(this->user_name))
		{
			(*CH).rem_user_from_channel(user, 1);
		}
	}
}

void User::ban_user(std::string user, std::string channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		if ((*CH).is_user_admin(user))
			return ;
		else if ((*CH).is_user_admin(this->user_name))
			(*CH).ban_user(user);
	}
}

void User::invite_user(std::string user, std::string channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		if ((*CH).get_mode() != 'i')
			return ;
		if ((*CH).is_user_admin(this->user_name))
			(*CH).invite_user(user);
	}
}

void User::change_topic(std::string topic, std::string channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		if ((*CH).is_user_admin(this->user_name))
			(*CH).change_topic(topic);
	}
}

void User::change_mode(char mode, std::string channel)
{
	if (mode != 'i' && mode != 'p' && mode != 'o')  // altre?
		return ;
	if (user_channels.find(channel) != user_channels.end())
	{
		if ((*CH).is_user_admin(this->user_name))
			(*CH).change_mode(mode);
	}
}

void User::change_password(std::string password, std::string channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		if ((*CH).is_user_admin(this->user_name))
			(*CH).change_password(password);
	}
}

void User::add_admin(std::string user, std::string channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		if ((*CH).is_user_admin(this->user_name) && !(*CH).is_user_admin(user))
			(*CH).add_admin(user);
	}
}

void User::unban_user(std::string user, std::string channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		if ((*CH).is_user_admin(this->user_name))
			(*CH).unban_user(user);
	}
}

void User::rem_message(std::string channel, std::string message)
{
	// non so se si puo' fare
}

void User::leave_admin(std::string channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		if ((*CH).get_users_count() == 1)
			return ;
		if ((*CH).is_user_admin(this->user_name))
			(*CH).rem_admin(this->user_name);
	}
}


