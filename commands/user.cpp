/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatkeski <fatkeski@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 20:50:49 by fatkeski          #+#    #+#             */
/*   Updated: 2025/10/24 14:19:49 by fatkeski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libs/ft_irc.hpp"
#include "../libs/Server.hpp"
#include "../libs/Client.hpp"

std::string executeUSER(Server& server, Client& client, Message& msg, std::vector<std::string>& params)
{
	if(!client.isPasswordValid())
	{
		server.sendMessage(client, ERR_PASSWDMISMATCH, ":USER :Password required");
		return(ERR_PASSWDMISMATCH);
	}

	if(params.size() != 3)
	{
		server.sendMessage(client, ERR_NEEDMOREPARAMS, "USER :Not enough parameters");
		return(ERR_NEEDMOREPARAMS);
	}

	if(msg.getTrailing().empty())
	{
		server.sendMessage(client, ERR_NEEDMOREPARAMS, "USER :No realname entry");
		return(ERR_NEEDMOREPARAMS);
	}
	else
	{
		client.setRealname(msg.getTrailing());
	}

	if((params[1] != "0") || (params[2] != "*"))
	{
		server.sendMessage(client, ERR_NEEDMOREPARAMS, "USER :Parameters formats incorrect");
		return(ERR_NEEDMOREPARAMS);
	}

	if(!server.isValidUsername(params[0]))
	{
		server.sendMessage(client, ERR_ALREADYREGISTERED, "Username is already in use");
		return(ERR_ALREADYREGISTERED);
	}

	if(!server.isValidNickname(params[0]))
	{
		server.sendMessage(client, ERR_NICKNAMEINUSE, "USER :username is used as nickname");
		return(ERR_NICKNAMEINUSE);
	}

	if(client.isAuthenticated())
	{
		server.sendMessage(client, ERR_ALREADYREGISTERED, "USER :You may not reregister");
		return(ERR_ALREADYREGISTERED);
	}
	client.setUsername(params[0]);
	sendStatusMessage(client.getFD(), "Username received!", client.getNC(), true);
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
