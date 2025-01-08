#include "server.hpp"

Server::Server(std::string password, int port) : password(password), port(port)
{
}

Server::~Server()
{
}

Channel::Channel(std::string name) : channel_name(name)
{
}

Channel::~Channel()
{
}

User::User(std::string name, std::string password) : user_name(name), user_password(password)
{
}

User::~User()
{

	
}

