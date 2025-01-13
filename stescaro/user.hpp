#ifndef USER_HPP
# define USER_HPP

# include "standard_libraries.hpp"

# ifndef CHANNEL_HPP
#  include "channel.hpp"
# endif
# ifndef SERVER_HPP
#  include "server.hpp"
# endif

class User
{
	private:
		const std::string		 		user_name;
		std::string		 				user_nickname;
		std::string		 				user_password;
		std::map<std::string, Channel*>	user_channels;
		Server							*server;

	public:
		User(std::string name, std::string nickname, std::string password);
		~User();

		void change_my_nickname(std::string nickname);
		void change_my_password(std::string password);
		void create_channel(std::string channel, std::string password);		// diventi admin
		void delete_channel(std::string channel);							// admin solo se e' l'unico admin
		void join_channel(Channel *channel, std::string password);		// invito o password se necessario
		void leave_channel(std::string channel);							// se e' l'unico admin, il potere passa al primo entrato
		void kick_user(std::string user, std::string channel);			  	// admin e non contro admin
		void ban_user(std::string user, std::string channel);			  	// admin e non contro admin
		void invite_user(std::string user, std::string channel);			// admin e non su un ban
		void change_topic(std::string topic, std::string channel);		  	// admin 
		void change_mode(char mode, std::string channel);					// admin
		void change_password(std::string password, std::string channel);	// admin
		void add_admin(std::string user, std::string channel);				// admin, diventano pari
		void unban_user(std::string user, std::string channel);			 	// admin
		void rem_message(std::string channel, std::string message);  		// non so se si puo' fare
		void leave_admin(std::string channel);								// admin che perde i propri poteri, se e' l'unico admin non puo' farlo
		void accept_invite(std::string channel);							// se e' stato invitato
		//void u_send_message(std::string message);
		//void u_print_messages();
		////virtual void u_recv_message(std::string user); ???????

		std::string get_user_name() const;
		
};

#endif