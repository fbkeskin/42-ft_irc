/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatkeski <fatkeski@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 16:46:35 by fatkeski          #+#    #+#             */
/*   Updated: 2025/10/24 14:14:41 by fatkeski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libs/ft_irc.hpp"

class Message
{
	private:
		std::string input;
		std::string source;
		std::string command;
		std::string trailing;
		std::vector<std::string> parameters;
	public:
		Message(std::string input);
		void formatInput();
		void print(int fd);

		std::string getSource();
		std::string getCommand();
		std::string getTrailing();
		std::vector<std::string> getParameters();
};
