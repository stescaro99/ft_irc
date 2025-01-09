#include "server.hpp"

Server::Server(std::string password, int port) : password(password), port(port)
{
}

Server::~Server()
{
}

Channel::Channel(std::string name, User &creator) : channel_name(name)
{
	ch_users[creator.get_user_name()] = creator;
	ch_admin.push_back(creator.get_user_name());
}

Channel::~Channel()
{
}

User::User(std::string name, std::string nickname, std::string password) : user_name(name), user_password(password), user_nickname(nickname)
{
}

User::~User()
{
}

