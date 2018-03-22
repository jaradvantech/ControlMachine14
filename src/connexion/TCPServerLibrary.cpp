/*
 * TCPServerLibrary.cpp
 *
 *  Created on: Dec 22, 2017
 *      Author: Jose Andres Grau Martinez
 */

#include <TCPServerLibrary.h>


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
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <StorageInterface.h>
#include <connexionDisplay.h>


#define TRUE   1
#define FALSE  0

int KeepConnectionOpened = 1;

inline const char * const TCPSBoolToString(bool b) {
	return b ? "1" : "0";
}


void * OpenServer(void *Arg)
{
	pthread_detach((unsigned long)pthread_self());
	printf("despues");

    int opt = TRUE;
    int master_socket , addrlen , new_socket , client_socket[30] ,
          max_clients = 30 , activity, i , valread , sd;
    int max_sd;
    struct sockaddr_in address;

    std::chrono::time_point<std::chrono::system_clock> previous_response_time[30];
    for(int i=0;i<30;i++){
    	previous_response_time[i]=std::chrono::system_clock::now();
    }
    char bufferRead[1025];  //data buffer of 1K
    std::string bufferWrite;  //data buffer of 1K

    //set of socket descriptors
    fd_set readfds;

    //a message
    char *message = "Communication established.\r\n";

    //initialise all client_socket[] to 0 so not checked
    for (i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;
    }

    //create a master socket
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //set master socket to allow multiple connections ,
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,

          sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    //type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( 24601 );

    //bind the socket to localhost port 24601
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", 24601);

    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    //accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections ...");


    while(KeepConnectionOpened)
    {
        //clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        //add child sockets to set
        for ( i = 0 ; i < max_clients ; i++)
        {
            //socket descriptor

            sd = client_socket[i];

            //if valid socket descriptor then add to read list
            if(sd > 0)
                FD_SET( sd , &readfds);

            //highest file descriptor number, need it for the select function
            if(sd > max_sd)
                max_sd = sd;
        }

        //wait for an activity on one of the sockets , timeout is NULL ,
        //so wait indefinitely
        struct timeval tv;
        tv.tv_sec = 3;  /* 5 Secs Timeout */
        tv.tv_usec = 0;  // Not init'ing this can cause strange errors



        activity = select( max_sd + 1 , &readfds , NULL , NULL , &tv);

        if ((activity < 0) && (errno!=EINTR))
        {
            printf("select error");
        }
        //check for dead timed out sockets
        for (i = 0; i < max_clients; i++)
        {
			//if position is NOT empty
			if (client_socket[i] != 0)
			{
				std::chrono::duration<double> elapsed_time =
						std::chrono::system_clock::now()
								- previous_response_time[i];

				if (elapsed_time.count() > 10)
				{
					getpeername(client_socket[i], (struct sockaddr*) &address,
							(socklen_t*) &addrlen);
					printf("Host lost connection , ip %s , port %d \n",
							inet_ntoa(address.sin_addr),
							ntohs(address.sin_port));
					//Close the socket and mark as 0 in list for reuse
					close(client_socket[i]);
					client_socket[i] = 0;
				}
			}
		}


        //If something happened on the master socket ,
        //then its an incoming connection
        if (FD_ISSET(master_socket, &readfds))
        {

            if ((new_socket = accept(master_socket,
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            //inform user of socket number - used in send and receive commands
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n" ,
            		new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

            //send new connection greeting message
            if( send(new_socket, message, strlen(message), 0) != strlen(message) )
            {
                perror("send");
            }

            puts("Welcome message sent successfully");

            //add new socket to array of sockets
            for (i = 0; i < max_clients; i++)
            {
                //if position is empty
                if( client_socket[i] == 0 )
                {
                    client_socket[i] = new_socket;
                    previous_response_time[i]=std::chrono::system_clock::now();
                    printf("Adding to list of sockets as %d\n" , i);

                    break;
                }
            }
        }

        //else its some IO operation on some other socket
        for (i = 0; i < max_clients; i++)
        {
            sd = client_socket[i];

            if (FD_ISSET( sd , &readfds))
            {
                //Check if it was for closing , and also read the
                //incoming message
                if ((valread = read( sd , bufferRead, 1024)) == 0)
                {
                    //Somebody disconnected , get his details and print
                    getpeername(sd , (struct sockaddr*)&address , \
                        (socklen_t*)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n" ,
                          inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

                    //Close the socket and mark as 0 in list for reuse
                    close( sd );
                    client_socket[i] = 0;
                }

                //Echo back the message that came in
                else
                {

                    //set the string terminating NULL byte on the end
                	bufferRead[valread]= '\0';
                    previous_response_time[i]=std::chrono::system_clock::now();
                	bool ServerIsReady = true;
                	//TODO RBS, replace contains by substring(0,4)
                	if (boost::contains(bufferRead, "PGSI") && ServerIsReady)
                	{
						try {bufferWrite = Command_PGSI(bufferRead);}
						catch(...){
							std::cout << "Command_PGSI threw"<< std::endl;
							bufferWrite = "Error_PGSI\r\n";
						}
						std::cout << bufferWrite << std::endl;
					}
                	else if (boost::contains(bufferRead,"PWDA") && ServerIsReady)
                	{
                		try {bufferWrite = Command_PWDA(bufferRead);}
						catch(...){
							std::cout << "Command_PWDA threw"<< std::endl;
							bufferWrite = "Error_PWDA\r\n";
						}
					}
                	else if (boost::contains(bufferRead,"RGMV") && ServerIsReady)
                	{
                		try {bufferWrite = Command_RGMV(bufferRead);}
						catch(...){
							std::cout << "Command_RGMV threw"<< std::endl;
							bufferWrite = "Error_RGMV\r\n";
						}
					}
                	else if (boost::contains(bufferRead,"RFMV") && ServerIsReady)
                	{
                		try {bufferWrite = Command_RFMV(bufferRead);}
						catch(...){
							std::cout << "Command_RFMV threw"<< std::endl;
							bufferWrite = "Error_RFMV\r\n";
						}
                	}
                	else if (boost::contains(bufferRead,"RAMV") && ServerIsReady)
                	{
                		try {bufferWrite = Command_RAMV(bufferRead);}
						catch(...){
							std::cout << "Command_RAMV threw"<< std::endl;
							bufferWrite = "Error_RAMV\r\n";
						}
                	}
                	else if (boost::contains(bufferRead,"RDMV") && ServerIsReady)
                	{
                		try {bufferWrite = Command_RDMV(bufferRead);}
						catch(...){
							std::cout << "Command_RDMV threw"<< std::endl;
							bufferWrite = "Error_RDMV\r\n";
						}
                	}
                	else if (boost::contains(bufferRead,"RPRV") && ServerIsReady)
                	{
                		try {bufferWrite = Command_RPRV(bufferRead);}
						catch(...){
							std::cout << "Command_RPRV threw"<< std::endl;
							bufferWrite = "Error_RPRV\r\n";
						}
                	}
                	else if (boost::contains(bufferRead,"PING"))
                	{
                		/* RBS 20/03/2018
                		 * Used to test connection. If this fails, the display app
                		 * will try reestablishing the communication.
                		 */

                	    bufferWrite = "PING_PLC14_echo_reply\r\n";

                	    std::cout << ".";
                	    fflush(stdout);
                	}


                	else
                	{
						bufferWrite = "Incorrect!!\r\n";
					}




                	//std::cout << "Sent back: " << bufferWrite << std::endl;
                	//bufferWrite=bufferRead;
                    send(sd , bufferWrite.c_str() , strlen( bufferWrite.c_str() ) , 0 );
            		bufferWrite="";
            		std::this_thread::sleep_for(std::chrono::milliseconds(5));
                }
            }
        }
    }
    //return 0;
}
/*
std::string GetPlcInfo(){
	std::string Answer;
		Answer="PLC_INFO=";
		Answer+=GlobalArm[0]->StorageBinDirection;
		Answer+="_";
		Answer+=GlobalArm[0]->ManipulatorReset;
		Answer+="_";
	return Answer;
}
*/

