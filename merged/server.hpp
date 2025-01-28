#ifndef SERVER_HPP
#define SERVER_HPP

#include "standard_libraries.hpp"

class channel;
class User;

class Server
{
	private:
		const std::string			server_name;
		const std::string			password;
		const short					port;
		const int					socket_fd;
		std::vector<User*>	  		users;
		std::vector<Channel*>		channels;
		static bool					Signal;
		std::vector<struct pollfd>	fds;

		void take_str(std::string *dest, char *src);
		void print_all(int ufd, const std::string &mess, const std::string &nick);
		User *find_user(int fd) const;
		User *find_user(const std::string &user) const;
		Channel *find_channel(const std::string &channel) const;
		void konversations(short i, std::string &s);
		void split(std::string s, const std::string &delim, std::vector<std::string> &v);
		short is_command(const std::string &s) const;
		void set_mode_utility(std::vector<std::string> const &v, std::vector<std::string> &users, short &limit, std::string &password, size_t &n, const std::string &flags);
		std::vector<std::string> split_mode(const std::string &s);
		bool parameters_check(std::vector<std::string> const &flags, std::vector<std::string> const &admin, short limit, std::string password, User *user);

		// commands
		void do_command(short cmd, User *user, std::vector<std::string> const &v);
		void join(User *user, std::vector<std::string> const &v); //mess password sbagliata,
		void part(User *user, std::vector<std::string> const &v);
		void mode(User *user, std::vector<std::string> const &v);
		void topic(User *user, std::vector<std::string> const &v);
		void kick(User *user, std::vector<std::string> const &v);
		void invite(User *user, std::vector<std::string> const &v);
		void privmsg(User *user, std::vector<std::string> const &v);
		void quit(User *user);
		//void nick(User *user, std::vector<std::string> const &v);

		// messages
		void send_join_message(Channel *ch, User *user);
		void send_part_message(Channel *ch, User *user);

	public:
		Server(std::string const &password, int port);
		~Server();

		void server_init();
		void ser_socket();
		void accept_new_client();
		void receive_new_data(int fd);
		void close_fds();

		static void signal_handler(int signum);

		void add_user();
		void rem_user(const std::string &user);
		void add_channel(Channel *ch);
		void rem_channel(const std::string &channel);
		void send_message(const std::string &user, const std::string &message);
		bool is_user(const std::string &user) const;
		bool is_nick(const std::string &nick) const;
		bool is_channel(const std::string &channel) const;
		std::string convert_to_username(std::string const &nick) const;
		
};

#endif
