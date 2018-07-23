/*
 * algorithm_v2.cpp
 *
 *  Created on: Mar 26, 2018
 *      Author: Jose Andres Grau Martinez
 */

#include "algorithm_v2.h"
#include <string>
#include <iostream>
#include <vector>
#include <deque>
#include <boost/algorithm/string/predicate.hpp>
#include <SynchronizationPoints.h>
#include "StorageInterface.h"
#include "RoboticArmInfo.h"
#include "ConfigParser.h"
#include "ScoreCalculation.h"
#include "LineOperations.h"
#include <algorithm>

#define MODE_INPUT    0
#define MODE_INOUT    1
#define MODE_OUTPUT	  2
#define MODE_DISABLED 3
#define MODE_MAMUAL   4 //RBS this is just an idea (?) Not implemented.

#define GAP			  0

#define BRICK_READY_SPOT_ASSIGNED		-1
#define BRICK_READY_NO_SPOT_ASSIGNED	 1
#define BRICK_NOT_READY					 0

pthread_mutex_t read_mutex;
pthread_cond_t read_condition;

void SetNumberOfManipulators(OrderManager* _Manipulator_Order_List,
		std::vector<int>* _Pallet_LowSpeedPulse_Height_List,
		std::vector<int>* _Bricks_Ready_For_Output,
		std::vector<Brick>* _Manipulator_TakenBrick,
		std::deque<int>* _DNI_List,
		short _NumberOfManipulators)
{
	_Manipulator_Order_List->SetNumberOfArms(_NumberOfManipulators);
	for(int i=0;i<_NumberOfManipulators;i++)
	{
		std::deque<Order> _ListOfOrders;
		Brick NoBrick(0,0,0,0);
		_Manipulator_TakenBrick->push_back(NoBrick);
		_Pallet_LowSpeedPulse_Height_List->push_back(5000);
		_Pallet_LowSpeedPulse_Height_List->push_back(5000);
		_Bricks_Ready_For_Output->push_back(BRICK_NOT_READY);
		_Bricks_Ready_For_Output->push_back(BRICK_NOT_READY);
	}

	//Fill DNIs
	const int _MAXIMUMNUMBEROFBRICKSONLINE=10;
	for(int i=1;i<=_MAXIMUMNUMBEROFBRICKSONLINE;i++){
		_DNI_List->push_back(i);
	}
}

void update_list(std::deque<Brick>* mBrickList,
		std::vector<Brick>* _Manipulator_TakenBrick,
		int mEncoderAdvance,
		std::deque<int>* _Available_DNI_List,
		const std::vector<int>& _Manipulator_Fixed_Position,
		std::vector<int>* OUTPUTCOOLDOWN,
		bool FIXEDBRICKS)
{
	//Update position of every brick
	for(unsigned int i=0; i<OUTPUTCOOLDOWN->size(); i++)
	{
		if(OUTPUTCOOLDOWN->at(i)>0) OUTPUTCOOLDOWN->at(i) -= mEncoderAdvance;
		else OUTPUTCOOLDOWN->at(i) = 0;

	}
	if(FIXEDBRICKS == false)
	for(unsigned int i=1; i<mBrickList->size(); i++)
	{
		mBrickList->at(i).Position += mEncoderAdvance;


		//Check if a brick has been taken by a manipulator,
		//	For now, this check will be only based in the position and not yet by
		//	static std::vector<bool> HasDischarged_Previous={0,0,0,0,0,0,0,0,0,0};
		//	static std::vector<bool> PhotosensorOfManipulator_Previous={0,0,0,0,0,0,0,0,0,0};

		//Add that brick to _Manipulator_TakenBrick
		//remove that brick from mBrickList
		//remove also its gap from mBrickList

		if(mBrickList->at(i).Type !=0 && mBrickList->at(i).AssignedPallet!=0) //If it's a brick and therefore not a gap
		{
			int IndexAssignedManipulator =(mBrickList->at(i).AssignedPallet-1) / 2;

			if(mBrickList->at(i).Position > _Manipulator_Fixed_Position.at(IndexAssignedManipulator)-100 &&
			   mBrickList->at(i).Position < _Manipulator_Fixed_Position.at(IndexAssignedManipulator)+100 &&
			   getArm(IndexAssignedManipulator+1)->CatchOrDrop==1 // &&
			   //getArm(IndexAssignedManipulator+1)->PhotosensorOfManipulator == 1
			   ) //If it's in its range
			{
				//Add that brick to _Manipulator_TakenBrick
				//remove that brick from mBrickList
				//remove also its gap from mBrickList
				if(_Manipulator_TakenBrick->at(IndexAssignedManipulator).DNI!=0)
				{
					std::cout << "DNI That was going to be leaked returned successfully" << std::endl;
					int DNItoReturn = _Manipulator_TakenBrick->at(IndexAssignedManipulator).DNI;
					_Available_DNI_List->push_back(DNItoReturn);

				}
				_Manipulator_TakenBrick->at(IndexAssignedManipulator) = mBrickList->at(i);
				mBrickList->erase(mBrickList->begin()+i);
				mBrickList->erase(mBrickList->begin()+i);
			}
		}
	}

	//Check if it's out of the line
	if(mBrickList->size()>1 && mBrickList->at(1).Position>=_Manipulator_Fixed_Position.back()+2000){
		std::cout << "Brick abandons the line because its position is "<< mBrickList->begin()->Position << std::endl;
		int DNItoReturn = mBrickList->begin()->DNI;
		if(DNItoReturn>0){
			_Available_DNI_List->push_back(DNItoReturn);
		}
		mBrickList->pop_front();
	}
}

void update_order_list(OrderManager* _Manipulator_Order_List, int _EncoderAdvance)
{
	//Update position of every brick
	for(unsigned int i=0; i<_Manipulator_Order_List->NumberOfManipulators(); i++) //for every manipulator
	{
		for(unsigned int j=0; j<_Manipulator_Order_List->atManipulator(i)->NumberOfOrders(); j++) //for every order in that manipulator
		{
			_Manipulator_Order_List->atManipulator(i)->getOrder_byIndex(j)->When -= _EncoderAdvance; //decrease when it's going to happen. Will happen when it reaches 0
		}

		//Check if it's outdated TODO: Clear constant 6000 by K-K/10
		// That way there should be at any time a period of K/10 between every order

		if(_Manipulator_Order_List->atManipulator(i)->NumberOfOrders()>0 &&
				(
				((_Manipulator_Order_List->atManipulator(i)->getOrder_byIndex(0)->When<=-2000) &&
				getArm(i+1)->ManipulatorStatePosition==1)
				||
				 _Manipulator_Order_List->atManipulator(i)->getOrder_byIndex(0)->When<=-15000))// the first order TODO: JAGM remember that it has to be changed back to K, for safety reasons.
		{
			//PLC needs to have this cleaned to 0 between orders
				getArm(i+1)->WhatToDoWithTheBrick = 0;
				getArm(i+1)->CatchOrDrop = 0;
				getArm(i+1)->PulseZAxis = 0;
			std::cout << "Order expired, probably executed at manipulator "<< i+1 << std::endl;
			_Manipulator_Order_List->atManipulator(i)->RemoveFirstOrder();
		}
	}
}
void update_PalletHeight(std::vector<int>* _Pallet_LowSpeedPulse_Height_List,
		std::vector<Brick>* _Manipulator_TakenBrick,
		std::deque<Brick>* _ListOfBricksOnLine,
		const std::vector<int>& _Manipulator_Fixed_Position,
		std::deque<int>* _Available_DNI_List,
		std::vector<int>* _Bricks_Ready_For_Output,
		std::vector<int>* _OUTPUTCOOLDOWN)
{

	//TODO: This needs to be rewritten.
	static std::vector<bool> HasDischarged_Previous={0,0,0,0,0};
	static std::vector<bool> PhotosensorOfManipulator_Previous={0,0,0,0,0};


	for(unsigned int ArmIndex=0; ArmIndex<_Manipulator_TakenBrick->size(); ArmIndex++) //Iterate for every manipulator
	{
		// VALUES FOR THIS ITERATION

		bool HasDischarged = getArm(ArmIndex+1)->HasDischarged;
		bool PhotosensorOfManipulator = getArm(ArmIndex+1)->PhotosensorOfManipulator;

		int palletIndex =0;
		if (getArm(ArmIndex+1)->WhatToDoWithTheBrick == RIGHT_SIDE) 	 palletIndex=ArmIndex*2;	 // right
		else if	(getArm(ArmIndex+1)->WhatToDoWithTheBrick == LEFT_SIDE) palletIndex=ArmIndex*2+1;	 //left
		//----------------------------------------------------------------------
		//Brick stored at a pallet
		//RBS the manipulator just finished storing a brick?
		if(getArm(ArmIndex+1)->CatchOrDrop==CATCH &&
		   HasDischarged==DISCHARGED_FALSE &&
		   HasDischarged_Previous.at(ArmIndex)==0 &&
		   _Manipulator_TakenBrick->at(ArmIndex).DNI !=0)
		{
			_Pallet_LowSpeedPulse_Height_List->at(palletIndex)= getArm(ArmIndex+1)->ActualValueEncoder - RoboticArm::Z_AxisDeceletationDistance;

			std::cout<< "Updated height value of pallet " << palletIndex +1 << " to "<< _Pallet_LowSpeedPulse_Height_List->at(palletIndex) <<std::endl;



			std::cout<< "We are going to use the arm "<< ArmIndex+1 << " to put a brick " <<
					_Manipulator_TakenBrick->at(ArmIndex).Type << " at pallet " <<
					_Manipulator_TakenBrick->at(ArmIndex).AssignedPallet << std::endl;

			StorageAddBrick(_Manipulator_TakenBrick->at(ArmIndex).AssignedPallet,
				_Manipulator_TakenBrick->at(ArmIndex).Type >> 4,
				_Manipulator_TakenBrick->at(ArmIndex).Type & 15);
			Brick NoBrick(0,0,0,0);
			_Available_DNI_List->push_back(_Manipulator_TakenBrick->at(ArmIndex).DNI);
			_Manipulator_TakenBrick->at(ArmIndex)=NoBrick;
		}
		//----------------------------------------------------------------------
		//Brick on the line taken by a manipulator
		if(getArm(ArmIndex+1)->CatchOrDrop==CATCH &&
		   PhotosensorOfManipulator == PHOTOSENSOR_FALSE &&
		   PhotosensorOfManipulator_Previous.at(ArmIndex)==PHOTOSENSOR_TRUE)
		{

			std::cout<< "Arm "<< ArmIndex+1 << " has taken something from the line..."<< std::endl;
			//JAGM: The brick is added to _Manipulator_TakenBrick  on the function update_order_list
			//not in this spot. Maybe it's a good idea to add a TODO: here

		}
		//----------------------------------------------------------------------
		//Brick on a pallet taken by a manipulator
		if(getArm(ArmIndex+1)->CatchOrDrop==DROP &&
		   PhotosensorOfManipulator == PHOTOSENSOR_FALSE &&
		   PhotosensorOfManipulator_Previous.at(ArmIndex)==PHOTOSENSOR_TRUE)
		{
			std::cout<< "Arm "<< ArmIndex+1 << " has taken something from a pallet..."<< std::endl;

			std::cout<< "Brick taken at the pallet " << palletIndex+1 << std::endl;
						std::cout<< "The number of bricks before the pick up was " << StorageGetNumberOfBricks(palletIndex+1)<< std::endl; // says 0
			if(StorageGetNumberOfBricks(palletIndex+1)>0)
			{
				_Manipulator_TakenBrick->at(ArmIndex).Type = StorageGetRaw(palletIndex+1,StorageGetNumberOfBricks(palletIndex+1));
								_Manipulator_TakenBrick->at(ArmIndex).DNI = _Available_DNI_List->at(0);

				std::cout << "The brick taken was of the type " << _Manipulator_TakenBrick->at(ArmIndex).Type << std::endl;
				std::cout << "and was assigned the DNI " << _Manipulator_TakenBrick->at(ArmIndex).DNI  << std::endl;
								_Available_DNI_List->pop_front();
								StorageDeleteBrick(palletIndex+1,StorageGetNumberOfBricks(palletIndex+1));

			}
			_Pallet_LowSpeedPulse_Height_List->at(palletIndex)= getArm(ArmIndex+1)->ActualValueEncoder - RoboticArm::Z_AxisDeceletationDistance;
				std::cout<< "Updated height value to "<< _Pallet_LowSpeedPulse_Height_List->at(palletIndex) <<std::endl;
		}
		//----------------------------------------------------------------------
		//Brick discharged on the line
		if(getArm(ArmIndex+1)->CatchOrDrop==DROP &&
		   HasDischarged==DISCHARGED_FALSE &&
		   HasDischarged_Previous.at(ArmIndex)==0)
		{

			//We know that the manipulator has discharged something. But, where?
			for(unsigned int j=0; j<_ListOfBricksOnLine->size();j++)
			{
				//We guess that the discharge operation happened in the spot assigned to the tile
				if(_ListOfBricksOnLine->at(j).AssignedPallet == palletIndex+1)
				{
					_ListOfBricksOnLine->at(j).DNI =  _Manipulator_TakenBrick->at(ArmIndex).DNI;
					_ListOfBricksOnLine->at(j).Type =  _Manipulator_TakenBrick->at(ArmIndex).Type;
					_ListOfBricksOnLine->at(j).AssignedPallet =  _Manipulator_TakenBrick->at(ArmIndex).AssignedPallet;

					//_ListOfBricksOnLine->at(j).AssignedPallet=7; //DEBUG ONLY, the reorder function must be rewritten
					Algorithm::Set::order(_ListOfBricksOnLine->at(j));

					_Manipulator_TakenBrick->at(ArmIndex)= Brick(0,0,0,0);
					_OUTPUTCOOLDOWN->at(ArmIndex)=4000;
					std::cout<< "Arm "<< ArmIndex+1 << " has discharged something on the line..."<< std::endl;
					_Bricks_Ready_For_Output->at(palletIndex)=0;

				}
			}
		}

		HasDischarged_Previous.at(ArmIndex) = HasDischarged;
		PhotosensorOfManipulator_Previous.at(ArmIndex) = PhotosensorOfManipulator;
	}



/*
	for(unsigned int i=0; i< _Pallet_LowSpeedPulse_Height_List->size(); i++)
	{
		int ArmIndex=(i/2)+1; //001 , 101
		//Brick taken by a manipulator
		if(getArm(ArmIndex)->CatchOrDrop==DROP && getArm(ArmIndex)->PhotosensorOfManipulator == PHOTOSENSOR_FALSE &&
				PhotosensorOfManipulator_Previous.at(i)==PHOTOSENSOR_TRUE)
		{
			//Add Brick to the manipulator Taken Brick.
			// i will iterate every pallet to
			int pallet = 0;
			if (getArm(ArmIndex)->WhatToDoWithTheBrick == RIGHT_SIDE) 	 pallet=ArmIndex*2-1;	 // right
			else if	(getArm(ArmIndex)->WhatToDoWithTheBrick == LEFT_SIDE) pallet=ArmIndex*2;	 //left
			std::cout<< "Brick taken at the pallet " << pallet << std::endl;
			std::cout<< "The number of bricks before the pick up was " << StorageGetNumberOfBricks(pallet)<< std::endl; // says 0
			if(StorageGetNumberOfBricks(pallet)>0)
			{
				_Manipulator_TakenBrick->at(ArmIndex-1).Type = StorageGetRaw(pallet,StorageGetNumberOfBricks(pallet));
				_Manipulator_TakenBrick->at(ArmIndex-1).DNI = _Available_DNI_List->at(0);
				std::cout << "The pallet had " << StorageGetNumberOfBricks(pallet) << "bricks before the pick up" <<std::endl;
				std::cout << "The brick taken was of the type " << _Manipulator_TakenBrick->at(ArmIndex-1).Type << std::endl;
				std::cout << "and was assigned the DNI " << _Manipulator_TakenBrick->at(ArmIndex-1).DNI  << std::endl;
				_Available_DNI_List->pop_front();
				StorageDeleteBrick(pallet,StorageGetNumberOfBricks(pallet));
			}
			//PhotosensorOfManipulator_Previous.at(i)=1;
			_Pallet_LowSpeedPulse_Height_List->at(i)= getArm(ArmIndex)->ActualValueEncoder - RoboticArm::Z_AxisDeceletationDistance;
			std::cout<< "Updated height value to "<< _Pallet_LowSpeedPulse_Height_List->at(i) <<std::endl;
			PhotosensorOfManipulator_Previous.at(i) = getArm(ArmIndex)->PhotosensorOfManipulator;
			_Bricks_Ready_For_Output->at(i) = BRICK_NOT_READY;
		}
		//Brick discharged on the line
		else if(getArm(ArmIndex)->CatchOrDrop == DROP && getArm(ArmIndex)->HasDischarged == DISCHARGED_FALSE &&
				HasDischarged_Previous.at(i) == DISCHARGED_TRUE)
		{
			std::cout << "Discharge operation detected at manipulator " << ArmIndex <<std::endl;
			for(unsigned int j=0; j<_ListOfBricksOnLine->size();j++)
			{
				std::cout<< "_ListOfBricksOnLine->at(j).AssignedPallet " << _ListOfBricksOnLine->at(j).AssignedPallet
						 << "(i+1)" << (i+1) << std::endl;
				int pallet=0;
				if (getArm(ArmIndex)->WhatToDoWithTheBrick == RIGHT_SIDE) 	 pallet=ArmIndex*2-1;	 // right
				else if	(getArm(ArmIndex)->WhatToDoWithTheBrick == LEFT_SIDE) pallet=ArmIndex*2;	 //left
				if(_ListOfBricksOnLine->at(j).AssignedPallet==pallet)// &&
						//_ListOfBricksOnLine->at(j).Type==0					//&&
			       //_ListOfBricksOnLine->at(j).Position > _Manipulator_Fixed_Position.at(ArmIndex-1)-100 &&
				   //_ListOfBricksOnLine->at(j).Position < _Manipulator_Fixed_Position.at(ArmIndex-1)+100
				{//Discharged the brick in place
					_OUTPUTCOOLDOWN->at(ArmIndex-1)=2000;
					_ListOfBricksOnLine->at(j).Type=_Manipulator_TakenBrick->at(ArmIndex-1).Type;
					//_ListOfBricksOnLine->at(j).AssignedPallet=9; //DEBUG ONLY, the reorder function must be rewritten
					_ListOfBricksOnLine->at(j).DNI=_Manipulator_TakenBrick->at(ArmIndex-1).DNI;
					Brick Empty(0,0,0,0);
					_Manipulator_TakenBrick->at(ArmIndex-1)=Empty;
					std::cout<< "Brick discharged: DNI Assigned " << _ListOfBricksOnLine->at(j).DNI << ". Arm " << ArmIndex << std::endl;
					std::cout<< "Brick discharged in the correct place"  << _Manipulator_Fixed_Position.at(ArmIndex-1)  << "  " << _ListOfBricksOnLine->at(j).Position << ". Arm " << ArmIndex << std::endl;
					//Algorithm::Set::order(_ListOfBricksOnLine->at(j));
					//set_order(_ListOfBricksOnLine->at(j));
				}
			}
		}
		HasDischarged_Previous.at((ArmIndex*2)-2) = getArm(ArmIndex)->HasDischarged;
		HasDischarged_Previous.at((ArmIndex*2)-1) = getArm(ArmIndex)->HasDischarged;
		PhotosensorOfManipulator_Previous.at((ArmIndex*2)-2) = getArm(ArmIndex)->PhotosensorOfManipulator;
		PhotosensorOfManipulator_Previous.at((ArmIndex*2)-1) = getArm(ArmIndex)->PhotosensorOfManipulator;
	}
	*/
}

void FillDNIs(std::deque<int>* _DNI_List){
	const int _MAXIMUMNUMBEROFBRICKSONLINE=10;
	for(int i=1;i<=_MAXIMUMNUMBEROFBRICKSONLINE;i++){
		_DNI_List->push_back(i);
	}
}

void Choose_Best_Pallet(Brick* _BrickToClassify,
						OrderManager _Manipulator_Order_List,
						const std::vector<int>& _Manipulator_Fixed_Position,
						std::vector<int> _Manipulator_Modes,
						int _RawCurrentPackagingBrick,
						int _forced_pallet)
{
	//The best pallet has to be one that will be free for the brick when the brick arrives to that manipulator.

	//So first we have to check if the manipulator is going to be free.

	std::vector<int>ListOfPoints={10000,10000,10000,10000,10000,10000,10000,10000,10000,10000};

	for(unsigned int i=0; i<_Manipulator_Order_List.NumberOfManipulators(); i++) //Check for every manipulator
	{
		//bool ManipulatorWillBeReady = true; //Let's agree that unless noted otherwise the manipulator will be ready
		//CONDITION 1: Manipulator is enabled as input (0=in, 1=in/out, 2=out, 3=disabled)
		bool ManipulatorWillBeReady=true;
		if(_Manipulator_Modes.at(i) == MODE_OUTPUT || _Manipulator_Modes.at(i) == MODE_DISABLED)
		{
			ManipulatorWillBeReady = false;
		}
		else{
			for(unsigned int j=0; j<_Manipulator_Order_List.atManipulator(i)->NumberOfOrders(); j++) //And for every order
			{
				//----------------------------------------------------------------------
				//CONDITION 2: Manipulator unavailable if it's before the brick.
				if(_BrickToClassify->Position>_Manipulator_Fixed_Position.at(i))
				{
					ManipulatorWillBeReady = false;
					break; //pretty clear
				}

				//Once the action has started, we have to add the constant K that is the encoder units that will advance until the manipulator is ready again.

				//CONDITION 3: Manipulator will be still busy because of previous order
				//B2: new order,   B1: order already placed,  M: manipulator
				//
				//-----------B2------------B1------M----------------------------------------
				//							 <---->
				//						Old order distance
				//				<----------------->
				//				New order distance
				//
				//               <----K---> There must be K between an order that is already placed and the new order.
				//
				//
				//
				//
				//-----------------B2------------B1M----------------------------------------
				//                     <----K---> As we had this security distance, there is no problem.
				//
				//if(_Manipulator_Order_List.atManipulator(i)->getOrder_byIndex(j)->When + K > _Manipulator_Fixed_Position.at(i) -_BrickToClassify->Position
				//&& _Manipulator_Order_List.atManipulator(i)->getOrder_byIndex(j)->When < _Manipulator_Fixed_Position.at(i) -_BrickToClassify->Position)

				std::cout<< i <<"  " << j << "  "
						<< _Manipulator_Fixed_Position.at(i) - _Manipulator_Order_List.atManipulator(i)->getOrder_byIndex(j)->When << "   "
						<< _BrickToClassify->Position << std::endl;
				std::cout<< i <<"  " << j << "  "
						<< _Manipulator_Fixed_Position.at(i) - _Manipulator_Order_List.atManipulator(i)->getOrder_byIndex(j)->When -_BrickToClassify->Position << "   "
						<< K << std::endl;

				if( _Manipulator_Fixed_Position.at(i) - _Manipulator_Order_List.atManipulator(i)->getOrder_byIndex(j)->When > _BrickToClassify->Position	&&
						_Manipulator_Fixed_Position.at(i) - _Manipulator_Order_List.atManipulator(i)->getOrder_byIndex(j)->When -_BrickToClassify->Position < K)
				{
					ManipulatorWillBeReady = false;
					//TODO:EXCEPTION TO 3: it does not matter if it will be busy if it's an output operation, as we will cancel that output operation

					break; //pretty clear
				}


				//CONDITION 4:The pick down order must fit between other pick down orders without disrupt
				//If there are two orders and we want to place an order in between, we only have to do it
				//if all the orders can succeed.

				//B2: new order,   B1: order already placed,  M: manipulator
				//
				//-----------B1------------B2------M----------------------------------------
				//               <----K---> There must be K between the order that we want to place and the new order.
				//
				//
				//-----------------B1------------B2M----------------------------------------
				//                     <----K---> As we had this security distance, there is no problem.
				//

				if(_Manipulator_Order_List.atManipulator(i)->getOrder_byIndex(j)->When > _Manipulator_Fixed_Position.at(i) -_BrickToClassify->Position
				&& _Manipulator_Order_List.atManipulator(i)->getOrder_byIndex(j)->When - K < _Manipulator_Fixed_Position.at(i) -_BrickToClassify->Position )
				{
					ManipulatorWillBeReady = false;

					//TODO:EXCEPTION TO 4: it does not matter if it will be busy if it's an output operation, as we will cancel that output operation
					break; //pretty clear
				}

				//Otherwise, no problem at all.
				//------------------------------------------------------------------------
			}
			//If after checking all the orders of a manipulator
			//the manipulator is free, check the points
			if(ManipulatorWillBeReady)
			for(int j=0;j<2;j++) //check both sides
			{
				if(true) //If the pallet exists and it's not full
				{

					ListOfPoints.at(i*2+j) = CheckInputScore(i*2+j+1, _BrickToClassify->Type, _RawCurrentPackagingBrick);
					std::cout << "-Pallet checked:  " << i*2+j+1 << "   Points assigned " << ListOfPoints.at(i*2+j) << std::endl;
				}
			}
		}
	}
	//TODO:Check to clear order in case of an output.

	//Check minimum score TO CHANGE FOR STD::Minimum_at(vector)
	int PalletWithMinimumScore=0;
	for(int i=0;i<10;i++)
	{
		if(ListOfPoints.at(i)<ListOfPoints.at(PalletWithMinimumScore)) PalletWithMinimumScore=i;
	}
	std::cout << "The points assigned are " << ListOfPoints.at(PalletWithMinimumScore) << " goes to pallet:"<< PalletWithMinimumScore+1 << std::endl;
	PalletWithMinimumScore++;//To translate from index to actual pallet

#define __PRODUCTIONMODE 1


#if __PRODUCTIONMODE

	if(_forced_pallet !=0)	_BrickToClassify->AssignedPallet = _forced_pallet;
	else					_BrickToClassify->AssignedPallet = PalletWithMinimumScore;
#else



	static int last=0;
	static int last2=0;
	static int last3=0;
	static int last4=0;
	srand(time(NULL));
	int Random;

	while(Random==last || Random==last2 || Random==last3 || Random==last4 || Random>5 || Random<0)
	{
		Random=(rand()%5); //01234
	}

	last4=last3;
	last3=last2;
	last2=last;
	last=Random;
	Random*=2;//02468
	Random+=rand()%2+1;//12345678910

	std::cout << "Random mode, goes to "<< Random << std::endl;
	if(_forced_pallet !=0)	_BrickToClassify->AssignedPallet = _forced_pallet;
	else					_BrickToClassify->AssignedPallet = Random;


#endif


}



/*
void AddOrder(const Brick& mBrick, OrderManager* _ManipulatorOrderList, const std::vector<int>& _Manipulator_Fixed_Position)
{
	//What stands for what order 1=catch and pick down 0=retrieve from the storage and put on the line
	//Where is at which side, 0=left 1=right

	int mWho = (mBrick.AssignedPallet-1)/2;	//Who is which manipulator. From 0 to NMANIPULATORS-1

	Order OrderToPlace(0,0,0);
	OrderToPlace.What = mBrick.Type==0;//If has a Type, must be picked down, what = 0. If it's a hole, must retrieve. what = 1
	OrderToPlace.Where = mBrick.AssignedPallet%2 == 1; //0=left 1=right
	OrderToPlace.When = _Manipulator_Fixed_Position.at(mWho)-mBrick.Position; //When is the distance in encoder units that is left to reach the assigned manipulator

	//Here we have the order. But where to place it?
	//Orders must be ordered by its position.

	unsigned int i=0;
	while(i<_ManipulatorOrderList->atManipulator(mWho).size() && _ManipulatorOrderList->at(mWho).at(i).When < OrderToPlace.When){
				i++;
	}
	_ManipulatorOrderList->at(mWho).insert(_ManipulatorOrderList->at(mWho).begin()+i,OrderToPlace);
}
*/
void ProcessOrdersToPLC(OrderManager* _ManipulatorOrderList, const std::vector<int>& _Pallet_LowSpeedPulse_Height_List)
{
	for(unsigned int i=0;i<_ManipulatorOrderList->NumberOfManipulators();i++)
	{
		if(_ManipulatorOrderList->atManipulator(i)->NumberOfOrders()>0 &&
				(
				(_ManipulatorOrderList->atManipulator(i)->getOrder_byIndex(0)->What==false  && _ManipulatorOrderList->atManipulator(i)->getOrder_byIndex(0)->When<2000)
				||
				(_ManipulatorOrderList->atManipulator(i)->getOrder_byIndex(0)->What==true && _ManipulatorOrderList->atManipulator(i)->getOrder_byIndex(0)->When<14000)
				||
				(_ManipulatorOrderList->atManipulator(i)->getOrder_byIndex(0)->What==true && _ManipulatorOrderList->atManipulator(i)->getOrder_byIndex(0)->When>45000)
				)

		)
		{
			//WHAT TO DO WITH THE BRICK

			if(_ManipulatorOrderList->atManipulator(i)->getOrder_byIndex(0)->Where){
				getArm(i+1)->WhatToDoWithTheBrick = 1; //right side
			}
			else
			{
				getArm(i+1)->WhatToDoWithTheBrick = 2;	//left side
			}

			//CATH OR DROP

			if(_ManipulatorOrderList->atManipulator(i)->getOrder_byIndex(0)->What==false) getArm(i+1)->CatchOrDrop=1;
			else getArm(i+1)->CatchOrDrop=2;

			//VALUE OF CATH OR DROP
			if(RoboticArm::ActualValueOfTheLineEncoder + _ManipulatorOrderList->atManipulator(i)->getOrder_byIndex(0)->When <100000)
			{
				getArm(i+1)->ValueOfCatchDrop = RoboticArm::ActualValueOfTheLineEncoder + _ManipulatorOrderList->atManipulator(i)->getOrder_byIndex(0)->When;
			}
			else
			{
				getArm(i+1)->ValueOfCatchDrop = RoboticArm::ActualValueOfTheLineEncoder + _ManipulatorOrderList->atManipulator(i)->getOrder_byIndex(0)->When - 100000;
			}
			//THE PULSE OF Z AXIS
			//getArm(i+1)->PulseZAxis=_Pallet_LowSpeedPulse_Height_List.at(i*2+(getArm(i+1)->WhatToDoWithTheBrick-1));
			getArm(i+1)->PulseZAxis=_Pallet_LowSpeedPulse_Height_List.at(i*2+(getArm(i+1)->WhatToDoWithTheBrick-1));//0
			//std::cout<< i*2+(getArm(i+1)->WhatToDoWithTheBrick-1)<< std::endl;

			///////////////////////////////////////////////////////
			//1   3						  i*2+(getArm(i+1)->WhatToDoWithTheBrick-1
			//
			//0   1     2     3     4     i
			//
			//0   2
		}
		else
		{
				getArm(i+1)->WhatToDoWithTheBrick = 0;
				getArm(i+1)->CatchOrDrop = 0;
				getArm(i+1)->ValueOfCatchDrop = 0;
		}
	}
}

//-----------------------------------------------------------------------//
//Variables to be used at void * AlgorithmV2(void *Arg) AND ONLY THERE!!!//
namespace Algorithm {
	std::deque<int> Available_DNI_List;						//Has the DNI list, these ids are used

	//std::vector<std::deque<Order>> Manipulator_Order_List;  //Has the list of orders, the vector holds a list with orders of each manipulator
	OrderManager Manipulator_Order_List;
	std::deque<Brick> Bricks_Before_The_Line;				//List with the bricks between the photosensor 1 and the photosensor 4
	std::deque<Brick> Bricks_On_The_Line;					//List with the bricks between the photosensor 4 and the end of the line.

	std::vector<int> Bricks_Ready_For_Output;
	std::vector<int> Manipulator_Fixed_Position;
	std::vector<int> Manipulator_Modes;
	std::vector<int> Pallet_LowSpeedPulse_Height_List;
	std::vector<int> OUTPUTCOOLDOWN = {0,0,0,0,0};
	std::vector<Brick> Manipulator_TakenBrick;

	int CurrentPackagingColor, CurrentPackagingGrade;

	int ManipulatorOperationTime;
	long PreviousValueOfTheLineEncoder=RoboticArm::ActualValueOfTheLineEncoder;

	bool Enable_WhetherOrNotPutTheTileTo_16 = true;
	bool force_input = false;
	bool force_output = false;
	int	forced_pallet=0;
	bool FIXEDBRICKS = false;
}

//-----------------------------------------------------------------------//
//Getters and setters for other functions


	void Algorithm::Set::enable_WhetherOrNotPutTheTileTo_16(bool set_to){	Algorithm::Enable_WhetherOrNotPutTheTileTo_16 = set_to;}
	void Algorithm::Set::force_input(bool set_to){ Algorithm::force_input = set_to; }
	void Algorithm::Set::force_output(bool set_to){ Algorithm::force_output = set_to; }
	void Algorithm::Set::forced_pallet(int set_to){ Algorithm::forced_pallet = set_to; }
	void Algorithm::Set::FIXEDBRICKS(bool set_to){ Algorithm::FIXEDBRICKS = set_to; }
	void Algorithm::Set::order(Brick brick){
		Manipulator_Order_List.AddOrder(brick, Algorithm::Manipulator_Fixed_Position);
	}
	void Algorithm::Set::CurrentPackagingColor(int PackagingColor)
	{
		Algorithm::CurrentPackagingColor=PackagingColor;
		std::cout << "Setting current packaging color to " << Algorithm::CurrentPackagingColor << std::endl;
	}
	void Algorithm::Set::CurrentPackagingGrade(int PackagingGrade)
	{
		Algorithm::CurrentPackagingGrade=PackagingGrade;
		std::cout << "Setting current packaging type to " << Algorithm::CurrentPackagingGrade << std::endl;
	}
	void Algorithm::Set::ManipulatorOperationTime(int mManipulatorOperationTime)
	{
		Algorithm::ManipulatorOperationTime=mManipulatorOperationTime;
	}
	void Algorithm::Set::ManipulatorModes(std::vector<int> mModes)
	{
		Algorithm::Manipulator_Modes=mModes;
	}
	void Algorithm::Set::ManipulatorFixedPosition(std::vector<int> mManipulator_Fixed_Position){ Algorithm::Manipulator_Fixed_Position = mManipulator_Fixed_Position;}



	std::deque<int> Algorithm::Get::Available_DNI_List(){return Algorithm::Available_DNI_List;}
	OrderManager Algorithm::Get::Manipulator_Order_List(){return Algorithm::Manipulator_Order_List;}
	std::deque<Brick> Algorithm::Get::Bricks_Before_The_Line(){return Algorithm::Bricks_Before_The_Line;}
	std::deque<Brick> Algorithm::Get::Bricks_On_The_Line(){return Algorithm::Bricks_On_The_Line;}

	std::vector<int> Algorithm::Get::Bricks_Ready_For_Output(){return Algorithm::Bricks_Ready_For_Output;}
	std::vector<int> Algorithm::Get::Manipulator_Fixed_Position(){return Algorithm::Manipulator_Fixed_Position;}
	std::vector<int> Algorithm::Get::Manipulator_Modes(){return Algorithm::Manipulator_Modes;}
	std::vector<int> Algorithm::Get::Pallet_LowSpeedPulse_Height_List(){return Algorithm::Pallet_LowSpeedPulse_Height_List;}
	std::vector<Brick> Algorithm::Get::Manipulator_TakenBrick(){return Algorithm::Manipulator_TakenBrick;}
	std::vector<int> Algorithm::Get::IndexesOfBricksOnLine(std::deque<Brick> Bricks_On_The_Line)
	{
		std::vector<int> IndexesOfBricks;
		for(unsigned int i=0; i<Bricks_On_The_Line.size(); i++)
		{
			//If item i on the line is not a gap
			if(Bricks_On_The_Line.at(i).Type != 0)
			{
				IndexesOfBricks.push_back(i);
			}
		}

		return IndexesOfBricks;
	}




void CheckForBricksAtTheTop(std::vector<int>* _Bricks_Ready_For_Output, int _RawCurrentPackagingBrick)
{

	for(int i=9; i>=0; i--)//for every pallet
	{
		//_Bricks_Ready_For_Output:
		//	it's 0	when it does not match the top (BRICK_NOT_READY)
		//	it's -1 when it has been assigned a gap (BRICK_READY_SPOT_ASSIGNED)
		//	it's 1	when it matches the top but it has not been assigned a gap (BRICK_READY_NO_SPOT_ASSIGNED)
		/*
		if(i==4){
				std::cout<< "The top at the pallet " << i+1
						 << " is of type " << StorageGetRaw(i+1, StorageGetNumberOfBricks(i+1))
						 << " And the pallet has "
						 << StorageGetNumberOfBricks(i+1)
						 <<" Bricks"
						 << std::endl;


				std::cout<< _RawCurrentPackagingBrick << std::endl;
				std::cout<< std::endl;
		}
		*/

		//std::cout << _RawCurrentPackagingBrick << std::endl;

		bool MatchingTop = false;
		//std::cout << "Pallet NOB: " << i+1 << "  " << StorageGetNumberOfBricks(i+1) << std::endl;
		if(StorageGetNumberOfBricks(i+1)>0){
			//std::cout << "Pallet: " << i+1 << "  " << StorageGetRaw(i+1, StorageGetNumberOfBricks(i+1)) << std::endl;
			MatchingTop = (StorageGetRaw(i+1, StorageGetNumberOfBricks(i+1)) == _RawCurrentPackagingBrick);
		}

		if(MatchingTop == false)
		{
			_Bricks_Ready_For_Output->at(i) = BRICK_NOT_READY;
		}
		else if(MatchingTop && _Bricks_Ready_For_Output->at(i) != BRICK_READY_SPOT_ASSIGNED) //If the top matches and it has not been assigned yet
		{
			//Assign the brick to a gap.
			_Bricks_Ready_For_Output->at(i) = BRICK_READY_NO_SPOT_ASSIGNED;
			//std::cout<< "Brick at pallet " << i+1
			//		 << "matches the packing type and has not been assigned a manipulator for take out yet" <<std::endl;
		}
	}
}

void FindASpotForOutputBricks(std::deque<Brick>* Bricks_On_The_Line,
								std::vector<int>* _Bricks_Ready_For_Output,
								const std::vector<int>& _Manipulator_Fixed_Position,
								OrderManager* _ManipulatorOrderList,
								const std::vector<int>& _Manipulator_Modes,
								const std::vector<Brick>& _Manipulator_TakenBrick)
{
	//for(unsigned int i=0; i<_Bricks_Ready_For_Output->size(); i++) //For all the pallets
	int i=_Bricks_Ready_For_Output->size();
	while(i-->0)
	{
		int destinationManipulator = (i/2);
		int Spot=0;
		//If a gap is required for this pallet and its manipulator is enabled as output (2) or in/out (1)
		if	(
				_Bricks_Ready_For_Output->at(i) == BRICK_READY_NO_SPOT_ASSIGNED &&
				_Manipulator_Modes.at(destinationManipulator) == MODE_INOUT
			)
		{
			//std::cout<< "Confirmation that: Brick at pallet " << i+1
			//					 << "matches the packing type and has not been assigned a manipulator for take out yet" <<std::endl;
			//start checking from front to back
			for(unsigned int j=0; j<Bricks_On_The_Line->size(); j++)
			{
				Brick mBrick = Bricks_On_The_Line->at(j);
				Brick nextBrick(0,0,0,0);
				if(j<Bricks_On_The_Line->size()-1)
					 nextBrick = Bricks_On_The_Line->at(j);
				bool usableGap=true;
				//Find Gap
				//Check if big enough
				//Check when it's usable

				//CONDITION 1: It must be an empty gap for this manipulator
				if(mBrick.Type !=0) //We could add more things here. But it's better to keep it simple.
				{
					usableGap=false;
					std::cout << "Brick index " << j << " It's problem of condition 1" << std::endl;
				}

				//CONDITION 2: It has to be big enough, the size of the Gap > E
				else if(mBrick.Position - nextBrick.Position < E+E/2)
				{
					usableGap=false;
					std::cout << "Brick index " << j << " It's problem of condition 2" << std::endl;
				}

				//CONDITION 2.5: Gap out of range.
				if(nextBrick.Position+E > _Manipulator_Fixed_Position.at(i))
				{
					usableGap=false;
					std::cout << "Brick index " << j << " It's problem of condition 2.5" << std::endl;
				}
				//how do we know if it's usable?
				//By checking the orders. We need an spot of K between orders
				if(_ManipulatorOrderList->atManipulator(destinationManipulator)->NumberOfOrders()==0) Spot = Bricks_On_The_Line->at(j).Position;
				for(unsigned int k = 0; k < _ManipulatorOrderList->atManipulator(destinationManipulator)->NumberOfOrders();k++) //For all the orders of this manipulator
				{


					//a----------------b		Points where the manipulator is usable ManipulatorUsableBetweenB. relative to the beginning of the line
					//       c--------------d	Points where the gap is usable relative to the beginning of the line
					//ORDER B SHOULD BE ALWAYS SMALLER THAN ORDER A  //by smaller it means that order B should be always further than order A

					Order OrderB = *_ManipulatorOrderList->atManipulator(destinationManipulator)->getOrder_byIndex(k);
					Order OrderA(0,0,0);
					//if(k==_ManipulatorOrderList->atManipulator(destinationManipulator)->NumberOfOrders()-1) OrderA.When=100000; //In case that A does not exist, we set it far far away
					if(k==_ManipulatorOrderList->atManipulator(destinationManipulator)->NumberOfOrders()-1) OrderA.When=_Manipulator_Fixed_Position.at(destinationManipulator); //In case that A does not exist, we set it far far away
					else OrderA= *_ManipulatorOrderList->atManipulator(destinationManipulator)->getOrder_byIndex(k+1);

					//CONDITION 3: There must be enough space between two orders to place another one
					if(OrderA.When - OrderB.When > 2*K) // In theory K has already a safety margin on it, but we could add an extra +K/10
					{
						//Cool, we have two orders that have enough space in the middle to make something.
						//
						//The order is relative to the manipulator, must convert to relative to the beginning of the line
						//An order B will leave the manipulator usable again at UsableBetweenB:
						//
						//O            UBB		 B	  			 M
						//O			   |  <--K-->|<-OrderB.When->|
						//O<----???--->|	 	     				 |
						//O<------------PositionM--------------->|
						//UBB = PositionM - OrderB.When - K

						//The order is relative to the manipulator, must convert to relative to the beginning of the line
						//An order A will need that the manipulator is ready to be used again at that time
						//therefore, we can't place an order after UsableBetweenA:
						//
						//O            A	  UBA	  		   M
						//O			   |<--K-->|			   |
						//O<----???--->|<-----OrderA.When----->|
						//O<------------PositionM------------->|
						//UBA = PositionM - OrderA.When + K

						//OLD AND PROBABLY WRONG
						//int ManipulatorUsableBetweenA = _Manipulator_Fixed_Position.at(destinationManipulator)-K-(OrderA.When+K);
						//int ManipulatorUsableBetweenB = _Manipulator_Fixed_Position.at(destinationManipulator)-K-(OrderB.When+K);

						//NEW, PROBABLY GOOD
						//UBB should be bigger than UBA
						int ManipulatorUsableBetweenA = _Manipulator_Fixed_Position.at(destinationManipulator)-OrderA.When + K;
						int ManipulatorUsableBetweenB = _Manipulator_Fixed_Position.at(destinationManipulator)-OrderB.When - K;

						int GapUsableBetweenC = nextBrick.Position+E; //E has already a safety margin, but it could use a small increase
						//mBrick is a gap, so we take the minimum (as we want to operate as soon as possible)
						//between the position where the hand can operate and the position of the gap
						int GapUsableBetweenD = std::min(_Manipulator_Fixed_Position.at(destinationManipulator)-K,mBrick.Position);

						std::cout << ManipulatorUsableBetweenA << "   " << ManipulatorUsableBetweenB << "   "
								<< GapUsableBetweenC << "   " << GapUsableBetweenD << "   "<< std::endl;

						if(ManipulatorUsableBetweenB<GapUsableBetweenD && ManipulatorUsableBetweenB>GapUsableBetweenC)
						{
							Spot=ManipulatorUsableBetweenB;
							break;
						}
						else if (GapUsableBetweenD<ManipulatorUsableBetweenB && GapUsableBetweenD>ManipulatorUsableBetweenA)
						{
							Spot=GapUsableBetweenD;
							break;
						}else Spot=0;
					}
				}				
				std::cout<< "We've got quite far already "<< usableGap << "  in the spot "<< Spot <<std::endl;
					//If you have made it so far it's because there is no problem to place an order at that gap
				if(usableGap==true && (Spot-0)>=E) //If the place where the spot is located is bigger than a brick
				{
					//Before:
					//   j+1        j        j-1                  0
					//---------+---------+---------+---------+---------+
					//  Brick  |   SPOT  |  Brick  |   ...   |  FRONT  |
					//---------+---------+---------+---------+---------+

					//	j+2	 	  	j+1          j         j-1                  0
					//-------+---------------+---------+---------+---------+---------+
					//Brick  |  AssignedSpot |   SPOT  |  Brick  |   ...   |  FRONT  |
					//-------+---------------+---------+---------+---------+---------+

					//After:
					//   j+3       j+2         j+1         j      j-1                  0
					//---------+---------+--------------+-----+---------+---------+---------+
					//  Brick  | newSPOT | AssignedSpot |SPOT |  Brick  |   ...   |  FRONT  |
					//---------+---------+--------------+-----+---------+---------+---------+

					_Bricks_Ready_For_Output->at(i) = BRICK_READY_SPOT_ASSIGNED;
					std::cout << "Spot found!!, for the 'brick' with index " << j << " Assigned to Manipulator " << destinationManipulator  <<  std::endl;

					//Insert two items just after that place, so the middle spot can be assigned for the operation
					//while maintaining the brick-spot principle (Note: second spot must be as thin as possible in order to save space)

					int assignedSpotPosition = Spot;//It has already a security margin seted up lines ago
					int newSpotPosition = assignedSpotPosition - E; //situated behind Assigned spot

					//Add assigned spot behind SPOT
					Brick AssignedSpot(0, assignedSpotPosition, i+1, 0);
					Bricks_On_The_Line->insert(Bricks_On_The_Line->begin()+j+1, AssignedSpot);
					//AsignedSpot becomes j+1, SPOT2 is j

					//Add new spot behind AssignedSpot
					Brick newSpot(0, newSpotPosition, 0, 0);
					Bricks_On_The_Line->insert(Bricks_On_The_Line->begin()+j+2, newSpot);

					//The assigned spot is now at j+1
					Brick GapToUse = Bricks_On_The_Line->at(j+1);
					//GapToUse.Position = GapToUse.Position; //Small adjustment that is needed -.- OR MAYBE NOT???
							
					//The brick that now is at the position j-1.pos. Has a gap behind that starts at the position j.pos.
					//This means, that the brick has a size (security margin included) of  j-1.pos - j.pos
					//
					
					
					std::cout << "The 'gap' with index " << j << " is expected to be retreived at  " << _Manipulator_Fixed_Position.at(destinationManipulator) <<  std::endl;
					std::cout << "The 'gap' with index " << j << " currently is at  " << Bricks_On_The_Line->at(j+1).Position <<  std::endl;
					std::cout << "The 'gap' with index " << j << " is of size " << Bricks_On_The_Line->at(j+1).Position - Bricks_On_The_Line->at(j+2).Position  <<  std::endl;
					_ManipulatorOrderList->AddOrder(GapToUse, _Manipulator_Fixed_Position);
					break;
				}
			}
		}
		else if(_Manipulator_Modes.at(destinationManipulator) == MODE_OUTPUT)
		{
			//If the manipulator is flagged as OUTPUT, it can take a brick and delay the moment of let the brick down.
			//Manipulators flagged as OUTPUT will have just one order, for simplicity

			//As soon as a gap is required, the manipulator has to take the brick. For that a temporary order is issued
			static std::vector<bool> SPOTNEEDED = {0,0,0,0,0,0,0,0,0,0};
			static std::vector<bool> SPOTFOUND = {0,0,0,0,0,0,0,0,0,0};

			if(_ManipulatorOrderList->atManipulator(destinationManipulator)->NumberOfOrders()==0){
				SPOTFOUND.at(i)=0;					  //But we haven't found a place yet for this brick
				SPOTNEEDED.at(i)=0;					  //ANd we need a spot for this brick
			}
			//std::cout<<"This being checked for pallet " << i << "  " << _Bricks_Ready_For_Output->at(i) << std::endl;
			if(_ManipulatorOrderList->atManipulator(destinationManipulator)->NumberOfOrders()==0 &&
			   _Manipulator_TakenBrick.at(destinationManipulator).Type == 0 &&
				_Bricks_Ready_For_Output->at(i) == 1 && //This means that I have a brick that needs to go to the line
				getArm(destinationManipulator+1)->ManipulatorStatePosition==1 && //&&
				getArm(destinationManipulator+1)->ManipulatorRepositionState==2 //&&
				&& Algorithm::OUTPUTCOOLDOWN.at(destinationManipulator)<=0
				//getArm(destinationManipulator+1)->WhatToDoWithTheBrick == 0 &&
				//getArm(i+1)->CatchOrDrop == 0 &&
				//getArm(i+1)->PulseZAxis == 0
			    )
			{
				Brick TemporaryGapToUse(0, -50000, i+1, 0);
				_ManipulatorOrderList->AddOrder(TemporaryGapToUse, _Manipulator_Fixed_Position);
				_Bricks_Ready_For_Output->at(i) = BRICK_READY_SPOT_ASSIGNED; //This means that the brick has been already flagged as pick up.
				SPOTFOUND.at(i)=0;					  //But we haven't found a place yet for this brick
				SPOTNEEDED.at(i)=1;					  //ANd we need a spot for this brick

			}
			//This function has to keep delaying the order until a spot is finally found

			//The issue here is that both pallets satisfy this condition.
			else if(_ManipulatorOrderList->atManipulator(destinationManipulator)->NumberOfOrders()!=0 &&
					//_Manipulator_TakenBrick.at(destinationManipulator).Type != 1 &&
					SPOTFOUND.at(i) == 0 && SPOTNEEDED.at(i)==1
					)
			{
				if(_ManipulatorOrderList->atManipulator(destinationManipulator)->getOrder_byIndex(0)->When < 50000){
					_ManipulatorOrderList->atManipulator(destinationManipulator)->DelayFirstOrder(500);
				}
				SPOTFOUND.at(i)=0;
				SPOTNEEDED.at(i)=1;					  //ANd we need a spot for this brick
			}

			//And we check for the spots

			if(_ManipulatorOrderList->atManipulator(destinationManipulator)->NumberOfOrders()!=0 &&
			   _Manipulator_TakenBrick.at(destinationManipulator).Type != 0 &&
			   getArm(destinationManipulator+1)->DischargedTheBrickConfirm==1 &&
				SPOTFOUND.at(i) == 0 && SPOTNEEDED.at(i)==1)
			{
				for(unsigned int j=0; j<Bricks_On_The_Line->size(); j++)
				{
					Brick mBrick = Bricks_On_The_Line->at(j);
					Brick nextBrick(0,0,0,0);
					if(j<Bricks_On_The_Line->size()-1)
						 nextBrick = Bricks_On_The_Line->at(j+1);
					bool usableGap=true;
					//Find Gap
					//Check if big enough
					//Check when it's usable

					//CONDITION 1: It must be an empty gap for this manipulator
					if(mBrick.Type !=0) //We could add more things here. But it's better to keep it simple.
					{
						usableGap=false;
						std::cout << "Brick index " << j << " It's problem of condition 1. It's a brick" << std::endl;
					}

					//CONDITION 2: It has to be big enough, the size of the Gap > E
					else if(mBrick.Position - nextBrick.Position < E+E/2)
					{
						usableGap=false;
						std::cout << "Brick index " << j << " It's problem of condition 2: not big enough " << mBrick.Position - nextBrick.Position << " < " << E+E/2 << std::endl;
					}

					//CONDITION 2.5: Gap out of range.
					else if(nextBrick.Position+E+E/2 > _Manipulator_Fixed_Position.at(destinationManipulator))
					{
						usableGap=false;
						std::cout << "Brick index " << j << " It's problem of condition 2.5: Gap out of range" << nextBrick.Position+E+E/2 << " > "<< _Manipulator_Fixed_Position.at(destinationManipulator)  << std::endl;
					}

					//-----------------------------------------------------------------------//

					Spot=std::min(mBrick.Position,_Manipulator_Fixed_Position.at(destinationManipulator)); //The spot that we are going to use is the gap found

					if(usableGap==true && (Spot-0)>=E) //If the place where the spot is located is bigger than a brick
					{
						//Before:
						//   j+1        j        j-1                  0
						//---------+---------+---------+---------+---------+
						//  Brick  |   SPOT  |  Brick  |   ...   |  FRONT  |
						//---------+---------+---------+---------+---------+

						//	j+2	 	  	j+1          j         j-1                  0
						//-------+---------------+---------+---------+---------+---------+
						//Brick  |  AssignedSpot |   SPOT  |  Brick  |   ...   |  FRONT  |
						//-------+---------------+---------+---------+---------+---------+

						//After:
						//   j+3       j+2         j+1         j      j-1                  0
						//---------+---------+--------------+-----+---------+---------+---------+
						//  Brick  | newSPOT | AssignedSpot |SPOT |  Brick  |   ...   |  FRONT  |
						//---------+---------+--------------+-----+---------+---------+---------+

						SPOTFOUND.at(i) = 1;
						std::cout << "Spot found!!, for the 'brick' with index " << j << " Assigned to Manipulator " << destinationManipulator+1  <<  std::endl;

						//Insert two items just after that place, so the middle spot can be assigned for the operation
						//while maintaining the brick-spot principle (Note: second spot must be as thin as possible in order to save space)

						int assignedSpotPosition = Spot;//It has already a security margin seted up lines ago
						int newSpotPosition = assignedSpotPosition - E; //situated behind Assigned spot

						//Add assigned spot behind SPOT
						Brick AssignedSpot(0, assignedSpotPosition, i+1, 0);
						Bricks_On_The_Line->insert(Bricks_On_The_Line->begin()+j+1, AssignedSpot);
						//AsignedSpot becomes j+1, SPOT2 is j

						//Add new spot behind AssignedSpot
						Brick newSpot(0, newSpotPosition, 0, 0);
						Bricks_On_The_Line->insert(Bricks_On_The_Line->begin()+j+2, newSpot);

						//The assigned spot is now at j+1
						Brick GapToUse = Bricks_On_The_Line->at(j+1);
						//GapToUse.Position = GapToUse.Position; //Small adjustment that is needed -.- OR MAYBE NOT???

						//The brick that now is at the position j-1.pos. Has a gap behind that starts at the position j.pos.
						//This means, that the brick has a size (security margin included) of  j-1.pos - j.pos
						//


						//std::cout << "The 'gap' with index " << j << " is expected to be retrieved at  " << _Manipulator_Fixed_Position.at(destinationManipulator) <<  std::endl;
						//std::cout << "The 'gap' with index " << j << " currently is at  " << Bricks_On_The_Line->at(j+1).Position <<  std::endl;
						//std::cout << "The 'gap' with index " << j << " is of size " << Bricks_On_The_Line->at(j+1).Position - Bricks_On_The_Line->at(j+2).Position  <<  std::endl;

						_ManipulatorOrderList->ReplaceFirstOrder(GapToUse,_Manipulator_Fixed_Position);

						break;
					}
				}
			}
		}
	}
}

void RFIDSubroutine()
{
	//Asks for the RFID tags.
	for(int i=1;i<=10;i++)
	{
		StorageReadUID(i); 			//First read the UID of the pallet.
		if(StorageGetStoredUIDChangedFlag(i) && StorageGetStoredUIDChangedCount(i)>5)
		{  			//Check if there has been any pallet change
		    StorageClearStoredUIDChangedFlag(i);
			if(!boost::equals(StorageGetStoredUID(i),"0000000000000000"))
			{	//If the change is to another pallet, read it.
				StorageReadAllMemory(i);
				//cout<< "This simulates a memory Read" << endl;
			}
			else
			{						 									//If the change is to no pallet, clear the memory.
				StorageCleanPalletMemory(i);
			}
		}
	}
}
void CheckManualModes(OrderManager* _Manipulator_Order_List,
			 std::vector<int>* _Bricks_Ready_For_Output,
			 std::vector<Brick>* _Manipulator_TakenBrick,
			 std::deque<int>* Available_DNI_List){
	for(unsigned int i=0; i<_Manipulator_Order_List->NumberOfManipulators(); i++){

		if(getArm(i+1)->ManipulatorMode==0)
		{
			_Manipulator_Order_List->atManipulator(i)->ClearOrders();
			_Bricks_Ready_For_Output->at(i*2)=0;
			_Bricks_Ready_For_Output->at(i*2+1)=0;
			if(_Manipulator_TakenBrick->at(i).DNI!=0)Available_DNI_List->push_back(_Manipulator_TakenBrick->at(i).DNI);
			_Manipulator_TakenBrick->at(i)=Brick(0,0,0,0);
		}
	}

}

//-----------------------------------------------------------------------//
void * AlgorithmV2(void *Arg)
{
	using namespace Algorithm;
	/*Load configuration from file*******************/
	ConfigParser config("/etc/unit14/unit14.conf");
	CurrentPackagingColor = config.GetPackagingColor();
	CurrentPackagingGrade = config.GetPackagingGrade();
	Manipulator_Fixed_Position = config.GetArmPositions();
	Manipulator_Modes = config.GetManipulatorModes();
	SetNumberOfManipulators(&Manipulator_Order_List,
			&Pallet_LowSpeedPulse_Height_List,
			&Bricks_Ready_For_Output,
			&Manipulator_TakenBrick,
			&Available_DNI_List,
			Manipulator_Fixed_Position.size());

	if(Manipulator_Fixed_Position.size()>0){
		Bricks_On_The_Line.push_back(Brick(0,Manipulator_Fixed_Position.back()+2*E,0,0));
	} else {
		Bricks_On_The_Line.push_back(Brick(0,99999,0,0));
	}


	Synchro::DecreaseSynchronizationPointValue(0);
	while(1)
	{
		int RawCurrentPackagingBrick = ((CurrentPackagingGrade+1) << 4) + CurrentPackagingColor +1; //TODO: Check why +1
		//RFIDSubroutine();  WHAT THE FUCK!! THIS IS LEAKING MEMORY!!! JAGM: not really, just filling the stack too fast. Could be avoided by launching this in a different thread with a slower rate. As seen in main loop

		//std::cout<< "inside the algorithm loop"<<std::endl;
		//UPDATE ENCODER VALUES
		CheckManualModes(&Manipulator_Order_List, &Bricks_Ready_For_Output, &Manipulator_TakenBrick, &Available_DNI_List);

		int EncoderAdvance = Calculate_Advance(&PreviousValueOfTheLineEncoder);
		update_order_list(&Manipulator_Order_List, EncoderAdvance);
	    update_list(&Bricks_On_The_Line,
	    			&Manipulator_TakenBrick,
					EncoderAdvance,
					&Available_DNI_List,
					Manipulator_Fixed_Position,
					&OUTPUTCOOLDOWN,
					FIXEDBRICKS);
		update_PalletHeight(&Pallet_LowSpeedPulse_Height_List,
							&Manipulator_TakenBrick,
							&Bricks_On_The_Line,
							Manipulator_Fixed_Position,
							&Available_DNI_List,
							&Bricks_Ready_For_Output,
							&OUTPUTCOOLDOWN);
		//Check for full pallets



		//CHECK FOR INPUTas

		CheckPhotoSensor1(&Bricks_Before_The_Line); //Check for brick on photosensor 1 to store the grade
		bool NewBrickDetected = CheckPhotoSensor4(&Bricks_Before_The_Line, &Bricks_On_The_Line, &Available_DNI_List);
		//bool NewBrickDetected = false;
		if(NewBrickDetected ) //New brick detected!
		{
			//if brick should go through
			Brick* BrickToWorkWith = &(Bricks_On_The_Line.at(Bricks_On_The_Line.size()-2));//Remember, back is going to be a hole and (back - 1) the actual brick

			if(RawCurrentPackagingBrick==BrickToWorkWith->Type)
			{

				//if((RoboticArm::WhetherOrNotPutTheTileTo_16 && Enable_WhetherOrNotPutTheTileTo_16) || !force_input) //If the path is not blocked, go freely
				if(RoboticArm::WhetherOrNotPutTheTileTo_16 && false)
				{
					BrickToWorkWith->AssignedPallet=-3;//Because why not? -1 could mean go freely
				}
				else
				{
					Choose_Best_Pallet(BrickToWorkWith, Manipulator_Order_List, Manipulator_Fixed_Position, Manipulator_Modes, RawCurrentPackagingBrick, forced_pallet); //BrickToWorkWith is a pointer, take care
					Manipulator_Order_List.AddOrder(*BrickToWorkWith, Manipulator_Fixed_Position);
				}
			}
			else
			{
				Choose_Best_Pallet(BrickToWorkWith, Manipulator_Order_List, Manipulator_Fixed_Position, Manipulator_Modes, RawCurrentPackagingBrick, forced_pallet); //BrickToWorkWith is a pointer, take care
				Manipulator_Order_List.AddOrder(*BrickToWorkWith, Manipulator_Fixed_Position);
			}
		}

		//CHECK FOR OUTPUT
		//The output is:
		//Check that the output operation is allowed
		//if((RoboticArm::WhetherOrNotPutTheTileTo_16 && Enable_WhetherOrNotPutTheTileTo_16)  || force_output)
		if(RoboticArm::WhetherOrNotPutTheTileTo_16 || true)
		{
			//Draft()
			//Es tan simple como si hay una baldosa en el top de un palet del mismo tipo que lo que se esta empaquetando, se ponga en el primer hueco

			CheckForBricksAtTheTop(&Bricks_Ready_For_Output, RawCurrentPackagingBrick);

			//Will search at all the pallets looking for bricks that match the packing type and have not been assigned
			//This function will operate a vector, of 10 elements. it will write a 0 if it does not match the packing, a 1 if it does and has not been assigned yet.
			//It can write 0 at any time, it can write 1 only if there was not a -1 before.

			FindASpotForOutputBricks(&Bricks_On_The_Line,
										&Bricks_Ready_For_Output,
										Manipulator_Fixed_Position,
										&Manipulator_Order_List,
										Manipulator_Modes,
										Manipulator_TakenBrick); //Will check a spot for the pallets that have a 1. Once assigned a brick it will write -1;

			//Will check absolutely every damn gap that happens in the line
			//This function will place an order
		}
		//Give to the PLC the updates of the Order Line.

		//CHECK FOR REORDER

		//if I were to move the brick at the top of the pallet to another place, would it be better?

		//CHECK FOR FORCED OUTPUT
		//If a pallet has bricks of the matching type buried, try to recover it by moving
		//all the bricks to the other side (or other pallets) until there are no matching bricks left.
		//After that, return to IN/OUT Mode

		ProcessOrdersToPLC(&Manipulator_Order_List, Pallet_LowSpeedPulse_Height_List);

#define __BLOCKING_MODE 1
#if __BLOCKING_MODE
		pthread_mutex_lock(&read_mutex);
		pthread_cond_wait(&read_condition, &read_mutex);
		pthread_mutex_unlock(&read_mutex);
#endif

	}
	pthread_exit(NULL);
}
