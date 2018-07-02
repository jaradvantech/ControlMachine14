/*
 * LineOperations.cpp
 *
 *  Created on: Jul 2, 2018
 *      Author: baseconfig
 */

#include "LineOperations.h"
#include <deque>
#include <algorithm>
#include "algorithms/algorithm_v2_types.h"
#include "RoboticArmInfo.h"
#include <iostream>


int Calculate_Advance(long* PreviousValueOfTheLineEncoder)
{
	int EncoderAdvance=0;
	if(RoboticArm::ActualValueOfTheLineEncoder-*PreviousValueOfTheLineEncoder>=0){
		EncoderAdvance = RoboticArm::ActualValueOfTheLineEncoder-*PreviousValueOfTheLineEncoder; //Get how much the encoder did advance
		*PreviousValueOfTheLineEncoder+=EncoderAdvance; //Take this value and update for next iterations
	} else {
		EncoderAdvance = RoboticArm::ActualValueOfTheLineEncoder+100000-*PreviousValueOfTheLineEncoder; //Get how much the encoder did advance
		*PreviousValueOfTheLineEncoder+=EncoderAdvance-100000;
	}
	return EncoderAdvance;
}

void CheckPhotoSensor1(std::deque<Brick>* _BricksBeforeTheLine){
	static bool BrickOnTheQueueDetected_Trigger = false;

	if(RoboticArm::PCState==3){
	//Do nothing, manual mode
	}
	else if(RoboticArm::TheQueueOfPhotosensor_1 && BrickOnTheQueueDetected_Trigger==false)
	{ //We have a new brick on the photosensor 1
		BrickOnTheQueueDetected_Trigger=true;

		Brick BrickOnTheLine(0, 0, 0, 0); //

		_BricksBeforeTheLine->push_back(BrickOnTheLine);
		//std::cout << "Brick of the type "<< BrickOnTheLine.Type << " enters the line" << std::endl;
		std::cout << "brick detected PS1" << std::endl;
	}else if(RoboticArm::TheQueueOfPhotosensor_1==false){
		BrickOnTheQueueDetected_Trigger=false;
	}

	//When the brick enters the PhotoSensor1 it doesn't need to have a type instantly assigned.
	//as long as it has one before entering the photosensor 4 we are cool.
	//and if it doesn't, it will get one by default.

	//The purpose of this function is that the grading button is pressed right after the brick has entered
	//the photosensor.
	//So, if the grading color changes, the nearest brick to PS1 will change its type accordingly

	//As a side effect, if no button is pressed, the new bricks will have a type equal to LastType
	//And by default, the type is 17.


	//CONSTANTLY CHECK THE ACTUAL TYPE

	static int LastType=1;
	if(RoboticArm::TileGrade !=LastType && RoboticArm::TileGrade !=0)
	{
		LastType = RoboticArm::TileGrade;
	}
	//MODIFY THE LAST BRICK OF _BricksBeforeTheLine
	if(RoboticArm::PCState==3)
	{
		//Do nothing, manual mode
	}
	else if(LastType != 0 && _BricksBeforeTheLine->size() > 0)
	{
		_BricksBeforeTheLine->at(_BricksBeforeTheLine->size()-1).Type=LastType+16;
	}
}

bool CheckPhotoSensor4(std::deque<Brick>* _BricksBeforeTheLine, std::deque<Brick>* _BricksOnTheLine, std::deque<int>* Available_DNI_List)
{
	static int lastBrick_Position=RoboticArm::EnterTheTileStartingCodeValue;
	//static bool BrickOnTheLineDetected_Trigger = false;
	if(RoboticArm::PCState==3)
	{
	//Do nothing, manual mode
	}
	else if(RoboticArm::TheQueueOfPhotosensor_4 && RoboticArm::EnterTheTileStartingCodeValue !=lastBrick_Position
	&& 	_BricksBeforeTheLine->size()>0)
	{ //We have a new brick on the photosensor 4
		std::cout << "brick detected PS4" << std::endl;
		lastBrick_Position=RoboticArm::EnterTheTileStartingCodeValue;		//Update the values for future iterations

		if (Available_DNI_List->size()==0){
			std::cout<<" No available DNIs "<< Available_DNI_List->size() <<std::endl;
			return 0;
		}

		//Complete brick information
		_BricksBeforeTheLine->begin()->Position = RoboticArm::ActualValueOfTheLineEncoder-RoboticArm::EnterTheTileStartingCodeValue;
		_BricksBeforeTheLine->begin()->DNI = Available_DNI_List->front();

		//_BricksBeforeTheLine->begin()->Type=17;
		/////////////////////////////////////

		std::cout<<" DNI to assign"<< Available_DNI_List->front() <<std::endl;
		std::cout<<"Assigned DNI "<< _BricksBeforeTheLine->begin()->DNI <<std::endl;
		std::cout<<"Type of the brick " << _BricksBeforeTheLine->begin()->Type << std::endl;
		_BricksBeforeTheLine->begin()->AssignedPallet=0;
		//Add the hole that goes after this brick
		Brick Gap(0,0,0,0);
		Gap.Position = _BricksBeforeTheLine->front().Position - E - E/10; //Security margin of 1/10 of the size of the brick
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


