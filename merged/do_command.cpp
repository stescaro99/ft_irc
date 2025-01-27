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
		case 9: //NICK
		{
			nick(user, v);
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
			continue;
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
		}
		else
		{
			std::string error_msg = ":IRCSERV 442 " + user->get_user_nick() + " " + channels[i] + " :You're not on that channel\r\n";
			send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
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
		ch->get_modes(user);
		return ;
	}
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
			std::vector<std::string> users;
			set_mode_utility(v, users, limit, password, n, flags[i]);
			if (parameters_check(flags, users, limit, password, ch, user))
				return;
			for (size_t j = 1; j < flags[i].size(); j++)
			{
				char m = flags[i][j];
				if (m == 'i' && plus)
					ch->change_mode('i');
				else if (m == 'i')
					ch->change_mode('o');
				else if (m == 't')
					ch->set_topic_admin(plus);
				else if (m == 'k' && plus)
					ch->change_password(password);
				else if (m == 'k')
					ch->change_password("");
				else if (m == 'l' && plus)
					ch->change_limit(limit);
				else if (m == 'l')
					ch->change_limit(SHRT_MAX);
				else if (m == 'o' && plus)
				{
					for (size_t k = 0; k < users.size(); k++)
					{
						if (ch->is_user_inside(users[k]))
							ch->add_admin(users[k]);
					}
				}
				else if (m == 'o')
				{
					for (size_t k = 0; k < users.size(); k++)
					{
						if (ch->is_user_inside(users[k]))
							ch->rem_admin(users[k]);
					}
				}
				else
				{
					std::string error_msg = ":IRCSERV 472 " + user->get_user_nick() + " " + ch->get_name() + " :Unknown MODE flag\r\n";
					send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
				}
			}
			std::string mode_msg = ":" + user->get_user_nick() + "!" + user->get_user_name() + "@" + user->get_user_host() + " MODE " + ch->get_name() + " " + flags[i] + "\r\n";
			ch->c_send_message(user->get_user_name(), mode_msg, false);
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
				ch->rem_user_from_channel(user_name, true);
				std::string kick_msg = ":" + user->get_user_nick() + "!" + user->get_user_name() + "@" + user->get_user_host() + " KICK " + ch->get_name() + " " + users[j] + " " + comment + "\r\n";
				ch->c_send_message(user->get_user_name(), kick_msg, false);
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

void Server::quit(User *user)
{
	std::string quit_msg = ":" + user->get_user_nick() + "!" + user->get_user_name() + "@" + user->get_user_host() + " QUIT :Client closed connection\r\n";
	for (std::vector<User*>::iterator it = users.begin(); it != users.end(); it++)
	{
		if ((*it)->get_user_fd() != user->get_user_fd())
			send((*it)->get_user_fd(), quit_msg.c_str(), quit_msg.size(), 0);
		else
			rem_user((*it)->get_user_name());
	}
}

void Server::nick(User *user, std::vector<std::string> const &v)
{
	if (v.size() < 2)
	{
		std::string error_msg = ":IRCSERV 461 " + user->get_user_nick() + " NICK :Not enough parameters\r\n";
		send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
		return;
	}
	if (is_nick(v[1]))
	{
		std::string error_msg = ":IRCSERV 433 " + user->get_user_nick() + " " + v[1] + " :Nickname is already in use\r\n";
		send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
		return;
	}
	std::string old_nick = user->get_user_nick();
	user->change_my_nickname(v[1]);
	std::string nick_msg = ":" + old_nick + "!" + user->get_user_name() + "@" + user->get_user_host() + " NICK " + user->get_user_nick() + "\r\n";
	for (std::vector<User*>::iterator it = users.begin(); it != users.end(); it++)
	{
		if ((*it)->get_user_fd() != user->get_user_fd())
			send((*it)->get_user_fd(), nick_msg.c_str(), nick_msg.size(), 0);
	}
}
