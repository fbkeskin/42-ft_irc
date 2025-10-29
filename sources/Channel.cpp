/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatkeski <fatkeski@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 16:08:22 by fatkeski          #+#    #+#             */
/*   Updated: 2025/10/24 14:16:52 by fatkeski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libs/Channel.hpp"

Channel::Channel(int clientFD, std::string name)
{
	this->name = name;
	this->capacity = 0;
	this->limit = 0;
	this->topic = "default topic";
	this->topic_flag = false;
	this->invite_flag = false;
	members.insert(std::make_pair(clientFD, true));
}

int Channel::getLimit()
{
	return(this->limit);
}

int Channel::getCapacity()
{
	return(this->capacity);
}

std::string Channel::getName()
{
	return(this->name);
}

std::string Channel::getTopic()
{
	return(this->topic);
}

bool Channel::getTopicFlag()
{
	return(this->topic_flag);
}

void Channel::setTopicFlag(bool flag)
{
	this->topic_flag = flag;
}

bool Channel::isChannelInviteOnly()
{
	return(this->invite_flag);
}

void Channel::setInviteFlag(bool flag)
{
	this->invite_flag = flag;
}

void Channel::addClientToInviteList(std::string nickname)
{
	this->inviteList.insert(nickname);
}

void Channel::removeClientFromInviteList(std::string nickname)
{
	this->inviteList.erase(nickname);
}

bool Channel::isClientInvited(std::string nickname)
{
	std::set<std::string>::iterator result = this->inviteList.find(nickname);
	if(result == this->inviteList.end())
		return(false);
	return (true);
}

std::map<int, bool>& Channel::getMembers()
{
	return(this->members);
}

std::string Channel::getKey()
{
	return(this->key);
}

void Channel::setKey(std::string key)
{
	this->key = key;
}

void Channel::removeKey()
{
	this->key.clear();
}

void Channel::setLimit(int bound)
{
	this->limit = bound;
}

void Channel::setTopic(std::string topic)
{
	this->topic = topic;
}

void Channel::makeOperator(int clientFD)
{
	if (members.count(clientFD))
		members[clientFD] = true;
}

void Channel::removeOperatorPrivilige(int clientFD)
{
	if (members.count(clientFD))
		members[clientFD] = false;
}

bool Channel::isEmpty() const
{
	return members.empty();
}

bool Channel::isOperator(int clientFD)
{
	std::map<int, bool>::const_iterator it = members.find(clientFD);

	if ((it != members.end()) && (it->second == true))
		return (true);
	return (false);
}

bool Channel::hasAnyOperator()
{
	for (std::map<int, bool>::const_iterator it = members.begin(); it != members.end(); ++it)
	{
		if (it->second == (true))
			return (true);
	}
	return (false);
}


void Channel::addMember(int clientFD)
{
	capacity++;
	members.insert(std::make_pair(clientFD, 0));
}

bool Channel::hasMember(int clientFD)
{
	return (members.find(clientFD) != members.end());
}

void Channel::removeMember(int clientFD, Server& server)
{
	std::map<int, bool>::iterator it = members.find(clientFD);
	if (it == members.end())
		return;

	bool wasOperator = it->second;
	members.erase(it);
	this->capacity--;

	if (wasOperator && !this->hasAnyOperator())
	{
		int newOpFd = members.begin()->first;
		this->makeOperator(newOpFd);

		Client* newOp = server.getClientByFd(newOpFd);
		if (newOp)
		{
			std::string msg = ":" + newOp->getNickname() + "!" + newOp->getUsername() + "@localhost MODE " + this->getName() + " +o " + newOp->getNickname();
			this->broadcastMessage(server, msg, -1, false);
			std::cout << "[INFO] New operator assigned: " << newOp->getNickname() << " in channel " << this->getName() << std::endl;
		}
	}
}

void Channel::broadcastMessage(Server& server, const std::string& message, int sender, bool sendFlag)
{
	for (std::map<int, bool>::iterator it = members.begin(); it != members.end(); ++it)
	{
		int memberFD = it->first;
		if((memberFD == sender) && sendFlag)
			continue;
		Client* member = server.getClientByFd(memberFD);
		if (!member)
			continue;
		server.sendRawMessage(*member, message);
	}
}
