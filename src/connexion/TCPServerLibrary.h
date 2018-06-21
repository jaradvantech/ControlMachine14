/*
 * TCPServerLibrary.h
 *
 *  Created on: Dec 22, 2017
 *      Author: Jose Andres Grau Martinez
 */

#ifndef CONNEXION_TCPSERVERLIBRARY_H_
#define CONNEXION_TCPSERVERLIBRARY_H_


#define MAXPACKETSIZE 4096

#include <string>


void * OpenServer(void *Arg);
void SetFunctionToProcessMessages(std::string (*_FunctionToProcessMessages)(std::string));

#endif /* CONNEXION_TCPSERVERLIBRARY_H_ */
