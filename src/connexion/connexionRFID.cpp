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

int NUMBEROFRFIDREADERS;
std::vector<std::string> RFID_IP_ADRESS;
std::vector<int> RFID_PORT;
std::vector<RFIDReader*> RFIDManager;
std::deque<std::string> messageList;
std::deque<std::string> emergencyList;

//////////////////////////////////////////
//Functions to perform the normal working, external layer.
//////////////////////////////////////////

void RFIDReaders_Configure(){
	 //added by RBS on March 19th
	 ConfigParser config("/home/andres/unit14.conf");
	 RFID_PORT = config.GetServerPorts();
	 RFID_IP_ADRESS = config.GetServerIPs();
	 NUMBEROFRFIDREADERS = RFID_IP_ADRESS.size();

	 for(int i=0;i<NUMBEROFRFIDREADERS;i++){
		RFIDManager.push_back(new RFIDReader());
	}
}
void RFIDReaders_Start(){
	emergencyList.clear(); //Clear emergency list
	for(int i=0;i<NUMBEROFRFIDREADERS;i++){
		RFIDManager[i]->InitializeClient();
	}

	for(int i=0;i<NUMBEROFRFIDREADERS;i++){
		PetitionOf_ConfigUnit(i, RFID_IP_ADRESS[i], RFID_PORT[i], 0, 0, 0);

		for(int j=1;j<=4;j++){
			PetitionOf_ConfigChannel(i, j, 11, 1000, 4, 28,  1);
		}
	}
}

bool RFIDReader_isConnected(int RFIDServer){
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
	TCPConnexion=nullptr; //JAGM I hope that this does the trick
						  //and the TCP client doesn't leak memory anymore
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

	 while(1){
		 if (emergencyList.size()>0){
			 ListToUse = &emergencyList;
		 } else{
			 ListToUse = &messageList;
		 }
		 try{
			 if(ListToUse->size()>0){
				 /////////////////////
				 //PREPROCESS MESSAGE
				 /////////////////////
				 Message=ListToUse->front();
				 RFIDServer=boost::lexical_cast<int>(Message.substr(0, 1));  //The message has to have the RFIDServer to who the message is
				 Message=Message.substr(1); //Removes the RFIDServer to get the message that is going to be parsed.

				 //Send Command has to be blocking because how the servers work.
				 if(RFIDManager[RFIDServer]->SendCommand(Message)){ //Send Message
					 Answer=RFIDManager[RFIDServer]->ReadAnswer();   //Wait here until an answer is red.
					 if(Answer=="") throw std::runtime_error("Empty Answer");
				 if(processAnswer(Answer,RFIDServer)==0)	//Process here the answer.
				 {
					 throw std::runtime_error("Message Error");
				 }
				 ListToUse->pop_front();	//when the answer is processed, we just pop out the actual message
				 //std::cout << "Message poped" << std::endl;
				 } else{throw std::runtime_error("Error on send");}//throw exception maybe?
			 }
		 } catch( ... ){
			 emergencyList.clear();
			 //Close connection, but not sure if it's working as expected JAGM
			 RFIDManager[RFIDServer]->ShutdownConnection();
			 RFIDManager[RFIDServer]->InitializeClient();

			 PetitionOf_ConfigUnit(RFIDServer,RFID_IP_ADRESS[RFIDServer] , RFID_PORT[RFIDServer], 0, 0, 0);
				for(int j=1;j<=4;j++){
					PetitionOf_ConfigChannel(RFIDServer, j, 11, 1000, 4,28,  1);
				}
		 }

	 }
}
