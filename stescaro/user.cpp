#include "user.hpp"

//void User::change_my_password(std::string password)
//{
//	user_password = password;
//}

std::string User::getname()
{
	return user_name;
}

void	User::setName(std::string str)
{
	this->user_name = str;
}
void	User::setNick(std::string str)
{
	this->user_nikname = str;
}
void	User::setPass(std::string str)
{
	this->user_password = str;
}

std::string User::getNick()
{
	return(user_nikname);
}
User::User()
{
	state = 0;
}

short User::getState()
{
	return(state);
}

void User::plusState()
{
	state += 1;
}

User::User(std::string name, std::string nick): user_name(name), user_nikname(nick)
{

}

//void User::create_channel(std::string channel, std::string password)
//{
//	Channel new_channel(channel, this->user_name);
//	if (password && password != "")
//		new_channel.change_password(password);
//	user_channels.insert(std::pair<std::string, Channel>(channel, new_channel));
//}

//void User::delete_channel(std::string channel)
//{
//	if (user_channels.find(channel) != user_channels.end()
//		&& user_channels[channel].channel_admin.size() == 1
//		&& user_channels[channel].channel_admin[0] == this->user_name)
//	{
//		user_channels[channel].~Channel();
//		user_channels.erase(channel);
//	}
//}

//void User::join_channel(std::string channel, std::string password)
//{
//	//c'è da controllare che esista il canale e che la password sia corretta e che non sia tra i bannati

//}

//void User::leave_channel(std::string channel)
//{
//	if (user_channels.find(channel) != user_channels.end())
//	{
//		if (user_channels[channel].size() == 1)
//			user_channels.erase(channel);
//		else
//		{
//			for (std::vector<std::string>::iterator it = user_channels[channel].channel_admin.begin(); it != user_channels[channel].channel_admin.end(); ++it)
//			{
//				if (*it == this->user_name)
//				{
//					if (user_channels[channel].channel_admin.size() == 1 && user_channels[channel].channel_users[0] != this->user_name)
//						user_channels[channel].add_admin(user_channels[channel].channel_users[0]);
//					else if (user_channels[channel].channel_admin.size() == 1)
//						user_channels[channel].add_admin(user_channels[channel].channel_users[1]);
//					user_channels[channel].rem_admin(this->user_name);
//					break;
//				}
//			}
//			user_channels[channel].rem_user_from_channel(this->user_name);
//			user_channels.erase(channel);
//		}
//	}
//}

//void User::kick_user(std::string user, std::string channel)
//{
//	if (user_channels.find(channel) != user_channels.end())
//	{
//		for (std::vector<std::string>::iterator it = user_channels[channel].channel_admin.begin(); it != user_channels[channel].channel_admin.end(); ++it)
//		{
//			if (*it == user)
//				return ;
//		}
//	}
//	if (user_channels.find(channel) != user_channels.end())
//	{
//		for (std::vector<std::string>::iterator it = user_channels[channel].channel_admin.begin(); it != user_channels[channel].channel_admin.end(); ++it)
//		{
//			if (*it == this->user_name)
//			{
//				user_channels[channel].rem_user_from_channel(user);
//				break;
//			}
//		}
//	}
//}

//void User::ban_user(std::string user, std::string channel)
//{
//	if (user_channels.find(channel) != user_channels.end())
//	{
//		for (std::vector<std::string>::iterator it = user_channels[channel].channel_admin.begin(); it != user_channels[channel].channel_admin.end(); ++it)
//		{
//			if (*it == user)
//				return ;
//		}
//	}
//	if (user_channels.find(channel) != user_channels.end())
//	{
//		for (std::vector<std::string>::iterator it = user_channels[channel].channel_admin.begin(); it != user_channels[channel].channel_admin.end(); ++it)
//		{
//			if (*it == this->user_name)
//			{
//				user_channels[channel].rem_user_from_channel(user);
//				user_channels[channel].channel_banned.push_back(user);
//				break;
//			}
//		}
//	}
//}

void User::setFd(int fd)
{
	this->Fd = fd; 
}

int	 User::getFd()
{
	return(Fd);
}

void User::setIpAdd(std::string ipadd)
{
	IPadd = ipadd;
}

