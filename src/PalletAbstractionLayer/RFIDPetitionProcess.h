/*
 * RFIDPetitionProcess.h
 *
 *  Created on: Jan 29, 2018
 *      Author: andres
 */

#ifndef PALLETABSTRACTIONLAYER_RFIDPETITIONPROCESS_H_
#define PALLETABSTRACTIONLAYER_RFIDPETITIONPROCESS_H_


#include <string>
#include <iostream>
void PrepareForPetitions_StartUp();

void PrepareForPetitions_Restart();

int PetitionOf_ConfigUnit(int RFIDServer,std::string address , int port,
	    bool FailSafe, bool RegisterOutputDriver1, bool RegisterOutputDriver2);

int PetitionOf_ConfigChannel(int RFIDServer, short Channel, short Mode, short unsigned DataHoldTime, short unsigned LengthOfTagBlock,
		   short unsigned NumberOfBlocksOnTag,  bool ActivateDataHoldTime);

int PetitionOf_ReadUID(int RFIDServer, short Channel);

int PetitionOf_ReadAllMemory(int RFIDServer, short Channel);

int PetitionOf_FormatMemory(int RFIDServer, short Channel);

int PetitionOf_AddBrick(int RFIDServer, short Channel,int Grade, int Colour);

int PetitionOf_DeleteBrick(int RFIDServer, short Channel,int PositionToDelete);

int PetitionOf_EditBrick(int RFIDServer, short Channel,int Position, int Grade, int Colour);

int PetitionOf_InsertBrick(int RFIDServer, short Channel,int Position, int Grade, int Colour);

#endif /* PALLETABSTRACTIONLAYER_RFIDPETITIONPROCESS_H_ */
