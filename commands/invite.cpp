/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatkeski <fatkeski@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 20:42:24 by fatkeski          #+#    #+#             */
/*   Updated: 2025/10/23 20:23:52 by fatkeski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libs/ft_irc.hpp"
#include "../libs/Server.hpp"
#include "../libs/Client.hpp"

std::string executeINVITE(Server& server, Client& client, Message& msg, std::vector<std::string> params)
{
	if((msg.getParameters().size() != 2) || !(msg.getTrailing().empty()))
	{
		server.sendMessage(client, ERR_NEEDMOREPARAMS, "TOPIC :Not enough parameters");
		return(ERR_NEEDMOREPARAMS);
	}

	Channel* channel = server.getChannelByName(params[1]);

	if(!channel)
	{
		server.sendMessage(client, ERR_NOSUCHCHANNEL, client.getNickname() + params[0] + " :No such channel");
		return(ERR_NOSUCHCHANNEL);
	}
	if(!(channel->hasMember(client.getFD())))
	{
		server.sendMessage(client, ERR_NOTONCHANNEL, client.getNickname() + params[0] + " :You're not on that channel");
		return(ERR_NOTONCHANNEL);
	}

	if(channel->isChannelInviteOnly() && !(channel->isOperator(client.getFD())))
	{
		std::string errorMessage = std::string(ERR_CHANOPRIVSNEEDED) + " " + client.getNickname() + " " + channel->getName() + " :You're not channel operator";
		server.sendRawMessage(client, errorMessage);
		return(ERR_CHANOPRIVSNEEDED);
	}

	Client* invitedClient = server.getClientByNickname(params[0]);

	if(invitedClient)
	{
		if(channel->hasMember(invitedClient->getFD()))
		{
			server.sendMessage(client, ERR_USERONCHANNEL, client.getNickname() + params[0] + " :User is already in the channel");
			return(ERR_USERONCHANNEL);
		}
		std::string invitedMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost INVITE " + invitedClient->getNickname() + " :" + channel->getName();
		server.sendRawMessage(*invitedClient, invitedMsg);
		channel->addClientToInviteList(invitedClient->getNickname());
		std::string replyMessage = RPL_INVITING + client.getNickname() + " " + invitedClient->getNickname() + " " + channel->getName() + " :Inviting " + invitedClient->getNickname() + " to " + channel->getName();
		server.sendRawMessage(client, replyMessage);
	}
	else
	{
		server.sendMessage(client, ERR_NOSUCHNICK, client.getNickname() + params[0] + " :No such nickname");
		return(ERR_NOSUCHNICK);
	}
	return(IRC_SUCCESS);
}
