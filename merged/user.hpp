#ifndef USER_HPP
# define USER_HPP

# include "standard_libraries.hpp"

class Channel;
class Server;

class User
{
	private:
		const int						user_fd;
		std::string		 				user_name;
		std::string		 				user_nickname;
		std::map<std::string, Channel*>	user_channels;
		Server							&server;
		short 							state;			// 0 = passw, 1 = nick, 2 = user, 3 = scrittura
		short 							pass_tries;
		char							buff[1024];
		const std::string				user_host;
	
	public:
		User(Server &server, int fd);
		~User();

		void change_my_nickname(const std::string &nickname);
		void create_channel(const std::string &channel, const std::string &password);
		void join_channel(Channel *channel, const std::string &password);
		void leave_channel(const std::string &channel);

		std::string get_user_name() const;
		std::string get_user_nick() const;
		int get_user_fd() const;
		short get_state() const;
		short get_tries() const;
		char *get_buff();
		std::string get_user_host() const;

		std::string set_user_host(int fd) const;
		void set_user_name(const std::string &name);
		void set_user_nick(const std::string &nick);
		void set_user_state(short st);
		void increment_tries();
		void increment_state();

		void memset_buff();
};

#endif
