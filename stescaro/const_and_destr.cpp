#include "server.hpp"

Server::Server(const std::string &password, int port) : password(password), port(port)
{
}

Server::~Server()
{
}

Channel::Channel(const std::string &name, User *creator) : ch_name(name)
{
	ch_users.insert(std::pair<std::string, User *>(creator->get_user_name(), creator));
	ch_admin.push_back(creator->get_user_name());
}

Channel::~Channel()
{
}

User::User(const std::string &name, const std::string &nickname, const std::string &password, Server *server)
		: user_name(name), user_nickname(nickname), user_password(password), server(server)
{
}

User::~User()
{
}
