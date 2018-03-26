/*
 * Algorithm.cpp
 *
 *  Created on: Jan 22, 2018
 *      Author: andres
 */



#include "Algorithm.h"
#include "StorageInterface.h"
#include "RoboticArmInfo.h"
#include <iostream>
#include <deque>
#include <memory>
#include <array>
#include <stdlib.h> //REMOVE THIS ASAP!!!
#include <time.h>
#include <SynchronizationPoints.h>
///////////////////////////////////////////
//The score of the pallet has to take in account:
//-Position of the next equal brick in the pallet.
//-Position of the pallet in the line
//Somehow, the colour and grade of the packing bricks
///////////////////////////////////////////

//Working principle:
//-Every time that a brick enters the line all the bricks from the end to the beginning
// of the line will be evaluated individually.
//
// The algorithm iterates all the possible combinations
// that are available for the bricks in the line
//
//
//-If a manipulator has a 'reserved' value for some brick, and this brick,
// after the check of that manipulator, reaches the manipulator,
// that manipulator will take the brick
//
//
// A brick can cancel its reserved position if and only if can previously claim a
// 'most likely available at the time of arrival of that brick'. OPTIONAl
//
// Unloading:
// -To unload a brick there must be an empty space in the line,
// -the guarantee that if the operation is being performed,
//  a new brick at the beginning of the line will have at least an available manipulator
//  or will see this manipulator as 'most likely available at the time of arrival of that brick'.
//
// -
//
//
///////////////////////////
#define _NUMBEROFBRICKSONLINE 3
#define _NUMBEROFMANIPULATORS 5
#define _MAXIMUMNUMBEROFBRICKSONLINE 10
#define _LINELENGTH 80000
#define _MANIPULATORWORKINGTIME 6000
#define _ENCODERTOLERANCE 30

const int ManipulatorFixedPositions[_NUMBEROFMANIPULATORS+1]={80000,5870,9783,13696,17609,21522}; //RBS el tercero deberia ser entorno a 16k
std::array<long,_NUMBEROFMANIPULATORS+1> ManipulatorReadiness={80000,80000,80000,80000,80000,80000};
//JUST A REMINDER: BrickOnTheLine //0:Position  1: Grade And Colour coded  2: Assigned pallet  3:DNI
std::deque<std::array<long,4>> BricksOnTheLine_List; //0 is the brick more near to the end of the line.
std::deque<long> BricksOnTheLine_AvailableDNI;
class Node{
	public:
		//Node(int _brick,Node* _Parent);
		Node(int _brick, int _brickPosition, std::shared_ptr<Node> _Parent);
		int pallet;
		int brick;
		std::array<long,6> new_available_time;
		std::shared_ptr<Node> Parent;
		//Node* Parent; replaced with shared pointer
		int NumberOfBranches;
		int NumberOfCheckedPallets;
		//Node* Branch[_NUMBEROFMANIPULATORS]; replaced with shared pointer
		std::shared_ptr<Node> Branch[_NUMBEROFMANIPULATORS*2];
};

//Node::Node(int _brick,Node* _Parent){
Node::Node(int _brick,int _brickPosition, std::shared_ptr<Node> _Parent){
	Parent=_Parent;
	brick=_brick;

	NumberOfBranches=0;
	NumberOfCheckedPallets=0;
	pallet=0;
	if (Parent!=nullptr){
		new_available_time=Parent->new_available_time;

		Parent->NumberOfBranches++;
		pallet=Parent->NumberOfCheckedPallets;

		new_available_time[(pallet+1)/2]=_brickPosition-_MANIPULATORWORKINGTIME;
	}
}

void FillDINs(){
	for(int i=1;i<=_MAXIMUMNUMBEROFBRICKSONLINE;i++){
		BricksOnTheLine_AvailableDNI.push_back(i);
	}
}

int ScoreOfThePallet(int StorageNumber, int Grade, int Colour){

}

int CheckForFirstMatch(int StorageNumber,int Grade, int Colour){
	int NumberOfBricks=StorageGetNumberOfBricks(StorageNumber);
	int PositionOfMatch=NumberOfBricks;//
	int BrickRaw=(Colour << 4) + Grade;

	while(BrickRaw!=StorageGetRaw(StorageNumber,PositionOfMatch) || PositionOfMatch>=1){ //Iterate until a match or end of pallet
		PositionOfMatch--;
	}

	if(PositionOfMatch==0)return -1;

	return NumberOfBricks-PositionOfMatch;
	//0 means that the top of the pallet matches the input
	//-1 means not found
	//Any value means the number of bricks in between.
}

int CheckForSurfaceCoincidence(int StorageNumber,int Grade, int Colour){
	int NumberOfBricks=StorageGetNumberOfBricks(StorageNumber);
	int PositionOfMatch=NumberOfBricks;//
	int BrickRaw=(Colour << 4) + Grade;

	while(BrickRaw!=StorageGetRaw(StorageNumber,PositionOfMatch) || PositionOfMatch>=1){ //Iterate until a match or end of pallet
		PositionOfMatch--;
	}

	if(PositionOfMatch==0)return -1;

	return NumberOfBricks-PositionOfMatch;
	//returns the first storage that
	//-1 means not found
	//Any value means the number of bricks in between.
}

void WriteBranch(Node *mNode,int Brick ,int PalletPosition){
	//int BranchNumber = 0;
	//if(CheckRestrictions(&mNode,i)){
		//mNode->Branch[PalletPosition-1] = new Node(Brick, PalletPosition, mNode);
		//std::cout<<"Value " << mNode->Branch[PalletPosition-1]->brick << std::endl;
		//BranchNumber++;
	//}

}

//int CheckRestriction(Node *writingNode, int BrickIterator){
int CheckRestriction(std::shared_ptr<Node> writingNode, int BrickIterator){
	//-------------------------------------------------------------------------------------
	//the parent of mNode is set to nullptr.
	//If at some point writingNode becomes nullptr it means that the tree can't grow more.
	//When 5 is returned we have to stop the building
	if (writingNode==nullptr) return 5;
	//-------------------------------------------------------------------------------------
	//Every brick on the line has a depth level on the tree
	//if it returns 3, the tree can't grow any deep.
	//We should go up a layer.
	if (BrickIterator==BricksOnTheLine_List.size())return 3;
	//-------------------------------------------------------------------------------------
	//Assuming that the tree can grow deeper, we have to check if the brick could be assigned to that manipulator
	//We know the number of pallets that have been checked. And we want to ckeck the next one
	int TemporaryAssignedPallet=writingNode->NumberOfCheckedPallets+1;
	int TemporaryBrickAssignedManipulator=(TemporaryAssignedPallet+1)/2;
	//Now we know which manipulator will take care of the brick. But is this possible?
	//If the position of the brick is greater than the position of the manipulator, no.
	if (BricksOnTheLine_List.at(BrickIterator).at(0)>=ManipulatorFixedPositions[TemporaryBrickAssignedManipulator]-_ENCODERTOLERANCE) return 2; // brick has passed by the manipulator
	//And if the manipulator is going to be bussy when the brick arrives, neither.
	if (BricksOnTheLine_List.at(BrickIterator).at(0)>writingNode->new_available_time[TemporaryBrickAssignedManipulator]) return 2; // manipulator will be bussy when the brick arrives
	//When we return 2, we have to keep moving horizontally.
	//-------------------------------------------------------------------------------------
	//If we already checked the maximum number of pallets, this node can't grow anymore horizontally
	if (writingNode->NumberOfCheckedPallets>=_NUMBEROFMANIPULATORS*2) return 4;
	return 1;
}

void * Algorithm(void *Arg) {
	FillDINs();
	bool DoOnce=0;
	std::cout<< "here we are"<< std::endl;

	long lastBrick_Position=RoboticArm::EnterTheTileStartingCodeValue;
	long mPreviousValueOfTheLineEncoder=RoboticArm::ActualValueOfTheLineEncoder;
	std::deque<std::array<long,4>> BricksOnTheQueue_List; //0:Position  1: Grade And Colour coded  2: Assigned pallet  3:DNI
	bool BrickOnTheQueueDetected=false;
	while(1){
		//Update the bricks position.
		//std::cout << "Encoder actual " << RoboticArm::ActualValueOfTheLineEncoder << std::endl;
		//std::cout << "Encoder previous " << mPreviousValueOfTheLineEncoder << std::endl;

		int EncoderAdvance;
		if(RoboticArm::ActualValueOfTheLineEncoder-mPreviousValueOfTheLineEncoder>=0){
			EncoderAdvance = RoboticArm::ActualValueOfTheLineEncoder-mPreviousValueOfTheLineEncoder; //Get how much the encoder did advance
			mPreviousValueOfTheLineEncoder+=EncoderAdvance; //Take this value and update for next iterations
		} else {
			EncoderAdvance = RoboticArm::ActualValueOfTheLineEncoder+100000-mPreviousValueOfTheLineEncoder; //Get how much the encoder did advance
			mPreviousValueOfTheLineEncoder+=EncoderAdvance-100000;
		}
		//std::cout << "Encoder advance " << EncoderAdvance << std::endl;
		//We know how much the line advanced, it's time to update the bricks information
		for(int i=0;i<BricksOnTheLine_List.size();i++){
			BricksOnTheLine_List.at(i).at(0)+=EncoderAdvance; //Take every brick on the list and update its position.
		}
		//Update the new_available_time with the help of EncoderAdvance
		for(int i=1; i<_NUMBEROFMANIPULATORS;i++){
			if(ManipulatorReadiness[i]<ManipulatorFixedPositions[0])ManipulatorReadiness[i]+=EncoderAdvance;
		}

		//Check if it's out of the line
		if(BricksOnTheLine_List.size() && BricksOnTheLine_List.begin()->at(0)>=_LINELENGTH){
			std::cout << "Brick abandons the line because its position is "<< BricksOnTheLine_List.begin()->at(0) << std::endl;
			BricksOnTheLine_AvailableDNI.push_front(BricksOnTheLine_List.begin()->at(3));
			BricksOnTheLine_List.pop_front();
		}
		//Check for new bricks on the queue.
		int TileGrade = RoboticArm::TileGrade;
		if(TileGrade !=0 && BrickOnTheQueueDetected==false){ //We have a new brick on the photosensor 1
			BrickOnTheQueueDetected=true;

			std::array<long,4> BrickOnTheLine; //0:Position  1: Grade And Colour coded  2: Assigned pallet  3:DNI

			BrickOnTheLine = {0, TileGrade+16, 0,0};
			BricksOnTheQueue_List.push_back(BrickOnTheLine);
			std::cout << "Brick of the type "<< BrickOnTheLine[1] << " enters the line" << std::endl;
			std::cout << "brick detected PS1" << std::endl;
		}else if(TileGrade==0){
			BrickOnTheQueueDetected=false;

		}
		//Check for new bricks in the line.

		if(RoboticArm::EnterTheTileStartingCodeValue !=lastBrick_Position){ //We have a new brick on the photosensor 4
			std::cout << "brick detected PS4" << std::endl;
			lastBrick_Position=RoboticArm::EnterTheTileStartingCodeValue;		//Update the values for future iterations
			std::array<long,4> BrickOnTheLine = BricksOnTheQueue_List.front(); //0:Position  1: Grade And Colour coded  2: Assigned pallet  3:DNI
			BricksOnTheQueue_List.pop_front();
			int TypeOfBrick= BrickOnTheLine[1];
			std::cout << "Brick of the type "<< BrickOnTheLine[1] << " still is" << std::endl;
			BrickOnTheLine = {RoboticArm::ActualValueOfTheLineEncoder-RoboticArm::EnterTheTileStartingCodeValue, TypeOfBrick,0, BricksOnTheLine_AvailableDNI.at(0)};
			std::cout << "Brick of the type "<< BrickOnTheLine[1] << " enters the line again" << std::endl;
			BricksOnTheLine_AvailableDNI.pop_front();
			BricksOnTheLine_List.push_back(BrickOnTheLine);
			DoOnce=1; //This way the calculations will be activated
		}

		//Check for bricks that a manipulator has to take out.
		//If the position of a brick is within the range of the assigned manipulator, take it out.
		for(int i=0;i<BricksOnTheLine_List.size();i++){  //for every brick
			int ActualBrickPosition = BricksOnTheLine_List.at(i).at(0); //Take the position of that brick
			int ActualBrickRaw = BricksOnTheLine_List.at(i).at(1);		//take the colour and grade

			int ActualBrickAssignedPallet=BricksOnTheLine_List.at(i).at(2);     //
			int ActualBrickAssignedManipulator=(ActualBrickAssignedPallet+1)/2; //

			if(ActualBrickPosition>= ManipulatorFixedPositions[ActualBrickAssignedManipulator]-_ENCODERTOLERANCE &&
					ActualBrickPosition<= ManipulatorFixedPositions[ActualBrickAssignedManipulator]+_ENCODERTOLERANCE){//if it's between the range action of the manipulator
				//WE DON'T NEED TO GIVE ANY SPECIFIC ORDER HERE TO THE PLC.
				StorageAddBrick(ActualBrickAssignedPallet,ActualBrickRaw >> 4, ActualBrickRaw & 15);
				std::cout << "Taking out "<< i <<" to pallet "<< ActualBrickAssignedPallet << " the brick "<< ActualBrickRaw << std::endl;
				BricksOnTheLine_AvailableDNI.push_front(BricksOnTheLine_List.at(i).at(3));
				BricksOnTheLine_List.erase(BricksOnTheLine_List.begin()+i);
			} else if(ActualBrickPosition >= ManipulatorFixedPositions[ActualBrickAssignedManipulator]+_ENCODERTOLERANCE){
				//If for any reason a brick missed the manipulator, most likely because the program was bussy with the algorithm
				DoOnce=1; //recalculate?
			}
		}
		//Build tree.

		int BrickIterator=0;
		//Node* mNode=new Node(0,nullptr); //Create tree
		std::shared_ptr<Node>mNode = std::make_shared<Node>(0,0,nullptr);
		mNode->new_available_time=ManipulatorReadiness; //initially it's available for all the manipulators.
		//This is not true, I have to change this in every iteration to the real values. Because some manipulator might be busy when a brick enters the line

		//Node* writingNode=mNode;
		std::shared_ptr<Node> writingNode=mNode;
		//Node* resultNode;
		std::shared_ptr<Node>resultNode;

		int iterations=0;
		bool DoOnceExecuted=0;
		//TODO: Here we need to store the path and the points of the path
		std::deque<			//The first list holds the paths, so we can iterate through them
		std::deque<int>		//the second one holds in the position 0 the points and after that, the manipulator assigned to each brick.
		> PathAndPointsContainer;

		//We start with writingNode as the root of the tree, mNode.
		Synchro::DecreaseSynchronizationPointValue(0);
		while (DoOnce) {
			iterations++;
			//We check if this node can hold more nodes, we check if the tree can grow.
			//See the function CheckRestriction for details.
			switch(CheckRestriction(writingNode,BrickIterator)){
				case 1: //The node can be created
					writingNode->NumberOfCheckedPallets+=1;
					writingNode->Branch[writingNode->NumberOfBranches-1]
					= std::make_shared<Node>(BricksOnTheLine_List.at(BrickIterator).at(1),BricksOnTheLine_List.at(BrickIterator).at(0),writingNode);
					BrickIterator++; //We go down the tree, we take the next brick
					writingNode = writingNode->Branch[writingNode->NumberOfBranches-1];
					break;
				case 2: //The node can't be created for that pallet because of manipulator state
					writingNode->NumberOfCheckedPallets+=1;
					if(writingNode->NumberOfCheckedPallets<=_NUMBEROFMANIPULATORS*2) break;
					/* no break */
				case 3: //A new node can't be created because we are at the bottom level
					if(BrickIterator==BricksOnTheLine_List.size()) {
						resultNode=writingNode;
						std::deque<int> PossiblePathAndPoints;
						while(resultNode->Parent!=nullptr){
							std::cout<< "Brick " << resultNode->brick << " to  pallet " << resultNode->pallet << std::endl;
							//TODO: Add the possible path to a container

							PossiblePathAndPoints.push_front(resultNode->pallet);


							resultNode = resultNode->Parent;
						}
						//TODO: We have a possible path, it's time to evaluate it in form of points
						//Add the path and its points to a container
						int Points =3;
						PossiblePathAndPoints.push_front(Points); //points
						PathAndPointsContainer.push_back(PossiblePathAndPoints);//Add at the end
						std::cout<<"------------------"<<std::endl;
					}
					//BrickIterator--; //We return to the parent, we must take the previous brick
					//writingNode->NumberOfCheckedPallets+=1;
					//writingNode = writingNode->Parent;
					//break;
					/* no break */
				case 4: //A new node can't be create because the paths have been exhausted
					BrickIterator--; //We return to the parent, we must take the previous brick
					writingNode->NumberOfCheckedPallets+=1;
					writingNode = writingNode->Parent;
					break;
				case 5:
					DoOnce=false;
					DoOnceExecuted=1;
					break;
			}
		}
		//the tree building succeeded
		if(DoOnceExecuted && PathAndPointsContainer.size()!=0){
			if(DoOnceExecuted && ((PathAndPointsContainer.at(0).size()-1)==BricksOnTheLine_List.size())){
				//TODO: Soooo here we have all the paths and it's points! It's time to choose one from PathAndPointsContainer
				//for(int i=0;i<PathAndPointsContainer.size();i++){
					//PathAndPointsContainer.at(i).at(0);
				//}
				//TODO: Here the path to take is clear

				//TODO: Assign a pallet is as simple as BricksOnTheLine_List.at(NumberOfTheBrickOnTheLine).at(2)=Pallet
				srand(time(NULL));
				int Random=rand()%PathAndPointsContainer.size();

				for(int i=1;i<PathAndPointsContainer.at(Random).size();i++){
					BricksOnTheLine_List.at(i-1).at(2)=PathAndPointsContainer.at(Random).at(i);
				}
				//So, we have at BricksOnTheLine all the information.
				//It's time to give to the PLC what he wants to know.
				//The value of the encoder at which he has to operate
				for(int i=0;i<BricksOnTheLine_List.size();i++){  //for every brick
					int AssignedManipulator = (BricksOnTheLine_List.at(i).at(2)+1)/2;
					DesiredRoboticArm(AssignedManipulator)->CatchOrDrop=1;
/*
					DesiredRoboticArm(AssignedManipulator)->ValueOfCatchDrop=
							RoboticArm::ActualValueOfTheLineEncoder +
							ManipulatorFixedPositions[AssignedManipulator]-BricksOnTheLine_List.at(i).at(0);
				*/
					DesiredRoboticArm(AssignedManipulator)->ValueOfCatchDrop=
												ManipulatorFixedPositions[AssignedManipulator]+RoboticArm::EnterTheTileStartingCodeValue;
					std::cout << "Brick entered the line at pos: " << std::to_string(RoboticArm::EnterTheTileStartingCodeValue) << std::endl <<
							     "Manipulator " << std::to_string(AssignedManipulator) <<
								 " position is: " << std::to_string(ManipulatorFixedPositions[AssignedManipulator]) << std::endl <<
								 " therefore I guess pick up position is: " << std::to_string(DesiredRoboticArm(AssignedManipulator)->ValueOfCatchDrop) << std::endl;

				}
				mNode.reset();
				if(iterations!=0)std::cout<< "Iterations: "<< iterations << std::endl;
				DoOnceExecuted=0;
			}
		}else if(DoOnceExecuted)std::cout<<"For some reason it's  0"<<std::endl;
	}
}

std::deque<std::array<long,4>> GetListOfBricksOnTheLine(){
	return BricksOnTheLine_List;
}
