#include "bot.hpp"

/* void Bot::process_commands(const std::string &command)
{
    std::string response;

    std::cout << "Raw message: " << command << std::endl;

    size_t colon_pos = command.find(" :");
    if (colon_pos == std::string::npos)
        return;

    std::string actual_command = command.substr(colon_pos + 2); // +2 per saltare ": "

    std::cout << "command: " << command << std::endl;
    if (command == "!hello")
        response = "Ciao! Sono " + bot_nickname + "! Come posso aiutarti?\r\n";
    else if (command == "!ping")
        response = "Pong!\r\n";
    else if (command == "!time")
    {
        time_t now = time(0);
        response = "Sono le " + std::string(ctime(&now)) + "\r\n";
    }
    else if (command == "!toc") //testa o croce
    {
        srand(time(0));
        int res = rand() % 2;
        if (res == 0)
            response = "Testa!\r\n";
        else
            response = "Croce!\r\n";
    }
    else if (command == "!panna")
        response = "La panna cotta è uno dei dolci al cucchiaio più conosciuti e apprezzati non solo in Italia, ma anche all'estero Per la semplicità della sua preparazione, il gusto particolarmente delicato e il modo elegante in cui può essere impiattata e condita, è diventato un classico dolce di fine pasto perfetto per ogni occasione. Le origini precise di questo dolce sono sconosciute, si sa solo che è nato nella zona delle Langhe piemontesi all'inizio del Novecento.\r\n";
    else if (command == "!help")
    {
        response = "Ecco i comandi che puoi usare:\r\n";
        response += "!hello - saluta il bot\r\n";
        response += "!ping - giochi a ping pong\r\n";
        response += "!time - ottieni l'ora attuale\r\n";
        response += "!toc - testa o croce\r\n";
        response += "!panna - informazioni sulla panna cotta\r\n";
        response += "!help - mostra questo messaggio\r\n";
    }
    else if (command[0] == '!')
        response = "Scusami, non conosco il comando. Usa !help per vedere i comandi disponibili.\r\n";
    response = ":" + bot_nickname + "!" + bot_nickname + "@" + bot_host + " PRIVMSG " + bot_channel->get_name() + " :" + response + "\r\n"; 
    bot_channel->c_send_message(bot_nickname, response, true);
} */

void Bot::destroy_bot()
{
    server.rem_user(user_name);
}

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
    return 0;
}

void Server::command_bot(Channel *ch, User *user, std::string const &command)
{
    //stampare messaggio?
    short cmd = is_bot_command(command);
    if (command.size() > 6 && cmd > 1 && cmd < 8)
        ch->increment_bot_mood(1);
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
            ch->quote_bot();
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
        case 0:
            ch->wrong_bot(user, cmd); //aumenta il mood e risponde in base al mood
    }
}
