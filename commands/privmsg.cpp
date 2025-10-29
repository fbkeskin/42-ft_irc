/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatkeski <fatkeski@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/21 20:05:21 by fatkeski          #+#    #+#             */
/*   Updated: 2025/10/23 20:18:21 by fatkeski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libs/ft_irc.hpp"
#include "../libs/Server.hpp"
#include "../libs/Client.hpp"

std::string executePRIVMSG(Server& server, Client& client, Message& msg, std::vector<std::string> params)
{
	if(msg.getParameters().size() != 1)
	{
		server.sendMessage(client, ERR_NEEDMOREPARAMS, "PRIVMSG :Not enough parameters");
		return(ERR_NEEDMOREPARAMS);
	}

	if(msg.getTrailing().empty())
	{
		server.sendMessage(client, ERR_NOTEXTTOSEND, "PRIVMSG :No text to send");
		return(ERR_NOTEXTTOSEND);
	}

	std::string message = msg.getTrailing();

	std::stringstream ss(params[0]);
	std::string part;
	std::vector<std::string> inputs;
	while (std::getline(ss, part, ','))
	{
		if (!part.empty())
			inputs.push_back(part);
	}

	if (inputs.empty())
	{
		server.sendMessage(client, ERR_NEEDMOREPARAMS, "PRIVMSG :Not enough parameters");
		return (ERR_NEEDMOREPARAMS);
	}

	for (size_t i = 0; i < inputs.size(); i++)
	{
		if(inputs[i][0] == '#') // channel
		{
			
			Channel* c = server.getChannelByName(inputs[i]);
			if(!c)
				server.sendRawMessage(client, ":" + client.getHostname() + " " + ERR_NOSUCHCHANNEL + " " + client.getNickname() + " " + inputs[i] + " :No such channel");
			else
			{
				if(c->hasMember(client.getFD()))
				{
					std::string str = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost PRIVMSG " + c->getName() + " :" + message;
					c->broadcastMessage(server, str, client.getFD(), true);
				}
				else
				{
					server.sendRawMessage(client, ":" + client.getHostname() + " " + ERR_CANNOTSENDTOCHAN + " " + client.getNickname() + " " + inputs[i] + " :Cannot send to channel, client is not in the channel");
				}

			}
		}
		else // nick
		{
			Client* c = server.getClientByNickname(inputs[i]);
			if(!c)
				server.sendRawMessage(client, ":" + client.getHostname() + " " + ERR_NOSUCHNICK + " " + client.getNickname() + " " + inputs[i] + " :No such nickname");
			else
			{
				std::string str = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost PRIVMSG " + c->getNickname() + " :" + message;
				server.sendRawMessage(*c, str);
			}
		}
	}
	return (IRC_SUCCESS);
}
