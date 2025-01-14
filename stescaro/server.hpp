#ifndef SERVER_HPP
#define SERVER_HPP

#include "standard_libraries.hpp"

class Server
{
	private:
		const int				fd;
		const std::string		password;
		const short				port;
		std::vector<User*>	  	users;
		std::vector<Channel*>	channels;

	public:
		Server(const std::string &password, int port, int fd);
		~Server();

		void add_user(const std::string &user, const std::string &nick, int ufd);
		void rem_user(const std::string &user);
		void add_channel(Channel *ch);
		void rem_channel(const std::string &channel);
		void send_message(const std::string &user, const std::string &message);
		void print_messages();
		// void recv_message(const std::string &user);
		bool is_user(const std::string &user) const;
		bool is_channel(const std::string &channel) const;
};

#endif
