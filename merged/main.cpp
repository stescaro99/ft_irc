#include "standard_libraries.hpp"

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		return 1;
	}
	std::stringstream ss;
	ss << argv[2];
	short port;
	ss >> port;
	if (ss.fail())
	{
		return 1;
	}
	Server server(argv[1], port);
	std::cout <<"----SERVER----" << port <<std::endl;

	try
	{
		signal(SIGINT, Server::signal_handler);
		signal(SIGQUIT, Server::signal_handler);
		server.server_init();
	}
	catch(const std::exception& e)
	{
		server.close_server();
		std::cerr << e.what() << '\n';
	}
	return 0;
}