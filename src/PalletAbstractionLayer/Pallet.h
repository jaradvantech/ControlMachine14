/*
 * Pallet.h
 *
 *  Created on: Jan 27, 2018
 *      Author: andres
 */


#ifndef PALLET_H_
#define PALLET_H_
#include <string>



class IndividualPallet {
	private:
	public:
		IndividualPallet();
		std::string UID;
		//bool VirtualPallet;
		bool UIDChanged;
		int UIDChanged_count;
		short Brick[112]; //112 bytes info per Pallet
		//---------------------------------------------
		//Byte 0: 		Number of bricks in this pallet
		//Byte 1-100: 	Each byte codes a brick, 1 is the bottom
		//Byte 101-112: Unused yet

		//CreateFileForPallet(); NOT IMPLEMENTED

		//bool AddBrickToArray(int Colour, int Grade);					//Performs Only internal operations
		//bool DeleteBrickFromArray(short index, int Colour, int Grade); //Performs ONLY internal operations
		//bool InsertBrickToArray(short index, int Colour, int Grade); //
		//bool LoadBricksInformation();

};

IndividualPallet * DesiredPallet(int NumberOfThePallet);

void initPallet(int NumberOfPallets);

#endif /* PALLET_H_ */
