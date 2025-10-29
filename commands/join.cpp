/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatkeski <fatkeski@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 15:40:49 by fatkeski          #+#    #+#             */
/*   Updated: 2025/10/24 14:18:58 by fatkeski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libs/ft_irc.hpp"
#include "../libs/Server.hpp"
#include "../libs/Client.hpp"

bool isValidChannelName(const std::string& param, std::vector<std::string>& channelNames)
{
	if (param.empty())
		return (false);

	std::stringstream ss(param);
	std::string channel;

	while (std::getline(ss, channel, ','))
	{
		if (channel.empty() || channel.size() == 1)
			return (false);

		if (channel[0] != '#' && channel[0] != '&')
			return (false);

		if (channel.size() > 50)
			return (false);

		for (size_t i = 1; i < channel.size(); ++i)
		{
			char c = channel[i];
			if (c == ' ' || c == ',' || c == ':' || c == '\r' || c == '\n')
				return (false);
			if ((c >= 0 && c < 32) || (c == 127))
				return (false);
		}
		channelNames.push_back(channel);
	}
	return (!channelNames.empty());
}

Channel* getChannel(const std::string& mappedChannel, std::vector<Channel>& channels)
{
	for(size_t i = 0; i < channels.size(); i++)
	{
		if(mapString(channels[i].getName()) == mappedChannel)
			return(&(channels[i]));
	}
	return(NULL);
}

std::string executeJOIN(Server& server, Client& client, std::vector<std::string>& params)
{
	std::vector<std::string> channelNames;
	if (!isValidChannelName(params[0], channelNames))
	{
		server.sendMessage(client, ERR_NOSUCHCHANNEL, "JOIN :Invalid channel name");
		return (ERR_NOSUCHCHANNEL);
	}

	std::vector<Channel>& channels = server.getChannels();
	for(size_t i = 0; i < channelNames.size(); i++)
	{
		std::string mappedName = mapString(channelNames[i]);
		Channel* channel = getChannel(mappedName, channels);
		int clientFD = client.getFD();

		if (!channel)
		{
			if(params.size() != 1)
			{
				server.sendMessage(client, ERR_NEEDMOREPARAMS, "JOIN :Not enough parameters");
				return(ERR_NEEDMOREPARAMS);
			}

			Channel newChannel(client.getFD(), channelNames[i]);
			channels.push_back(newChannel);
			channel = &channels.back();
			channel->addMember(clientFD);

			std::string modeMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@" + server.getHostname() + " MODE " + channelNames[i] + " +o " + client.getNickname();
			server.sendRawMessage(client, modeMsg);

			std::string topicMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@" + server.getHostname() + " TOPIC " + channelNames[i] + " :" + channel->getTopic();
			server.sendRawMessage(client, topicMsg);
		}
		else
		{
			if((channel->getLimit() != 0) && (channel->getCapacity() >= channel->getLimit()))
			{
				server.sendMessage(client, ERR_CHANNELISFULL, (channel->getName() + " :Cannot join channel (+l) - Channel is full"));
				return(ERR_CHANNELISFULL);
			}
			if(channel->isChannelInviteOnly() && (!channel->isOperator(clientFD) && !channel->isClientInvited(client.getNickname())))
			{
				server.sendMessage(client, ERR_INVITEONLYCHAN, (channel->getName() + " Cannot join channel (+i) - Invite only"));
				return(ERR_INVITEONLYCHAN);
			}
			if (channel->getKey().empty())
			{
				if(params.size() == 1)
				{
					if(channel->hasMember(clientFD))
						return(IRC_SUCCESS);
					channel->addMember(clientFD);
				}
				else
				{
					server.sendMessage(client, ERR_NEEDMOREPARAMS, "JOIN :Not enough parameters");
					return(ERR_NEEDMOREPARAMS);
				}
			}
			else
			{
				// has key
				if(params.size() != 2)
				{
					server.sendMessage(client, ERR_NEEDMOREPARAMS, "JOIN :Not enough parameters, channel has key");
					return(ERR_NEEDMOREPARAMS);
				}
				else
				{
					if(channel->getKey() != params[1])
					{
						std::string errorMessage = std::string(ERR_BADCHANNELKEY) + " " + client.getNickname() + " " + channel->getName() + " :Cannot join channel (+k) - Bad Key";
						server.sendRawMessage(client, errorMessage);
						return(ERR_BADCHANNELKEY);
					}
					if(channel->hasMember(clientFD))
						return(IRC_SUCCESS);
					else
						channel->addMember(clientFD);
				}
			}
			if(channel->isClientInvited(client.getNickname()))
				channel->removeClientFromInviteList(client.getNickname());
		}

		server.sendRawMessage(client, ":" + client.getNickname() + "!" + client.getUsername() + "@localhost JOIN :" + channelNames[i]);

		std::string joinMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost JOIN :" + channelNames[i];
		channel->broadcastMessage(server, joinMsg, client.getFD(), true);

		std::map<int, bool>::iterator it;
		std::string userList;
		for (it = channel->getMembers().begin(); it != channel->getMembers().end(); ++it)
		{
			Client* member = server.getClientByFd(it->first);
			if (!member) continue;
			if (!userList.empty()) userList += " ";
			if (channel->isOperator(it->first))
				userList += "@";
			userList += member->getNickname();
		}

		server.sendRawMessage(client, ":" + server.getHostname() + " 353 " + client.getNickname() + " = " + channelNames[i] + " :" + userList);
		server.sendRawMessage(client, ":" + server.getHostname() + " 366 " + client.getNickname() + " " + channelNames[i] + " :End of /NAMES list");
	}
	return(IRC_SUCCESS);
}
