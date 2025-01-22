#include "standard_libraries.hpp"

void Server::send_join_message(Channel *ch, User *user)
{
	std::string r1 = ":" + user->get_user_nick() + "!" + user->get_user_name() + "@" + user->get_user_host() + " JOIN :" + ch->get_name() + "\r\n";
	ch->c_send_message(user->get_user_name(), r1, true);
	std::string r2 = ":IRCSERV 353 " + user->get_user_nick() + " = " + ch->get_name() + " :" + ch->get_users_list() + "\r\n";
	send(user->get_user_fd(), r2.c_str(), r2.size(), 0);
	std::string r3 = ":IRCSERV 366 " + user->get_user_nick() + " " + ch->get_name() + " :End of NAMES list\r\n";
	send(user->get_user_fd(), r3.c_str(), r3.size(), 0);
	std::string r4;
	if (ch->get_topic() == "")
		r4 = ":IRCSERV 332 " + user->get_user_nick() + " " + ch->get_name() + " :No topic is set\r\n";
	else
		r4 = ":IRCSERV 332 " + user->get_user_nick() + " " + ch->get_name() + " :" + ch->get_topic() + "\r\n";
	send(user->get_user_fd(), r4.c_str(), r4.size(), 0);
}

void Channel::c_send_message(const std::string &user, const std::string &message, bool only_usr)
{
	for (std::map<std::string, User*>::iterator it = ch_users.begin(); it != ch_users.end(); it++)
	{
		if (only_usr && it->first != user)
			continue;
		send(it->second->get_user_fd(), message.c_str(), message.size(), 0);
	}
}