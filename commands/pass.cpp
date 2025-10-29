/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatkeski <fatkeski@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/11 17:51:17 by fatkeski          #+#    #+#             */
/*   Updated: 2025/10/23 16:49:22 by fatkeski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libs/ft_irc.hpp"
#include "../libs/Server.hpp"
#include "../libs/Client.hpp"

std::string executePASS(Server& server, Client& client, std::vector<std::string>& params, std::string password)
{
	if(client.isAuthenticated())
	{
		server.sendMessage(client, ERR_ALREADYREGISTERED, "You may not reregister");
		return(ERR_ALREADYREGISTERED);
	}
	else
	{
		if((params.size() == 1) && (params[0] == password))
			client.setPasswordStatus(true);
		else
		{
			server.sendMessage(client, ERR_PASSWDMISMATCH, "Password incorrect");
			return(ERR_PASSWDMISMATCH);
		}
	}
	sendStatusMessage(client.getFD(), "Password is corret!", client.getNC(), true);

	return(IRC_SUCCESS);
}
