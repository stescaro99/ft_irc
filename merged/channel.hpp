#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "standard_libraries.hpp"

class User;
class channel;

class Channel
{
	private:
		const std::string				ch_name;
		std::map<std::string, User*>	ch_users;
		int								ch_fd;								//poi lo castiamo a const
		std::vector<std::string>		ch_admin;
		std::string				 		ch_topic;
		char				 			ch_mode;							// i = invite only, p = password protected, o = open 
		std::string						ch_password;
		std::vector<std::string>		ch_banned;
		std::vector<std::string>		ch_invited;

	public:
		Channel(const std::string &name, User *creator);
		~Channel();

		void add_user_to_channel(User *user, const std::string &password);
		void rem_user_from_channel(const std::string &user, bool is_kick);
		void c_send_message(const std::string &user, const std::string &message);
		void c_print_messages();
		void add_admin(const std::string &user);						
		void rem_admin(const std::string &user);						
		void change_topic(const std::string &topic);					
		void change_mode(char mode);							
		void change_password(const std::string &password);				
		bool is_user_admin(const std::string &user) const;
		void ban_user(const std::string &user);
		void invite_user(const std::string &user);									
		void unban_user(const std::string &user);

		//virtual void recv_message(const std::string &user);
		size_t get_users_count() const;
		size_t get_admins_count() const;
		char get_mode() const;
		std::string get_name() const;
};

#endif
