/*
 * connexionRFID.h
 *
 *  Created on: Dec 12, 2017
 *      Author: Jose Andres Grau Martinez
 */

#ifndef connexionRFID_h
#define connexionRFID_h



#include <TCPClientLibrary.h>

const int MAXIMUMNUMBEROFTRIES=5;

//////////////////////////////////////////
//Functions to perform the normal working, external layer.
//////////////////////////////////////////
void RFIDReaders_Configure();

void RFIDReaders_Start();

bool RFIDReader_isConnected(int RFIDServer);

void RFIDReader_StoreRFIDReader_Config(int RFIDServer, std::string address,
		int port, bool FailSafe, bool RegisterOutputDriver1, bool RegisterOutputDriver2 );

void RFIDReader_StoreRFIDReader_Channel_Config(int RFIDServer, short Channel, short Mode, short unsigned DataHoldTime, short unsigned LengthOfTagBlock,
		short unsigned NumberOfBlocksOnTag,  bool ActivateDataHoldTime);

bool RFIDReader_OpenConnexion(int RFIDServer, std::string address , int port);

void AddMessage(std::string mMessage);

void EmergencyAddMessage(std::string mMessage);

/////////////////////////////////////////////////////////////////////
//RFIDReader class. Just provides encapsulation for the TCPClient
/////////////////////////////////////////////////////////////////////
struct RFIDReader_Config{
	public:
	int RFIDServer;
	std::string _address;
	int _port;
	bool _FailSafe;
	bool _RegisterOutputDriver1;
	bool _RegisterOutputDriver2;
};
struct RFIDReader_Channel_Config{
	public:
		short _Channel;
		short _Mode;
		short unsigned _DataHoldTime;
		short unsigned _LengthOfTagBlock;
		short unsigned _NumberOfBlocksOnTag;
		bool _ActivateDataHoldTime;
};

class RFIDReader
{
	private:
		TCPClient *TCPConnexion;
	public:
		//IndividualPallet *Pallet[4];
		RFIDReader_Config _Reader_ConfigInfo;
		RFIDReader_Channel_Config _Channel_ConfigInfo[4];

		RFIDReader();
		int InitializeClient();
		bool OpenConnexion(std::string address , int port);
		bool isConnected();
		void ShutdownConnection();
		bool SendCommand(std::string mMessage);
		std::string ReadAnswer();
};

/////////////////////////////////////////////////////////////////////
//Loop to take the commands from the list and send them to the corresponding server
/////////////////////////////////////////////////////////////////////
void * RFIDLoop(void *Arg);




#endif /* CONNEXIONRFID_H */
