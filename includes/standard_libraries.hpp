#ifndef STANDARD_LIBRARIES_HPP
#define STANDARD_LIBRARIES_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <csignal>
#include <cstring>
#include <fstream>
#include <cstdio>
#include <sstream>
#include <climits>
#include <ctime>
#include <cerrno>

#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

//#include <readline/history.h>

#include "user.hpp"
#include "channel.hpp"
#include "server.hpp"
#include "bot.hpp"

/* typedef struct	s_request
{
	std::string					nick_sender;
	std::vector<std::string>	nick_receivers;
	std::string					filename;
	unsigned short				size;
	std::string					ip;	
}				t_request;
 */
# define BOT_NAME "Bot"
# define CH user_channels[channel]
# define Red "\033[31m"
# define Green "\033[32m"
# define Yellow "\033[33m"
# define Blue "\033[34m"
# define Magenta "\033[35m"
# define Cyan "\033[36m"
# define Reset "\033[0m"

#endif
