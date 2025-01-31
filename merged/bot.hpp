#ifndef BOT_HPP
# define BOT_HPP

#include "standard_libraries.hpp"
//#include "user.hpp"

class Channel;
class Server;
class User;

class Bot : public User
{
    private:
        unsigned short  mood;
        Channel         *bot_channel;

    public:
        Bot(Server &server, int fd, Channel *channel);
        ~Bot();

        void increment_mood(short max);
        void destroy_bot();
        void set_channel(Channel *channel);
        short get_mood() const;
        std::string get_insults() const;
};

#endif