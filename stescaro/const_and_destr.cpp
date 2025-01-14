#include "server.hpp"

Server::Server(const std::string &password, int port, int fd) : password(password), port(port), fd(fd)
{
}

Server::~Server()
{
}

Channel::Channel(const std::string &name, User *creator) : ch_name(name), ch_fd(55)    // cambiare con funzione adatta
{
	ch_users.insert(std::pair<std::string, User *>(creator->get_user_name(), creator));
	ch_admin.push_back(creator->get_user_name());
}

Channel::~Channel()
{
}

User::User(const std::string &name, const std::string &nickname, Server &server, int fd)
		: user_name(name), user_nickname(nickname), server(server), user_fd(fd)
{
}

User::~User()
{
}
