/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatkeski <fatkeski@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 18:49:35 by fatkeski          #+#    #+#             */
/*   Updated: 2025/10/24 14:14:32 by fatkeski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "../libs/ft_irc.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Channel.hpp"

#include <poll.h>

#define BUFFER_SIZE 512

#define POLL_TIMEOUT -1

extern volatile bool signal_flag;

class Server
{
	private:
		int port;
		int server_fd;
		std::string hostname;
		std::string password;
		std::vector<Client> clients;
		std::vector<Channel> channels;
		std::vector<struct pollfd> poll_fds;

	public:
		static void signalHandler(int signum);
		Server(int port, std::string password);

		//member func
		int configureServer();
		int runServer();
		Client* getClientByFd(int fd);
		int getClientIndexByFd(int clientFD);
		Client* getClientByNickname(std::string name);
		std::vector<Client> getClients();
		std::string getHostname();
		std::vector<Channel>& getChannels();
		Channel* getChannelByName(std::string name);
		void removeChannelByName(std::string name);

		void broadcastUpdatesToChannels(int clientFD, std::string str);

		void removeClient(int index);
		int acceptNewClient();
		int handleClientData(int index);
		std::string executeCommand(Client& client, Message& msg);
		void sendMessage(Client& client, const std::string& numericCode, const std::string& message);
		void sendRawMessage(Client& client, const std::string& rawMessage);

		void removeClientByFd(int fd);
		void removeEmptyChannels();
		void cleanup();

		// validate func
		bool isValidUsername(std::string str);
		bool isValidNickname(std::string str);

		~Server();
};

#endif