/*
 * Pallet.cpp
 *
 *  Created on: Jan 27, 2018
 *      Author: andres
 */


#include "Pallet.h"

IndividualPallet* Pallet[10];

IndividualPallet::IndividualPallet(){
	//Empty Constructor
	UID="SERVER_NOT_READY";
	//VirtualPallet=0;
	UIDChanged=0;
	UIDChanged_count=0;

	Brick[0]=17;
	for(int i=1;i<112;i++){					//For each byte starting at position 0, so: for all memory positions
		Brick[i]=1;		//Write in the computer's memory the value used for empty
	}
}

IndividualPallet * DesiredPallet(int NumberOfThePallet){
	return Pallet[NumberOfThePallet-1];
}
void initPallet(int NumberOfPallets){
	for(int i=0;i<NumberOfPallets;i++){
		Pallet[i]=new IndividualPallet();
	}

}
