#include "standard_libraries.hpp"

static bool is_valid_password(const char *s)
{
	if (isspace(s[0]))
		return false;
	for (size_t i = 0; s[i]; i++)
	{
		if (s[i] < 32 || s[i] > 126 || s[i] == '\n' || s[i] == '\r')
			return false;
	}
	return true;
}

static bool is_path_or_home(const std::string &s)
{
	if (s.size() < 4)
		return false;
	if (s.substr(0, 4) == "PWD=" || s.substr(0, 5) == "HOME=")
		return true;
	return false;
}

static std::vector<std::string> env_vec(char **env)
{
	std::vector<std::string> envs;
	for (size_t i = 0; env[i]; i++)
	{
		if (is_path_or_home(env[i]))
			envs.push_back(env[i]);
	}
	return envs;
}

int main(int argc, char **argv, char **env)
{
	if (argc != 3)
	{
		std::cerr << "Usage: ./ircserv <port> <server_password>" << std::endl;
		return 1;
	}
	std::vector<std::string> envs = env_vec(env);
	if (envs.size() != 2 || envs[0].size() < 5 || envs[1].size() < 6)
	{
		std::cerr << "Please set PWD and HOME in your environment." << std::endl;
		return 1;
	}
	std::stringstream ss;
	ss << argv[1];
	unsigned short port;
	ss >> port;
	if (ss.fail() || port < 1025)
	{
		std::cerr << "Invalid port! Please use a number between 1025 and 65535." << std::endl;
		return 1;
	}
	std::string password = argv[2];
	if (!is_valid_password(argv[2]) || password == "")
	{
		std::cerr << "Invalid password" << std::endl;
		return 1;
	}
	Server server(password, port, envs);
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
