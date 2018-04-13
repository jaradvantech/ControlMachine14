/*
 * VirtualPalletOperations.cpp
 *
 *  Created on: Feb 1, 2018
 *      Author: andres
 */




#include "VirtualPalletOperations.h"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

void VirtualPallet_PetitionOf_ReadUID(IndividualPallet* mPallet, int RFIDServer,int Channel){
	std::string mUID;
	mUID="VIRTUALPALLET_";
	mUID+=(boost::format("%02u")%(RFIDServer*4+(Channel))).str();

	std::string PreviousUID=mPallet->UID;

	mPallet->UID=mUID;								  //Store in the computer memory the value of the UID

	if(!boost::equal(mPallet->UID,PreviousUID)){
		mPallet->UIDChanged=1;
		mPallet->UIDChanged_count=0;
	}
	if (mPallet->UIDChanged == 1
			&& boost::equal(mPallet->UID, PreviousUID)) {
		mPallet->UIDChanged_count += 1;
	}
}

void VirtualPallet_PetitionOf_ReadAllMemory(IndividualPallet*mPallet,int  RFIDServer,int Channel){

}
void VirtualPallet_PetitionOf_FormatMemory(IndividualPallet*mPallet,int  RFIDServer,int Channel){
	mPallet->Brick[0]=17;
	for(int i=1;i<112;i++){					//For each byte starting at position 0, so: for all memory positions
		mPallet->Brick[i]=1;		//Write in the computer's memory the value used for empty
	}
}
void VirtualPallet_PetitionOf_AddBrick(IndividualPallet*mPallet,int  RFIDServer,int Channel,int Grade,int Colour){
	if(mPallet->Brick[0]>=100+17){

		std::cout << "The limit of 100 bricks has been reached: " <<  Channel << std::endl; //DEBUG
		std::cout << "+-----------------------------------------------------" <<std::endl;
	} else{
	mPallet->Brick[0]+=1;											 //Increase the computers memory byte 0 AKA number of bricks in one
	mPallet->Brick[mPallet->Brick[0]-17]=(Grade<<4)+Colour; //Go to the memory position of the new brick and write the information
	}
}
void VirtualPallet_PetitionOf_DeleteBrick(IndividualPallet*mPallet,int  RFIDServer,int Channel,int PositionToDelete){
	short NumberOfBricks = mPallet->Brick[0]-17;
	mPallet->Brick[0]-=1;											 //Decrease the computers memory byte 0 AKA number of bricks in one

	for(int i=PositionToDelete;i<=NumberOfBricks;i++){
		mPallet->Brick[i]=
				mPallet->Brick[i+1];//Skip the one to delete
	}
}
void VirtualPallet_PetitionOf_EditBrick(IndividualPallet*mPallet,int  RFIDServer,int Channel,int Position, int Grade,int  Colour){

}
void VirtualPallet_PetitionOf_InsertBrick(IndividualPallet*mPallet,int RFIDServer,int Channel,int Position, int Grade, int Colour){

}
void VirtualPallet_Init(IndividualPallet* mPallet[]){

}
