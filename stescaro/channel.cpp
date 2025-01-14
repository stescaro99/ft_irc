#include "channel.hpp"

void Channel::add_user_to_channel(User *user, const std::string &password)
{
	if (ch_users.find((*user).get_user_name()) == ch_users.end())
	{
		if (ch_mode == 'p' && ch_password != password)
			return ;
		else if (ch_mode == 'i')
		{
			for (size_t i = 0; i < ch_invited.size(); i++)
			{
				if (ch_invited[i] == (*user).get_user_name())
					break ;
				if (i == ch_invited.size() - 1)
					return ;
			}
		}
		ch_users[(*user).get_user_name()] = user;
	}
}

void Channel::rem_user_from_channel(const std::string &user, bool is_kick)
{
	if (ch_users.size() == 1)
		return ;
	if (is_kick)
	{
		if (ch_users.find(user) != ch_users.end())
		{	
			ch_users.erase(user);
			User *u = ch_users[user];
			(*u).leave_channel(ch_name);
		}
	}
	if (ch_admin.size() == 1 && ch_admin[0] == user)
		rem_admin(user);
	if (ch_users.find(user) != ch_users.end())
		ch_users.erase(user);
}

void Channel::add_admin(const std::string &user)
{
	if (ch_users.find(user) == ch_users.end())
		return ;
	if (is_user_admin(user))
		return ;
	ch_admin.push_back(user);
}

void Channel::rem_admin(const std::string &user)
{
	if (ch_users.size() == 1)
		return ;
	if (is_user_admin(user))
	{
		if (ch_admin.size() == 1 && (*(ch_users[0])).get_user_name() != user)
			add_admin((*(ch_users[0])).get_user_name());
		else if (ch_admin.size() == 1)
		{
			for (std::map<std::string, User*>::iterator it = ch_users.begin(); it != ch_users.end(); it++)
			{
				if (it->first != user)
				{
					add_admin(it->first);
					break ;
				}
			}
		}
		for (std::vector<std::string>::iterator it = ch_admin.begin(); it != ch_admin.end(); it++)
		{
			if (*it == user)
			{
				ch_admin.erase(it);
				break ;
			}
		}
	}
}

void Channel::change_topic(const std::string &topic)
{
	ch_topic = topic;
}

void Channel::change_mode(char mode)
{
	ch_mode = mode;
}

void Channel::change_password(const std::string &password)
{
	ch_password = password;
}

bool Channel::is_user_admin(const std::string &user) const
{
	for (size_t i = 0; i < ch_admin.size(); i++)
	{
		if (ch_admin[i] == user)
			return (true);
	}
	return (false);
}

void Channel::ban_user(const std::string &user)
{
	if (ch_users.find(user) != ch_users.end())
	{
		ch_banned.push_back(user);
		for (std::map<std::string, User*>::iterator it = ch_users.begin(); it != ch_users.end(); it++)
		{
			if (it->first == user)
			{
				User *u = it->second;
				(*u).leave_channel(ch_name);
			}
		}
		ch_users.erase(user);
	}
}

void Channel::invite_user(const std::string &user)
{
	if (ch_users.find(user) == ch_users.end())
	{
		for (std::vector<std::string>::iterator it = ch_invited.begin(); it != ch_invited.end(); it++)
		{
			if (*it == user)
			{
				for (std::vector<std::string>::iterator it = ch_banned.begin(); it != ch_banned.end(); it++)
				{
					if (*it == user)
						return ;
				}
				break ;
			}
		}
		ch_invited.push_back(user);
	}
}

void Channel::unban_user(const std::string &user)
{
	for (std::vector<std::string>::iterator it = ch_banned.begin(); it != ch_banned.end(); it++)
	{
		if (*it == user)
		{
			ch_banned.erase(it);
			break ;
		}
	}
}
