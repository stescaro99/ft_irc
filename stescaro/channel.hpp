#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "user.hpp"

# ifndef CH
# define CH user_channels[channel]
# endif

class Channel
{
	private:
		const std::string				ch_name;
		std::map<std::string, User*>	ch_users;
		std::vector<std::string>		ch_admin;
		std::string				 		ch_topic;
		char				 			ch_mode;							// i = invite only, p = password protected, o = open 
		std::string						ch_password;
		std::vector<std::string>		ch_banned;
		std::vector<std::string>		ch_invited;

	public:
		Channel(std::string name, User *creator);
		~Channel();

		void add_user_to_channel(User *user, std::string password);
		void rem_user_from_channel(std::string user, bool is_kick);
		void c_send_message(std::string user, std::string message);
		void c_print_messages();
		void add_admin(std::string user);						
		void rem_admin(std::string user);						
		void change_topic(std::string topic);					
		void change_mode(char mode);							
		void change_password(std::string password);				
		bool is_user_admin(std::string user) const;
		void ban_user(std::string user);
		void invite_user(std::string user);									
		void unban_user(std::string user);

		//virtual void recv_message(std::string user);
		size_t get_users_count() const;
		size_t get_admins_count() const;
		char get_mode() const;
		std::string get_name() const;
};

#endif