#include "standard_libraries.hpp"

Server::Server(std::string const &password, unsigned short port) : server_name(":IRCSERV"), password(password), port(port), socket_fd(socket(AF_INET, SOCK_STREAM, 0))
{
	if (socket_fd == -1)
		throw(std::runtime_error("failed to create socket"));
}

Server::~Server()
{
	close_fds();
	for (size_t i = 0; i < channels.size(); i++)
		delete channels[i];
	channels.clear();
	for (size_t i = 0; i < users.size(); i++)
		delete users[i];
	users.clear();
	for (size_t i = 0; i < bots.size(); i++)
		delete bots[i];
	for (size_t j = 0; j < fds.size(); j++)
		close(fds[j].fd);
	if (socket_fd != 1)
		close(socket_fd);
}

Channel::Channel(const std::string &name, User *creator) : ch_name(name)
{
	ch_users[creator->get_user_name()] = creator;
	ch_admin.push_back(creator->get_user_name());
	ch_invite = false;
	ch_limit = SHRT_MAX;
	topic_only_admin = false;
	ch_bot = NULL;
	bot_name = "";
}

Channel::~Channel()
{
	ch_users.clear();
	ch_admin.clear();
}

User::User(Server &server, int fd, std::string const &priv_ip) : user_fd(fd), user_name(""), user_nickname(""), server(server), state(0), pass_tries(0), user_host(set_user_host(fd)), user_priv_ip(priv_ip)
{
	if (user_fd == -1)
		throw(std::runtime_error("failed to create user"));
}

User::~User()
{
	close(user_fd);
	user_channels.clear();
}

Bot::Bot(Server &server, int fd, Channel *channel) : User(server, fd, ""), mood(50)
{
	bot_channel = channel;
	struct sockaddr_in	usadd;
	socklen_t			len = sizeof(usadd);
	bot_fd = accept(server.get_socket_fd(), (sockaddr *)&(usadd), &len);
	if (bot_fd == -1)
	{
		std::cerr << "accept failed: " << std::strerror(errno) << std::endl;
		close(fd);
		return;
	}
	if (fcntl(bot_fd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cout << "user fcntl failed" << std::endl;
		return;
	}
	increment_state();
	increment_state();
	increment_state();
}

Bot::~Bot()
{
	bot_channel = NULL;
	close(user_fd);
	close(bot_fd);
}