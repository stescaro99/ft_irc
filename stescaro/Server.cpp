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
		if (users[i]->getFd() == fd)
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
	std::cout << " a wild signal appared" << std::endl;
	Server::Signal = true;
}

void Server::CloseFds()
{
	for (size_t i = 0; i < users.size(); i++)
	{
		std::cout << Red << "User <" << users[i]->getname() << "> disconnected" << Reset << std::endl;
		close(users[i]->getFd());
	}
	if (socketfd != 1)
	{
		std::cout << Magenta << "close server" << Reset << std::endl;
		close(socketfd);
	}
}

Server::Server()
{
	socketfd = -1;
}

Server::Server(std::string password, int port): password(password), port(port)
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

void Server::takeStr(std::string *dest, char *src)
{
	*dest = src;
	size_t i = dest->find("\n");
	if (i != std::string::npos)
		*dest = dest->substr(0, i);
	//std::cout << "a" <<std::endl;
	std::fill(src, src + strlen(src), '\0');
}

void Server::AcceptNewClient()
{
	std::string name, UsPassword, nick;
	User *user = new User();
	struct sockaddr_in	usadd;
	struct pollfd		NewPoll;
	socklen_t			len = sizeof(usadd);

	int incofd = accept(socketfd, (sockaddr *)&(usadd), &len);
	if (incofd == -1)
		{
			std::cout << "accept failed" << std::endl;
			return;
		}

	NewPoll.fd = incofd;
	NewPoll.events = POLLIN;
	NewPoll.revents = 0;

	//if (fcntl(incofd, F_SETFL, O_NONBLOCK) == -1)
	//{
	//	std::cout << "user fcntl failed" << std::endl;
	//	return;
	//}

	//char buff[1000];
	//memset(buff, 0, sizeof(buff));
	//int PTry = 0;
	//while(1)
	//{
	//	write(incofd, "Insert password\n", 17);
	//	recv(incofd, buff, sizeof(buff) -1, 0);
	//	takeStr(&UsPassword, buff);
	//	if (UsPassword != password)
	//	{
	//		PTry++;
	//		write(incofd, "Incorrect password\n", 20);
	//		if (PTry == 3)
	//		{
	//			write(incofd, "too many tries\n", 15);
	//			close(incofd);
	//			return ;
	//		}
	//		continue ;
	//	}
	//	break ;
	//}
	//write(incofd, "select a name\n", 15); 
	//recv(incofd, buff, sizeof(buff) -1, 0);
	//takeStr(&name, buff);
	//write(incofd, "select a nickname\n", 19);
	//recv(incofd, buff, sizeof(buff) - 1, 0);
	//takeStr(&nick, buff);
	//			// provare a cancellare terminale
	user->setFd(incofd);
	user->setIpAdd(inet_ntoa((usadd.sin_addr)));
	//user->setName(name);
	//user->setNick(nick);
	
	if (fcntl(incofd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cout << "user fcntl failed" << std::endl;
		return;
	}

	users.push_back(user);
	fds.push_back(NewPoll);

	write(incofd, "Insert password\n", 17);	
	std::cout  << Green << "client <" << incofd << "> is connect" << Reset << std::endl;

}

User *Server::findUser(int fd)
{
	for (std::vector<User *>::iterator i = users.begin(); i != users.end(); i++)
		if ((*i)->getFd() == fd)
			return(*i);
	std::cout << "error" << std::endl;
	return (NULL);
}

void Server::printall(int Usfd, std::string mess, std::string nick)
{
	for (std::vector<User *>::iterator i = users.begin(); i != users.end(); i++)
	{
		if ((*i)->getFd() != Usfd)
		{
			write((*i)->getFd(), nick.c_str(), nick.length());
			write((*i)->getFd(), mess.c_str(), mess.length());
		}
	}
}

void Server::ReceiveNewData(int fd)
{
	char buff[1024];
	memset(buff, 0, sizeof(buff));

	ssize_t bytes = recv(fd, buff, sizeof(buff) - 1, 0);
	User *i = findUser(fd);
	if (bytes <= 0)
	{
		std::cout << Red << "user " << i->getNick() << " disconnected"  << Reset << std::endl;
		ClearUser(fd);
		close(fd);
	}
	std::string UsPassword, name, nick;
	static int PTry = 0;
	switch (i->getState())
	{
	case 0:
		while(1)
		{
			//write(i->getFd(), "Insert password\n", 17);
			takeStr(&UsPassword, buff);
			if (UsPassword != password)
			{
				PTry++;
				write(i->getFd(), "Incorrect password\n", 20);
				if (PTry == 3)
				{
					write(i->getFd(), "too many tries\n", 15);
					close(i->getFd());
					return ;
				}
				write(i->getFd(), "Insert password\n", 17);
				continue ;
			}
			else
			{
				write(i->getFd(), "select a name\n", 15);
				i->plusState();
			}
			break ;
		}
		break;
	case 1:
		//write(i->getFd(), "select a name\n", 15); 
		takeStr(&name, buff);
		i->setName(name);
		i->plusState();
		write(i->getFd(), "select a nickname\n", 19);
		break;
	case 2:
		//write(i->getFd(), "select a nickname\n", 19);
		takeStr(&nick, buff);
		i->setNick(nick);
		i->plusState();
		break;
	case 3:
		buff[bytes] = '\0';
		std::cout << Yellow << "user <" << i->getNick() << "> data:"  << Reset << buff;
		break;
	}
}