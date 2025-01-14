#include "user.hpp"
#include "server.hpp"
#include <sstream>

int main(int ac, char *ag[])
{
	std::string  str(ag[1]), str2(ag[2]);
	std::stringstream ss;
	ss << str2;
	int port;
	ss >> port;
	Server ser(str, port);
	std::cout <<"----SERVER----" << port <<std::endl;

	try
	{
		signal(SIGINT, Server::SignalHandler);
		signal(SIGQUIT, Server::SignalHandler);
		ser.ServerInit();
	}
	catch(const std::exception& e)
	{
		ser.CloseFds();
		std::cerr << e.what() << '\n';
	}
	
}