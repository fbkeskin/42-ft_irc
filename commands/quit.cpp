/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatkeski <fatkeski@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 13:41:59 by fatkeski          #+#    #+#             */
/*   Updated: 2025/10/23 17:23:56 by fatkeski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libs/ft_irc.hpp"
#include "../libs/Server.hpp"
#include "../libs/Client.hpp"

void removeClientFromChannels(Server& server, Client& client, std::string trailing)
{
	std::vector<Channel>& channels = server.getChannels();

	for (size_t i = 0; i < channels.size(); ++i)
	{
		if (channels[i].hasMember(client.getFD()))
		{
			if(trailing.empty())
				trailing = "no reason";
			std::string quitMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " QUIT :" + trailing;
			channels[i].broadcastMessage(server, quitMsg, client.getFD(), true);
			channels[i].removeMember(client.getFD(), server);
		}
	}
}

std::string executeQUIT(Server& server, Client& client, Message& msg)
{
	if(!(msg.getParameters().empty()))
	{
		server.sendMessage(client, ERR_NEEDMOREPARAMS, "QUIT :Not enough parameters");
		return(ERR_NEEDMOREPARAMS);
	}

	removeClientFromChannels(server, client, msg.getTrailing());
	server.removeEmptyChannels();
	server.removeClientByFd(client.getFD());

	return(IRC_SUCCESS);
}
