#include "standard_libraries.hpp"

void Server::send_join_message(Channel *ch, User *user)
{
	// JOIN MESSAGE
	std::string join_msg = ":" + user->get_user_nick() + "!" + user->get_user_name() + "@" + user->get_user_host() + " JOIN :" + ch->get_name() + "\r\n";
	ch->c_send_message(user->get_user_name(), join_msg, true);

	// NAMES MESSAGE
	std::string names_msg = ":IRCSERV 353 " + user->get_user_nick() + " = " + ch->get_name() + " :" + ch->get_users_list() + "\r\n";
	send(user->get_user_fd(), names_msg.c_str(), names_msg.size(), 0);

	// END OF NAMES MESSAGE
	std::string eon_msg = ":IRCSERV 366 " + user->get_user_nick() + " " + ch->get_name() + " :End of NAMES list\r\n";
	send(user->get_user_fd(), eon_msg.c_str(), eon_msg.size(), 0);

	// TOPIC MESSAGE
	std::string topic_msg;
	if (ch->get_topic().empty())
		topic_msg = ":IRCSERV 332 " + user->get_user_nick() + " " + ch->get_name() + " :No topic is set\r\n";
	else
		topic_msg = ":IRCSERV 332 " + user->get_user_nick() + " " + ch->get_name() + " :" + ch->get_topic() + "\r\n";
	send(user->get_user_fd(), topic_msg.c_str(), topic_msg.size(), 0);
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