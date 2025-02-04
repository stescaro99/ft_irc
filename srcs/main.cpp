#include "standard_libraries.hpp"

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: ./ircserv <port> <server_password>" << std::endl;
		return 1;
	}
	std::stringstream ss;
	ss << argv[1];
	unsigned short port;
	ss >> port;
	if (ss.fail() || port < 1024)
	{
		std::cerr << "Invalid port! Please use a number between 1025 and 65535." << std::endl;
		return 1;
	}
	std::string password = argv[2];
	if (password == "\127")
	{
		std::cerr << "Invalid password" << std::endl;
		return 1;
	}
	Server server(password, port);
	std::cout <<"----SERVER----" <<std::endl;

	try
	{
		signal(SIGINT, Server::signal_handler);
		signal(SIGQUIT, Server::signal_handler);
		server.server_init();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return 0;
}
