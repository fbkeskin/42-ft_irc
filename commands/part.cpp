/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatkeski <fatkeski@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/21 16:52:17 by fatkeski          #+#    #+#             */
/*   Updated: 2025/10/23 20:37:49 by fatkeski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libs/ft_irc.hpp"
#include "../libs/Server.hpp"
#include "../libs/Client.hpp"

std::string executePART(Server& server, Client& client, Message& msg, std::vector<std::string> params)
{
	if((msg.getParameters().size() != 1))
	{
		server.sendMessage(client, ERR_NEEDMOREPARAMS, "PART :Not enough parameters");
		return(ERR_NEEDMOREPARAMS);
	}

	std::stringstream ss(params[0]);
	std::string channelName;
	std::vector<std::string> inputChannels;
	while (std::getline(ss, channelName, ','))
	{
		if (!channelName.empty())
			inputChannels.push_back(channelName);
	}

	if (inputChannels.empty())
	{
		server.sendMessage(client, ERR_NEEDMOREPARAMS, "PART :Not enough parameters");
		return (ERR_NEEDMOREPARAMS);
	}

	std::vector<Channel>& allChannels = server.getChannels();

	for (size_t i = 0; i < inputChannels.size(); ++i)
	{
		std::string mappedName = mapString(inputChannels[i]);
		Channel* channel = NULL;

		for (size_t j = 0; j < allChannels.size(); ++j)
		{
			if (mapString(allChannels[j].getName()) == mappedName)
			{
				channel = &allChannels[j];
				break;
			}
		}

		if (!channel)
		{
			server.sendMessage(client, ERR_NOSUCHCHANNEL, inputChannels[i] + " :No such channel");
			continue;;
		}

		int clientFD = client.getFD();

		if (!channel->hasMember(clientFD))
		{
			server.sendMessage(client, ERR_NOTONCHANNEL, channel->getName() + " :You're not on that channel");
			continue;
		}

		std::string reason;
		
		if(msg.getTrailing().empty())
			reason = "no reason";
		else
			reason = msg.getTrailing();

		std::string partMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost PART " + channel->getName() + " :" + reason;

		channel->broadcastMessage(server, partMsg, clientFD, false);

		channel->removeMember(clientFD, server);

		if (channel->getMembers().empty())
		{
			server.removeChannelByName(channel->getName());
		}
	}

	return(IRC_SUCCESS);
}
