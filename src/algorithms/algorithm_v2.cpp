/*
 * algorithm_v2.cpp
 *
 *  Created on: Mar 26, 2018
 *      Author: baseconfig
 */



#include "algorithm_v2.h"
#include <string>
#include <iostream>
#include <vector>
#include <deque>
#include <SynchronizationPoints.h>
#include "StorageInterface.h"
#include "RoboticArmInfo.h"

#define _LINELENGTH 80000

void SetNumberOfManipulators(std::vector<std::deque<Order>>* _Manipulator_Order_List, short _NumberOfManipulators)
{
	for(int i=0;i<_NumberOfManipulators;i++)
	{
		std::deque<Order> _ListOfOrders;
		_Manipulator_Order_List->push_back(_ListOfOrders);
	}

	delete _Manipulator_Order_List;
}

void Cancel_Order(std::deque<Brick>* _BricksOnTheLine, std::vector<std::deque<Order>>* _ManipulatorOrderList)
{


	delete _BricksOnTheLine;
	delete _ManipulatorOrderList;
}

void Cancel_Order(Brick* _BrickOnTheLine, std::vector<std::deque<Order>>* _ManipulatorOrderList)
{


	delete _BrickOnTheLine;
	delete _ManipulatorOrderList;
}

void Cancel_Order(int index, std::vector<std::deque<Order>>* Manipulator_Order_List)
{



	delete Manipulator_Order_List;
}

//------------------------BRICK CLASS ----------------------------------------
short Brick::size = 0; //former E
Brick::Brick(short argType, int argPosition, short argAssignedManipulator, short argDNI)
{
	Type = argType;
	Position = argPosition;
	AssignedManipulator = argAssignedManipulator;
	DNI=argDNI;
}

//------------------------ORDER CLASS ----------------------------------------
Order::Order(int mWhen, bool mWhere, bool mWhat)
{

	When = mWhen;
	Where = mWhere;
	What = mWhat;
}



void cancel_Order(std::deque<Brick>* mList_Of_Bricks, std::deque){

}



int Calculate_Advance(long &PreviousValueOfTheLineEncoder)
{
	int EncoderAdvance;
	if(RoboticArm::ActualValueOfTheLineEncoder-PreviousValueOfTheLineEncoder>=0){
		EncoderAdvance = RoboticArm::ActualValueOfTheLineEncoder-PreviousValueOfTheLineEncoder; //Get how much the encoder did advance
		PreviousValueOfTheLineEncoder+=EncoderAdvance; //Take this value and update for next iterations
	} else {
		EncoderAdvance = RoboticArm::ActualValueOfTheLineEncoder+100000-PreviousValueOfTheLineEncoder; //Get how much the encoder did advance
		PreviousValueOfTheLineEncoder+=EncoderAdvance-100000;
	}
	return EncoderAdvance;
}

void CheckPhotoSensor1(std::deque<Brick>* _BricksBeforeTheLine){
	static bool BrickOnTheQueueDetected_Trigger = false;
	if(RoboticArm::TileGrade !=0 && RoboticArm::TheQueueOfPhotosensor_1 && BrickOnTheQueueDetected_Trigger==false)
	{ //We have a new brick on the photosensor 1
		BrickOnTheQueueDetected_Trigger=true;

		Brick BrickOnTheLine(0, RoboticArm::TileGrade+16, 0,0); //

		_BricksBeforeTheLine->push_back(BrickOnTheLine);
		std::cout << "Brick of the type "<< BrickOnTheLine.Type << " enters the line" << std::endl;
		std::cout << "brick detected PS1" << std::endl;
	}else if(RoboticArm::TileGrade ==0 && RoboticArm::TheQueueOfPhotosensor_1){
		std::cout << "Why the fuck is this even showing?  PS1 acting weird" << std::endl;
		BrickOnTheQueueDetected_Trigger=false;
	}else{
		BrickOnTheQueueDetected_Trigger=false;
	}
}

bool CheckPhotoSensor4(std::deque<Brick>* _BricksBeforeTheLine, std::deque<Brick>* _BricksOnTheLine, std::deque<int>* Available_DNI_List)
{
	static int lastBrick_Position;
	if(RoboticArm::EnterTheTileStartingCodeValue !=lastBrick_Position){ //We have a new brick on the photosensor 4
		std::cout << "brick detected PS4" << std::endl;
		lastBrick_Position=RoboticArm::EnterTheTileStartingCodeValue;		//Update the values for future iterations


		//Complete brick information
		_BricksBeforeTheLine->front().Position = RoboticArm::ActualValueOfTheLineEncoder-RoboticArm::EnterTheTileStartingCodeValue;
		_BricksBeforeTheLine->front().DNI = Available_DNI_List->begin();
		_BricksBeforeTheLine->front().AssignedManipulator=0;
		//Add the hole that goes after this brick
		Brick Gap;
		Gap.Position = _BricksBeforeTheLine->front().Position - Brick::size - Brick::size/10; //Security margin of 1/10 of the size of the brick
		Gap.Type = 0; //It has no brick because it's a gap -.-

		//Add brick to the line
		_BricksOnTheLine->push_back(_BricksBeforeTheLine->front());	//And now, it enters the line
		_BricksOnTheLine->push_back(Gap);	//And the hole behind it too

		Available_DNI_List->pop_front();
		_BricksBeforeTheLine->pop_front();
		return true;
	}
	return false;
}


void update_list(std::deque<Brick>* mBrickList, int mEncoderAdvance, std::deque<int>* Available_DNI_List)
{
	//Update position of every brick
	for(int i=0; i<mBrickList->size; i++)
	{
		mBrickList->at(i).Position += mEncoderAdvance;
	}

	//Check if it's out of the line
	if(mBrickList->size()>0 && mBrickList->at(0)>=_LINELENGTH){
		std::cout << "Brick abandons the line because its position is "<< mBrickList->begin()->Position << std::endl;
		Available_DNI_List->push_back(mBrickList->begin()->DNI);
		mBrickList->pop_front();
	}
}

void FillDINs(std::deque<int>* _DNI_List){
	const int _MAXIMUMNUMBEROFBRICKSONLINE=10;
	for(int i=1;i<=_MAXIMUMNUMBEROFBRICKSONLINE;i++){
		_DNI_List->push_back(i);
	}
}

int CheckForSurfaceCoincidence(int StorageNumber, int Grade, int Colour){

	return StorageGetRaw(StorageNumber, StorageGetNumberOfBricks(StorageNumber)) == (Colour << 4) + Grade;
}

	//0 means that the top of the pallet matches the input
	//-1 means not found
	//Any value means the number of bricks in between.
int CheckForFirstMatch(int StorageNumber,int Grade, int Colour){
	int NumberOfBricks=StorageGetNumberOfBricks(StorageNumber);
	int PositionOfMatch=NumberOfBricks;//
	int BrickRaw=(Colour << 4) + Grade;

	while(BrickRaw!=StorageGetRaw(StorageNumber,PositionOfMatch) || PositionOfMatch>=1){ //Iterate until a match or end of pallet
		PositionOfMatch--;
	}

	if(PositionOfMatch==0)return -1;

	return NumberOfBricks-PositionOfMatch;
}

int CheckQuantityContained(int StorageNumber, int Grade, int Colour){
	int NumberOfBricks=StorageGetNumberOfBricks(StorageNumber);
	int BrickRaw=(Colour << 4) + Grade;
	int matches=0;

	for(int i=0; i<NumberOfBricks; i++)
		if(StorageGetRaw(StorageNumber, i) == BrickRaw)
			matches++;

	return matches;
}

int Choose_Best_Pallet()
{

}

void * AlgorithmV2(void *Arg)
{

	std::deque<int> Available_DNI_List;



	std::vector<std::deque<Order>> Manipulator_Order_List;
	std::deque<Brick> Bricks_Before_The_Line;
	std::deque<Brick> Bricks_On_The_Line;

	int CurrentPackagingColor, CurrentPackagingGrade;

	int ManipulatorOperationTime;
	long PreviousValueOfTheLineEncoder=RoboticArm::ActualValueOfTheLineEncoder;



	SetNumberOfManipulators(&Manipulator_Order_List, 5);

	Synchro::DecreaseSynchronizationPointValue(0);
	while(1)
	{

		//UPDATE ENCODER VALUES
		int EncoderAdvance = Calculate_Advance(PreviousValueOfTheLineEncoder);
		update_list(&Bricks_On_The_Line, EncoderAdvance, &Available_DNI_List);

		//CHECK FOR INPUT

		CheckPhotoSensor1(&Bricks_Before_The_Line); //Check for brick on photosensor 1 to store the grade

		if(CheckPhotoSensor4(&Bricks_Before_The_Line, &Bricks_On_The_Line, &Available_DNI_List)) //New brick detected!
		{
			//if brick should go through

				// if 16 is blocked
					//Choose_Best_Pallet();
				// else
					//goes through!
			//else
				//Choose_Best_Pallet();
		}



		//CHECK FOR OUTPUT




	}
}
