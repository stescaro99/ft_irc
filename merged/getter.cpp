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
		if ((*i)->get_name() == channel)
			return(*i);
	return (NULL);
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

void Channel::get_modes(User *user) const
{
	std::string modes = ":IRCSERVER 324 " + user->get_user_nick() + " " + ch_name + " +";
	if (ch_invite)
		modes += "i";
	if (topic_only_admin)
		modes += "t";
	if (ch_password != "")
		modes += "k";
	if (ch_limit != 0)
		modes += "l";
	modes += "\r\n";
	send(user->get_user_fd(), modes.c_str(), modes.size(), 0);
}