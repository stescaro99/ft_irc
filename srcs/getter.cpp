#include "standard_libraries.hpp"

std::string User::get_user_name() const
{
	return (user_name);
}

size_t Channel::get_users_count() const
{
	return (ch_users.size());
}

size_t Channel::get_admins_count() const
{
	return (ch_admin.size());
}

bool Channel::get_mode() const
{
	return (ch_invite);
}

std::string Channel::get_name() const
{
	return (ch_name);
}

std::string Channel::get_topic() const
{
	return (ch_topic);
}

std::string User::get_user_nick() const
{
	return (user_nickname);
}

int User::get_user_fd() const
{
	return (user_fd);
}

short User::get_state() const
{
	return (state);
}

short User::get_tries() const
{
	return (pass_tries);
}

char *User::get_buff()
{
	return (buff);
}

std::string User::get_user_host() const
{
	return (user_host);
}

User *Server::find_user(int fd) const
{
	for (std::vector<User *>::const_iterator i = users.begin(); i != users.end(); i++)
		if ((*i)->get_user_fd() == fd)
			return(*i);
	return (NULL);
}

User *Server::find_user(const std::string &user) const
{
	for (std::vector<User *>::const_iterator i = users.begin(); i != users.end(); i++)
		if ((*i)->get_user_name() == user)
			return(*i);
	return (NULL);
}

Channel *Server::find_channel(const std::string &channel) const
{
	for (std::vector<Channel *>::const_iterator i = channels.begin(); i != channels.end(); i++)
	{
		if ((*i)->get_name() == channel)
			return(*i);
	}
	std::cout << "Channel not found" << std::endl;
	return (NULL);
}

Bot *Server::find_bot(const std::string &bot) const
{
	for (std::vector<Bot *>::const_iterator i = bots.begin(); i != bots.end(); i++)
	{
		if ((*i)->get_user_nick() == bot)
			return(*i);
	}
	std::cout << "Bot not found" << std::endl;
	return (NULL);
}

int Server::get_socket_fd() const
{
	return (socket_fd);
}

std::string Server::get_home() const
{
	return (env[0].substr(5));
}

bool Channel::get_topic_only_admin() const
{
	return (topic_only_admin);
}

std::string Channel::get_password() const
{
	return (ch_password);
}

short Channel::get_limit() const
{
	return (ch_limit);
}

std::string Channel::get_new_admin() const
{
	return (ch_admin[ch_admin.size() - 1]);
}

Bot *Channel::get_bot() const
{
	return (ch_bot);
}

std::string Channel::get_bot_name() const
{
	return (bot_name);
}

std::string Bot::get_insults() const
{
	std::string adj[] = {"user", "mean", "ass hole", "pratface", "stupid", "son of a bitch", "dumb", "world's worst", "shitface", "idiot", "dirty old bastard", "moron", "fucking stupid ass hole", "one that not even his mother wants at home", "pisano", "pisano di merda"};
	unsigned short index = mood / 50;
	if (index < 15)
		return (adj[index]);
	return (adj[15]);
}

short Bot::get_mood() const
{
	return (mood);
}

int Bot::get_bot_fd() const
{
	return (bot_fd);
}

std::vector<std::string> Channel::get_users() const
{
	std::vector<std::string> v;
	for (std::map<std::string, User*>::const_iterator it = ch_users.begin(); it != ch_users.end(); it++)
	{
		if (!it->first.empty())
			v.push_back(it->second->get_user_nick());
	}
	return v;
}

std::string User::get_priv_ip() const
{
	std::vector<std::string> v;
	Server::split(user_priv_ip, ".", v);
	unsigned short nbrs[4];
	size_t tot;
	for (short i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << v[i];
		ss >> nbrs[i];
		ss.str("");
		ss.clear();
	}
	tot = nbrs[3] + 256 * nbrs[2] + 65536 * nbrs[1] + 16777216 * nbrs[0];
	std::stringstream s;
	s << tot;
	return (s.str());
}