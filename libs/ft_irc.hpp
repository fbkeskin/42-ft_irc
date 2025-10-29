/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatkeski <fatkeski@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 17:40:07 by fatkeski          #+#    #+#             */
/*   Updated: 2025/10/23 21:25:53 by fatkeski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_IRC_HPP
#define FT_IRC_HPP

#include <iostream>
#include <string>
#include <sstream>

#include <vector>
#include <set>
#include <map>
#include <algorithm>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <cerrno>	// errno
#include <cstring>	// strerror
#include <cstdlib>	// exit
#include <cctype>

#include <csignal>

#include <fcntl.h> // fcntl()
#include <unistd.h> // fcntl()

#include "ft_defines.hpp"

// IRC chars
#define IRC_COLOR_CHAR  "\x03"
#define IRC_RESET_CHAR  "\x0F"
#define IRC_COLOR_GREEN "03"
#define IRC_COLOR_RED   "04"

// ANSI chars
#define ANSI_COLOR_GREEN "\033[32m"
#define ANSI_COLOR_RED   "\033[31m"
#define ANSI_RESET_CHAR  "\033[0m"

#define MAX_CLIENT_COUNT 25
#define NICKNAME_SIZE 30

class Client;
class Server;
class Message;
class Channel;

void	printErrorMessage(const std::string& funcName);
void sendStatusMessage(int clientFd, const std::string& msg, bool isNC, bool status);
void	sendNumericReply(int clientFd, const std::string& serverName, const std::string& code, const std::string& nickname, const std::string& message);

int		setNonBlocking(int sd);

std::string mapString(const std::string& str);

std::string executePASS(Server& server, Client& client, std::vector<std::string>& params, std::string password);
std::string executeNICK(Server& server, Client& client, std::vector<std::string>& params);
std::string executeUSER(Server& server, Client& client, Message& msg, std::vector<std::string>& params);
std::string executeJOIN(Server& server, Client& client, std::vector<std::string>& params);
std::string executeQUIT(Server& server, Client& client, Message& msg);
std::string executeWHO(Server& server, Client& client, std::vector<std::string>& params);
std::string executeMODE(Server& server, Client& client, Message& msg, std::vector<std::string> params);
std::string executeTOPIC(Server& server, Client& client, Message& msg, std::vector<std::string> params);
std::string executeINVITE(Server& server, Client& client, Message& msg, std::vector<std::string> params);
std::string executeKICK(Server& server, Client& client, Message& msg, std::vector<std::string> params);
std::string executePART(Server& server, Client& client, Message& msg, std::vector<std::string> params);
std::string executePRIVMSG(Server& server, Client& client, Message& msg, std::vector<std::string> params);


#endif
