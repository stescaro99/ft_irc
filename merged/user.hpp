#ifndef USER_HPP
# define USER_HPP

# include "standard_libraries.hpp"

class User
{
	private:
		int								user_fd;
		std::string		 				user_name;
		std::string		 				user_nickname;
		std::string						IPadd;
		std::map<std::string, Channel*>	user_channels;
		Server							&server;
		short 							state;
		short 							pass_tries;
	
	public:
		User(Server &server, int fd);
		User(const std::string &name, const std::string &nickname, Server &server, int fd);
		~User();

		//void change_color(const std::string &color);    								// cambi il nick aggiungendo il colore iniziale e il reset finale
		void change_my_nickname(const std::string &nickname);
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
		std::string get_user_nick() const;
		int get_user_fd() const;
		std::string get_IPadd() const;
		short get_state() const;
		short get_tries() const;

		void set_user_name(const std::string &name);
		void set_user_nick(const std::string &nick);
		void set_user_IPadd(const std::string &ipadd);
		void set_user_state(short st);
		void increment_tries();
		void increment_state();
};

#endif
