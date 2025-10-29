/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_defines.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatkeski <fatkeski@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/11 16:13:51 by fatkeski          #+#    #+#             */
/*   Updated: 2025/10/24 14:15:55 by fatkeski         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_DEFINES_HPP
#define FT_DEFINES_HPP

#define SERVER_NAME "kizlarsoruyor"
#define IRC_SUCCESS "0"
#define IRC_ERROR "1"

// Connection Errors Codes
#define ERR_NEEDMOREPARAMS  "461"
#define ERR_ALREADYREGISTERED "462"
#define ERR_PASSWDMISMATCH  "464"
#define ERR_NOTREGISTERED  "451"
#define ERR_UNKNOWNCOMMAND  "421"
#define ERR_NONICKNAMEGIVEN "431"
#define ERR_ERRONEUSNICKNAME "432"
#define ERR_NICKNAMEINUSE "433"

#define ERR_NOTEXTTOSEND "412"
#define ERR_BADCHANMASK "476"
#define ERR_BADCHANNELKEY "475"
#define ERR_NOSUCHNICK "401"
#define ERR_NOSUCHCHANNEL "403"
#define ERR_CANNOTSENDTOCHAN "404"
#define ERR_CHANOPRIVSNEEDED "482"
#define ERR_CHANNELISFULL "471"
#define ERR_USERNOTINCHANNEL "441"
#define ERR_NOTONCHANNEL "442"
#define ERR_USERONCHANNEL "443"
#define ERR_INVITEONLYCHAN "473"

// Numeric RPL(Reply) Codes
#define	RPL_WELCOME "001" // "<nick> :Welcome to the Internet Relay Chat Network <nick>!<user>@<host>"
#define RPL_WHOREPLY "352"
#define RPL_ENDOFWHO "315"
#define RPL_CHANNELMODEIS "324"
#define RPL_INVITING "341"
#define RPL_TOPIC "332"

#endif
