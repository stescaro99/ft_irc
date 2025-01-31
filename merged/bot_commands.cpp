#include "standard_libraries.hpp"

void Server::create_bot(Channel *ch, User *user, const std::string &nick)
{
    std::string bot_nick = get_bot_nick(nick);
    struct sockaddr_in	usadd;
	struct pollfd		new_poll;
	socklen_t			len = sizeof(usadd);

    if (ch->get_bot_name() != "")
	{
		Bot *b = find_bot(ch->get_bot_name());
		ch->bot_join(user, b, bot_nick);
		return ;
	}
	Bot *bot = new Bot(*this, accept(socket_fd, (sockaddr *)&(usadd), &len), ch);
	int incofd = bot->get_user_fd();
	if (incofd == -1)
	{
		std::cout << "accept failed" << std::endl;
		delete bot;
		return;
	}
    bot->set_user_nick(bot_nick);
    bot->set_user_name(bot_nick);
	std::string welcome = "INFO: " + bot_nick + "!" + bot_nick + "@" + bot->get_user_host() + " Welcome in the server\r\n";
    send(incofd, welcome.c_str(), welcome.length(), 0);
	new_poll.fd = incofd;
	new_poll.events = POLLIN;
	new_poll.revents = 0;
	if (fcntl(incofd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cout << "bot fcntl failed" << std::endl;
		return;
	}
	bots.push_back(bot);
	fds.push_back(new_poll);
	std::cout  << Cyan << bot_nick << " <" << incofd << "> is connect" << Reset << std::endl;
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
	Server::send_join_message(this, user);
	if (bot_name == "")
	{
		bot_name = b_name;
		ch_bot = bot;
		bot->set_channel(this);
		//bot manda un messaggio di notifica entrata agli altri
		return ;
	}
	if (b_name != bot_name)
	{
		ch_bot = bot;
		bot->increment_mood(100);
		wrong_bot(user, -1);
		return ;
	}
}