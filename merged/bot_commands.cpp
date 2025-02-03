#include "standard_libraries.hpp"

void Server::create_bot(Channel *ch, User *user, const std::string &nick)
{
	if (!ch->is_user_admin(user->get_user_name()))
	{
		std::string mess = "ERROR: " + user->get_user_nick() + " you are not a channel operator\r\n";
		ch->c_send_message(user->get_user_nick(), mess, false);
		return;
	}
	if (ch->get_users_count() == (size_t)ch->get_limit())
	{
		std::string mess = "ERROR: " + user->get_user_nick() + " the channel is full\r\n";
		ch->c_send_message(user->get_user_nick(), mess, false);
		return;
	}
	std::string bot_nick = get_bot_nick(nick);
	if (ch->get_bot_name() != "")
	{
		Bot *b = find_bot(ch->get_bot_name());
		ch->bot_join(user, b, bot_nick);
		return ;
	}

	int bot_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (bot_fd == -1)
    {
        std::cerr << "socket creation failed: " << std::strerror(errno) << std::endl;
        return;
    }

    // Connect the bot socket to the server's listening socket
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (connect(bot_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        std::cerr << "connect failed: " << std::strerror(errno) << std::endl;
        close(bot_fd);
        return;
    }
	Bot *bot = new Bot(*this, bot_fd, ch);
	
	bot->set_user_nick(bot_nick);
	bot->set_user_name(bot_nick);
	std::string welcome = "INFO: " + bot_nick + "!" + bot_nick + "@" + bot->get_user_host() + " Welcome in the server\r\n";
	send(bot_fd, welcome.c_str(), welcome.length(), 0);
	
	struct pollfd new_poll;
	new_poll.fd = bot_fd;
	new_poll.events = POLLIN;
	new_poll.revents = 0;
	if (fcntl(bot_fd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cout << "bot fcntl failed" << std::endl;
		close(bot_fd);
		delete bot;
		return;
	}
	bots.push_back(bot);
	fds.push_back(new_poll);
	std::cout  << Cyan << bot_nick << " <" << bot_fd << "> is connect" << Reset << std::endl;
	ch->bot_join(user, bot, bot_nick);
}

void Channel::bot_join(User *user, Bot *bot, std::string b_name)
{
	if (ch_bot)
	{
		std::string mess = "ERROR: " + bot->get_user_nick() + " already a bot in the channel\r\n";
		c_send_message(ch_name, mess, false);
		return;
	}
	Server::send_join_message(this, bot);
	if (bot_name == "")
	{
		bot_name = b_name;
		ch_bot = bot;
		bot->set_channel(this);
		std::string mess = bot_name + " " + user->get_user_nick() + " I'm here to help you but I'm not your slave! Ask me something useful (!help) or I'll get mad\r\n";
		c_send_message(ch_name, mess, false);
		return ;
	}
	if (b_name != bot_name)
	{
		ch_bot = bot;
		bot->increment_mood(100);
		wrong_bot(user, -1);
		return ;
	}
	ch_bot = bot;
	bot->increment_mood(-100);
}

void Channel::bot_kick(User *user)
{
	if (!ch_bot)
	{
		std::string mess = "ERROR: No bot in the channel\r\n";
		c_send_message(ch_name, mess, false);
		return;
	}
	if (!is_user_admin(user->get_user_name()))
	{
		ch_bot->increment_mood(50);
		std::string mess = bot_name + " " + user->get_user_nick() + " you are not a channel operator, loser!\r\n";
		c_send_message(bot_name, mess, true);
		return;
	}
	ch_bot->increment_mood(300);
	if (user)
	{
		std::string mess = bot_name + " " + user->get_user_nick() + ", the " + ch_bot->get_insults() + " ,turned me off. I'll remember you once I'm back, stronger than ever!\r\n";
		c_send_message(bot_name, mess, true);
	}
	Server::send_part_message(this, ch_bot);
	ch_bot = NULL;
}

void Channel::game_bot(User *user)
{
	if (!ch_bot)
	{
		std::string mess = "ERROR: No bot in the channel\r\n";
		c_send_message(ch_name, mess, false);
		return;
	}
	ch_bot->increment_mood(-50);
	if (ch_bot->get_mood() > 699)
	{
		std::string mess = bot_name + " " + user->get_user_nick() + ", You really expected me to play with you after all you've done? I'm a bot, not a toy. I'm out of here!\r\n";
		c_send_message(bot_name, mess, true);
		return;
	}
	std::string m = bot_name + " ok, " + user->get_user_nick() + ", lets play Heads or Tails!\r\n" + bot_name + " flipping a coin...\r\n";
	c_send_message(bot_name, m, true);
	sleep(2);
	std::srand(std::time(0));
	std::string mess = bot_name + " " + user->get_user_nick() + " you get ";
	if (std::rand() % 2)
		mess += "heads!\r\n";
	else
		mess += "tails!\r\n";
	c_send_message(bot_name, mess, true);
}

void Channel::time_bot()
{
	if (!ch_bot)
	{
		std::string mess = "ERROR: No bot in the channel\r\n";
		c_send_message(ch_name, mess, false);
		return;
	}
	ch_bot->increment_mood(-10);
	if (ch_bot->get_mood() > 699)
	{
		std::string mess = bot_name + " I'm not your personal assistant. Buy a watch!\r\n";
		c_send_message(bot_name, mess, true);
		return;
	}
	std::time_t t = std::time(0);
	std::string mess = bot_name + " the time is " + std::ctime(&t);
	c_send_message(bot_name, mess, true);
}

void Channel::quote_bot()
{
	std::string quotes[] = {"", ""};
	std::srand(std::time(0));
	short n = std::rand() % 10; //change 10 with the number of quotes
	ch_bot->increment_mood(-20);
	std::string mess = bot_name + " " + quotes[n] + "\r\n";
	c_send_message(bot_name, mess, true);
}

void Channel::six_bus_bot()
{
	if (!ch_bot)
	{
		std::string mess = "ERROR: No bot in the channel\r\n";
		c_send_message(ch_name, mess, true);
		return;
	}
	if (ch_bot->get_mood() > 699)
	{
		std::string mess = bot_name + " Your bus will never arrive, and if it will, I hope it'll run you over!\r\n";
		c_send_message(bot_name, mess, true);
		return;
	}
	if (ch_bot->get_mood() > 399)
	{
		std::string mess = bot_name + " Next bus is in 5 minutes, but it's not a 6, fool! Start camping here\r\n";
		c_send_message(bot_name, mess, true);
		return;
	}
	if (ch_bot->get_mood() > 99)
	{
		std::string mess = bot_name + " IDK, I'm not a bus driver!\r\n";
		c_send_message(bot_name, mess, true);
		return;
	}
	std::string mess = bot_name + " I'm sorry, it'll never pass. You'll have to walk!\r\n";
	c_send_message(bot_name, mess, true);
}

void Channel::help_bot(User *user)
{
	if (!ch_bot)
	{
		std::string mess = "ERROR: No bot in the channel\r\n";
		c_send_message(ch_name, mess, false);
		return;
	}
	ch_bot->increment_mood(-1);
	std::string mess = bot_name + " my commands are:\n!game - to play Heads or Tails\n!time - to know the time\n!quote - to get a random quote\n!bus - to know when the next 6 bus will pass\n!panna - to know the panna cotta recipe\n!help - to see this message again\n!info - to know more about the channel or commands\r\n";
	send(user->get_user_fd(), mess.c_str(), mess.length(), 0);
}

void Channel::panna_bot()
{
	if (!ch_bot)
	{
		std::string mess = "ERROR: No bot in the channel\r\n";
		c_send_message(ch_name, mess, false);
		return;
	}
	ch_bot->increment_mood(-100);
	std::string mess = bot_name + " Nice choice! Panna cotta recipe:\n1. Boil the cream with the sugar and the vanilla bean\n2. Soak the gelatin in cold water\n3. Squeeze the gelatin and add it to the cream\n4. Pour the mixture into the molds and let it cool\n5. Put the panna cotta in the fridge for at least 4 hours\n6. Serve with the caramel sauce\nPanna_cotta Gently offered by sfabi, Yummy!!!\r\n";
	c_send_message(bot_name, mess, true);
}

void Channel::sorry_bot()
{
	if (!ch_bot)
	{
		std::string mess = "ERROR: No bot in the channel\r\n";
		c_send_message(ch_name, mess, false);
		return;
	}
	std::srand(std::time(0));
	short mood = ch_bot->get_mood();
	if (std::rand() % 3)
	{
		ch_bot->increment_mood(-(mood / 2));
		std::string mess = bot_name + " Ok, I forgive you, but I'll keep an eye on you\r\n";
		c_send_message(bot_name, mess, true);
		return;
	}
	ch_bot->increment_mood(mood);
	std::string mess = bot_name + " Sorry?!?! I hate JB, I'll never forgive you for this!\r\n";
	c_send_message(bot_name, mess, true);
}

void Channel::wrong_bot(User *user, short code)
{
	if (!ch_bot)
	{
		std::string mess = "ERROR: No bot in the channel\r\n";
		c_send_message(ch_name, mess, false);
		return;
	}
	//short mood = ch_bot->get_mood();
	std::string mess = bot_name + " ";
	int mood = ch_bot->get_mood();
	switch (code)
	{
		case -1:
			mess += user->get_user_nick() + ", the " + ch_bot->get_insults() + ", did you really try to add another bot? I rule this channel! Now that I'm back, stronger than ever, I'll take my revenge on you!\r\n";
			break;
		case 0:
			if (mood > 699)
				mess += "Someone kick out this loser, " + user->get_user_nick() + "'s so stupid that he can't even read the commands!\r\n";
			else if (mood > 399)
				mess += user->get_user_nick() + " you're so dumb that you can't even read the commands!\r\n";
			else if (mood > 99)
				mess += user->get_user_nick() + " are you stupid? This is not a valid command!\r\n";
			else
				mess += user->get_user_nick() + " you might have typed something wrong, this command doesn't exist!\r\n";
	}
	c_send_message(bot_name, mess, true);
}

void Server::bot_info(Channel *ch, User *user, std::string const &jcmd)
{
	Bot *ch_bot = ch->get_bot();
	if (!ch_bot)
	{
		std::string mess = "ERROR: No bot in the channel\r\n";
		ch->c_send_message(jcmd, mess, false);
		return;
	}
	std::string bot_name = ch_bot->get_user_nick();
	if (jcmd.size() < 6)
	{
		std::stringstream ss;
		ss << ch_bot->get_mood();
		std::string mess = bot_name + " anger points =" + ss.str() + "\r\n";
		send(user->get_user_fd(), mess.c_str(), mess.length(), 0);
		return;
	}
	std::string args = jcmd.substr(6);
	if (is_channel(args))
	{
		Channel *ch = find_channel(args);
		size_t users = ch->get_users_count();
		std::stringstream ss;
		ss << users;
		std::string mess = bot_name + " " + ch->get_name() + " has " + ss.str() + " users:\n" + ch->get_users_list() + "\r\n";
		send(user->get_user_fd(), mess.c_str(), mess.length(), 0);
		return;
	}
	std::string mess = bot_name + " the channels in this server are:\n" + get_channels_list() + "\nThe users are:\n" + get_users_list() + "\r\n";
	send(user->get_user_fd(), mess.c_str(), mess.length(), 0);
}

void Channel::pisano_bot()
{
	if (!ch_bot)
	{
		std::string mess = "ERROR: No bot in the channel\r\n";
		c_send_message(ch_name, mess, false);
		return;
	}
	ch_bot->increment_mood(10000);
	std::string mess = bot_name + " PISANO A ME?!?!?!?! TE NE PENTIRAI PER TUTTA LA VITA, " + ch_bot->get_insults() + "!\r\n";
	c_send_message(bot_name, mess, true);
}