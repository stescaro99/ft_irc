#include "standard_libraries.hpp"

void Server::print_all(int Usfd,const std::string &mess, const std::string &nick)
{
	if (Usfd != fds[0].fd)  // stampa anche sul server: tenere???
	{
		std::cout << Yellow << "[" << Reset << nick;
		std::cout << Yellow << "] " << Reset << mess << std::endl;
	}
	for (std::vector<User *>::iterator i = users.begin(); i != users.end(); i++)
	{
		if ((*i)->get_user_fd() != Usfd)
		{
			// send((*i)->get_user_fd(), Yellow , 6, 0);
			// send((*i)->get_user_fd(), "[", 1, 0);
			// send((*i)->get_user_fd(), Reset, 4, 0);
			send((*i)->get_user_fd(), nick.c_str(), nick.length(), 0);
			// send((*i)->get_user_fd(), Yellow, 6, 0);
			// send((*i)->get_user_fd(), "] ", 2, 0);
			send((*i)->get_user_fd(), " ", 1, 0);  //tmp
			// send((*i)->get_user_fd(), Reset, 4, 0);
			send((*i)->get_user_fd(), mess.c_str(), mess.length(), 0);
			// send((*i)->get_user_fd(), "\n", 1, 0);
		}
	}
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