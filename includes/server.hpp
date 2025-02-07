#ifndef SERVER_HPP
#define SERVER_HPP

#include "standard_libraries.hpp"

class channel;
class User;
class Bot;
typedef struct	s_request t_request;

class Server
{
	private:
		const std::string			server_name;
		const std::string			password;
		const unsigned short		port;
		const int					socket_fd;
		std::vector<User*>	  		users;
		std::vector<Channel*>		channels;
		static bool					Signal;
		std::vector<struct pollfd>	fds;
		std::vector<Bot*>			bots;

		void take_str(std::string *dest, char *src);
		void print_all(int ufd, const std::string &mess, const std::string &nick);
		void konversations(short i, std::string &s);
		std::string get_bot_nick(const std::string &nick, const std::string &bot_name);
		short is_command(const std::string &s) const;
		void set_mode_utility(std::vector<std::string> const &v, std::vector<std::string> &users, short &limit, std::string &password, size_t &n, const std::string &flags);
		std::vector<std::string> split_mode(const std::string &s);
		bool parameters_check(std::vector<std::string> const &flags, std::vector<std::string> const &admin, short limit, std::string password, User *user, Channel *ch);

		// commands
		void do_command(short cmd, User *user, std::vector<std::string> const &v);
		void join(User *user, std::vector<std::string> const &v);
		void part(User *user, std::vector<std::string> const &v);
		void mode(User *user, std::vector<std::string> const &v);
		void topic(User *user, std::vector<std::string> const &v);
		void kick(User *user, std::vector<std::string> const &v);
		void invite(User *user, std::vector<std::string> const &v);
		void privmsg(User *user, std::vector<std::string> const &v);
		void command_bot(Channel *ch, User *user, std::string const &command);
		void create_bot(Channel *ch, User *user, std::string const &nick);
		void dcc(User *user, std::vector<std::string> const &v);
		void quit(User *user);

	public:
		Server(std::string const &password, unsigned short port);
		~Server();

		void server_init();
		void ser_socket();
		void receive_new_data(int fd);
		void close_fds();

		static void signal_handler(int signum);
		static void send_join_message(Channel *ch, User *user);
		static void send_part_message(Channel *ch, User *user);
		static void split(std::string s, const std::string &delim, std::vector<std::string> &v);

		void add_user();
		void rem_user(const std::string &user);
		void add_channel(Channel *ch);
		void rem_channel(const std::string &channel);
		bool is_user(const std::string &user) const;
		bool is_nick(const std::string &nick) const;
		bool is_bot(const std::string &nick) const;
		void bot_info(Channel *ch, User *user, std::string const &jcmd);
		bool is_channel(const std::string &channel) const;
		std::string convert_to_username(std::string const &nick) const;
		
		User *find_user(int fd) const;
		User *find_user(const std::string &user) const;
		Channel *find_channel(const std::string &channel) const;
		Bot *find_bot(const std::string &nick) const;
		int get_socket_fd() const;

		std::string get_channels_list() const;
		std::string get_users_list() const;
};

#endif
