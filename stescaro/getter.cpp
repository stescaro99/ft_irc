#include "server.hpp"

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
