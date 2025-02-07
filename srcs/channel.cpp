#include "channel.hpp"

void Channel::add_user_to_channel(User *user, const std::string &password)
{
	if (ch_users.find((*user).get_user_name()) == ch_users.end())
	{
		if (ch_password != password)
			return ;
		else if (ch_invite)
		{
			for (size_t i = 0; i < ch_invited.size(); i++)
			{
				if (ch_invited[i] == (*user).get_user_name())
				{
					ch_invited.erase(ch_invited.begin() + i);
					break ;
				}
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
			User *u = ch_users[user];
			u->leave_channel(ch_name);
			return ;
		}
	}
	if (is_user_admin(user))
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
		ch_admin.clear();
	if (is_user_admin(user))
	{
		if (ch_admin.size() == 1)
		{
			for (std::map<std::string, User*>::iterator it = ch_users.begin(); it != ch_users.end(); it++)
			{
				if (it->first != user)
				{
					add_admin(it->first);
					std::string admin_msg = ":IRCSERV MODE " + ch_name + " +o " + (it->second)->get_user_nick() + "\r\n";
					c_send_message(it->first, admin_msg, false);
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

void Channel::change_mode(bool mode)
{
	ch_invite = mode;
	if (mode == false)
		ch_invited.clear();
}

void Channel::change_password(const std::string &password)
{
	ch_password = password;
}

void Channel::invite_user(const std::string &user)
{
	if (ch_users.find(user) == ch_users.end())
	{
		for (std::vector<std::string>::iterator it = ch_invited.begin(); it != ch_invited.end(); it++)
		{
			if (*it == user)
				break ;
		}
		ch_invited.push_back(user);
	}
}

void Channel::change_limit(short limit)
{
	ch_limit = limit;
}

void Channel::accept_client(int socket_fd, std::vector<std::string> dcc_info, size_t size, std::string const &user)
{
	for (std::map<std::string, User*>::iterator it = ch_users.begin(); it != ch_users.end(); it++)
	{
		if ((*it).first != user)
			(*it).second->accept_client(socket_fd, dcc_info, size);
	}
}