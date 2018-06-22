/*
 * TCPServerLibrary.cpp
 *
 *  Created on: Dec 22, 2017
 *      Author: Jose Andres Grau Martinez
 */

#include <connexion/TCPServerLibrary.h>

#include <stdio.h>
#include <iostream>
#include <string.h>   //strlen
#include <string>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <boost/algorithm/string/predicate.hpp>
#include <pthread.h>
#include "math.h"
#include <chrono>
#include <thread>
#include "SynchronizationPoints.h"
#include <sstream>

#define TRUE		1
#define FALSE  		0
#define BUFFER_SIZE 4096
#define PORT		23623

int KeepConnectionOpened = 1;


class _TCPClient
{
	public:
	_TCPClient(int _client_socket,std::chrono::time_point<std::chrono::system_clock> _previous_response_time );
	int client_socket;
	std::chrono::time_point<std::chrono::system_clock> previous_response_time;
};

_TCPClient::_TCPClient(	int _client_socket,std::chrono::time_point<std::chrono::system_clock> _previous_response_time )
{
	client_socket = _client_socket;
	previous_response_time = _previous_response_time;
}

void CheckForDeadTimedOutSockets(std::vector<_TCPClient>& TCPClients, struct sockaddr_in& address)
{
	//check for dead timed out sockets
	//-----------------------------------------------------------------------------
	for (unsigned int i = 0; i < TCPClients.size(); i++)
	{
		//if position is NOT empty

		std::chrono::duration<double> elapsed_time =std::chrono::system_clock::now() - TCPClients.at(i).previous_response_time;

		if (elapsed_time.count() > 10)
		{
			int addrlen = sizeof(address);
			getpeername(TCPClients.at(i).client_socket,
					(struct sockaddr*) (&address), (socklen_t*) (&addrlen));
			printf("Host lost connection , ip %s , port %d \n",
					inet_ntoa(address.sin_addr), ntohs(address.sin_port));
			//Close the socket and mark as 0 in list for reuse
			close(TCPClients.at(i).client_socket);
			TCPClients.erase(TCPClients.begin() + i);
		}
	}
}

void AddChildSocketsToSet(fd_set *readfds, int& max_sd,
		std::vector<_TCPClient>& TCPClients)
{
	//add child sockets to set
	for (unsigned int i = 0; i < TCPClients.size(); i++)	//TODO: REWRITE THIS LOOP WITH OPERATOR OVERLOAD
	{
		//add socket to set
		FD_SET(TCPClients.at(i).client_socket, readfds);
		//highest file descriptor number, need it for the select function
		if (TCPClients.at(i).client_socket > max_sd)
			max_sd = TCPClients.at(i).client_socket;
	}
}

struct timeval SetTimeOut(int seconds, int useconds)
{
	struct timeval tv;
	tv.tv_sec = seconds;  /* 5 Secs Timeout */
	tv.tv_usec = useconds;  // Not init'ing this can cause strange errors
	return tv;
}

void CheckForIncomingConnections(fd_set* readfds, int master_socket, struct sockaddr_in& address,
		std::vector<_TCPClient>& TCPClients)
{
	//-----------------------------------------------------------------------------
	//If something happened on the master socket ,
	//then its an incoming connection
	int addrlen = sizeof(address);
	if (FD_ISSET(master_socket, readfds))
	{
		int new_socket;
		if ((new_socket = accept(master_socket, (struct sockaddr*) (&address),
				(socklen_t*) (&addrlen))) < 0)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}
		//inform user of socket number - used in send and receive commands
		printf("New connection , socket fd is %d , ip is : %s , port : %d\n",
				new_socket, inet_ntoa(address.sin_addr),
				ntohs(address.sin_port));
		/*send new connection greeting message
		char* message = "Communication established.\r\n";
		if (send(new_socket, message, strlen(message), 0) != strlen(message))
		{
			perror("send");
		}
		puts("Welcome message sent successfully");
		*/
		_TCPClient newClient(new_socket, std::chrono::system_clock::now());
		TCPClients.push_back(newClient);
		printf("Adding to list of sockets as %d\n", int(TCPClients.size() - 1));
	}
}

void ClientHasDisconnected(int sd, int& addrlen, int i,
		struct sockaddr_in& address, std::vector<_TCPClient>& TCPClients)
{
	//Somebody disconnected , get his details and print
	getpeername(sd, (struct sockaddr*) (&address), (socklen_t*) (&addrlen));
	printf("Host disconnected , ip %s , port %d \n",
			inet_ntoa(address.sin_addr), ntohs(address.sin_port));
	//Close the socket and delete from the list
	close(sd);
	TCPClients.erase(TCPClients.begin() + i);
}

void CreateMasterSocket(int& master_socket, int opt,struct sockaddr_in& address)
{
	//create a master socket
	if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	//set master socket to allow multiple connections ,
	if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char*) (&opt),
			sizeof(opt)) < 0)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	//type of socket created
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	//bind the socket to localhost port PORT
	if (bind(master_socket, (struct sockaddr*) (&address), sizeof(address)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	printf("Listener on port %d \n", PORT);
	//try to specify maximum of 3 pending connections for the master socket
	if (listen(master_socket, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	//accept the incoming connection
	puts("Waiting for connections ...");
}

std::string (*FunctionToProcessMessages)(std::string)=nullptr;

void SetFunctionToProcessMessages(std::string (*_FunctionToProcessMessages)(std::string))
{
	FunctionToProcessMessages=_FunctionToProcessMessages;
}


void * OpenServer(void *Arg)
{
	pthread_detach((unsigned long)pthread_self());
	printf("TCP Server thread Started");


    int opt = TRUE;
    std::vector<_TCPClient> TCPClients;

    int master_socket , addrlen, valread;

    struct sockaddr_in address;

    char bufferRead[BUFFER_SIZE];  //data buffer of 1K
    std::string bufferWrite;  //data buffer of 1K

    //set of socket descriptors
    fd_set readfds;

    //create a master socket
	CreateMasterSocket(master_socket, opt, address);
	Synchro::DecreaseSynchronizationPointValue(0);
    while(KeepConnectionOpened)
    {
        //clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(master_socket, &readfds);
        int max_sd = master_socket;

        //add childs sockets to set
		AddChildSocketsToSet(&readfds, max_sd, TCPClients);

        //wait for an activity on one of the sockets , timeout is set below ,
        //-----------------------------------------------------------------------------
        struct timeval tv = SetTimeOut(3,0);
        int activity = select( max_sd + 1 , &readfds , NULL , NULL , &tv);
        //Check for error in the select
        if ((activity < 0) && (errno!=EINTR))   printf("select error");
        //-----------------------------------------------------------------------------

        //check for dead timed out sockets
        //-----------------------------------------------------------------------------
		CheckForDeadTimedOutSockets(TCPClients, address);
		//-----------------------------------------------------------------------------

        //If something happened on the master socket ,
        //then its an incoming connection
		CheckForIncomingConnections(&readfds, master_socket, address, TCPClients);
		//-----------------------------------------------------------------------------

        //else its some IO operation on some other socket
        for (unsigned int i = 0; i < TCPClients.size(); i++)
        {
            if (FD_ISSET(TCPClients.at(i).client_socket , &readfds))
            {
                //read the incoming message
            	valread = read( TCPClients.at(i).client_socket , bufferRead, BUFFER_SIZE);
            	//Check if it was for closing
                if (valread==0)	ClientHasDisconnected(TCPClients.at(i).client_socket, addrlen, i, address, TCPClients);
                else
                {
					//set the string terminating NULL byte on the end
					bufferRead[valread]= '\0';
					TCPClients.at(i).previous_response_time=std::chrono::system_clock::now();

                	//The buffer can be filled with more than one message, and that's an issue
                    //Make a list
                	std::string bufferRead_Line;
                	std::stringstream BufferToParse(bufferRead);

                	while(std::getline(BufferToParse, bufferRead_Line))
                	{
                		//std::cout<<"Casa"<<std::endl;
                		if(bufferRead_Line.size()>3)
                		{
                			//std::cout<<bufferRead_Line.capacity()<<std::endl;
                			//std::string bufferRead_Line2 = bufferRead_Line;

        					//PROCESS MESSAGE
        					if(FunctionToProcessMessages==nullptr)bufferWrite=bufferRead_Line;
        					else bufferWrite=FunctionToProcessMessages(bufferRead_Line);

        					if(!boost::contains(bufferRead_Line, "PGSI") &&
        					   !boost::contains(bufferRead_Line, "PWDA") &&
							   !boost::contains(bufferRead_Line, "RGMV") &&
							   !boost::contains(bufferRead_Line, "RFMV") &&
							   !boost::contains(bufferRead_Line, "RAMV") &&
							   !boost::contains(bufferRead_Line, "RDMV") &&
							   !boost::contains(bufferRead_Line, "RPRV") &&
							   !boost::contains(bufferRead_Line, "ALSC") &&
							   !boost::contains(bufferRead_Line, "CHAL") &&
							   !boost::contains(bufferRead_Line, "SCAP") &&
							   !boost::contains(bufferRead_Line, "WADI") &&
							   !boost::contains(bufferRead_Line, "PLRD") &&
							   !boost::contains(bufferRead_Line, "FOTP") &&
							   !boost::contains(bufferRead_Line, "GDIS") &&
        					   !boost::contains(bufferRead_Line, "PING")
        					){
            					std::cout << "Received  :  " << bufferRead_Line << std::endl;
            					std::cout << "Sent back :  " << bufferWrite << std::endl;
        					}

        					send(TCPClients.at(i).client_socket , bufferWrite.c_str() , strlen( bufferWrite.c_str() ) , 0 );
        					std::cout << "Sent: " << bufferWrite.c_str() << std::endl;
        					bufferWrite="";
                		}
                	}
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    return nullptr;
}

