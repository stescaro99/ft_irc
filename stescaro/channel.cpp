#include "channel.hpp"

void Channel::add_user_to_channel(User &user, std::string password)
{
	if (ch_users.find(user.get_user_name()) == ch_users.end())
	{
		if (ch_mode == 'p' && ch_password != password)
			return ;
		else if (ch_mode == 'i' && ch_invited.find(user.get_user_name()) == ch_invited.end())
			return ;
		ch_users[user.get_user_name()] = user;
	}
}

void Channel::rem_user_from_channel(std::string user)
{
	if (ch_users.find(user) != ch_users.end())
		ch_users.erase(user);
}

void Channel::add_admin(std::string user)
{
	if (ch_admin.find(user) == ch_admin.end())
		ch_admin.push_back(user);
}

void Channel::rem_admin(std::string user)
{
	if (ch_admin.find(user) != ch_admin.end())
		ch_admin.erase(user);
}

void Channel::change_topic(std::string topic)
{
	ch_topic = topic;
}

void Channel::change_mode(char mode)
{
	ch_mode = mode;
}

void Channel::change_password(std::string password)
{
	ch_password = password;
}