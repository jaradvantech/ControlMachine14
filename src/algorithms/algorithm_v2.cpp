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
#include "ConfigParser.h"


void SetNumberOfManipulators(std::vector<std::deque<Order>>* _Manipulator_Order_List, short _NumberOfManipulators)
{
	for(int i=0;i<_NumberOfManipulators;i++)
	{
		std::deque<Order> _ListOfOrders;
		_Manipulator_Order_List->push_back(_ListOfOrders);
	}
}

void Cancel_Order(std::deque<Brick>* _BricksOnTheLine, std::vector<std::deque<Order>>* _ManipulatorOrderList)
{


}

void Cancel_Order(Brick* _BrickOnTheLine, std::vector<std::deque<Order>>* _ManipulatorOrderList)
{

}

void Cancel_Order(int index, std::vector<std::deque<Order>>* Manipulator_Order_List)
{

}

//------------------------BRICK CLASS ----------------------------------------
short Brick::size = 0; //former E
Brick::Brick(short argType, int argPosition, short argAssignedPallet, short argDNI)
{
	Type = argType;
	Position = argPosition;
	AssignedPallet = argAssignedPallet;
	DNI=argDNI;
}

//------------------------ORDER CLASS ----------------------------------------
Order::Order(int mWhen, bool mWhere, bool mWhat)
{

	When = mWhen;
	Where = mWhere;
	What = mWhat;
}

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
	int TileGrade = RoboticArm::TileGrade;
	if(TileGrade !=0 && RoboticArm::TheQueueOfPhotosensor_1 && BrickOnTheQueueDetected_Trigger==false)
	{ //We have a new brick on the photosensor 1
		BrickOnTheQueueDetected_Trigger=true;

		Brick BrickOnTheLine(TileGrade+16,0, 0,0); //

		_BricksBeforeTheLine->push_back(BrickOnTheLine);
		std::cout << "Brick of the type "<< BrickOnTheLine.Type << " enters the line" << std::endl;
		std::cout << "brick detected PS1" << std::endl;
	}else if(TileGrade ==0 && RoboticArm::TheQueueOfPhotosensor_1){
		std::cout << "Why the fuck is this even showing?  PS1 acting weird" << std::endl;
		BrickOnTheQueueDetected_Trigger=false;
	}else if(RoboticArm::TheQueueOfPhotosensor_1==false){
		BrickOnTheQueueDetected_Trigger=false;
	}
}

bool CheckPhotoSensor4(std::deque<Brick>* _BricksBeforeTheLine, std::deque<Brick>* _BricksOnTheLine, std::deque<int>* Available_DNI_List)
{
	static int lastBrick_Position=RoboticArm::EnterTheTileStartingCodeValue;
	if(RoboticArm::EnterTheTileStartingCodeValue !=lastBrick_Position){ //We have a new brick on the photosensor 4
		std::cout << "brick detected PS4" << std::endl;
		lastBrick_Position=RoboticArm::EnterTheTileStartingCodeValue;		//Update the values for future iterations


		//Complete brick information
		_BricksBeforeTheLine->begin()->Position = RoboticArm::ActualValueOfTheLineEncoder-RoboticArm::EnterTheTileStartingCodeValue;
		_BricksBeforeTheLine->begin()->DNI = Available_DNI_List->front();
		std::cout<<" DNI to assign"<< Available_DNI_List->front() <<std::endl;
		std::cout<<"Assigned DNI "<< _BricksBeforeTheLine->begin()->DNI <<std::endl;
		_BricksBeforeTheLine->begin()->AssignedPallet=0;
		//Add the hole that goes after this brick
		Brick Gap(0,0,0,0);
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


void update_list(std::deque<Brick>* mBrickList, int mEncoderAdvance, std::deque<int>* _Available_DNI_List, const std::vector<int>& _Manipulator_Fixed_Position)
{
	//Update position of every brick
	for(unsigned int i=0; i<mBrickList->size(); i++)
	{
		mBrickList->at(i).Position += mEncoderAdvance;
	}

	//Check if it's out of the line
	if(mBrickList->size()>0 && mBrickList->at(0).Position>=_Manipulator_Fixed_Position.back()+500){
		std::cout << "Brick abandons the line because its position is "<< mBrickList->begin()->Position << std::endl;
		_Available_DNI_List->push_back(mBrickList->begin()->DNI);
		mBrickList->pop_front();
	}
}

void update_order_list(std::vector<std::deque<Order>>* _Manipulator_Order_List, int _EncoderAdvance)
{
	//Update position of every brick
	for(unsigned int i=0; i<_Manipulator_Order_List->size(); i++) //for every manipulator
	{
		for(unsigned int j=0; j<_Manipulator_Order_List->at(i).size(); j++) //for every order in that manipulator
		{
			_Manipulator_Order_List->at(i).at(j).When -= _EncoderAdvance; //decrease when it's going to happen. Will happen when it reaches 0
		}

		//Check if it's outdated
		if(_Manipulator_Order_List->at(i).size()>0 && _Manipulator_Order_List->at(i).begin()->When<=0){
			std::cout << "Order expired, probably executed at manipulator "<< i << std::endl;
			_Manipulator_Order_List->at(i).pop_front();
		}
	}
}

void FillDNIs(std::deque<int>* _DNI_List){
	const int _MAXIMUMNUMBEROFBRICKSONLINE=10;
	for(int i=1;i<=_MAXIMUMNUMBEROFBRICKSONLINE;i++){
		_DNI_List->push_back(i);
	}
}

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

int CheckQuantityContained(int StorageNumber, int Grade, int Colour)
{
	int NumberOfBricks=StorageGetNumberOfBricks(StorageNumber);
	int BrickRaw=(Colour << 4) + Grade;
	int matches=0;

	for(int i=0; i<NumberOfBricks; i++)
		if(StorageGetRaw(StorageNumber, i) == BrickRaw)
			matches++;

	return matches;
}

void Choose_Best_Pallet(Brick* _BrickToClassify,const std::vector<std::deque<Order>>& _Manipulator_Order_List ,const std::vector<int>& Manipulator_Fixed_Position)
{
	//The best pallet has to be one that will be free for the brick when the brick arrives to that manipulator.

	//So first we have to check if the manipulator is going to be free.

	std::vector<int>ListOfAvailableManipulators;
	ListOfAvailableManipulators.reserve(10);

	for(unsigned int i=0;i<_Manipulator_Order_List.size();i++) //Check for every manipulator
	{
		bool ManipulatorWillBeReady = true; //Let's agree that unless noted otherwhise the manipulator will be ready
		for(unsigned int j=0;j<_Manipulator_Order_List.at(i).size();j++) //And for every order
		{
			//----------------------------------------------------------------------
			//CONDITION 1: Manipulator unavailable if it's before the brick.
			if(_BrickToClassify->Position>Manipulator_Fixed_Position.at(i))
			{
				ManipulatorWillBeReady = false;
				break; //pretty clear
			}

			//_Manipulator_Order_List.at(i).at(j).When; //This tells us how many encoder units until the action starts.
			//Once the action has started, we have to add the constant K that is the encoder units that will advance until the manipulator is ready again.

			//CONDITION 2: Manipulator still busy because of previous order

			if(_Manipulator_Order_List.at(i).at(j).When + 3000 > Manipulator_Fixed_Position.at(i) -_BrickToClassify->Position
			&& _Manipulator_Order_List.at(i).at(j).When < Manipulator_Fixed_Position.at(i) -_BrickToClassify->Position)
			{
				ManipulatorWillBeReady = false;
				break; //pretty clear
			}


			//CONDITION 3:The pick down order must fit between other pick down orders without disrupt
			//If there are two orders and we want to place an order in between, we only have to do it
			//if all the orders can success.
			if(_Manipulator_Order_List.at(i).at(j).When > Manipulator_Fixed_Position.at(i) -_BrickToClassify->Position
			&& _Manipulator_Order_List.at(i).at(j).When - 3000 < Manipulator_Fixed_Position.at(i) -_BrickToClassify->Position )
			{
				ManipulatorWillBeReady = false;
				break; //pretty clear
			}

			//Otherwise, no problem at all.
			//------------------------------------------------------------------------
		}
	}
	_BrickToClassify->AssignedPallet = 5;
}

void AddOrder(const Brick& mBrick, std::vector<std::deque<Order>>* _ManipulatorOrderList, const std::vector<int>& _Manipulator_Fixed_Position)
{
	//What stands for what order 1=catch and pick down 0=retrieve from the storage and put on the line
	//Where is at which side, 0=left 1=right

	int mWho = (mBrick.AssignedPallet-1)/2;	//Who is which manipulator. From 1 to NMANIPULATORS

	Order OrderToPlace(0,0,0);
	OrderToPlace.What = mBrick.Type!=0;//If has a Type, must be picked down. If it's a hole, must retrieve.
	OrderToPlace.Where = mBrick.AssignedPallet%2; //0=left 1=right
	OrderToPlace.When = _Manipulator_Fixed_Position.at(mWho)-mBrick.Position; //When is the distance in encoder units that is left to reach the assigned manipulator

	//Here we have the order. But where to place it?
	//Orders must be ordered by its position.

	unsigned int i=0;
	while(i<_ManipulatorOrderList->at(mWho).size() && _ManipulatorOrderList->at(mWho).at(i).When > OrderToPlace.When){
				i++;
	}
	_ManipulatorOrderList->at(mWho).insert(	_ManipulatorOrderList->at(mWho).begin()+i,OrderToPlace);
}

void ProcessOrdersToPLC(const std::vector<std::deque<Order>>& _ManipulatorOrderList)
{
	for(unsigned int i=0;i<_ManipulatorOrderList.size();i++)
	{
		if(_ManipulatorOrderList.at(i).size()>0)
		{
			DesiredRoboticArm(i+1)->StorageBinDirection = _ManipulatorOrderList.at(i).begin()->Where;
			DesiredRoboticArm(i+1)->CatchOrDrop = _ManipulatorOrderList.at(i).begin()->What+1;
			DesiredRoboticArm(i+1)->ValueOfCatchDrop = RoboticArm::ActualValueOfTheLineEncoder + _ManipulatorOrderList.at(i).begin()->When;
		}
		else
		{
			DesiredRoboticArm(i+1)->CatchOrDrop = 0;
		}
	}
}

//-----------------------------------------------------------------------//
//Variables to be used at void * AlgorithmV2(void *Arg) AND ONLY THERE!!!//
std::deque<int> Available_DNI_List;						//Has the DNI list, these ids are used

std::vector<std::deque<Order>> Manipulator_Order_List;  //Has the list of orders, the vector holds a list with orders of each manipulator
std::deque<Brick> Bricks_Before_The_Line;				//List with the bricks between the photosensor 1 and the photosensor 4
std::deque<Brick> Bricks_On_The_Line;					//List with the bricks between the photosensor 4 and the end of the line.

std::vector<int> Manipulator_Fixed_Position;
std::vector<int> Manipulator_Modes;

int CurrentPackagingColor, CurrentPackagingGrade;

int ManipulatorOperationTime;
long PreviousValueOfTheLineEncoder=RoboticArm::ActualValueOfTheLineEncoder;
//-----------------------------------------------------------------------//
//Getters and setters for other functions
std::deque<Brick> GetListOfBricksOnTheLine(void){
	return Bricks_On_The_Line;
}
void Algorithm_SetCurrentPackagingColor(int PackagingColor){
	CurrentPackagingColor=PackagingColor;
}
void Algorithm_SetCurrentPackagingGrade(int PackagingGrade){
	CurrentPackagingGrade=PackagingGrade;
}
void Algorithm_SetManipulatorOperationTime(int mManipulatorOperationTime){
	ManipulatorOperationTime=mManipulatorOperationTime;
}
void Algorithm_SetManipulatorModes(std::vector<int> mModes){
	Manipulator_Modes=mModes;
}

//-----------------------------------------------------------------------//
void * AlgorithmV2(void *Arg)
{


	/*Load configuration from file*******************/
	ConfigParser config("/home/baseconfig/unit14.conf");
	CurrentPackagingColor = config.GetPackagingColor();
	CurrentPackagingGrade = config.GetPackagingGrade();
	Manipulator_Fixed_Position = config.GetArmPositions();
	Manipulator_Modes = config.GetManipulatorModes();
	SetNumberOfManipulators(&Manipulator_Order_List, Manipulator_Fixed_Position.size());
	FillDNIs(&Available_DNI_List);


	Synchro::DecreaseSynchronizationPointValue(0);
	while(1)
	{
		//std::cout<< "inside the algorithm loop"<<std::endl;
		//UPDATE ENCODER VALUES
		int EncoderAdvance = Calculate_Advance(&PreviousValueOfTheLineEncoder);
		update_order_list(&Manipulator_Order_List, EncoderAdvance);
		update_list(&Bricks_On_The_Line, EncoderAdvance, &Available_DNI_List, Manipulator_Fixed_Position);

		//CHECK FOR INPUT

		CheckPhotoSensor1(&Bricks_Before_The_Line); //Check for brick on photosensor 1 to store the grade
		bool NewBrickDetected = CheckPhotoSensor4(&Bricks_Before_The_Line, &Bricks_On_The_Line, &Available_DNI_List);
		if(NewBrickDetected) //New brick detected!
		{
			//if brick should go through
			int RawCurrentPackagingBrick = (CurrentPackagingColor << 4) + CurrentPackagingGrade;
			Brick* BrickToWorkWith = &(Bricks_On_The_Line.at(Bricks_On_The_Line.size()-2));//Remember, back is going to be a hole and (back - 1) the actual brick

			if(RawCurrentPackagingBrick==BrickToWorkWith->Type)
			{
				if(RoboticArm::WhetherOrNotPutTheTileTo_16) //If the path is not blocked, go freely
				{
					BrickToWorkWith->AssignedPallet=-1;//Because why not? -1 could mean go freely
				}
				else
				{
					Choose_Best_Pallet(BrickToWorkWith, Manipulator_Order_List, Manipulator_Fixed_Position); //BrickToWorkWith is a pointer, take care
					AddOrder(*BrickToWorkWith, &Manipulator_Order_List, Manipulator_Fixed_Position);
				}
			}
			else
			{
				Choose_Best_Pallet(BrickToWorkWith, Manipulator_Order_List, Manipulator_Fixed_Position); //BrickToWorkWith is a pointer, take care
			}
		}
		//CHECK FOR OUTPUT


		//Give to the PLC the updates of the Order Line.
		ProcessOrdersToPLC(Manipulator_Order_List);
	}
}
