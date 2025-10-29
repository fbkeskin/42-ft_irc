/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatkeski <fatkeski@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 17:07:50 by fatkeski          #+#    #+#             */
/*   Updated: 2025/10/23 14:11:46 by fatkeski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libs/ft_irc.hpp"
#include "../libs/Server.hpp"

int checkPort(const char* port)
{
	int					num;
	std::string			str = port;
	std::stringstream	ss(str);

	ss >> num;
	if (ss.fail() || !ss.eof())
		return(-1);
	if(num < 1024 || num > 65535)
		return(-2);
	return(num);
}

std::string checkPassword(const char* password)
{
	std::string str = password;

	for(size_t i = 0; i < str.length(); i++)
	{
		if((!std::isprint(str[i])) || (str[i] == ' '))
			return("");
	}
	return(str);
}


int main(int argc, char* argv[])
{
	if(argc != 3 || argv[1][0] == '\0' || argv[2][0] == '\0')
	{
		std::cerr << "Error: Invalid Argument Input!" << std::endl;
		return (1);
	}

	int portNum = checkPort(argv[1]);
	if(portNum < 0)
	{
		if(portNum == -1)
			std::cerr << "Error: Invalid Port Format!" << std::endl;
		else if(portNum == -2)
			std::cerr << "Error: Invalid Port Range!" << std::endl;
		return (1);
	}

	std::string password = checkPassword(argv[2]);
	if(password.empty())
	{
		std::cerr << "Error: Invalid Password Format!" << std::endl;
		return (1);
	}

	Server server(portNum, password);
	server.configureServer();
	server.runServer();

	return (0);
}
