#ifndef SERVER_HPP
#define SERVER_HPP

//#include "channel.hpp"
#include "user.hpp"

class User;

class Server
{
    private:
		int							socketfd;
		static bool					Signal;
        const std::string          password;
        short				port;
        std::vector<User*>			users;
		std::vector<struct pollfd>	fds;
		
		void takeStr(std::string *dest, char *src);
		User *findUser(int fd);
		void printall(int Usfd, std::string mess, std::string nick);
        //std::vector<&Channel>       channels;

    public:
        Server();
		Server(std::string password, int port);
        ~Server(){};

		void ServerInit();
		void serSocket();
		void AcceptNewClient();
		void ReceiveNewData(int fd);

		static void SignalHandler(int signum);

		void CloseFds();
		void ClearUser(int fd);
		//void add_user(std::string user, std::string password);
		//void rem_user(std::string user);
		//void add_channel(std::string channel);    // fatto da User???
		//void rem_channel(std::string channel);    // fatto da User???
		//void send_message(std::string user, std::string message);
		//void print_messages();
		// void recv_message(std::string user);
};

#endif