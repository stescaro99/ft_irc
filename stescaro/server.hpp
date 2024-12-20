#ifndef SERVER_HPP
#define SERVER_HPP

#include "channel.hpp"

class Server
{
    private:
        static std::string          password;
        static short                port;
        std::vector<&User>          users;
        std::vector<&Channel>       channels;

    public:
        Server(std::string password, int port);
        ~Server();

        void add_user(std::string user, std::string password);
        void rem_user(std::string user);
        //void add_channel(std::string channel);    // fatto da User???
        //void rem_channel(std::string channel);    // fatto da User???
        void send_message(std::string user, std::string message);
        void print_messages();
        // void recv_message(std::string user);
};

#endif