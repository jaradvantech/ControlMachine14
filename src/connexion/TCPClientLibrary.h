/*
 * TCPClientLibrary.h
 *
 *  Created on: Dec 16, 2017
 *      Author: Jose Andres Grau Martinez
 */

#ifndef CONNEXION_TCPCLIENTLIBRARY_H_
#define CONNEXION_TCPCLIENTLIBRARY_H_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netdb.h>
#include <vector>
#include <deque>

class TCPClient
{
  private:
    int sock;
    std::string address;
    int port;
    struct sockaddr_in server;
	volatile bool _connected;
  public:
    TCPClient();
    bool setup(std::string address, int port);
    bool Send(std::string data);
    std::string receive(int size = 4096);
    std::string read();
	void detach();
	bool Connected();
};




#endif /* CONNEXION_TCPCLIENTLIBRARY_H_ */
