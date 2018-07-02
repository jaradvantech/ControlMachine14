/*
 * connexionRFID.c
 *
 *  Created on: Dec 12, 2017
 *      Author: Jose Andres Grau Martinez
 */

#include <connexionRFID.h>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <PalletAbstractionLayer/RFIDAnswerProcess.h>
#include <PalletAbstractionLayer/RFIDPetitionProcess.h>
#include "ConfigParser.h"
#include <SynchronizationPoints.h>

int NUMBEROFRFIDREADERS;
std::vector<std::string> RFID_IP_ADRESS;
std::vector<int> RFID_PORT;
std::vector<RFIDReader*> RFIDManager;
std::deque<std::string> messageList;
std::deque<std::string> emergencyList;

//////////////////////////////////////////
//Functions to perform the normal working, external layer.
//////////////////////////////////////////

void RFIDReaders_Configure()
{
	 ConfigParser config(CONFIG_FILE);
	 RFID_PORT = config.GetServerPorts();
	 RFID_IP_ADRESS = config.GetServerIPs();
	 NUMBEROFRFIDREADERS = config.GetNumberOfRFIDservers();

	 for(int i=0;i<NUMBEROFRFIDREADERS;i++)
	 {
		RFIDManager.push_back(new RFIDReader());
	}
}
void RFIDReaders_Start()
{
	emergencyList.clear(); //Clear emergency list
	for(int i=0;i<NUMBEROFRFIDREADERS;i++)
	{
		RFIDManager[i]->InitializeClient();
	}

	for(int i=0;i<NUMBEROFRFIDREADERS;i++){
		PetitionOf_ConfigUnit(i, RFID_IP_ADRESS[i], RFID_PORT[i], 0, 0, 0);

		for(int j=1;j<=4;j++){
			PetitionOf_ConfigChannel(i, j, 11, 1000, 4, 28,  1);
		}
	}
}

bool RFIDReader_isConnected(int RFIDServer)
{
	return RFIDManager[RFIDServer]->isConnected();
}

void RFIDReader_StoreRFIDReader_Config(int RFIDServer, std::string address,
		int port, bool FailSafe, bool RegisterOutputDriver1, bool RegisterOutputDriver2 ){
	RFIDManager[RFIDServer]->_Reader_ConfigInfo.RFIDServer = RFIDServer;
	RFIDManager[RFIDServer]->_Reader_ConfigInfo._address = address;
	RFIDManager[RFIDServer]->_Reader_ConfigInfo._port = port;
	RFIDManager[RFIDServer]->_Reader_ConfigInfo._FailSafe = FailSafe;
	RFIDManager[RFIDServer]->_Reader_ConfigInfo._RegisterOutputDriver1 = RegisterOutputDriver1;
	RFIDManager[RFIDServer]->_Reader_ConfigInfo._RegisterOutputDriver2 = RegisterOutputDriver2;

}

void RFIDReader_StoreRFIDReader_Channel_Config(int RFIDServer, short Channel, short Mode, short unsigned DataHoldTime, short unsigned LengthOfTagBlock,
short unsigned NumberOfBlocksOnTag,  bool ActivateDataHoldTime){
	RFIDManager[RFIDServer]->_Channel_ConfigInfo[Channel-1]._Channel=Channel;
	RFIDManager[RFIDServer]->_Channel_ConfigInfo[Channel-1]._Mode=Mode;
	RFIDManager[RFIDServer]->_Channel_ConfigInfo[Channel-1]._DataHoldTime=DataHoldTime;
	RFIDManager[RFIDServer]->_Channel_ConfigInfo[Channel-1]._LengthOfTagBlock=LengthOfTagBlock;
	RFIDManager[RFIDServer]->_Channel_ConfigInfo[Channel-1]._NumberOfBlocksOnTag=NumberOfBlocksOnTag;
	RFIDManager[RFIDServer]->_Channel_ConfigInfo[Channel-1]._ActivateDataHoldTime=ActivateDataHoldTime;

}

bool RFIDReader_OpenConnexion(int RFIDServer, std::string address , int port){
	return RFIDManager[RFIDServer]->OpenConnexion(address, port);
}


void AddMessage(std::string mMessage){
	messageList.push_back(mMessage);
}
void EmergencyAddMessage(std::string mMessage){
	emergencyList.push_back(mMessage);
}
/////////////////////////////////////////////////////////////////////
//RFIDReader class. Just provides encapsulation for the TCPClient
/////////////////////////////////////////////////////////////////////
RFIDReader::RFIDReader()
{
	TCPConnexion=nullptr;
	 InitializeClient();
}

int RFIDReader::InitializeClient(){
	delete TCPConnexion;
	TCPConnexion = new TCPClient;
	return 1;
}

void RFIDReader::ShutdownConnection(){
	TCPConnexion->detach();
}

bool RFIDReader::isConnected(){
	return TCPConnexion->Connected();
}

bool RFIDReader::OpenConnexion(std::string address , int port){
	return TCPConnexion->setup(address, port);
}

bool RFIDReader::SendCommand(std::string mMessage){
	return 	TCPConnexion->Send(mMessage);
}

std::string RFIDReader::ReadAnswer(){
	return 	TCPConnexion->read();
}

/////////////////////////////////////////////////////////////////////
//Loop to take the commands from the list and send them to the corresponding server
/////////////////////////////////////////////////////////////////////

void * RFIDLoop(void *Arg){
	 //Here we have the RFID Checking loop
	 std::string Answer="";
	 std::string Message="";
	 int RFIDServer;

	 std::deque<std::string>* ListToUse;
		Synchro::DecreaseSynchronizationPointValue(0);
	 while(1)
	 {
		 if (emergencyList.size()>0)
			 ListToUse = &emergencyList;
		 else
			 ListToUse = &messageList;

		 try
		 {
			 if(ListToUse->size()>0)
			 {
				 //PREPROCESS MESSAGE
				 Message=ListToUse->front();
				 //The message has to have the RFIDServer to who the message is
				 RFIDServer=boost::lexical_cast<int>(Message.substr(0, 1));
				 //Removes the RFIDServer to get the message that is going to be parsed. //RBS I don't like this
				 Message=Message.substr(1);

				 //Send Command has to be blocking because how the servers work.
				 if(RFIDManager[RFIDServer]->isConnected() != true)
					 throw std::runtime_error("Server disconnected");

				 if(RFIDManager[RFIDServer]->SendCommand(Message))
				 {
					 //Wait here until an answer is read.
					 Answer=RFIDManager[RFIDServer]->ReadAnswer();

					 if(Answer=="")
						 throw std::runtime_error("Empty Answer");

					 //Process here the answer.
					 if(processAnswer(Answer,RFIDServer)==0)
						 throw std::runtime_error("Message Error");

					 //when the answer is processed, we just pop out the actual message
					 ListToUse->pop_front();
				 }
				 else
					 throw std::runtime_error("Error on send");
			 }

		 } catch( ... ){
			//RBS This code does not work for a number of reasons
			//	-isConnected() Only tells if it has ever connected, but won't detect a disconnection
			//	-for some mysterious reason, we never get in the list messages destinated to unreachable servers
			//	-And therefore, exceptions are not being thrown and this is never executing.
			// UPDATE: RBS It's fixed (2/Jul/2018)

			emergencyList.clear();
			RFIDManager[RFIDServer]->ShutdownConnection();

			std::this_thread::sleep_for(std::chrono::seconds(3));
			RFIDManager[RFIDServer]->InitializeClient();

			//Update IP and Port
			ConfigParser config(CONFIG_FILE);
			PetitionOf_ConfigUnit(RFIDServer, config.GetServerIPs().at(RFIDServer), config.GetServerPorts().at(RFIDServer), 0, 0, 0);

			for(int j=1;j<=4;j++)
			{
				PetitionOf_ConfigChannel(RFIDServer, j, 11, 1000, 4,28,  1);
			}
		 }
		 //Wait Before polling again
		 std::this_thread::sleep_for(std::chrono::microseconds(500));
	 }
}
