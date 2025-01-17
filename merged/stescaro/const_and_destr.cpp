#include "server.hpp"

Server::Server(std::string const &password, int port) : password(password), port(port)
{
	socket_fd = -1;
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

User::User(Server &server) : user_name(""), user_nickname(""), user_fd(-1), server(server), state(0), pass_tries(0)
{
}

User::User(const std::string &name, const std::string &nickname, Server &server, int fd)
		: user_name(name), user_nickname(nickname), server(server), user_fd(fd)
{
}

User::~User()
{
}
