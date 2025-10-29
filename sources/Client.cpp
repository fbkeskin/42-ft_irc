/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatkeski <fatkeski@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 14:41:03 by fatkeski          #+#    #+#             */
/*   Updated: 2025/10/23 17:25:16 by fatkeski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libs/Client.hpp"


Client::Client(int fd, struct sockaddr_in addr)
{
	this->fd = fd;
	this->address = addr;
	char ip_str[INET_ADDRSTRLEN];
	this->is_nc = true;
	inet_ntop(AF_INET, &(addr.sin_addr), ip_str, sizeof(ip_str));
	this->hostname = ip_str;
	this->passwordStatus = false;
	this->authenticateStatus = false;

	this->username = "*";
	this->nickname = "*";

	std::cout << "Client created: fd=" << fd << " from " << hostname << ":" << ntohs(addr.sin_port) << std::endl;
}

int Client::getFD()
{
	return(this->fd);
}

std::string Client::getPartialBuffer()
{
	return(this->partialBuffer);
}

bool Client::isPasswordValid()
{
	return(this->passwordStatus);
}

bool Client::isAuthenticated()
{
	return(this->authenticateStatus);
}

std::string Client::getHostname() const
{
	return(this->hostname);
}

std::string Client::getUsername() const
{
	return(this->username);
}

std::string Client::getNickname() const
{
	return(this->nickname);
}

std::string Client::getRealname() const
{
	return(this->realname);
}

bool Client::getNC()
{
	return(this->is_nc);
}


void Client::setPasswordStatus(bool status)
{
	this->passwordStatus = status;
}

void Client::setAuthenticatedStatus()
{
	if((this->passwordStatus) && (this->username != "*") && (this->nickname != "*"))
		this->authenticateStatus = true;
}

void Client::setPartialBuffer(std::string message)
{
	this->partialBuffer = message;
}

void Client::clearPartialBuffer()
{
	this->partialBuffer.clear();
}

void Client::setUsername(const std::string& username)
{
	this->username = username;
}

void Client::setNickname(const std::string& nickname)
{
	this->nickname = nickname;
}

void Client::setRealname(const std::string& realname)
{
	this->realname = realname;
}

void Client::setNC(bool status)
{
	this->is_nc = status;
}

Client::~Client()
{

}
