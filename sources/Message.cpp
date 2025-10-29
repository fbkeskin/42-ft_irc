/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatkeski <fatkeski@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 17:00:51 by fatkeski          #+#    #+#             */
/*   Updated: 2025/10/24 14:17:30 by fatkeski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libs/Message.hpp"

Message::Message(std::string input)
{
	this->source = "";
	this->command = "";
	this->trailing = "";

	this->input = input;
	formatInput();
}

void Message::formatInput()
{
	size_t start = 0;
	while((start < (this->input).length()) && (std::isspace(this->input[start])))
	{
		start++;
	}

	// assign source
	if(start < this->input.length() && input[start] == ':')
	{
		size_t source_end = start + 1;
		while(source_end < input.length() && !std::isspace(this->input[source_end]))
		{
			source_end++;
		}
		this->source = input.substr(start + 1, source_end - start);
		start = source_end;

		while((start < this->input.length()) && (std::isspace(this->input[start])))
		{
			start++;
		}
	}

	// assign command
	size_t command_end = start;
	while(command_end < input.length() && !std::isspace(this->input[command_end]))
	{
		command_end++;
	}
	this->command = input.substr(start, command_end - start);
	start = command_end;

	// tariling varsa ata
	size_t index = input.find(':', start);
	if(index != std::string::npos)
	{
		int trailing_len = input.length() - index;
		this->trailing = input.substr(index + 1, trailing_len);
		index--;
		while(std::isspace(this->input[index]))
			index--;
	}
	else
		index = input.length();

	// assign parameteres
	while(start < index)
	{
		std::string temp = "";
		while((start < index ) && (std::isspace(this->input[start])))
			start++;

		size_t count = 0;
		while((start <= index ) && !(std::isspace(this->input[start])))
		{
			start++;
			count++;
		}

		temp = input.substr(start - count , count);
		if(!temp.empty())
			this->parameters.push_back(temp);
	}
}

void Message::print(int fd)
{
	std::cout << std::endl;
	std::cout << "[IRC Message] " << std::endl;
	std::cout << "Source: ";
	if(this->source.empty())
		std::cout << "(empty)";
	else
		std::cout << this->source ;

	std::cout << ", " << "Command: " ;
	if(this->command.empty())
		std::cout << "(empty)";
	else
		std::cout << this->command;

	std::cout << ", " << "Parameters: ";
	if(this->parameters.empty())
	{
		std::cout << "(empty)";
	}
	else
	{
		for(size_t i = 0; i < this->parameters.size(); i++)
		{
			std::cout << "[" << this->parameters[i] << "]";
			if(i < this->parameters.size() - 1)
				std::cout << " ";
		}
	}

	std::cout << ", " << "Trailing: ";
	if(this->trailing.empty())
		std::cout << "(empty)";
	else
		std::cout << this->trailing;
	std::cout << " from client fd: " << fd << std::endl;
}

std::string Message::getSource()
{
	return(this->source);
}

std::string Message::getCommand()
{
	return(this->command);
}

std::string Message::getTrailing()
{
	return(this->trailing);
}

std::vector<std::string> Message::getParameters()
{
	return(this->parameters);
}
