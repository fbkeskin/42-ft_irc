/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatkeski <fatkeski@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 18:53:26 by fatkeski          #+#    #+#             */
/*   Updated: 2025/10/24 14:18:32 by fatkeski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libs/Server.hpp"

volatile bool signal_flag = true;

void Server::signalHandler(int signum)
{
	(void)signum;
	signal_flag = false;
	std::cout << "\nSignal received, shutting down gracefully..." << std::endl;
}

Server::Server(int port, std::string password)
{
	signal_flag = true;
	signal(SIGINT, Server::signalHandler);
	this->server_fd = -1;
	this->port = port;
	this->password = password;
}

int Server::configureServer()
{
	this->server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(server_fd == -1) {
		printErrorMessage("socket");
		return(-1);
	}

	// for address already in use ERROR
	int yes = 1;
	if (setsockopt(this->server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
	{
		printErrorMessage("setsockopt SO_REUSEADDR");
		close(this->server_fd);
		return(-1);
	}

	if(setNonBlocking(this->server_fd) == -1)
	{
		printErrorMessage("fcntl");
		close(this->server_fd);
		return(-1);
	}

	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(this->port);
	// padding
	memset(address.sin_zero, 0, sizeof(address.sin_zero));

	if(bind(this->server_fd, (struct sockaddr *)(&address), sizeof(address)) < 0)
	{
		printErrorMessage("bind");
		close(this->server_fd);
		return(-1);
	}
	char ip_str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(address.sin_addr), ip_str, sizeof(ip_str));
	this->hostname = ip_str;

	if(listen(server_fd, MAX_CLIENT_COUNT) < 0)
	{
		printErrorMessage("listen");
		close(this->server_fd);
		return(-1);
	}
	std::cout << "server listening, fd: " << this->server_fd << std::endl;

	struct pollfd server_pollfd;

	server_pollfd.fd = this->server_fd;
	server_pollfd.events = POLLIN;
	server_pollfd.revents = 0;

	poll_fds.push_back(server_pollfd);

	return(0);
}

int Server::runServer()
{
	while(signal_flag)
	{
		int ready_events_count = poll(&(this->poll_fds[0]), static_cast<nfds_t>(this->poll_fds.size()), POLL_TIMEOUT);

		if(ready_events_count == -1)
		{
			printErrorMessage("poll");
			cleanup();
			return(-1);
		}
		if(ready_events_count == 0)
			continue;
		for(size_t i = 0; i < this->poll_fds.size(); i++)
		{
			if(poll_fds[i].revents == 0)
				continue;

			if (poll_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
			{
				int badfd = poll_fds[i].fd;
				std::cerr << "[WARN] poll revents on fd=" << badfd << std::endl;
				removeClientByFd(badfd);
				break;
			}

			if(poll_fds[i].revents & POLLIN)
			{
				if(poll_fds[i].fd == this->server_fd)
					acceptNewClient();
				else
					handleClientData(i);
			}
		}
	}
	return(0);
}

int Server::acceptNewClient()
{
	struct sockaddr_in client_address;
	socklen_t client_len = sizeof(client_address);

	int client_fd = accept(this->server_fd, (struct sockaddr *)(&client_address), &client_len);
	if(client_fd < 0)
	{
		printErrorMessage("accept");
		return(-1);
	}

	if(setNonBlocking(client_fd) == -1)
	{
		printErrorMessage("fcntl for client");
		close(client_fd);
		return(-1);
	}

	struct pollfd client_pollfd;

	client_pollfd.fd = client_fd;
	client_pollfd.events = POLLIN;
	client_pollfd.revents = 0;

	poll_fds.push_back(client_pollfd);

	// get IP address
	char ip_str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(client_address.sin_addr), ip_str, sizeof(ip_str));

	if(clients.size() >= MAX_CLIENT_COUNT)
	{
		std::cerr << "Max client limit reached!" << std::endl;
		close(client_fd);
		return(-1);
	}

	Client new_client(client_fd, client_address);
	clients.push_back(new_client);

	return(0);
}


int Server::handleClientData(int index)
{
	int fd = poll_fds[index].fd;
	char buffer[BUFFER_SIZE + 1];

	ssize_t bytes_read = recv(fd, buffer, BUFFER_SIZE, 0);
	buffer[bytes_read] = '\0';
	if(bytes_read > 0)
	{
		Client* client = getClientByFd(fd);
		if(!client)
			return(-1);

		std::string data(buffer, bytes_read);

		if (!client->getPartialBuffer().empty())
		{
			data = client->getPartialBuffer() + data;
			client->clearPartialBuffer();
		}

		size_t pos;
		bool found = false;
		while ((pos = data.find("\r\n")) != std::string::npos)
		{
			found = true;
			std::string oneCommand = data.substr(0, pos);
			data.erase(0, pos + 2);

			if (oneCommand.empty())
				continue;

			Message msg(oneCommand);
			msg.print(client->getFD());
			executeCommand(*client, msg);
		}
		if (!found && data.find('\n') != std::string::npos)
		{
			std::string warn = "NOTICE * :Invalid message format (use \\r\\n line endings)";
			this->sendRawMessage(*client, warn);
			std::cout << "[WARN] Invalid line ending (\\n instead of \\r\\n) from client: " << client->getNickname() << " (fd=" << fd << ")\n";
		}
		// if we have a part left unfinished with \r\n
		if (!data.empty())
		{
			client->setPartialBuffer(data);
		}
	}
	else if(bytes_read == 0)
	{
		std::cout << "Client disconnected! fd=" << fd << std::endl;

		Client* disconnectedClient = getClientByFd(fd);

		if (disconnectedClient)
		{
			std::string nickname = disconnectedClient->getNickname();
			for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); it++)
			{
				Channel& ch = *it;

				if (ch.hasMember(fd))
				{
					std::string partMsg = ":" + nickname + "!" + disconnectedClient->getUsername() + "@localhost PART " + ch.getName() + " :Client disconnected";
					ch.broadcastMessage(*this, partMsg, fd, false);
					ch.removeMember(fd, *this);
				}
			}
			removeEmptyChannels();
		}
		removeClient(index);
	}
	else
	{
		if((errno != EWOULDBLOCK) && (errno != EAGAIN))
		{
			printErrorMessage("recv");
			removeClient(index);
		}
	}

	return(0);
}

std::string Server::getHostname()
{
	return(this->hostname);
}


Client* Server::getClientByFd(int fd)
{
	for(size_t i = 0; i < clients.size(); i++)
	{
		if(clients[i].getFD() == fd)
		{
			return (&(clients[i]));
		}
	}
	return (NULL);
}

std::vector<Client> Server::getClients()
{
	return(this->clients);
}

std::vector<Channel>& Server::getChannels()
{
	return(this->channels);
}

int Server::getClientIndexByFd(int clientFD)
{
	for (size_t i = 0; i < poll_fds.size(); ++i)
	{
		if (poll_fds[i].fd == clientFD)
			return static_cast<int>(i);
	}
	return (-1);
}

Channel* Server::getChannelByName(std::string name)
{
	Channel* channel = NULL;

	for (size_t i = 0; i < channels.size(); ++i)
	{
		if (mapString(channels[i].getName()) == mapString(name))
		{
			channel = &channels[i];
			break;
		}
	}
	return(channel);
}

Client* Server::getClientByNickname(std::string name)
{
	Client* client = NULL;

	for (size_t i = 0; i < clients.size(); ++i)
	{
		if (mapString(clients[i].getNickname()) == mapString(name))
		{
			client = &clients[i];
			break;
		}
	}
	return(client);
}

bool Server::isValidUsername(std::string str)
{
	std::string mappedUser = mapString(str);

	for(size_t i = 0; i < clients.size(); i++)
	{
		if(mapString(clients[i].getUsername()) == mappedUser)
			return(false);
	}
	return(true);
}

bool Server::isValidNickname(std::string str)
{
	std::string mappedNick = mapString(str);

	for(size_t i = 0; i < clients.size(); i++)
	{
		if(mapString(clients[i].getNickname()) == mappedNick)
			return(false);
	}
	return(true);
}

void Server::removeClient(int index)
{
	if(index == 0 || index >= (int)(poll_fds.size()))
		return;

	int fd = poll_fds[index].fd;
	poll_fds[index] = poll_fds.back();
	poll_fds.pop_back();

	for(size_t i = 0; i < clients.size(); i++) {
		if(clients[i].getFD() == fd)
		{
			clients[i] = clients.back();
			clients.pop_back();
			break;
		}
	}
	close(fd);
}

void Server::removeClientByFd(int fd)
{
	int index = getClientIndexByFd(fd);
	if (index == -1)
	{
		close(fd);
		return;
	}
	removeClient(index);
}

void Server::broadcastUpdatesToChannels(int clientFD, std::string str)
{
	for (size_t i = 0; i < channels.size(); ++i)
	{
		if (channels[i].hasMember(clientFD))
		{
			channels[i].broadcastMessage(*this, str, clientFD, false);
		}
	}
}

void Server::cleanup()
{
	std::cout << "[INFO] Cleaning up server resources..." << std::endl;

	for (size_t i = 0; i < clients.size(); i++)
	{
		std::string quitMsg = "ERROR :Server shutting down\r\n";
		send(clients[i].getFD(), quitMsg.c_str(), quitMsg.length(), 0);
		close(clients[i].getFD());
	}

	if (server_fd != -1)
	{
		close(server_fd);
		server_fd = -1;
	}

	clients.clear();
	channels.clear();
	poll_fds.clear();

	std::cout << "[INFO] Cleanup completed." << std::endl;
}

void Server::sendMessage(Client& client, const std::string& numericCode, const std::string& message)
{
	std::string fullMsg = ":" + std::string(SERVER_NAME) + " " + numericCode + " " + client.getNickname() + " :" + message + "\r\n";

	ssize_t bytes_sent = send(client.getFD(), fullMsg.c_str(), fullMsg.size(), 0);
	if (bytes_sent == -1)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			std::cerr << "[WARN] Send buffer full for fd=" << client.getFD() << std::endl;
		}
		else
		{
			printErrorMessage("send");
			removeClientByFd(client.getFD());
		}
	}
	std::cout << "[SEND to " << client.getFD() << "] " << fullMsg;
}

void Server::sendRawMessage(Client& client, const std::string& rawMessage)
{
	std::string fullMsg = rawMessage + "\r\n";
	send(client.getFD(), fullMsg.c_str(), fullMsg.size(), 0);
	std::cout << "[SEND to " << client.getFD() << "] " << fullMsg;
}

void Server::removeEmptyChannels()
{
	for (size_t i = 0; i < channels.size(); )
	{
		if (channels[i].isEmpty())
		{
			std::cout << "[INFO] Channel " << channels[i].getName() << " removed (empty)\n";
			channels.erase(channels.begin() + i);
		}
		else
			++i;
	}
}

void Server::removeChannelByName(std::string name)
{
	std::string mappedName = mapString(name);

	for (std::vector<Channel>::iterator it = this->channels.begin(); it != this->channels.end(); ++it)
	{
		if (mapString(it->getName()) == mappedName)
		{
			std::cout << "[INFO] Channel removed: " << it->getName() << std::endl;
			this->channels.erase(it);
			return;
		}
	}

	std::cout << "[WARN] Channel not found for removal: " << name << std::endl;
}

std::string Server::executeCommand(Client& client, Message& msg)
{
	std::string ret;
	std::vector<std::string> params = msg.getParameters();
	std::string mappedCommand = mapString(msg.getCommand());

	if(mappedCommand == "cap")
	{
		client.setNC(false);
		return(IRC_SUCCESS);
	}

	if((mappedCommand != "pass") && (mappedCommand != "nick") && (mappedCommand != "user") && (mappedCommand != "join") && (mappedCommand != "kick") && (mappedCommand != "quit") && (mappedCommand != "who") && (mappedCommand != "mode") && (mappedCommand != "topic") && (mappedCommand != "invite") && (mappedCommand != "part") && (mappedCommand != "privmsg"))
	{
		sendMessage(client, ERR_UNKNOWNCOMMAND, "Unknown command");
		return(ERR_UNKNOWNCOMMAND);
	}

	if ((mappedCommand.empty()) || (params.empty() && mappedCommand != "quit"))
	{
		sendMessage(client, ERR_NEEDMOREPARAMS, "Not enough parameters");
		return (ERR_NEEDMOREPARAMS);
	}

	if ((!client.isAuthenticated()) && (mappedCommand != "pass") && (mappedCommand != "nick") && (mappedCommand != "user"))
	{
		sendMessage(client, ERR_NOTREGISTERED, "You have not registered");
		return (IRC_ERROR);
	}

	if(mappedCommand == "pass")
	{
		return(executePASS(*this, client, params, this->password));
	}
	else if(mappedCommand == "nick")
	{
		return(executeNICK(*this, client, params));
	}
	else if(mappedCommand == "user")
	{
		return(executeUSER(*this, client, msg, params));
	}
	else if(mappedCommand == "topic")
	{
		return(executeTOPIC(*this, client, msg, params));
	}
	else if(mappedCommand == "join")
	{
		return(executeJOIN(*this, client, params));
	}
	else if(mappedCommand == "privmsg")
	{
		return(executePRIVMSG(*this, client, msg, params));
	}
	else if(mappedCommand == "who")
	{
		return(executeWHO(*this, client, params));
	}
	else if(mappedCommand == "mode")
	{
		return(executeMODE(*this, client, msg, params));
	}
	else if(mappedCommand == "invite")
	{
		return(executeINVITE(*this, client, msg, params));
	}
	else if(mappedCommand == "kick")
	{
		return(executeKICK(*this, client, msg, params));
	}
	else if(mappedCommand == "part")
	{
		return(executePART(*this, client, msg, params));
	}
	else if(mappedCommand == "quit")
	{
		return(executeQUIT(*this, client, msg));
	}
	return(0);
}

Server::~Server()
{
	if (server_fd != -1)
		cleanup();
	std::cout << "destructor called" << std::endl;
}
