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
	UID="";
	VirtualPallet=0;
	UIDChanged=0;
	UIDChanged_count=0;
}

IndividualPallet * DesiredPallet(int NumberOfThePallet){
	return Pallet[NumberOfThePallet-1];
}
void initPallet(int NumberOfPallets){
	for(int i=0;i<NumberOfPallets;i++){
		Pallet[i]=new IndividualPallet();
	}

}
