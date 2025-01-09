#include "user.h"

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
	Channel new_channel(channel, *this);

	if (password && password != "")
	{
		new_channel.change_password(password);
		new_channel.change_mode('p');
	}
	user_channels.insert(std::pair<std::string, Channel>(channel, new_channel));
}

void User::delete_channel(std::string channel)
{
	if (user_channels.find(channel) != user_channels.end()
		&& CH.ch_admin.size() == 1
		&& CH.ch_admin[0] == this->user_name)
	{
		CH.~Channel();
		user_channels.erase(channel);
	}
}

void User::join_channel(std::string channel, std::string password)
{
	//c'è da controllare che esista il canale e che la password e mode siano corrette e che non sia tra i bannati
}

void User::leave_channel(std::string channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		if (CH.ch_user.size() == 1)
		{
			CH.~Channel();
			user_channels.erase(channel);
		}
		else
		{
			for (std::vector<std::string>::iterator it = CH.ch_admin.begin(); it != CH.ch_admin.end(); ++it)
			{
				if (*it == this->user_name)
				{
					if (CH.ch_admin.size() == 1 && CH.ch_users[0] != this->user_name)
						CH.add_admin(CH.ch_users[0]);
					else if (CH.ch_admin.size() == 1)
						CH.add_admin(CH.ch_users[1]);
					CH.rem_admin(this->user_name);
					break;
				}
			}
			CH.rem_user_from_channel(this->user_name);
			user_channels.erase(channel);
		}
	}
}

void User::kick_user(std::string user, std::string channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		for (std::vector<std::string>::iterator it = CH.ch_admin.begin(); it != CH.ch_admin.end(); ++it)
		{
			if (*it == user)
				return ;
		}
	}
	if (user_channels.find(channel) != user_channels.end())
	{
		for (std::vector<std::string>::iterator it = CH.ch_admin.begin(); it != CH.ch_admin.end(); ++it)
		{
			if (*it == this->user_name)
			{
				CH.rem_user_from_channel(user);
				break;
			}
		}
	}
}

void User::ban_user(std::string user, std::string channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		for (std::vector<std::string>::iterator it = CH.ch_admin.begin(); it != CH.ch_admin.end(); ++it)
		{
			if (*it == user)
				return ;
		}
	}
	if (user_channels.find(channel) != user_channels.end())
	{
		for (std::vector<std::string>::iterator it = CH.ch_admin.begin(); it != CH.ch_admin.end(); ++it)
		{
			if (*it == this->user_name)
			{
				CH.rem_user_from_channel(user);
				CH.ch_banned.push_back(user);
				break;
			}
		}
	}
}

void User::invite_user(std::string user, std::string channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		if (CH.channel_mode != 'i')
			return ;
		for (std::vector<std::string>::iterator it = CH.ch_admin.begin(); it != CH.ch_admin.end(); ++it)
		{
			if (*it == this->user_name)
			{
				for (std::vector<std::string>::iterator it = CH.ch_users.begin(); it != CH.ch_users.end(); ++it)
				{
					if (*it == user)
						return ;
				}
				for (std::vector<std::string>::iterator it = CH.ch_banned.begin(); it != CH.ch_banned.end(); ++it)
				{
					if (*it == user)
						return ;
				}
				for (std::vector<std::string>::iterator it = CH.ch_invited.begin(); it != CH.ch_invited.end(); ++it)
				{
					if (*it == user)
						return ;
				}
				CH.ch_invited.push_back(user);
				break;
			}
		}
	}
}

void User::change_topic(std::string topic, std::string channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		for (std::vector<std::string>::iterator it = CH.ch_admin.begin(); it != CH.ch_admin.end(); ++it)
		{
			if (*it == this->user_name)
			{
				CH.change_topic(topic);
				break;
			}
		}
	}
}

void User::change_mode(char mode, std::string channel)
{
	if (mode != 'i' && mode != 'p' && mode != 'o')
		return ;
	if (user_channels.find(channel) != user_channels.end())
	{
		for (std::vector<std::string>::iterator it = CH.ch_admin.begin(); it != CH.ch_admin.end(); ++it)
		{
			if (*it == this->user_name)
			{
				CH.change_mode(mode);
				break;
			}
		}
	}
}

void User::change_password(std::string password, std::string channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		for (std::vector<std::string>::iterator it = CH.ch_admin.begin(); it != CH.ch_admin.end(); ++it)
		{
			if (*it == this->user_name)
			{
				CH.change_password(password);
				break;
			}
		}
	}
}

void User::add_admin(std::string user, std::string channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		for (std::vector<std::string>::iterator it = CH.ch_admin.begin(); it != CH.ch_admin.end(); ++it)
		{
			if (*it == this->user_name)
			{
				CH.add_admin(user);
				break;
			}
		}
	}
}

void User::unban_user(std::string user, std::string channel)
{
	if (user_channels.find(channel) != user_channels.end())
	{
		for (std::vector<std::string>::iterator it = CH.ch_admin.begin(); it != CH.ch_admin.end(); ++it)
		{
			if (*it == this->user_name)
			{
				for (std::vector<std::string>::iterator it = CH.ch_banned.begin(); it != CH.ch_banned.end(); ++it)
				{
					if (*it == user)
					{
						CH.ch_banned.erase(it);
						break;
					}
				}
				break;
			}
		}
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
		if (CH.ch_users.size() == 1)
			return ;
		for (std::vector<std::string>::iterator it = CH.ch_admin.begin(); it != CH.ch_admin.end(); ++it)
		{
			if (*it == this->user_name)
			{
				if (CH.ch_admin.size() == 1 && CH.ch_users[0] != this->user_name)
						CH.add_admin(CH.ch_users[0]);
				else if (CH.ch_admin.size() == 1)
						CH.add_admin(CH.ch_users[1]);
				CH.rem_admin(this->user_name);
				break;
			}
		}
	}
}


