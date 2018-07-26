//============================================================================
// Name        : Prueba1.cpp
// Author      : Jose Andres Grau Martinez & Roberto Salinas rosich
// Version     :
// Copyright   : JARA ADVANTECH S.L.
// Description : Control program for Machine 14 (Intelligent Picking Station)
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
#include "connexion/connexionDisplay.h"
#include "ConfigParser.h"
#include "PLC_communications.h"
#include "DB_AbstractionLayer.h"


pthread_t DisplayServerThread;
pthread_t RFIDClientThread;
pthread_t PLCThread;
pthread_t AlgorithmThread;


void init(int numberOfArms){

	//--------------------------------------------------------------------
	// OPEN DISPLAY COMMUNICATIONS
	//--------------------------------------------------------------------
	Synchro::IncreaseSynchronizationPointValue(0);
	InitDisplayParser();
	pthread_create(&DisplayServerThread, NULL, OpenServer, (void *)0);

	//--------------------------------------------------------------------
	// CONFIGURE PLCs
	//--------------------------------------------------------------------
	configurePLC();

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

	initPallet(numberOfArms*2);
	//Wait until all previous threads are working in their loops
	Synchro::IncreaseSynchronizationPointValue(0);

	pthread_create(&AlgorithmThread, NULL, AlgorithmV2, (void *)0);

	while(Synchro::GetSynchronizationPointValue(0)!=0)	 {
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	//Wait until the algorithm has entered in its loop

	getMySQL_Manager()->InitMySQL("192.168.0.151","Machine14");
	InsertRegister();
	RetrieveRegister();
	//TestMySQL();
}

int main()
{
	ConfigParser config(CONFIG_FILE);
	int total_arms = config.GetNumberOfArms();
	int total_RFIDservers = config.GetNumberOfRFIDservers();
	std::cout<< "Starting control program for the intelligent Picking Station" << std::endl;
	std::cout<< "This unit-14 has " << total_arms << " manipulators and " << total_RFIDservers << " RFID Servers." << std::endl;
	std::cout<< "The IP address configuration is as follows:" << std::endl;
	std::cout<< ">PLC....................." << config.GetPLCAddress() << std::endl;
	for(int i=0; i<total_RFIDservers; i++)
	{
		std::cout<< ">RFID Server " << i+1 << "..........." << config.GetServerIPs().at(i) << ":"<<  config.GetServerPorts().at(i) << std::endl;
	}
	std::cout<< "Powered By JARA" << std::endl;

	init(total_arms);

	std::cout << std::endl << "Initialization routine completed." << std::endl;



#define FORMATPALLETS 0
#if FORMATPALLETS
	 for(int i=1;i<=10;i++){
		StorageFormatMemory(i);
		std::cout << "Pallet " << i << "initialized succesfully" << std::endl;
	 }
#else
	while(1){

		//cout<< "MAIN PROGRAM END CYCLE" << endl;
		//Asks for the RFID tags.
		for(int i=1;i<=total_arms*2;i++)
		{
			if(StorageReadUID(i)==0)
			{
				std::this_thread::sleep_for(std::chrono::seconds(1)); 			//First read the UID of the pallet.
			}
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

	return 0;
}



