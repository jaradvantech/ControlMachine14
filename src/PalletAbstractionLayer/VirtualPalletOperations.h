/*
 * VirtualPalletOperations.h
 *
 *  Created on: Feb 1, 2018
 *      Author: andres
 */

#ifndef PALLETABSTRACTIONLAYER_VIRTUALPALLETOPERATIONS_H_
#define PALLETABSTRACTIONLAYER_VIRTUALPALLETOPERATIONS_H_

#include "Pallet.h"
void VirtualPallet_PetitionOf_ReadUID(IndividualPallet* mPallet, int RFIDServer,int Channel);
void VirtualPallet_PetitionOf_ReadAllMemory(IndividualPallet*mPallet,int  RFIDServer,int Channel);
void VirtualPallet_PetitionOf_FormatMemory(IndividualPallet*mPallet,int  RFIDServer,int Channel);
void VirtualPallet_PetitionOf_AddBrick(IndividualPallet*mPallet,int  RFIDServer,int Channel,int Grade,int Colour);
void VirtualPallet_PetitionOf_DeleteBrick(IndividualPallet*mPallet,int  RFIDServer,int Channel,int PositionToDelete);
void VirtualPallet_PetitionOf_EditBrick(IndividualPallet*mPallet,int  RFIDServer,int Channel,int Position, int Grade,int  Colour);
void VirtualPallet_PetitionOf_InsertBrick(IndividualPallet*mPallet,int RFIDServer,int Channel,int Position, int Grade, int Colour);
void VirtualPallet_Init(IndividualPallet* mPallet[]);

#endif /* PALLETABSTRACTIONLAYER_VIRTUALPALLETOPERATIONS_H_ */
