#include "standard_libraries.hpp"

void Server::print_all(int Usfd,const std::string &mess, const std::string &nick)
{
	for (std::vector<User *>::iterator i = users.begin(); i != users.end(); i++)
	{
		if ((*i)->get_user_fd() != Usfd)
		{
			std::string new_mess = nick + " " + mess;
			send((*i)->get_user_fd(), new_mess.c_str(), new_mess.size(), 0);
		}
	}
}

void Channel::c_send_message(const std::string &user, const std::string &message, bool not_usr) const
{
	for (std::map<std::string, User*>::const_iterator it = ch_users.begin(); it != ch_users.end(); it++)
	{
		if (not_usr && it->first == user)
			continue;
		send(it->second->get_user_fd(), message.c_str(), message.size(), 0);
	}
}

void Server::send_join_message(Channel *ch, User *user)
{
	// JOIN MESSAGE
	std::string join_msg = ":" + user->get_user_nick() + "!" + user->get_user_name() + "@" + user->get_user_host() + " JOIN :" + ch->get_name() + "\r\n";
	send(user->get_user_fd(), join_msg.c_str(), join_msg.size(), 0);
	ch->c_send_message(user->get_user_name(), join_msg, false);
	
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

void Server::send_part_message(Channel *ch, User *user)
{
	std::string part_msg = ":" + user->get_user_nick() + "!" + user->get_user_name() + "@" + user->get_user_host() + " PART :" + ch->get_name() + "\r\n";
	
	send(user->get_user_fd(), part_msg.c_str(), part_msg.size(), 0);
	ch->c_send_message(user->get_user_name(), part_msg, true);
}

void Channel::channel_info(User *user) const
{
	std::string mode_msg = ":IRCSERV 324 " + user->get_user_nick() + " " + ch_name + " +";

    if (ch_invite)
        mode_msg += "i";
    if (topic_only_admin)
        mode_msg += "t";
    if (!ch_password.empty())
        mode_msg += "k";
    if (ch_limit != SHRT_MAX)
        mode_msg += "l";
    mode_msg += "\r\n";
	send(user->get_user_fd(), mode_msg.c_str(), mode_msg.size(), 0);

	std::string msg;
	if (ch_limit != SHRT_MAX)
	{
		std::string limit;
		std::stringstream ss;
		ss << ch_limit;
		ss >> limit;
		msg = ":IRCSERV +l limit is set to:" + limit + "\r\n";
	}
	else
		msg = ":IRCSERV -l no limit set\r\n";
	send(user->get_user_fd(), msg.c_str(), msg.size(), 0);
	msg.clear();
	if (ch_password != "")
		msg = ":IRCSERV +k protected by password: " + ch_password + "\r\n";
	else
		msg = ":IRCSERV -k no password set\r\n";
	send(user->get_user_fd(), msg.c_str(), msg.size(), 0);
	msg.clear();
	if (ch_invite)
		msg = ":IRCSERV +i invite only\r\n";
	else
		msg = ":IRCSERV -i no invite only set\r\n";
	send(user->get_user_fd(), msg.c_str(), msg.size(), 0);
	msg.clear();
	if (topic_only_admin)
		msg = ":IRCSERV +t only channel operators can change topic\r\n";
	else
		msg = ":IRCSERV -t every user inside the channel can change the topic\r\n";
	send(user->get_user_fd(), msg.c_str(), msg.size(), 0);
	msg.clear();
	if (is_user_admin(user->get_user_name()))
		msg = ":IRCSERV +o You are channel operator\r\n";
	else
		msg = ":IRCSERV -o You are not channel operator\r\n";
	send(user->get_user_fd(), msg.c_str(), msg.size(), 0);	
}