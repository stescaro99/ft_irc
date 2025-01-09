#include "server.hpp"

void Server::ClearUser(int fd)
{
	for (size_t i = 0; i < fds.size(); i++)
	{
		if (fds[i].fd == fd)
		{
			fds.erase(fds.begin() + i);
			break;
		}
	}
	for (size_t i = 0; i < users.size(); i++)
	{
		if (users[i].getFd() == fd)
		{
			users.erase(users.begin() + i);
			break;
		}
	}
}

bool Server::Signal = false;

void Server::SignalHandler(int signum)
{
	(void)signum;
	std::cout << "a wild signal appared" << std::endl;
	Server::Signal = true;
}

void Server::CloseFds()
{
	for (size_t i = 0; i < users.size(); i++)
	{
		std::cout << "User <" << users[i].getname() << "> disconnected" << std::endl;
		close(users[i].getFd());
	}
	if (socketfd != 1)
	{
		std::cout << "close server" << std::endl;
		close(socketfd);
	}
}

Server::Server()
{
	socketfd = -1;
}

void Server::serSocket()
{
	struct	sockaddr_in	add;
	struct	pollfd		NewPoll;
	add.sin_family = AF_INET;
	add.sin_port = htons(this->port);
	add.sin_addr.s_addr = INADDR_ANY;

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd == -1)
		throw(std::runtime_error("faild to create soket"));
	int en = 1;
	if(setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
		throw(std::runtime_error("faild to set on soket"));
	if(fcntl(socketfd, F_SETFL, O_NONBLOCK) == -1)
		throw(std::runtime_error("faild to set NONBLOCK on soket"));
	if (bind(socketfd, (struct sockaddr *)&add, sizeof(add)) == -1)
		throw(std::runtime_error("faild to bind soket"));
	if(listen(socketfd, SOMAXCONN) == -1)
		std::runtime_error("lissen faild");
	
	NewPoll.fd = socketfd;
	NewPoll.events = POLLIN;
	NewPoll.revents = 0;
	fds.push_back(NewPoll);

}

void Server::ServerInit()
{
	port = 4444;

	serSocket();

	std::cout << "server connected" << std::endl;
	while(Server::Signal == false)
	{
		if ((poll(&fds[0],fds.size(), -1) == -1) && Server::Signal == false)
			throw(std::runtime_error("poll faild"));
		for (size_t i = 0; i < fds.size(); i++)
		{
			if (fds[i].revents & POLLIN)
			{
				if (fds[i].fd == socketfd)
					AcceptNewClient();
				else
					ReceiveNewData(fds[i].fd);
			}
		}
	}
	CloseFds();
}

void Server::AcceptNewClient()
{
	User user;
	struct sockaddr_in	usadd;
	struct pollfd		NewPoll;
	socklen_t			len = sizeof(usadd);

	int incofd = accept(socketfd, (sockaddr *)&(usadd), &len);
	if (incofd == -1)
		{
			std::cout << "accept failed" << std::endl;
			return;
		}
	if (fcntl(incofd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cout << "user fcntl failed" << std::endl;
		return;
	}

	NewPoll.fd = incofd;
	NewPoll.events = POLLIN;
	NewPoll.revents = 0;

	user.setFd(incofd);
	user.setIpAdd(inet_ntoa((usadd.sin_addr)));
	users.push_back(user);
	fds.push_back(NewPoll);

	std::cout << "client <" << incofd << "> is connect" << std::endl;

}

void Server::ReceiveNewData(int fd)
{
	char buff[1024];
	memset(buff, 0, sizeof(buff));

	ssize_t bytes = recv(fd, buff, sizeof(buff) - 1, 0);

	if (bytes <= 0)
	{
		std::cout << "user " << fd << "disconnected" << std::endl;
		ClearUser(fd);
		close(fd);
	}
	else
	{
		buff[bytes] = '\0';
		std::cout << "user <" << fd << "> data:" << buff;
	}
}