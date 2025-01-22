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

char Channel::get_mode() const
{
	return (ch_mode);
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

std::string User::get_IPadd() const
{
	return (IPadd);
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

Channel *Server::find_channel(const std::string &channel) const
{
	for (std::vector<Channel *>::const_iterator i = channels.begin(); i != channels.end(); i++)
		if ((*i)->get_name() == channel)
			return(*i);
	return (NULL);
}