/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatkeski <fatkeski@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 14:01:34 by fatkeski          #+#    #+#             */
/*   Updated: 2025/10/23 21:23:00 by fatkeski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libs/ft_irc.hpp"
#include "../libs/Server.hpp"
#include "../libs/Client.hpp"

std::string executeMODE(Server& server, Client& client, Message& msg, std::vector<std::string> params)
{
	if((params.empty() || params.size() < 2) || !(msg.getTrailing().empty()))
	{
		server.sendMessage(client, ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
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
	if(!(channel->isOperator(client.getFD())))
	{
		std::string errorMessage = std::string(ERR_CHANOPRIVSNEEDED) + " " + client.getNickname() + " " + channel->getName() + " :You're not channel operator";
		server.sendRawMessage(client, errorMessage);
		return(ERR_CHANOPRIVSNEEDED);
	}

	if((params.size() == 2) && params[1] == "+t")
	{
		channel->setTopicFlag(true);
		std::string reply = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost MODE " + channel->getName() + " +t";
		channel->broadcastMessage(server, reply, client.getFD(), false);
	}
	else if((params.size() == 2) && params[1] == "-t")
	{
		channel->setTopicFlag(false);
		std::string reply = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost MODE " + channel->getName() + " -t";
		channel->broadcastMessage(server, reply, client.getFD(), false);
	}
	else if((params.size() == 3) && params[1] == "+k")
	{
		channel->setKey(params[2]);
		std::string reply = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost MODE " + channel->getName() + " +k";
		channel->broadcastMessage(server, reply, client.getFD(), true);
		server.sendRawMessage(client,  ":" + client.getNickname() + "!" + client.getUsername() + "@localhost MODE " + channel->getName() + " " + channel->getKey() + " +k");
	}
	else if((params.size() == 2) && params[1] == "-k")
	{
		channel->removeKey();
		std::string reply = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost MODE " + channel->getName() + " -k";
		channel->broadcastMessage(server, reply, client.getFD(), false);
	}
	else if((params.size() == 3) && params[1] == "+l")
	{
		std::stringstream ss(params[2]);
		int bound;
		char c;

		if (!(ss >> bound) || (ss >> c) || bound <= 0)
		{
			server.sendMessage(client, ERR_NEEDMOREPARAMS, "MODE :Invalid channel limit value. Must be a positive integer");
			return (ERR_NEEDMOREPARAMS);
		}

		channel->setLimit(bound);
		std::string reply = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost MODE " + channel->getName() + " +l " + params[2];
		channel->broadcastMessage(server, reply, client.getFD(), false);
	}
	else if((params.size() == 2) && params[1] == "-l")
	{
		channel->setLimit(0);
		std::string reply = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost MODE " + channel->getName() + " -l";
		channel->broadcastMessage(server, reply, client.getFD(), false);
	}
	else if((params.size() == 2) && params[1] == "+i")
	{
		channel->setInviteFlag(true);
		std::string reply = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost MODE " + channel->getName() + " +i";
		channel->broadcastMessage(server, reply, client.getFD(), false);
	}
	else if((params.size() == 2) && params[1] == "-i")
	{
		channel->setInviteFlag(false);
		std::string reply = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost MODE " + channel->getName() + " -i";
		channel->broadcastMessage(server, reply, client.getFD(), false);
	}
	else if((params.size() == 3) && params[1] == "+o")
	{
		Client* target = server.getClientByNickname(params[2]);
		if(!target)
		{
			server.sendMessage(client, ERR_NOSUCHNICK, client.getNickname() + " " + params[2] + " :No such nickname");
			return (ERR_NOSUCHNICK);
		}
		if(!channel->hasMember(target->getFD()))
		{
			server.sendMessage(client, ERR_USERNOTINCHANNEL, target->getNickname() + " " + channel->getName() + " :They aren't on that channel");
			return (ERR_USERNOTINCHANNEL);
		}
		channel->makeOperator(target->getFD());
		std::string reply = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost MODE " + channel->getName() + " +o " + target->getNickname();
		channel->broadcastMessage(server, reply, client.getFD(), false);
	}
	else if((params.size() == 3) && params[1] == "-o")
	{
		Client* target = server.getClientByNickname(params[2]);
		if(!target)
		{
			server.sendMessage(client, ERR_NOSUCHNICK, client.getNickname() + " " + params[2] + " :No such nickname");
			return (ERR_NOSUCHNICK);
		}
		if(!channel->hasMember(target->getFD()))
		{
			server.sendMessage(client, ERR_USERNOTINCHANNEL, target->getNickname() + " " + channel->getName() + " :They aren't on that channel");
			return (ERR_USERNOTINCHANNEL);
		}
		channel->removeOperatorPrivilige(target->getFD());
		std::string reply = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost MODE " + channel->getName() + " -o " + target->getNickname();
		channel->broadcastMessage(server, reply, client.getFD(), false);
	}
	else
	{
		server.sendMessage(client, ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
		return(ERR_NEEDMOREPARAMS);
	}
	return(IRC_SUCCESS);
}
