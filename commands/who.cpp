/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   who.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatkeski <fatkeski@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 15:35:40 by fatkeski          #+#    #+#             */
/*   Updated: 2025/10/24 14:20:39 by fatkeski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libs/Client.hpp"
#include "../libs/Server.hpp"
#include "../libs/ft_irc.hpp"

std::string executeWHO(Server &server, Client &client,
	std::vector<std::string> &params)
{
	Channel	*channel;
	int		memberFD;
	Client	*member;
	Client	*target;

	if (params.empty())
	{
		server.sendMessage(client, ERR_NEEDMOREPARAMS, "WHO :Not enough parameters");
		return (ERR_NEEDMOREPARAMS);
	}
	std::string mask = params[0];
	if (mask[0] == '#')
	{
		channel = server.getChannelByName(mask);
		if (!channel)
		{
			server.sendMessage(client, ERR_NOSUCHCHANNEL, mask
				+ " :No such channel");
			return (ERR_NOSUCHCHANNEL);
		}

		std::map<int, bool> &members = channel->getMembers();
		for (std::map<int,
			bool>::const_iterator it = members.begin(); it != members.end(); ++it)
		{
			memberFD = it->first;
			int role = it->second; // 0: regular, 1: op
			member = server.getClientByFd(memberFD);
			if (member)
			{
				std::string flags = "H"; // all members
				if (role == 1)
					flags += "@"; // channel op
				if (channel->isOperator(member->getFD()))
				{
					flags += "*"; // IRC op
				}
				std::string whoMessage = ":" + std::string(SERVER_NAME)
					+ " 352 " + client.getNickname() + " " + mask + " "
					+ member->getUsername() + " " + member->getHostname() + " "
					+ std::string(SERVER_NAME) + " " + member->getNickname()
					+ " " + flags + " :0 " + member->getRealname();
				server.sendRawMessage(client, whoMessage);
			}
		}
		// end of the WHO list
		server.sendRawMessage(client, ":" + std::string(SERVER_NAME) + " 315 "
			+ client.getNickname() + " " + mask + " :End of WHO list");
	}
	else
	{
		target = server.getClientByNickname(mask);
		if (target)
		{
			std::string flags = "H";
			std::string whoMessage = ":" + std::string(SERVER_NAME) + " 352 "
				+ client.getNickname() + " * " + target->getUsername() + " "
				+ target->getHostname() + " " + std::string(SERVER_NAME) + " "
				+ target->getNickname() + " " + flags + " :0 "
				+ target->getRealname();
			server.sendRawMessage(client, whoMessage);
			server.sendRawMessage(client, ":" + std::string(SERVER_NAME)
				+ " 315 " + client.getNickname() + " " + mask
				+ " :End of WHO list");
		}
		else
		{
			server.sendMessage(client, ERR_NOSUCHNICK, mask
				+ " :No such nick/channel");
			return (ERR_NOSUCHNICK);
		}
	}
	return (IRC_SUCCESS);
}
