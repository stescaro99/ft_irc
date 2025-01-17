#ifndef SERVER_HPP
#define SERVER_HPP

#include "standard_libraries.hpp"

class User;
class Channel;

struct PollComp 
{
    bool operator()(const pollfd& lhs, const pollfd& rhs) const
	{
        return lhs.fd < rhs.fd;
    }
};

class Server
{
	private:
		const std::string							password;
		const short									port;
		const int									socket_fd;
		std::map<struct pollfd, User*, PollComp>	users;
		std::vector<Channel*>						channels;
		struct pollfd								server_poll;
		static bool									signal;

		void take_str(std::string *dest, char *src);
		User *find_user(int fd);
		void print_all(int ufd, const std::string &mess, const std::string &nick);

	public:
		Server(std::string const &password, int port);
		~Server();

		void ser_socket();
		void accept_new_client();
		void receive_new_data(int fd);
		void close_server();
		void server_init();

		static void signal_handler(int signum);

		void add_user();
		void rem_user(const std::string &user);
		void add_channel(Channel *ch);
		void rem_channel(const std::string &channel);
		void send_message(const std::string &user, const std::string &message);
		bool is_user(const std::string &user) const;
		bool is_channel(const std::string &channel) const;
};

#endif
