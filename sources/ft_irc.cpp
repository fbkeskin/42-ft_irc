/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatkeski <fatkeski@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 14:01:22 by fatkeski          #+#    #+#             */
/*   Updated: 2025/10/23 16:49:10 by fatkeski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libs/ft_irc.hpp"

void printErrorMessage(const std::string& funcName)
{
	std::string errMsg = std::strerror(errno);

	std::cerr << "[ERROR] " << funcName << " failed: " << errMsg << " (errno=" << errno << ")" << std::endl;
}

int setNonBlocking(int sd)
{
	int flags = fcntl(sd, F_GETFL);
	if(flags == -1)
		return(-1);
	flags = flags | O_NONBLOCK;
	int ret = fcntl(sd, F_SETFL, flags);

	return(ret);
}

void sendStatusMessage(int clientFd, const std::string& msg, bool status)
{
	std::string colorMsg;

	if (status)
		colorMsg = IRC_COLOR_CHAR + std::string(IRC_COLOR_GREEN) + "[OK] " + msg + IRC_RESET_CHAR + "\r\n";
	else
		colorMsg = IRC_COLOR_CHAR + std::string(IRC_COLOR_RED) + "[ERROR] " + msg + IRC_RESET_CHAR + "\r\n";

	send(clientFd, colorMsg.c_str(), colorMsg.length(), 0);
}

void sendStatusMessage(int clientFd, const std::string& msg, bool isNC, bool status)
{
	std::string colorMsg;

	if (isNC)
	{
		if (status)
			colorMsg = std::string(ANSI_COLOR_GREEN) + "[OK] " + msg + ANSI_RESET_CHAR+ "\r\n";
		else
			colorMsg = std::string(ANSI_COLOR_RED) + "[ERROR] " + msg + ANSI_RESET_CHAR + "\r\n";
	}
	else // HexChat
	{
		if (status)
			colorMsg = IRC_COLOR_CHAR + std::string(IRC_COLOR_GREEN) + "[OK] " + msg + IRC_RESET_CHAR + "\r\n";
		else
			colorMsg = IRC_COLOR_CHAR + std::string(IRC_COLOR_RED) + "[ERROR] " + msg + IRC_RESET_CHAR + "\r\n";
	}
	send(clientFd, colorMsg.c_str(), colorMsg.length(), 0);
}

std::string mapString(const std::string& str)
{
	std::string mapped_nick = str;

	std::transform(mapped_nick.begin(), mapped_nick.end(), mapped_nick.begin(), (int(*)(int))std::tolower);

	return (mapped_nick);
}

// :<sunucu_adı> <hata_kodu> <istemci_nick> <komut_veya_ilgili_parametreler> :<hata_mesajı>
void sendNumericReply(int clientFd, const std::string& serverName, const std::string& code, const std::string& nickname, const std::string& message)
{
	std::ostringstream reply;
	reply << ":" << serverName << " " << code << " " << nickname << " :" << message << "\r\n";

	std::string replyStr = reply.str();
	send(clientFd, replyStr.c_str(), replyStr.length(), 0);
}


