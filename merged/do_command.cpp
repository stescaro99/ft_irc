#include "standard_libraries.hpp"


void Server::do_command(short cmd, User *user, std::vector<std::string> const &v)
{
	switch (cmd)
	{
		case 1: // JOIN
		{
			join(user, v);
			break;
		}
		case 2: // PART
		{
			part(user, v);
			break;
		}
		case 3: //MODE
		{
			mode(user, v);
			break;
		}
		case 4: //TOPIC
		{
			topic(user, v);
			break;
		}
		case 5: //KICK
		{
			kick(user, v);
			break;
		}
		case 6: //INVITE
		{
			invite(user, v);
			break;
		}
		case 7: //PRIVMSG
		{
			privmsg(user, v);
			break;
		}
		case 8: //QUIT
		{
			quit(user);
			break;
		}
	}
}

void Server::join(User *user, std::vector<std::string> const &v)
{
	if (v.size() < 2 || v.size() > 3)
	{
		std::string error_msg = ":IRCSERV 461 " + user->get_user_nick() + " JOIN :Not enough parameters\r\n";
		send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
		return;
	}
	std::vector<std::string> channels;
	std::vector<std::string> passwords;
	split(v[1], ",", channels);
	if (v.size() > 2)
		split(v[2], ",", passwords);
	while (channels.size() != passwords.size())
		passwords.push_back("");
	for (size_t i = 0; i < channels.size(); i++)
	{
		if (channels[i][0] != '#' && channels[i][0] != '&')
		{
			std::string error_msg = ":IRCSERV 403 " + user->get_user_nick() + " " + channels[i] + " :No such channel\r\n";
			send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
			continue;
		}
		if (is_channel(channels[i]) && find_channel(channels[i])->is_user_inside(user->get_user_name()))
		{
			std::string error_msg = ":IRCSERV 443 " + user->get_user_nick() + " " + channels[i] + " :is already on channel\r\n";
			send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
			continue;
		}
		if (is_channel(channels[i]))
			user->join_channel(find_channel(channels[i]), passwords[i]);
		else
			user->create_channel(channels[i], passwords[i]);
		Channel *ch = find_channel(channels[i]);
		if (ch && ch->is_user_inside(user->get_user_name()))
			send_join_message(ch, user);
	}
}

void Server::part(User *user, std::vector<std::string> const &v)
{
	if (v.size() < 2)
	{
		std::string error_msg = ":IRCSERV 461 " + user->get_user_nick() + " PART :Not enough parameters\r\n";
		send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
		return;
	}
	std::vector<std::string> channels;
	split(v[1], ",", channels);
	for (size_t i = 0; i < channels.size(); i++)
	{
		if (channels[i][0] != '#' && channels[i][0] != '&')
		{
			std::string error_msg = ":IRCSERV 403 " + user->get_user_nick() + " " + channels[i] + " :No such channel\r\n";
			send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
			continue;
		}
		Channel *ch = find_channel(channels[i]);
		if (!ch)
		{
			std::string error_msg = ":IRCSERV 403 " + user->get_user_nick() + " " + channels[i] + " :No such channel\r\n";
			send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
			continue;
		}
		else if (!ch->is_user_inside(user->get_user_name()))
		{
			std::string error_msg = ":IRCSERV 442 " + user->get_user_nick() + " " + channels[i] + " :You're not on that channel\r\n";
			send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
			continue;
		}
		send_part_message(ch, user);
		user->leave_channel(channels[i]);
	}
}


void Server::mode(User *user, std::vector<std::string> const &v)
{
	if (v.size() < 2 || v[1].find(',') != std::string::npos)
	{
		std::string error_msg = ":IRCSERV 461 " + user->get_user_nick() + " MODE :Not enough parameters\r\n";
		send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
		return;
	}
	Channel *ch = find_channel(v[1]);
	if (!ch)
	{
		std::string error_msg = ":IRCSERV 403 " + user->get_user_nick() + " " + v[1] + " :No such channel\r\n";
		send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
		return;
	}
	else if (v.size() == 2)
	{
		ch->channel_info(user);
		return ;
	}
	else if (v[2] == "+b")
		return;
	else if (!ch->is_user_admin(user->get_user_name()))
	{
		std::string error_msg = ":IRCSERV 482 " + user->get_user_nick() + " " + ch->get_name() + " :You're not channel operator\r\n";
		send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
		return;
	}
	size_t n = 2;
	while (n < v.size())
	{
		std::vector<std::string> flags = split_mode(v[n]);
		for (size_t i = 0; i < flags.size(); i++)
		{
			bool plus = (flags[i][0] == '+');
			short limit = 0;
			std::string password;
			std::vector<std::string> new_admins;
			set_mode_utility(v, new_admins, limit, password, n, flags[i]);
			if (parameters_check(flags, new_admins, limit, password, user, ch))
				return;
			for (size_t j = 1; j < flags[i].size(); j++)
			{
				char m = flags[i][j];
				if (m == 'i' && plus)
				{
					ch->change_mode(true);
					std::string mode_msg = ":IRCSERV MODE " + ch->get_name() + " +i\r\n";
					ch->c_send_message(user->get_user_name(), mode_msg, false);
				}
				else if (m == 'i')
				{
					ch->change_mode(false);
					std::string mode_msg = ":IRCSERV MODE " + ch->get_name() + " -i\r\n";
					ch->c_send_message(user->get_user_name(), mode_msg, false);
				}
				else if (m == 't')
				{
					ch->set_topic_admin(plus);
					std::string mode_msg = ":IRCSERV MODE " + ch->get_name() + " " + (plus ? "+" : "-") + "t\r\n";
				}
				else if (m == 'k' && plus)
				{
					ch->change_password(password);
					std::string mode_msg = ":IRCSERV MODE " + ch->get_name() + " +k " + password + "\r\n";
					ch->c_send_message(user->get_user_name(), mode_msg, false);
				}
				else if (m == 'k')
				{
					ch->change_password("");
					std::string mode_msg = ":IRCSERV MODE " + ch->get_name() + " -k\r\n";
					ch->c_send_message(user->get_user_name(), mode_msg, false);
				}
				else if (m == 'l' && plus)
				{
					ch->change_limit(limit);
					std::string l;
					std::stringstream ss;
					ss << limit;
					ss >> l;
					std::string mode_msg = ":IRCSERV MODE " + ch->get_name() + " +l " + l + "\r\n";
					ch->c_send_message(user->get_user_name(), mode_msg, false);
				}
				else if (m == 'l')
				{
					ch->change_limit(SHRT_MAX);
					std::string mode_msg = ":IRCSERV MODE " + ch->get_name() + " -l\r\n";
				}
				else if (m == 'o' && plus)
				{
					for (size_t k = 0; k < new_admins.size(); k++)
					{
						if (ch->is_user_inside(new_admins[k]))
						{
							ch->add_admin(new_admins[k]);
							std::string mode_msg = ":IRCSERV MODE " + ch->get_name() + " +o " + new_admins[k] + "\r\n";
							ch->c_send_message(user->get_user_name(), mode_msg, false);
						}
					}
				}
				else if (m == 'o')
				{
					for (size_t k = 0; k < new_admins.size(); k++)
					{
						if (ch->is_user_inside(new_admins[k]))
						{
							ch->rem_admin(new_admins[k]);
							std::string mode_msg = ":IRCSERV MODE " + ch->get_name() + " -o " + new_admins[k] + "\r\n";
							ch->c_send_message(user->get_user_name(), mode_msg, false);
						}
					}
				}
				else if (m != 'b')
				{
					std::string error_msg = ":IRCSERV 472 " + user->get_user_nick() + " " + ch->get_name() + " :Unknown MODE flag\r\n";
					send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
				}
			}
			//std::string mode_msg = ":" + user->get_user_nick() + "!" + user->get_user_name() + "@" + user->get_user_host() + " MODE " + ch->get_name() + " " + flags[i] + "\r\n";
			//ch->c_send_message(user->get_user_name(), mode_msg, false);
		}
	}
}


void Server::topic(User *user, std::vector<std::string> const &v)
{
	Channel *ch = find_channel(v[1]);

	if (ch && ch->is_user_inside(user->get_user_name()))
	{
		if (v.size() == 2)
		{
			std::string topic = ch->get_topic();
			if (topic.empty())
				topic = "No topic is set";
			std::string topic_msg = ":IRCSERV 332 " + user->get_user_nick() + " " + ch->get_name() + " :" + topic + "\r\n";
			send(user->get_user_fd(), topic_msg.c_str(), topic_msg.size(), 0);
		}
		else
		{
			if (v[2][0] != ':')
			{
				std::string error_msg = ":IRCSERV 461 " + user->get_user_nick() + " TOPIC :Not enough parameters\r\n";
				send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
				return;
			}
			if (!ch->get_topic_only_admin() || ch->is_user_admin(user->get_user_name()))
			{
				std::string new_topic = v[2].substr(1);
				ch->change_topic(new_topic);
				std::string topic_msg = ":" + user->get_user_nick() + "!" + user->get_user_name() + "@" + user->get_user_host() + " TOPIC " + ch->get_name() + " :" + new_topic + "\r\n";
				ch->c_send_message(user->get_user_name(), topic_msg, false);
			}
			else
			{
				std::string error_msg = ":IRCSERV 482 " + user->get_user_nick() + " " + ch->get_name() + " :You're not channel operator\r\n";
				send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
			}
		}
	}
	else if (!ch)
	{
		std::string error_msg = ":IRCSERV 403 " + user->get_user_nick() + " " + v[1] + " :No such channel\r\n";
		send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
		return;
	}
	else
	{
		std::string error_msg = ":IRCSERV 442 " + user->get_user_nick() + " " + ch->get_name() + " :You're not on that channel\r\n";
		send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
		return;
	}
}

void Server::kick(User *user, std::vector<std::string> const &v)
{
	if (v.size() < 3)
	{
		std::string error_msg = ":IRCSERV 461 " + user->get_user_nick() + " KICK :Not enough parameters\r\n";
		send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
		return;
	}
	std::vector<std::string> channels;
	split(v[1], ",", channels);
	std::vector<std::string> users;
	split(v[2], ",", users);
	std::string comment = "";
	if (v.size() > 3 && v[3][0] == ':')
		comment = v[3];
	for (size_t i = 0; i < channels.size(); i++)
	{
		Channel *ch = find_channel(channels[i]);
		if (!ch)
		{
			std::string error_msg = ":IRCSERV 403 " + user->get_user_nick() + " " + channels[i] + " :No such channel\r\n";
			send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
			continue;
		}
		if (!ch->is_user_admin(user->get_user_name()))
		{
			std::string error_msg = ":IRCSERV 482 " + user->get_user_nick() + " " + ch->get_name() + " :You're not channel operator\r\n";
			send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
			continue;
		}
		for (size_t j = 0; j < users.size(); j++)
		{
			std::string user_name = convert_to_username(users[j]);
			if (ch->is_user_inside(user_name))
			{
				std::string kick_msg = ":" + user->get_user_nick() + "!" + user->get_user_name() + "@" + user->get_user_host() + " KICK " + ch->get_name() + " " + users[j] + " " + comment + "\r\n";
				ch->c_send_message(user->get_user_name(), kick_msg, false);
				ch->rem_user_from_channel(user_name, true);
			}
			else
			{
				std::string error_msg = ":IRCSERV 441 " + user->get_user_nick() + " " + users[j] + " " + ch->get_name() + " :No such user on that channel\r\n";
				send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
			}
		}
	}
}

void Server::invite(User *user, std::vector<std::string> const &v)
{
	if (v.size() < 3)
	{
		std::string error_msg = ":IRCSERV 461 " + user->get_user_nick() + " INVITE :Not enough parameters\r\n";
		send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
		return;
	}
	std::vector<std::string> channels;
	split(v[2], ",", channels);
	std::vector<std::string> users;
	split(v[1], ",", users);
	for (size_t i = 0; i < channels.size(); i++)
	{
		Channel *ch = find_channel(channels[i]);
		if (!ch)
		{
			std::string error_msg = ":IRCSERV 403 " + user->get_user_nick() + " " + channels[i] + " :No such channel\r\n";
			send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
			continue;
		}
		if (!ch->is_user_admin(user->get_user_name()))
		{
			std::string error_msg = ":IRCSERV 482 " + user->get_user_nick() + " " + ch->get_name() + " :You're not channel operator\r\n";
			send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
			continue;
		}
		for (size_t j = 0; j < users.size(); j++)
		{
			User *u = find_user(convert_to_username(users[j]));
			if (u)
			{
				ch->invite_user(u->get_user_nick());
				std::string invite_msg = ":" + user->get_user_nick() + "!" + user->get_user_name() + "@" + user->get_user_host() + " INVITE " + u->get_user_nick() + " :" + ch->get_name() + "\r\n";
				send(u->get_user_fd(), invite_msg.c_str(), invite_msg.size(), 0);
			}
			else
			{
				std::string error_msg = ":IRCSERV 401 " + user->get_user_nick() + " " + users[j] + " :No such nickname\r\n";
				send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
			}
		}
	}
}

void Server::privmsg(User *user, std::vector<std::string> const &v)
{
	if (v.size() < 3 || v[2][0] != ':')
	{
		std::string error_msg = ":IRCSERV 461 " + user->get_user_nick() + " PRIVMSG :Not enough parameters\r\n";
		send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
		return;
	}
	if (v[2].substr(0, 10) == ":DCC SEND ")
	{
		handle_dcc_send(user, v[2].substr(10));
		//dcc(user, v);
		return ;
	}
	if (v[2].substr(0, 12) == ":DCC ACCEPT ")
	{
		//dcc_accept(user, v);
		return ;
	}
	if (v[2].substr(0, 2) == ":!")
	{
		Channel *ch = find_channel(v[1]);
		if (!ch)
		{
			std::string error_msg = ":IRCSERV 403 " + user->get_user_nick() + " " + v[1] + " :No such channel\r\n";
			send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
			return;
		}
		if (!ch->is_user_inside(user->get_user_name()))
		{
			std::string error_msg = ":IRCSERV 404 " + user->get_user_nick() + " " + ch->get_name() + " :Cannot send to channel\r\n";
			send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
			return;
		}
		command_bot(ch, user, v[2].substr(1));
		return ;
	}
	std::vector<std::string> us_or_ch;
	split(v[1], ",", us_or_ch);
	std::string message = v[2].substr(1);
	for (size_t i = 0; i < us_or_ch.size(); i++)
	{
		if (is_channel(us_or_ch[i]))
		{
			Channel *ch = find_channel(us_or_ch[i]);
			if (ch && ch->is_user_inside(user->get_user_name()))
			{
				std::string privmsg = ":" + user->get_user_nick() + "!" + user->get_user_name() + "@" + user->get_user_host() + " PRIVMSG " + ch->get_name() + " :" + message + "\r\n";
				ch->c_send_message(user->get_user_name(), privmsg, true);
			}
			else if (!ch)
			{
				std::string error_msg = ":IRCSERV 403 " + user->get_user_nick() + " " + us_or_ch[i] + " :No such channel\r\n";
				send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
			}
			else
			{
				std::string error_msg = ":IRCSERV 442 " + user->get_user_nick() + " " + ch->get_name() + " :You're not on that channel\r\n";
				send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
			}
		}
		else
		{
			User *u = find_user(convert_to_username(us_or_ch[i]));
			if (u)
			{
				std::string privmsg = ":" + user->get_user_nick() + "!" + user->get_user_name() + "@" + user->get_user_host() + " PRIVMSG " + u->get_user_nick() + " :" + message + "\r\n";
				send(u->get_user_fd(), privmsg.c_str(), privmsg.size(), 0);
			}
			else
			{
				std::string error_msg = ":IRCSERV 401 " + user->get_user_nick() + " " + us_or_ch[i] + " :No such nickname\r\n";
				send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
			}
		}
	}
}
/* 
void Server::dcc(User *user, std::vector<std::string> const &v)
{
	std::string tmp = v[2].substr(10);
	std::vector<std::string> us_or_ch;
	split(v[1], ",", us_or_ch);
	std::vector<std::string> dcc_info;
	split(tmp, " ", dcc_info);
	if (dcc_info.size() != 4)
	{
		std::string error_msg = ":IRCSERV 461 " + user->get_user_nick() + " PRIVMSG :Not enough parameters\r\n";
		send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
		return;
	}
	unsigned short m_port;
	std::stringstream ss;
	ss << dcc_info[2];
	ss >> m_port;
	ss.clear();
	size_t size;
	ss << dcc_info[3];
	ss >> size;
	if (dcc_info[1] != user->get_user_host() && (user->get_user_host() != "localhost" || dcc_info[1] != "127.0.0.1"))
	{
		std::string error_msg = ":IRCSERV 461 " + user->get_user_nick() + " DCC :Invalid IP address\r\n";
		send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
		return;
	}
	if (port < 1024)
	{
		std::string error_msg = ":IRCSERV 461 " + user->get_user_nick() + " DCC :Invalid port\r\n";
		send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
		return;
	}
	if (size < 1)
	{
		std::string error_msg = ":IRCSERV 461 " + user->get_user_nick() + " DCC :Invalid file size\r\n";
		send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
		return;
	}
	std::string dcc_msg = ":" + user->get_user_nick() + "!" + user->get_user_name() + "@" + user->get_user_host() + " NOTICE " + user->get_user_nick() + " :DCC SEND " + dcc_info[0] + " " + dcc_info[1] + " " + dcc_info[2] + " " + dcc_info[3] + "\r\n";
	for (size_t i = 0; i < us_or_ch.size(); i++)
	{
		if (is_channel(us_or_ch[i]))
		{
			Channel *ch = find_channel(us_or_ch[i]);
			if (ch && ch->is_user_inside(user->get_user_name()))
			{
				ch->c_send_message(user->get_user_name(), dcc_msg, true);
			}
			else if (!ch)
			{
				std::string error_msg = ":IRCSERV 403 " + user->get_user_nick() + " " + us_or_ch[i] + " :No such channel\r\n";
				send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
			}
			else
			{
				std::string error_msg = ":IRCSERV 442 " + user->get_user_nick() + " " + ch->get_name() + " :You're not on that channel\r\n";
				send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
			}
		}
		else
		{
			User *u = find_user(convert_to_username(us_or_ch[i]));
			if (u)
			{
				send(u->get_user_fd(), dcc_msg.c_str(), dcc_msg.size(), 0);
			}
			else
			{
				std::string error_msg = ":IRCSERV 401 " + user->get_user_nick() + " " + us_or_ch[i] + " :No such nickname\r\n";
				send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
			}
		}
	}
}

void Server::dcc_accept(User *user, std::vector<std::string> const &v)
{
	std::string tmp = v[2].substr(12);
	std::vector<std::string> dcc_info;
	split(tmp, " ", dcc_info);
	if (dcc_info.size() != 3)
	{
		std::string error_msg = ":IRCSERV 461 " + user->get_user_nick() + " PRIVMSG :Not enough parameters\r\n";
		send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
		return;
	}
	unsigned short m_port;
	std::stringstream ss;
	ss << dcc_info[1];
	ss >> m_port;
	ss.clear();
	long position;
	ss << dcc_info[2];
	ss >> position;
	if (m_port != port)
	{
		std::string error_msg = ":IRCSERV 461 " + user->get_user_nick() + " DCC :Invalid port\r\n";
		send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
		return;
	}
	if (position < 0)
	{
		std::string error_msg = ":IRCSERV 461 " + user->get_user_nick() + " DCC :Invalid position\r\n";
		send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
		return;
	}
	std::string acc_msg = ":" + user->get_user_nick() + "!" + user->get_user_name() + "@" + user->get_user_host() + " NOTICE " + user->get_user_nick() + " :DCC ACCEPT " + dcc_info[0] + " " + dcc_info[1] + " " + dcc_info[2] + "\r\n";
	send(user->get_user_fd(), acc_msg.c_str(), acc_msg.size(), 0);
} */

void Server::quit(User *user)
{
	std::string quit_msg = ":" + user->get_user_nick() + "!" + user->get_user_name() + "@" + user->get_user_host() + " QUIT :Client closed connection\r\n";
	for (std::vector<User*>::iterator it = users.begin(); it != users.end(); it++)
	{
		if ((*it)->get_user_fd() != user->get_user_fd())
			send((*it)->get_user_fd(), quit_msg.c_str(), quit_msg.size(), 0);
	}
	rem_user(user->get_user_name());
	//std::cout << users.size() << std::endl;
	if (users.size() == 0)
		throw std::runtime_error("No users left, server closed");
}

void Server::handle_dcc_send(User *user, const std::string &message)
{
    std::vector<std::string> v;
	split(message, " ", v);

	if (v.size() != 4)
	{
		std::string error_msg = ":IRCSERV 461 " + user->get_user_nick() + " PRIVMSG :Not enough parameters\r\n";
		send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
		return;
	}

	std::string filename = v[0];
	unsigned short port;
	std::stringstream ss;
	ss << v[2];
	ss >> port;
	ss.clear();
	unsigned long size;
	ss << v[3];
	ss >> size;
	ss.clear();
	unsigned long ip;
	ss << v[1];
	ss >> ip;
	ss.clear();
	// Converti l'IP da stringa a formato numerico
    struct in_addr addr;
    addr.s_addr = htonl(ip);
    std::string ip_address = inet_ntoa(addr);

    // Avvia il trasferimento del file
    start_dcc_send(user, filename, ip_address, port, size);
}

void Server::start_dcc_send(User *user, const std::string &filename, const std::string &ip, unsigned short n_port, unsigned long size)
{
    (void)size;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "Errore nella creazione del socket" << std::endl;
        return;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(n_port);
    inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr);

	//aggiunto
	struct timeval timeout;
    timeout.tv_sec = 10; // 10 seconds timeout
    timeout.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
	//fine aggiunto

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "Errore nella connessione al client" << std::endl;
        close(sockfd);
        return;
    }

    std::ifstream file(filename.c_str(), std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Errore nell'apertura del file" << std::endl;
        close(sockfd);
        return;
    }
	//aggiunto
	std::string request = "DCC SEND " + filename + " " + ip + " " + std::to_string(n_port) + " " + std::to_string(size) + "\r\n";
	send(sockfd, request.c_str(), request.size(), 0);
	//fine aggiunto
    char buffer[1024];
    while (file.read(buffer, sizeof(buffer)))
    {
        send(sockfd, buffer, file.gcount(), 0);
    }
    if (file.gcount() > 0)
    {
        send(sockfd, buffer, file.gcount(), 0);
    }

    file.close();
    close(sockfd);
    std::cout << "File trasferito con successo" << std::endl;
}