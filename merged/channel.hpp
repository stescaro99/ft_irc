#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "standard_libraries.hpp"

class User;
class channel;
class Bot;

class Channel
{
	private:
		const std::string				ch_name;
		std::map<std::string, User*>	ch_users;
		std::vector<std::string>		ch_admin;
		std::string				 		ch_topic;
		bool				 			ch_invite;
		std::string						ch_password;
		std::vector<std::string>		ch_invited;
		short							ch_limit;
		bool							topic_only_admin;
		Bot								*ch_bot;
		std::string						bot_name;

	public:
		Channel(const std::string &name, User *creator);
		~Channel();

		void add_user_to_channel(User *user, const std::string &password);
		void rem_user_from_channel(const std::string &user, bool is_kick);
		void add_admin(const std::string &user);
		void rem_admin(const std::string &user);
		void change_topic(const std::string &topic);
		void change_mode(bool mode);
		void change_password(const std::string &password);
		void change_limit(short limit);
		void invite_user(const std::string &user);
		
		void c_send_message(const std::string &user, const std::string &message, bool not_usr) const;
		void channel_info(User *user) const;

		size_t get_users_count() const;
		size_t get_admins_count() const;
		bool get_mode() const;
		std::string get_name() const;
		std::string get_topic() const;
		bool get_topic_only_admin() const;
		std::string get_password() const;
		short get_limit() const;
		std::string get_new_admin() const;
		Bot *get_bot() const;
		std::string get_bot_name() const;

		bool is_user_admin(const std::string &user) const;
		bool is_user_invited(const std::string &user) const;
		bool is_user_inside(const std::string &user) const;
		std::string get_users_list() const;

		void set_topic_admin(bool only_admin);
		void set_bot(Bot *bot);

		void bot_join(User *user, Bot *bot, std::string b_name);
		void bot_kick(User *user);
		void game_bot(User *user);
		void time_bot();
		void quote_bot();
		void six_bus_bot();
		void help_bot(User *user);
		void panna_bot();
		void wrong_bot(User *user, short code);
		void sorry_bot();
		void increment_bot_mood(short max);
};

#endif
