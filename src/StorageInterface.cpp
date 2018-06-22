/*
 * Storage.cpp
 *
 *  Created on: Dec 20, 2017
 *      Author: Jose Andres Grau Martinez
 */


#include <StorageInterface.h>
#include <map>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <PalletAbstractionLayer/Pallet.h>
#include <PalletAbstractionLayer/VirtualPalletOperations.h>
#include <PalletAbstractionLayer/RFIDPetitionProcess.h>

int StorageConfigureRFID_StartUp(){
    PrepareForPetitions_StartUp();
	return 1;
}
int StorageConfigureRFID_Restart(){
    PrepareForPetitions_Restart();
	return 1;
}
//TODO: Memory leak on IndividualPallet* mPallet=
//////////////////////////////////////////////////////////////////////////////
/// Read/Write to RFID  AND/OR WRITE PROGRAM MEMORY.
/// OPERATION LAYER. ALL OPERATIONS HAVE TO BE CALLED THROUGH THIS FUNCTIONS.
/// This functions just call the corresponding petition.
//////////////////////////////////////////////////////////////////////////////

int StorageReadUID(int StorageNumber){
	int RFIDServer, Channel;

	RFIDServer=(StorageNumber-1) / 4;
	Channel=StorageNumber-RFIDServer*4;

	//RFIDManager[RFIDServer]->ReadUID(Channel);
	//IndividualPallet* mPallet=DesiredPallet(RFIDServer*4+(Channel));
	//if(mPallet->VirtualPallet) VirtualPallet_PetitionOf_ReadUID(mPallet, RFIDServer,Channel);
	///else 	PetitionOf_ReadUID(RFIDServer,Channel);

	return PetitionOf_ReadUID(RFIDServer,Channel);
}

int StorageReadAllMemory(int StorageNumber){
	int RFIDServer, Channel;

	RFIDServer=(StorageNumber-1) / 4;
	Channel=StorageNumber-RFIDServer*4;

	//RFIDManager[RFIDServer]->ReadAllMemory(Channel);
	//IndividualPallet* mPallet=DesiredPallet(RFIDServer*4+(Channel));
	//if(mPallet->VirtualPallet) VirtualPallet_PetitionOf_ReadAllMemory(mPallet, RFIDServer,Channel);
	//else PetitionOf_ReadAllMemory(RFIDServer,Channel);
	PetitionOf_ReadAllMemory(RFIDServer,Channel);
	return 1;
}

int StorageFormatMemory(int StorageNumber){
	int RFIDServer, Channel;

	RFIDServer=(StorageNumber-1) / 4;
	Channel=StorageNumber-RFIDServer*4;

	//RFIDManager[RFIDServer]->FormatMemory(Channel);
	//IndividualPallet* mPallet=DesiredPallet(RFIDServer*4+(Channel));
	//if(mPallet->VirtualPallet) VirtualPallet_PetitionOf_FormatMemory(mPallet, RFIDServer,Channel);
	//else PetitionOf_FormatMemory(RFIDServer,Channel);
	PetitionOf_FormatMemory(RFIDServer,Channel);
	return 1;
}

int StorageAddBrick(int StorageNumber,int Grade, int Colour){
	int RFIDServer, Channel;

	RFIDServer=(StorageNumber-1) / 4;
	Channel=StorageNumber-RFIDServer*4;

	//RFIDManager[RFIDServer]->AddBrick(Channel,Grade,Colour);
	//IndividualPallet* mPallet=DesiredPallet(RFIDServer*4+(Channel));
	//if(mPallet->VirtualPallet) VirtualPallet_PetitionOf_AddBrick(mPallet, RFIDServer,Channel,Grade,Colour);
	//else PetitionOf_AddBrick(RFIDServer,Channel,Grade,Colour);
	PetitionOf_AddBrick(RFIDServer,Channel,Grade,Colour);
	return 1;
}

int StorageDeleteBrick(int StorageNumber,int Position){
	int RFIDServer, Channel;

	RFIDServer=(StorageNumber-1) / 4;
	Channel=StorageNumber-RFIDServer*4;

	//RFIDManager[RFIDServer]->DeleteBrick(Channel,Position);
	//IndividualPallet* mPallet=DesiredPallet(RFIDServer*4+(Channel));
	//if(mPallet->VirtualPallet) VirtualPallet_PetitionOf_DeleteBrick(mPallet, RFIDServer,Channel,Position);
	//else PetitionOf_DeleteBrick(RFIDServer,Channel,Position);
	PetitionOf_DeleteBrick(RFIDServer,Channel,Position);
	return 1;
}

int StorageEditBrick(int StorageNumber,int Position,int Grade, int Colour){
	int RFIDServer, Channel;

	RFIDServer=(StorageNumber-1) / 4;
	Channel=StorageNumber-RFIDServer*4;

	//RFIDManager[RFIDServer]->DeleteBrick(Channel,Position);
	//IndividualPallet* mPallet=DesiredPallet(RFIDServer*4+(Channel));
	//if(mPallet->VirtualPallet) VirtualPallet_PetitionOf_EditBrick(mPallet, RFIDServer,Channel,Position, Grade, Colour);
	//else PetitionOf_EditBrick(RFIDServer,Channel,Position, Grade, Colour);
	PetitionOf_EditBrick(RFIDServer,Channel,Position, Grade, Colour);
	return 1;
}

int StorageInsertBrick(int StorageNumber, int Position, int Grade, int Colour ){
	int RFIDServer, Channel;

	RFIDServer=(StorageNumber-1) / 4;
	Channel=StorageNumber-RFIDServer*4;

	//RFIDManager[RFIDServer]->DeleteBrick(Channel,Position);
	//IndividualPallet* mPallet=DesiredPallet(RFIDServer*4+(Channel));
	//if(mPallet->VirtualPallet) VirtualPallet_PetitionOf_InsertBrick(mPallet,RFIDServer,Channel,Position, Grade, Colour);
	//else PetitionOf_InsertBrick(RFIDServer,Channel,Position, Grade, Colour);
	PetitionOf_InsertBrick(RFIDServer,Channel,Position, Grade, Colour);
	return 1;
}

int StorageCleanPalletMemory(int StorageNumber){
	int RFIDServer, Channel;

	RFIDServer=(StorageNumber-1) / 4;
	Channel=StorageNumber-RFIDServer*4;
	IndividualPallet* mPallet=DesiredPallet(RFIDServer*4+(Channel));
	mPallet->Brick[0]=17;
	for(int i=1;i<112;i++){					//For each byte starting at position 0, so: for all memory positions
		mPallet->Brick[i]=1;		//Write in the computer's memory the value used for empty
	}
	return 1;
}

bool StorageClearStoredUIDChangedFlag(int StorageNumber){
	int RFIDServer, Channel;

	RFIDServer=(StorageNumber-1) / 4;
	Channel=StorageNumber-RFIDServer*4;
	IndividualPallet* mPallet=DesiredPallet(RFIDServer*4+(Channel));
	return mPallet->UIDChanged=0;
}
/////////////////////////////////////////////
/// Read ONLY PROGRAM MEMORY
/// VISIBLE LAYER. ALL the program has to use this functions to know the pallet state
/////////////////////////////////////////////

std::string StorageGetStoredUID(int StorageNumber){
	int RFIDServer, Channel;

	RFIDServer=(StorageNumber-1) / 4;
	Channel=StorageNumber-RFIDServer*4;
	IndividualPallet* mPallet=DesiredPallet(RFIDServer*4+(Channel));
	return mPallet->UID;
}

bool StorageGetStoredUIDChangedFlag(int StorageNumber){
	int RFIDServer, Channel;

	RFIDServer=(StorageNumber-1) / 4;
	Channel=StorageNumber-RFIDServer*4;
	IndividualPallet* mPallet=DesiredPallet(RFIDServer*4+(Channel));
	return mPallet->UIDChanged;
}

int StorageGetStoredUIDChangedCount(int StorageNumber){
	int RFIDServer, Channel;

	RFIDServer=(StorageNumber-1) / 4;
	Channel=StorageNumber-RFIDServer*4;
	IndividualPallet* mPallet=DesiredPallet(RFIDServer*4+(Channel));
	return mPallet->UIDChanged_count;
}

int StorageGetRaw(int StorageNumber,int Position){
	int RFIDServer, Channel;

	RFIDServer=(StorageNumber-1) / 4;
	Channel=StorageNumber-RFIDServer*4;
	IndividualPallet* mPallet=DesiredPallet(RFIDServer*4+(Channel));
	//if(Position==0) return 1;
	return mPallet->Brick[Position+1]; //RBS because position 0 represents the number of bricks
}

int StorageGetBrickColor(int StorageNumber,int Position){
	int RFIDServer, Channel;

	RFIDServer=(StorageNumber-1) / 4;
	Channel=StorageNumber-RFIDServer*4;
	IndividualPallet* mPallet=DesiredPallet(RFIDServer*4+(Channel));
	//if(Position==0) return 1;
	return mPallet->Brick[Position] & 15;
}

int StorageGetBrickGrade(int StorageNumber,int Position){
	int RFIDServer, Channel;

	RFIDServer=(StorageNumber-1) / 4;
	Channel=StorageNumber-RFIDServer*4;
	IndividualPallet* mPallet=DesiredPallet(RFIDServer*4+(Channel));
	//if(Position==0) return 1;
	return mPallet->Brick[Position] >> 4;
}

int StorageGetNumberOfBricks(int StorageNumber){
	int RFIDServer, Channel;

	RFIDServer=(StorageNumber-1) / 4;
	Channel=StorageNumber-RFIDServer*4;
	IndividualPallet* mPallet=DesiredPallet(RFIDServer*4+(Channel));
	return mPallet->Brick[0]-17;
}

