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
#include <boost/algorithm/string/predicate.hpp>
#include <Algorithm.h>
//#include <Logs.h>

TS7Client *ClientPlc14;
StructInfoPlc InfoPlc14;



pthread_t DisplayServerThread;
pthread_t RFIDClientThread;
pthread_t PLCThread;
pthread_t AlgorithmThread;
////Prueba
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

bool Check(TS7Client *Client, int Result,char * function){
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
	while(1){
		//std::chrono::_V2::high_resolution_clock::time_point t1 = std::chrono::_V2::high_resolution_clock::now();
		//Check(ClientPlc14,PerformGlobalReading(),"GLOBAL READING");
		PerformGlobalReading();
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
		Check(ClientPlc14,PerformGlobalWriting(),"GLOBAL Writing");
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
		//std::chrono::_V2::high_resolution_clock::time_point t2 = std::chrono::_V2::high_resolution_clock::now();

		//auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
		//cout << "Duration is: " << duration;
	}
}

void init(){
	//--------------------------------------------------------------------
    // CONFIGURE PLCs
	//--------------------------------------------------------------------
	 ClientPlc14 = new TS7Client();

	 InfoPlc14.PlcAddress = "192.168.0.199";
	 InfoPlc14.PlcRack = 0;
	 InfoPlc14.PlcSlot = 1;
	 CliConnectPLC(ClientPlc14,InfoPlc14);
	 //--------------------------------------------------------------------
	 // OPEN DISPLAY COMMUNICATIONS
	 //--------------------------------------------------------------------
	 int Port =24601;
	 pthread_create(&DisplayServerThread, NULL, OpenServer, (void *)0);
	 //--------------------------------------------------------------------
	 // CONFIGURE RFID Servers
	 //--------------------------------------------------------------------
	 //StorageConfigureRFID_StartUp();
	 //pthread_create(&RFIDClientThread, NULL, RFIDLoop, (void *)0);
	 //--------------------------------------------------------------------
	 // CONFIGURE ROBOTIC ARMS
	 //--------------------------------------------------------------------
		printf("Here");
	 initGlobalArms(ClientPlc14);
	 pthread_create(&PLCThread, NULL, PLCAdquisitionLoop, (void *)0);
	 //--------------------------------------------------------------------
	 // CONFIGURE PALLETS
	 //--------------------------------------------------------------------
	 initPallet(10);
	 for(int i=1;i<=10;i++){
		DesiredPallet(i)->VirtualPallet=1;
		StorageFormatMemory(i);
	 }
		int age;
		cin >> age;
	 pthread_create(&AlgorithmThread, NULL, Algorithm, (void *)0);


}
int main() {

	init();

	//if (){


		printf("Here");


		//CliDisconnectPLC(ClientPlc14);

	//}

		int age;
		cin >> age;
		cin >> age;

		//Algorithm implementation.
		while(1){

			for(int i=1;i<=10;i++){
				StorageReadUID(i); 			//First read the UID of the pallet.
				if(StorageGetStoredUIDChangedFlag(i) && StorageGetStoredUIDChangedCount(i)>5){  			//Check if there has been any pallet change
				    StorageClearStoredUIDChangedFlag(i);
					if(!boost::equals(StorageGetStoredUID(i),"0000000000000000")){	//If the change is to another pallet, read it.
						StorageReadAllMemory(i);
						//cout<< "This simulates a memory Read" << endl;
					}else{						 									//If the change is to no pallet, clear the memory.
						StorageCleanPalletMemory(i);
					}
				}
			}

			//cout<< "MAIN PROGRAM END CYCLE" << endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}

	//}
	exit(0);
	return 0;
}



