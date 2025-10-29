/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatkeski <fatkeski@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 15:58:17 by fatkeski          #+#    #+#             */
/*   Updated: 2025/10/24 14:21:10 by fatkeski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "../libs/ft_irc.hpp"
#include "Server.hpp"

class Channel
{
	private:
		int	capacity;
		int	limit;
		std::string name;
		std::string topic;
		bool topic_flag; // for opt auth: true
		bool invite_flag;
		std::string key;
		// client_fd + role(role = false(not op), role = true(operator))
		std::map<int, bool> members;
		std::set<std::string> inviteList;

	public:
		Channel(int clientFD, std::string name);

		int getLimit();
		int getCapacity();

		std::string getKey();
		void setKey(std::string key);
		void removeKey();

		std::string getName();
		std::string getTopic();
		bool getTopicFlag();
		void setTopicFlag(bool flag);
		bool isChannelInviteOnly();
		void setInviteFlag(bool flag);
		std::map<int, bool>& getMembers();

		void setLimit(int bound);
		// void setName(std::string name);
		void setTopic(std::string topic);
		bool isEmpty() const;
		void addMember(int clientFD);

		void addClientToInviteList(std::string nickname);
		void removeClientFromInviteList(std::string nickname);
		bool isClientInvited(std::string nickname);

		bool hasMember(int clientFD);
		bool isOperator(int clientFD);
		void makeOperator(int clientFD);
		void removeOperatorPrivilige(int clientFD);
		void removeMember(int clientFD, Server& server);
		void broadcastMessage(Server& server, const std::string& message, int sender, bool sendFlag);

		bool hasAnyOperator();

};

#endif
