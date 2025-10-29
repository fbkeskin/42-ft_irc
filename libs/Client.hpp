/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatkeski <fatkeski@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 19:33:46 by fatkeski          #+#    #+#             */
/*   Updated: 2025/10/23 16:44:55 by fatkeski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../libs/ft_irc.hpp"

class Client
{
	private:
		int fd;
		struct sockaddr_in address;
		bool is_nc;
		std::string partialBuffer;
		bool authenticateStatus;
		bool passwordStatus;
		std::string username;
		std::string nickname;
		std::string realname;
		std::string hostname;
	public:
		Client(int fd, struct sockaddr_in addr);
		int getFD();
		bool isAuthenticated();
		bool isPasswordValid();
		std::string getPartialBuffer();
		std::string getHostname() const;
		std::string getUsername() const;
		std::string getNickname() const;
		std::string getRealname() const;
		bool getNC();


		void setPasswordStatus(bool status);
		void setAuthenticatedStatus();
		void setPartialBuffer(std::string message);

		void setUsername(const std::string& username);
		void setNickname(const std::string& nickname);
		void setRealname(const std::string& nickname);
		void setNC(bool status);

		void clearPartialBuffer();

		~Client();

};

#endif
