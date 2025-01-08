#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "user.hpp"
#include "standard_libraries.hpp"

class Channel
{
	private:
		static std::string				channel_name;
		std::map<std::string, &User>	channel_users;
		std::vector<std::string>		channel_admin;
		std::string				 		channel_topic;
		char				 			channel_mode;
		std::string						channel_password;
		std::vector<std::string>		channel_banned;

	public:
		Channel(std::string name, std::string creator);
		~Channel();

		void add_user_to_channel(std::string user, std::string password);
		void rem_user_from_channel(std::string user);
		void c_send_message(std::string user, std::string message);
		void c_print_messages();
		void add_admin(std::string user);
		void rem_admin(std::string user);
		void change_topic(std::string topic);
		void change_mode(std::string mode);
		void change_password(std::string password);
		
		//virtual void recv_message(std::string user);
};

#endif