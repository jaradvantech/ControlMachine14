/*
 * ScoreCalculation.cpp
 *
 *  Created on: Jun 30, 2018
 *      Author: RBS
 */

#include "ScoreCalculation.h"
#include "StorageInterface.h"

int CheckForSurfaceCoincidence(int StorageNumber, int Grade, int Colour)
{

	return StorageGetRaw(StorageNumber, StorageGetNumberOfBricks(StorageNumber)) == (Colour << 4) + Grade;
}

int CheckForFirstMatch(int StorageNumber,int Grade, int Colour)
{
	int NumberOfBricks=StorageGetNumberOfBricks(StorageNumber);
	int PositionOfMatch=NumberOfBricks;
	int BrickRaw=(Colour << 4) + Grade;

	while(BrickRaw!=StorageGetRaw(StorageNumber,PositionOfMatch) || PositionOfMatch>=1) //Iterate until a match or end of pallet
	{
		PositionOfMatch--;
	}

	if(PositionOfMatch==0)return -1;

	return NumberOfBricks-PositionOfMatch;
}

int CheckQuantityContained(int StorageNumber, int BrickRaw)
{
	int NumberOfBricks=StorageGetNumberOfBricks(StorageNumber);
	int matches=0;

	for(int i=0; i<NumberOfBricks; i++)
		if(StorageGetRaw(StorageNumber, i) == BrickRaw)
			matches++;

	return matches;
}

int CheckInputScore(int NumberOfPallet, int TypeOfTheBrick , int _RawCurrentPackagingBrick)
{
	//When checking the Input Score we have to take in account :
	//-TOP at the pallet matches our brick
	//-Total ammount of bricks
	//-Percentage of equal in the pallet
	//-Relative position to the beginning of the line

	//---------------------------------------------------------
	int POINTS_OF_TOP_matches_our_brick;

	if(StorageGetNumberOfBricks(NumberOfPallet)>0 &&
			TypeOfTheBrick != StorageGetRaw(NumberOfPallet, StorageGetNumberOfBricks(NumberOfPallet))
	&& _RawCurrentPackagingBrick != StorageGetRaw(NumberOfPallet, StorageGetNumberOfBricks(NumberOfPallet)))
	{//If our brick does not match the top, but the packing type is different from the top, just bad points
		POINTS_OF_TOP_matches_our_brick=30;
	}
	else if(StorageGetNumberOfBricks(NumberOfPallet)>0 &&
			TypeOfTheBrick != StorageGetRaw(NumberOfPallet, StorageGetNumberOfBricks(NumberOfPallet))
		 && _RawCurrentPackagingBrick == StorageGetRaw(NumberOfPallet, StorageGetNumberOfBricks(NumberOfPallet)))
	{//If our brick does not match the top, but the packing type is equal to the top, very bad points
		POINTS_OF_TOP_matches_our_brick=100;

	}
	else if(StorageGetNumberOfBricks(NumberOfPallet)>0 &&
			TypeOfTheBrick == StorageGetRaw(NumberOfPallet, StorageGetNumberOfBricks(NumberOfPallet)))
	{//If our brick matches the top, regardless packing type, good points
		POINTS_OF_TOP_matches_our_brick=5;
	}
	else if(StorageGetNumberOfBricks(NumberOfPallet)==0)
	{
		//If the pallet is empty, it doesn't match the top. but it's slightly better
		POINTS_OF_TOP_matches_our_brick = 15;
	}

	//---------------------------------------------------------
	//---------------------------------------------------------
	int POINTS_OF_Total_ammount=StorageGetNumberOfBricks(NumberOfPallet);

	//---------------------------------------------------------
	//---------------------------------------------------------

	int POINTS_OF_Percentaje_of_equal_in_the_pallet=1;
	//TODO: uncomment this
	//POINTS_OF_Percentaje_of_equal_in_the_pallet=100-100*
	//	((float)CheckQuantityContained(NumberOfPallet, _BrickToClassify.Type))
	//						/
	//	((float)StorageGetNumberOfBricks(NumberOfPallet)));
	//---------------------------------------------------------
	//---------------------------------------------------------
	int POINTS_OF_Relative_position=(NumberOfPallet+1)/2;

	return POINTS_OF_TOP_matches_our_brick * 10
		 + POINTS_OF_Percentaje_of_equal_in_the_pallet * 1
		 + POINTS_OF_Total_ammount * 1
		 + POINTS_OF_Relative_position * 1;
}



