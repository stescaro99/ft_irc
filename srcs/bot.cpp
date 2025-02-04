#include "bot.hpp"

static bool valid_nick(const std::vector<std::string> &v, const std::string &s)
{
	if (s.find(":") != std::string::npos)
		return false;
	if (v.size() > 2)
		return true;
	short n;
	std::stringstream ss;
	ss << v[1];
	ss >> n;
	if (!ss.fail())
		return false;
	return true;
}

std::string Server::get_bot_nick(const std::string &nick)
{
	static int n = 1;
	std::string bot_nick = BOT_NAME;
	std::vector<std::string> v;
    split(nick, " ", v);
	if (v.size() == 1)
	{
		std::stringstream ss;
		ss << n;
		bot_nick += "_" + ss.str();
		n++;
	}
	else if (valid_nick(v, nick))
	{
		for (size_t i = 1; i < v.size(); i++)
			bot_nick += "_" + v[i];
	}
	return bot_nick;
}

static short is_bot_command(const std::string &command)
{
	std::string cmd = command.substr(0, command.find(" "));
	if (cmd == "!addbot")
		return 1;
	else if (cmd == "!game")
		return 2;
	else if (cmd == "!time")
		return 3;
	else if (cmd == "!quote")
		return 4;
	else if (cmd == "!bus")
		return 5;
	else if (cmd == "!help")
		return 6;
	else if (cmd == "!panna")
		return 7;
	else if (cmd == "!kickbot")
		return 8;
	else if (cmd == "!info")
		return 9;
	else if (cmd == "!sorry")
		return 10;
	else if (cmd == "!pisano")
		return 11;
	return 0;
}

void Server::command_bot(Channel *ch, User *user, std::string const &command)
{
	short cmd = is_bot_command(command);
	if (command.length() > 6 && cmd > 1 && cmd < 8 && !ch->get_bot())
		ch->increment_bot_mood(1);
	if (cmd != 9 && cmd != 6)
    {
        std::string mess = user->get_user_nick() + " " + command + "\r\n";
		ch->c_send_message(user->get_user_name(), mess, true);
    }
	switch (cmd)
	{
		case 1:
			create_bot(ch, user, command);
			break;
		case 2:
			ch->game_bot(user);
			break;
		case 3:
			ch->time_bot();
			break;
		case 4:
			ch->quote_bot(); //probabile crash
			break;
		case 5:
			ch->six_bus_bot();
			break;
		case 6:
			ch->help_bot(user);
			break;
		case 7:
			ch->panna_bot();
			break;
		case 8:
			ch->bot_kick(user); //si incazza con chi ha spento il bot a seconda del mood
			break;
		case 9:
			bot_info(ch, user, command);
			break;
		case 10:
			ch->sorry_bot();
			break;
		case 11:
			ch->pisano_bot();
			break;
		case 0:
			ch->wrong_bot(user, 0); //aumenta il mood e risponde in base al mood
	}
}
