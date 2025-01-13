#ifndef SERVER_HPP
#define SERVER_HPP

#include "channel.hpp"

class Server
{
	private:
		const std::string		password;
		const short				port;
		std::vector<User*>	  	users;
		std::vector<Channel*>	channels;

	public:
		Server(std::string password, int port);
		~Server();

		void add_user(std::string user, std::string nick, std::string password);
		void rem_user(std::string user);
		void add_channel(Channel *ch);
		void rem_channel(std::string channel);
		void send_message(std::string user, std::string message);
		void print_messages();
		// void recv_message(std::string user);
		bool is_user(std::string user) const;
		bool is_channel(std::string channel) const;
};

#endif