#include <iostream>
#include "stdio.h"
#include <pthread.h>
#include <thread>
#include <chrono>
#include <connexionPLC.h>
#include <connexionPLCdatahandler.h>
#include <TCPServerLibrary.h>
#include <RoboticArmInfo.h>
#include <SynchronizationPoints.h>
#include "ConfigParser.h"
#include "PLC_communications.h"

TS7Client *ClientPlc14;
StructInfoPlc InfoPlc14;

/* RBS
 * TODO: rewrite this file to use more cplusplusy <iostream> instead of plain old printf.
 */

bool Check(TS7Client *Client, int Result, const char * function)
{
    printf("\n");
    printf("+-----------------------------------------------------\n");
    printf("| %s\n",function);
    printf("+-----------------------------------------------------\n");
    if (Result==0)
    {
        printf("| Result         : OK\n");
        printf("| Execution time : %d ms\n",Client->ExecTime());
        printf("+-----------------------------------------------------\n");
    }
    else
    {
        printf("| ERROR !!! \n");
        if (Result<0)
            printf("| Library Error (-1)\n");
        else
            printf("| %s\n",CliErrorText(Result).c_str());
        printf("+-----------------------------------------------------\n");
    }
    return Result==0;
}

bool CliConnectPLC(TS7Client *Client, StructInfoPlc Config)
{

    int res = Client->ConnectTo(Config.PlcAddress,Config.PlcRack,Config.PlcSlot);
    if (Check(Client, res, "PLC Connection"))
    {
          printf("  Connected to   : %s (Rack=%d, Slot=%d)\n",Config.PlcAddress,Config.PlcRack,Config.PlcSlot);
          printf("  PDU Requested  : %d bytes\n",Client->PDURequested());
          printf("  PDU Negotiated : %d bytes\n",Client->PDULength());
    }
    return res==0;
}

void CliDisconnectPLC(TS7Client *Client)
{
     Client->Disconnect();
}

void attemptConnection(void)
{
	ConfigParser config(CONFIG_FILE);
	bool success = false;
	do{
		InfoPlc14.PlcAddress = config.GetPLCAddress().c_str();
		InfoPlc14.PlcRack = 0;
		InfoPlc14.PlcSlot = 1;
		success = CliConnectPLC(ClientPlc14,InfoPlc14);
		if(!success)
		{
			//If connection failed, try again in 5 seconds.
			std::this_thread::sleep_for(std::chrono::seconds(5));
		}
	} while(!success);
}

void * PLCAdquisitionLoop(void *Arg)
{

	Synchro::DecreaseSynchronizationPointValue(0);

	//RBS
	int ReadResult, WriteResult;

	while(1)
	{
		//std::chrono::_V2::high_resolution_clock::time_point t1 = std::chrono::_V2::high_resolution_clock::now();
		//Check(ClientPlc14,PerformGlobalReading(),"GLOBAL READING");
		ReadResult = PerformGlobalReading();
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
		//Check(ClientPlc14,,"GLOBAL Writing");
		WriteResult = PerformGlobalWriting();
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
		//std::chrono::_V2::high_resolution_clock::time_point t2 = std::chrono::_V2::high_resolution_clock::now();

		//auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
		//cout << "Duration is: " << duration;

		if((ReadResult != 0) || (WriteResult != 0))
		{
			std::cerr << "Could not connect to PLC, trying to reconnect..." << std::endl;
			CliDisconnectPLC(ClientPlc14);
			std::this_thread::sleep_for(std::chrono::seconds(2));
			attemptConnection();
		}
	}
	pthread_exit(NULL);
}

void configurePLC()
{
	 ClientPlc14 = new TS7Client();
	 attemptConnection();
}
