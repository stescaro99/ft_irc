#ifndef SERVER_HPP
#define SERVER_HPP

#include "standard_libraries.hpp"

class Server
{
	private:
		const std::string			password;
		const short					port;
		int							socket_fd;				//poi si casta a const
		std::vector<User*>	  		users;
		std::vector<Channel*>		channels;
		//aggiunti fgori
		static bool					Signal;
		std::vector<struct pollfd>	fds;			//fd di cosa? conviene mappa utenti?

		void take_str(std::string *dest, char *src);
		User *find_user(int fd);
		void print_all(int ufd, const std::string &mess, const std::string &nick);

	public:
		//Server(); utile?
		Server(std::string const &password, int port);
		~Server();

		void server_init();
		void ser_socket();
		void accept_new_client();
		void receive_new_data(int fd);
		void close_fds();
		void clear_user(int fd);

		static void signal_handler(int signum);		//static void???

		void add_user(const std::string &user, const std::string &nick, int ufd);
		void rem_user(const std::string &user);
		void add_channel(Channel *ch);
		void rem_channel(const std::string &channel);
		void send_message(const std::string &user, const std::string &message);
		//void print_message(int fd, std::string nick, const std::string &message);
		// void recv_message(const std::string &user);
		bool is_user(const std::string &user) const;
		bool is_channel(const std::string &channel) const;
};

#endif
