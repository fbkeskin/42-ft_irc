/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatkeski <fatkeski@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/21 15:52:49 by fatkeski          #+#    #+#             */
/*   Updated: 2025/10/23 20:34:51 by fatkeski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libs/ft_irc.hpp"
#include "../libs/Server.hpp"
#include "../libs/Client.hpp"

std::string executeKICK(Server& server, Client& client, Message& msg, std::vector<std::string> params)
{
	if((msg.getParameters().size() != 2))
	{
		server.sendMessage(client, ERR_NEEDMOREPARAMS, "KICK :Not enough parameters");
		return(ERR_NEEDMOREPARAMS);
	}

	Channel* channel = server.getChannelByName(params[0]);

	if(!channel)
	{
		server.sendMessage(client, ERR_NOSUCHCHANNEL, params[0] + " :No such channel");
		return(ERR_NOSUCHCHANNEL);
	}

	if(!(channel->isOperator(client.getFD())))
	{
		std::string errorMessage = std::string(ERR_CHANOPRIVSNEEDED) + " " + client.getNickname() + " " + channel->getName() + " :You're not channel operator";
		server.sendRawMessage(client, errorMessage);
		return(ERR_CHANOPRIVSNEEDED);
	}

	if(channel->getCapacity() < 2)
	{
		std::cout << "aaa" << std::endl;
		std::string errorMessage = std::string(ERR_CHANOPRIVSNEEDED) + " " + client.getNickname() + " " + channel->getName() + " :You're the only one on the channel, you can't throw yourself away";
		server.sendRawMessage(client, errorMessage);
		return(ERR_CHANOPRIVSNEEDED);
	}

	Client* target = server.getClientByNickname(params[1]);
	if(!target)
	{
		server.sendMessage(client, ERR_NOSUCHNICK, client.getNickname() + params[1] + " :No such nickname");
		return(ERR_NOSUCHNICK);
	}
	if(!(channel->hasMember(target->getFD())))
	{
		server.sendMessage(client, ERR_USERNOTINCHANNEL, target->getNickname() + " " + channel->getName() + " :They aren't on that channel");
		return(ERR_USERNOTINCHANNEL);
	}

	std::string reason;
	if(msg.getTrailing().empty())
		reason = "no reason";
	else
		reason = msg.getTrailing();

	std::string message = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " KICK " + channel->getName() + " " + target->getNickname() + " :" + reason;

	channel->broadcastMessage(server, message, client.getFD(), false);
	channel->removeMember(target->getFD(), server);
	server.removeEmptyChannels();

	return(IRC_SUCCESS);
}
