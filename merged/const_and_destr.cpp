#include "standard_libraries.hpp"

Server::Server(std::string const &password, int port) : server_name(":IRCSERVER"), password(password), port(port), socket_fd(socket(AF_INET, SOCK_STREAM, 0))
{
	if (socket_fd == -1)
		throw(std::runtime_error("failed to create socket"));
}

Server::~Server()
{
}

Channel::Channel(const std::string &name, User *creator) : ch_name(name)
{
	ch_users.insert(std::pair<std::string, User *>(creator->get_user_name(), creator));
	ch_admin.push_back(creator->get_user_name());
	ch_invite = false;
	ch_limit = SHRT_MAX;
	topic_only_admin = false;
}

Channel::~Channel()
{
}

User::User(Server &server, int fd) : user_fd(fd), user_name(""), user_nickname(""), server(server), state(0), pass_tries(0), user_host(set_user_host(fd))
{
	if (user_fd == -1)
		throw(std::runtime_error("failed to create user"));
}

User::~User()
{
}
