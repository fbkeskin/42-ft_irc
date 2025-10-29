/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatkeski <fatkeski@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 18:35:17 by fatkeski          #+#    #+#             */
/*   Updated: 2025/10/23 21:55:34 by fatkeski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libs/ft_irc.hpp"
#include "../libs/Server.hpp"
#include "../libs/Client.hpp"

bool isValidNickname(const std::string& nick)
{
	if (nick.empty() || nick.size() > NICKNAME_SIZE)
		return (false);

	if (nick[0] == '#' || nick[0] == '&' || nick[0] == ':')
		return (false);

	for (size_t i = 0; i < nick.size(); ++i)
	{
		char c = nick[i];
		if (!std::isalnum(static_cast<unsigned char>(c)) && (c != '-') && (c != '[') && (c != ']') && (c != '\\') && (c != '`') && (c != '^') && (c != '{') && (c != '}') && (c != '|') && (c != '_'))
			return (false);
	}
	return (true);
}

std::string executeNICK(Server& server, Client& client, std::vector<std::string>& params)
{
	if(!client.isPasswordValid())
	{
		server.sendMessage(client, ERR_PASSWDMISMATCH, "NICK :Password required");
		return(ERR_PASSWDMISMATCH);
	}

	if(params.size() != 1)
	{
		server.sendMessage(client, ERR_ERRONEUSNICKNAME, "NICK :Erroneous nickname");
		return(ERR_ERRONEUSNICKNAME);
	}
	if((!isValidNickname(params[0])))
	{
		server.sendMessage(client, ERR_ERRONEUSNICKNAME, "NICK :Erroneous nickname, Nickname has not valid chars");
		return(ERR_ERRONEUSNICKNAME);
	}
	if(!server.isValidNickname(params[0]))
	{
		server.sendMessage(client, ERR_NICKNAMEINUSE, "NICK :Nickname is already in use");
		return(ERR_NICKNAMEINUSE);
	}

	if(!server.isValidUsername(params[0]))
	{
		server.sendMessage(client, ERR_NICKNAMEINUSE, "NICK :nickname is used as username");
		return(ERR_NICKNAMEINUSE);
	}

	// changed nickname
	if(client.isAuthenticated())
	{
		std::string reply = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " NICK :" + params[0];
		server.broadcastUpdatesToChannels(client.getFD(), reply);
	}

	client.setNickname(params[0]);
	sendStatusMessage(client.getFD(), "Nickname received!", client.getNC(), true);

	if(!client.isAuthenticated())
	{
		client.setAuthenticatedStatus();
		if(client.isAuthenticated())
		{
			std::cout << "[CLIENT " << client.getFD() << "] REGISTERED SUCCESSFULLY " << std::endl;
			sendNumericReply(client.getFD(), SERVER_NAME, RPL_WELCOME, client.getNickname(), "Welcome To Internet Relay Chat " + client.getNickname() + "!" + client.getUsername() + "@localhost");
		}
	}
	return(IRC_SUCCESS);
}
