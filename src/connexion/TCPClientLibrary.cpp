/*
 * TCPClientLibrary.cpp
 *
 *  Created on: Dec 16, 2017
 *      Author: Jose Andres Grau Martinez
 */
#include <TCPClientLibrary.h>


TCPClient::TCPClient()
{
	sock = -1;
	port = 0;
	address = "";
	_connected=0;
}

bool TCPClient::setup(std::string address , int port)
{
  	if(sock == -1)
	{
		sock = socket(AF_INET , SOCK_STREAM , 0);
		if (sock == -1)
		{
      			printf("| Could not create socket\n");
    		}
        }
  	if(inet_addr(address.c_str()) == -1)
  	{
    		struct hostent *he;
    		struct in_addr **addr_list;
    		if ( (he = gethostbyname( address.c_str() ) ) == NULL)
    		{
		      herror("gethostbyname");
      		      printf("| Failed to resolve hostname\n");
		      return false;
    		}
	   	addr_list = (struct in_addr **) he->h_addr_list;
    		for(int i = 0; addr_list[i] != NULL; i++)
    		{
      		      server.sin_addr = *addr_list[i];
		      break;
    		}
  	}
  	else
  	{
    		server.sin_addr.s_addr = inet_addr( address.c_str() );
  	}
  	server.sin_family = AF_INET;
  	server.sin_port = htons( port );
  	struct timeval tv;
  	tv.tv_sec = 3;  /* 5 Secs Timeout */
  	tv.tv_usec = 0;  // Not init'ing this can cause strange errors
  	int iSetOption=1;
  	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv,sizeof(struct timeval));
  	//setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv,sizeof(struct timeval));
  	setsockopt(sock, SOL_SOCKET,SO_REUSEADDR,(char*)&iSetOption,sizeof(iSetOption));
  	setsockopt(sock, SOL_SOCKET,SO_REUSEPORT,(char*)&iSetOption,sizeof(iSetOption));
  	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
  	{
	    	printf("+-----------------------------------------------------\n");
	    	perror("| Connect failed. Error");
    	    printf("| FAILED to connect to the RFID Receiver at %s\n",address.c_str());
    	    printf("+-----------------------------------------------------\n");
    		return 0;
  	} else {
  		_connected=1;
    printf("\n");
    printf("+-----------------------------------------------------\n");
    printf("| Connected to the RFID Receiver at %s\n",address.c_str());
    printf("+-----------------------------------------------------\n");

    }
  	return true;
}

bool TCPClient::Send(std::string data)
{
	int sendresult=send(sock , data.c_str() , strlen( data.c_str() ) , 0);
	//std::cout << "This happends when send: " << sendresult << std::endl;
	if(sendresult <= 0)	{
		std::cout << "Send failed : " << data << std::endl;
		return false;
	}
	return true;
}

std::string TCPClient::receive(int size)
{
  	char buffer[size];
  	std::string reply;

	if( recv(sock , buffer , size, 0) < 0)// sizeof(buffer)
  	{
	    printf("receive failed!\n");
	    reply ="";
  	}
	buffer[size]='\0';
  	reply = buffer;
	//cout << "Server Response:" << reply << endl;
  	return reply;
}

std::string TCPClient::read()
{
	//int MaxAttempts =3;
	//int ActualAttempt=0;
	//bool Success=1;
	//while(ActualAttempt<MaxAttempts){
	//	ActualAttempt++;
		char buffer[1] = {};
		std::string reply;
		//Success=1;
		while (buffer[0] != '\n') {
			int recvresult=recv(sock , buffer , sizeof(buffer) , 0);
			//std::cout << "This happends when send: " << recvresult << std::endl;
				if( recvresult <= 0)
				{
					//Success=0;
					printf("read failed!\n");
					break;
				}
			reply += buffer[0];
		}
		//cout << "Server Response:" << reply << endl;
	//	if(Success)
		return reply;
	//}
}

void TCPClient::detach()
{

	if(_connected!=0){
		shutdown(sock,2);
  	if (close(sock)<0)
  	{
	    	printf("+-----------------------------------------------------\n");
	    	perror("| Disconnection failed. Error");
    	    printf("| FAILED to disconnect RFID Receiver\n");
    	    printf("+-----------------------------------------------------\n");
  	} else {
  		sock=-1;
  		port = 0;
  		address = "";
  		_connected=0;
  	printf("+-----------------------------------------------------\n");
    printf("| Disconnected from RFID Receiver\n");
    printf("+-----------------------------------------------------\n");
  	}
	}else{
	  	printf("+-----------------------------------------------------\n");
	    printf("| Tried to close a flagged as closed socket\n");
	    printf("+-----------------------------------------------------\n");
	}
}

bool TCPClient::Connected() {return _connected;}

