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
	if (v.size() < 3 || v[1].find(',') != std::string::npos || (v[2][1] != '+' && v[2][1] != '-'))
	{
		// errore comando non valido
		return;
	}
	Channel *ch = find_channel(v[1]);
	if (!ch)
	{
		std::string error_msg = ":IRCSERV 403 " + user->get_user_nick() + " " + v[1] + " :No such channel\r\n";
		send(user->get_user_fd(), error_msg.c_str(), error_msg.size(), 0);
		return;
	}
	else
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
			short limit = 0;
			std::string password;
			std::vector<std::string> users;
			std::string				 mess = ":" + server_name;
			bool plus = (flags[i][0] == '+');
			set_mode_utility(v, users, limit, password, n, flags[i]);
			for (size_t j = 1; j < flags[i].size(); j++)
			{
				char m = flags[i][j];
				if (m == 'i' && plus)
				{
					ch->change_mode('i');
					mess = mess + " MODE " + ch->get_name() + " now is invite only";
					ch->c_send_message("", mess, false);
				}
				else if (m == 'i')
				{
					ch->change_mode('o');
					mess = mess + " MODE "+ ch->get_name() + " now is open";
					ch->c_send_message("", mess, false);
				}
				else if (m == 't')
				{
					ch->set_topic_admin(plus);
					mess = mess + " MODE "+ ch->get_name() + " now is " + ;
					ch->c_send_message("", mess, false);
				}
				else if (m == 'k' && plus)
				{
					ch->change_password(password);
					mess = mess + " MODE "+ ch->get_name() + " now is " + ;
					ch->c_send_message("", mess, false);
				}
				else if (m == 'k')
				{
					ch->change_password("");
					mess = mess + " MODE "+ ch->get_name() + " now is " + ;
					ch->c_send_message("", mess, false);
				}
				else if (m == 'l' && plus)
				{
					ch->change_limit(limit);
					mess = mess + " MODE "+ ch->get_name() + " now is " + ;
					ch->c_send_message("", mess, false);
				}
				else if (m == 'l')
				{
					ch->change_limit(SHRT_MAX);
					mess = mess + " MODE "+ ch->get_name() + " now is " + ;
					ch->c_send_message("", mess, false);
				}
				else if (m == 'o' && plus)
				{
					for (size_t k = 0; k < users.size(); k++)
					{
						if (ch->is_user_inside(users[k]))
						{
							ch->add_admin(users[k]);
							mess = mess + " MODE "+ ch->get_name() + " now is " + ;
							ch->c_send_message("", mess, false);
						}
					}
				}
				else if (m == 'o')
				{
					for (size_t k = 0; k < users.size(); k++)
					{
						if (is_user(users[k]))
						{
							ch->rem_admin(users[k]);
							mess = mess + " MODE "+ ch->get_name() + " now is " + ;
							ch->c_send_message("", mess, false);
						}
					}
				}
				else
				{
					// errore flag non valido
				}
			}
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
