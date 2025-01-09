#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "user.hpp"

# ifndef CH
# define CH user_channels[channel]
# endif

class Channel
{
	private:
		static std::string				ch_name;
		std::map<std::string, &User>	ch_users;
		std::vector<std::string>		ch_admin;
		std::string				 		ch_topic;
		char				 			ch_mode;			// i = invite only, p = password protected, o = open 
		std::string						ch_password;
		std::vector<std::string>		ch_banned;
		std::vector<std::string>		ch_invited;

	public:
		Channel(std::string name, User &creator);
		~Channel();

		void add_user_to_channel(User &user, std::string password);			//fatto
		void rem_user_from_channel(std::string user);						//fatto
		void c_send_message(std::string user, std::string message);
		void c_print_messages();
		void add_admin(std::string user);									//fatto
		void rem_admin(std::string user);									//fatto
		void change_topic(std::string topic);								//fatto
		void change_mode(char mode);										//fatto
		void change_password(std::string password);							//fatto
		
		//virtual void recv_message(std::string user);
};

#endif