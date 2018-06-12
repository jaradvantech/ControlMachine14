//============================================================================
// Name        : Prueba1.cpp
// Author      : Jose Andres Grau Martinez
// Version     :
// Copyright   :
// Description :
//============================================================================

#include <iostream>
#include "stdio.h"
#include <pthread.h>
#include <thread>
#include <chrono>
#include <connexionPLC.h>
#include <connexionPLCdatahandler.h>
#include <connexionRFID.h>
#include <TCPServerLibrary.h>
#include <RoboticArmInfo.h>
#include <StorageInterface.h>
#include <PalletAbstractionLayer/Pallet.h>
#include <algorithms/algorithm_v2.h>
#include <SynchronizationPoints.h>
#include <boost/algorithm/string/predicate.hpp>
//#include <Logs.h>

TS7Client *ClientPlc14;
StructInfoPlc InfoPlc14;



pthread_t DisplayServerThread;
pthread_t RFIDClientThread;
pthread_t PLCThread;
pthread_t AlgorithmThread;

//RBS
void AttemptReconnection(void);



//------------------------------------------------------------------------------
// SysSleep (copied from sysutils.cpp) multiplatform millisec sleep
//------------------------------------------------------------------------------
void SysSleep(longword Delay_ms)
{
#ifdef OS_WINDOWS
	Sleep(Delay_ms);
#else
    struct timespec ts;
    ts.tv_sec = (time_t)(Delay_ms / 1000);
    ts.tv_nsec =(long)((Delay_ms - ts.tv_sec) * 1000000);
    nanosleep(&ts, (struct timespec *)0);
#endif
}

bool Check(TS7Client *Client, int Result, const char * function){
    printf("\n");
    printf("+-----------------------------------------------------\n");
    printf("| %s\n",function);
    printf("+-----------------------------------------------------\n");
    if (Result==0) {
        printf("| Result         : OK\n");
        printf("| Execution time : %d ms\n",Client->ExecTime());
        printf("+-----------------------------------------------------\n");
    } else {
        printf("| ERROR !!! \n");
        if (Result<0)
            printf("| Library Error (-1)\n");
        else
            printf("| %s\n",CliErrorText(Result).c_str());
        printf("+-----------------------------------------------------\n");
    }
    return Result==0;
}
bool CliConnectPLC(TS7Client *Client, StructInfoPlc Config){

    int res = Client->ConnectTo(Config.PlcAddress,Config.PlcRack,Config.PlcSlot);
    if (Check(Client, res, "UNIT Connection")) {
          printf("  Connected to   : %s (Rack=%d, Slot=%d)\n",Config.PlcAddress,Config.PlcRack,Config.PlcSlot);
          printf("  PDU Requested  : %d bytes\n",Client->PDURequested());
          printf("  PDU Negotiated : %d bytes\n",Client->PDULength());
    }
    return res==0;
}
void CliDisconnectPLC(TS7Client *Client){
     Client->Disconnect();
}

void * PLCAdquisitionLoop(void *Arg){

	Synchro::DecreaseSynchronizationPointValue(0);

	//RBS
	int ReadResult, WriteResult;

	while(1){
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
			AttemptReconnection();
	}
	return nullptr;
}

//RBS
void AttemptReconnection(void)
{
	std::cout << "Could not connect to PLC, trying to reconnect..." << std::endl;
	CliDisconnectPLC(ClientPlc14);

	//Wait for a couple of seconds before reconnecting
	std::this_thread::sleep_for(std::chrono::seconds(2));

	CliConnectPLC(ClientPlc14, InfoPlc14);
}

void init(){
	//--------------------------------------------------------------------
    // CONFIGURE PLCs
	//--------------------------------------------------------------------
	 ClientPlc14 = new TS7Client();

	 InfoPlc14.PlcAddress = "192.168.0.10";
	 InfoPlc14.PlcRack = 0;
	 InfoPlc14.PlcSlot = 1;
	 CliConnectPLC(ClientPlc14,InfoPlc14);
	 //--------------------------------------------------------------------
	 // OPEN DISPLAY COMMUNICATIONS
	 //--------------------------------------------------------------------
	 Synchro::IncreaseSynchronizationPointValue(0);
	 pthread_create(&DisplayServerThread, NULL, OpenServer, (void *)0);
	 //--------------------------------------------------------------------
	 // CONFIGURE RFID Servers
	 //--------------------------------------------------------------------
	 Synchro::IncreaseSynchronizationPointValue(0);
	 StorageConfigureRFID_StartUp();
	 pthread_create(&RFIDClientThread, NULL, RFIDLoop, (void *)0);
	 //--------------------------------------------------------------------
	 // CONFIGURE ROBOTIC ARMS
	 //--------------------------------------------------------------------
	 Synchro::IncreaseSynchronizationPointValue(0);
	 initGlobalArms(ClientPlc14);
	 pthread_create(&PLCThread, NULL, PLCAdquisitionLoop, (void *)0);
	 //--------------------------------------------------------------------
	 // CONFIGURE PALLETS
	 //--------------------------------------------------------------------
	 while(Synchro::GetSynchronizationPointValue(0)!=0)	 {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	 }

	 initPallet(10);
	 //Wait until all previous threads are working in their loops
	 Synchro::IncreaseSynchronizationPointValue(0);

	 pthread_create(&AlgorithmThread, NULL, AlgorithmV2, (void *)0);

	 while(Synchro::GetSynchronizationPointValue(0)!=0)	 {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	 }
	 //Wait until the algorithm has entered in its loop
}
int main() {

	init();

	//if (){


		printf("Here");


		//CliDisconnectPLC(ClientPlc14);

	//}

#define FORMATPALLETS 0
#if FORMATPALLETS
		 for(int i=1;i<=10;i++){
			StorageFormatMemory(i);
			std::cout << "Pallet " << i << "initialized succesfully" << std::endl;
		 }
#else
		//Algorithm implementation.

		while(1){

			//cout<< "MAIN PROGRAM END CYCLE" << endl;
			//Asks for the RFID tags.
			for(int i=1;i<=10;i++)
			{
				if(StorageReadUID(i)==0) std::this_thread::sleep_for(std::chrono::seconds(1)); 			//First read the UID of the pallet.
				else
				{
					if(StorageGetStoredUIDChangedFlag(i) && StorageGetStoredUIDChangedCount(i)>5)
					{  			//Check if there has been any pallet change
						StorageClearStoredUIDChangedFlag(i);
						if(!boost::equals(StorageGetStoredUID(i),"0000000000000000"))
						{	//If the change is to another pallet, read it.
							StorageReadAllMemory(i);
							//cout<< "This simulates a memory Read" << endl;
						}
						else
						{						 									//If the change is to no pallet, clear the memory.
							StorageCleanPalletMemory(i);
						}
					}
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
#endif

	//}
	exit(0);
	return 0;
}
//TODO:
/*
C++

	-Check input algorithm
	-Do output algorithm
	-Check reconnections and connections when the C++ program starts first


Android
	-Add motor control for tablet
	-Add support for SCAP
	-Add support for lateral movement
	-
*/


