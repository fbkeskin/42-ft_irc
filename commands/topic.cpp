/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatkeski <fatkeski@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 18:35:02 by fatkeski          #+#    #+#             */
/*   Updated: 2025/10/23 20:08:52 by fatkeski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libs/ft_irc.hpp"
#include "../libs/Server.hpp"
#include "../libs/Client.hpp"

std::string executeTOPIC(Server& server, Client& client, Message& msg, std::vector<std::string> params)
{
	std::string newTopic =  msg.getTrailing();

	if((msg.getParameters().size() != 1))
	{
		server.sendMessage(client, ERR_NEEDMOREPARAMS, "TOPIC :Not enough parameters");
		return(ERR_NEEDMOREPARAMS);
	}

	Channel* channel = server.getChannelByName(params[0]);

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

	if(msg.getTrailing().empty())
	{
		std::string title = ":" + std::string(SERVER_NAME) + " " + RPL_TOPIC + " " + client.getNickname() + " " + channel->getName() + " :" + channel->getTopic();
		server.sendRawMessage(client, title);
		return(RPL_TOPIC);
	}

	if(channel->getTopicFlag() && !(channel->isOperator(client.getFD())))
	{
		std::string errorMessage = std::string(ERR_CHANOPRIVSNEEDED) + " " + client.getNickname() + " " + channel->getName() + " :You're not channel operator";
		server.sendRawMessage(client, errorMessage);
		return(ERR_CHANOPRIVSNEEDED);
	}

	channel->setTopic(newTopic);
	std::string reply = ":" + client.getNickname() + "!" + client.getUsername() + "@" + server.getHostname() + " TOPIC " + channel->getName() + " " + newTopic;
	channel->broadcastMessage(server, reply, client.getFD(), false);

	return(IRC_SUCCESS);
}
