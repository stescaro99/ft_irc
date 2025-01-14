#ifndef USER_HPP
# define USER_HPP

# include "standard_libraries.hpp"

class User
{
	private:
		const std::string		 		user_name;
		std::string		 				user_nickname;
		std::string		 				user_password;
		std::map<std::string, Channel*>	user_channels;
		Server							*server;
		const int						fd;

	public:
		User(const std::string &name, const std::string &nickname, const std::string &password, Server *server, int fd);
		~User();

		void change_my_nickname(const std::string &nickname);
		void change_my_password(const std::string &password);
		void create_channel(const std::string &channel, const std::string &password);	// diventi admin
		void delete_channel(const std::string &channel);								// admin solo se e' l'unico admin
		void join_channel(Channel *channel, const std::string &password);				// invito o password se necessario
		void leave_channel(const std::string &channel);									// se e' l'unico admin, il potere passa al primo entrato
		void kick_user(const std::string &user, const std::string &channel);			// admin e non contro admin
		void ban_user(const std::string &user, const std::string &channel);			  	// admin e non contro admin
		void invite_user(const std::string &user, const std::string &channel);			// admin e non su un ban
		void change_topic(const std::string &topic, const std::string &channel);		// admin 
		void change_mode(char mode, const std::string &channel);						// admin
		void change_password(const std::string &password, const std::string &channel);	// admin
		void add_admin(const std::string &user, const std::string &channel);			// admin, diventano pari
		void unban_user(const std::string &user, const std::string &channel);			// admin
		void rem_message(const std::string &channel, const std::string &message);  		// non so se si puo' fare
		void leave_admin(const std::string &channel);									// admin che perde i propri poteri, se e' l'unico admin non puo' farlo
		void accept_invite(const std::string &channel);									// se e' stato invitato
		//void u_send_message(const std::string &message);
		//void u_print_messages();
		////virtual void u_recv_message(const std::string &user); ???????

		std::string get_user_name() const;
};

#endif
