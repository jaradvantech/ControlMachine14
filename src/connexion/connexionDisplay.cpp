/*
 * connexionDisplay.cpp
 *
 *  Created on: Dec 22, 2017
 *      Author: Jose Andres Grau Martinez
 */


#include <connexionDisplay.h>
#include <string>
#include <StorageInterface.h>
#include <RoboticArmInfo.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <Algorithm.h>

inline const char * const BoolToString(bool b)
{
	return b ? "1" : "0";
}

/*
 * PLC Get the information of Single Robotic Arm
 * Rev.: RBS
 */
std::string Command_PGSI(std::string const& Buffer)
{
	std::string Answer;
	int mPlcNumber = boost::lexical_cast<int>(Buffer.substr(5, 2));
	int mArmNumber = boost::lexical_cast<int>(Buffer.substr(8, 2));
	RoboticArm* mArm = DesiredRoboticArm(mArmNumber);

	/*Arm specific variables*/
	Answer  = "PGSI_";
	Answer += (boost::format("%02u") % mPlcNumber).str();
	Answer += "_";
	Answer += (boost::format("%02u") % mArmNumber).str();
	Answer += "_";
	Answer += (boost::format("%01u") % mArm->HasDischarged).str();
	Answer += (boost::format("%01u") % mArm->PhotosensorOfManipulator).str();
	Answer += (boost::format("%01u") % mArm->ManipulatorStatePosition).str();
	Answer += (boost::format("%01u") % mArm->DischargedTheBrickConfirm).str();
	Answer += (boost::format("%01u") % mArm->LeftStorageBinSecurity).str();
	Answer += (boost::format("%01u") % mArm->RightStorageBinSecurity).str();
	Answer += "_";
	Answer += (boost::format("%05u") % abs(mArm->AlarmArray)).str();
	Answer += "_";
	Answer += (boost::format("%01u") % mArm->ManipulatorRepositionState).str();
	Answer += "_";
	Answer += (boost::format("%010u") % abs(mArm->ActualValueEncoder)).str();
	Answer += "_";

	/*Common Variables*/
	Answer += (boost::format("%01u") % mArm->TheQueueOfPhotosensor_1).str();
	Answer += (boost::format("%01u") % mArm->TheQueueOfPhotosensor_2).str();
	Answer += (boost::format("%01u") % mArm->TheQueueOfPhotosensor_3).str();
	Answer += (boost::format("%01u") % mArm->TheQueueOfPhotosensor_4).str();
	Answer += (boost::format("%01u") % mArm->StationInterlock_16).str();
	Answer += (boost::format("%01u") % mArm->WhetherOrNotPutTheTileTo_16).str();
	Answer += "_";
	Answer += (boost::format("%05u") % abs(mArm->EquipmentAlarmArray)).str();
	Answer += "_";
	Answer += (boost::format("%03u") % mArm->TileGrade).str();
	Answer += "_";
	Answer += (boost::format("%03u") % mArm->ChangeColor).str();
	Answer += "_";
	Answer += (boost::format("%03u") % mArm->SystemState).str();
	Answer += "_";
	Answer += (boost::format("%010u") % abs(mArm->ActualValueOfTheLineEncoder)).str();
	Answer += "_";
	Answer += (boost::format("%010u") % abs(mArm->EnterTheTileStartingCodeValue)).str();
	Answer += "\r\n";
	return Answer;

}


/*
 * RBS; PLC Write DAta
 */
std::string Command_PWDA(std::string const& Buffer){
	int mArmNumber = boost::lexical_cast<int>(Buffer.substr(8, 2));
	RoboticArm* mArm = DesiredRoboticArm(mArmNumber);

	mArm->StorageBinDirection = readBool(Buffer.substr(11, 1));
	mArm->ManipulatorReset = readBool(Buffer.substr(12, 1));
	mArm->StorageBinFullA = readBool(Buffer.substr(13, 1));
	mArm->StorageBinFullB = readBool(Buffer.substr(14, 1));
	mArm->BarCodeReadStateA = readBool(Buffer.substr(15, 1));
	mArm->BarCodeReadStateB = readBool(Buffer.substr(16, 1));
	mArm->ManipulatorMode = readBool(Buffer.substr(17, 1));
	mArm->VacuumValve = readBool(Buffer.substr(18, 1));
	mArm->ManualForwardBackward = boost::lexical_cast<short>(Buffer.substr(20,3));
	mArm->ManualLeftRight = boost::lexical_cast<short>(Buffer.substr(24,3));
	mArm->ManualUpDown = boost::lexical_cast<short>(Buffer.substr(28,3));
	mArm->CatchOrDrop =  boost::lexical_cast<short>(Buffer.substr(32,3));
	mArm->WhatToDoWithTheBrick = boost::lexical_cast<short>(Buffer.substr(36,3));
	mArm->PulseZAxis = boost::lexical_cast<int>(Buffer.substr(40,6));
	mArm->ValueOfCatchDrop = boost::lexical_cast<long>(Buffer.substr(47,6));

	mArm->CommunicationExchange = readBool(Buffer.substr(54, 1));
	mArm->TestPattern = readBool(Buffer.substr(55, 1));
	mArm->InquiryTheTile = readBool(Buffer.substr(56, 1));
	mArm->TransmissionManualDebugging = readBool(Buffer.substr(57, 1));
	mArm->PCState = (short) boost::lexical_cast<int>(Buffer.substr(59,3));
	mArm->Z_AxisDeceletationDistance = boost::lexical_cast<int>(Buffer.substr(63,6));
	mArm->Z_AxisStandbyValue = boost::lexical_cast<int>(Buffer.substr(70,6));
	mArm->ThePulseOfX_AxisGoBackToTheWaitingPositionInAdvance = boost::lexical_cast<int>(Buffer.substr(77,6));
	mArm->ThePulseOfZ_AxisAdvanceDownInAdvance = boost::lexical_cast<int>(Buffer.substr(84,6));

	return "\r\n";
}

std::string Command_RGMV(std::string const& Buffer) {
	std::string Answer;
	int mPalletNumber = boost::lexical_cast<int>(Buffer.substr(5, 2)) + 1;
	//StorageReadAllMemory(mPalletNumber);
	Answer = "RGMV_";
	Answer += Buffer.substr(5, 2); //mPalletNumber
	Answer += "_";

	for (int i = 0; i <= StorageGetNumberOfBricks(mPalletNumber); i++) {

		Answer += char(StorageGetRaw(mPalletNumber, i));
	}
	Answer += "\r\n";

	return Answer;
}

std::string Command_RFMV(std::string const& Buffer) {
	std::string Answer;

	int mPalletNumber = boost::lexical_cast<int>(Buffer.substr(5, 2)) + 1;
	StorageFormatMemory(mPalletNumber);
	//StorageReadAllMemory(mPalletNumber);
	Answer = "RFMV_";
	Answer += Buffer.substr(5, 2);
	Answer += "\r\n";

	return Answer;
}

std::string Command_RAMV(std::string const& Buffer) {

	std::string Answer;

	int mPalletNumber = boost::lexical_cast<int>(Buffer.substr(5, 2)) + 1;
	char mCharacterToAdd = Buffer.at(8);
	int mCharacterToAdd_int = (int) mCharacterToAdd;

	StorageAddBrick(mPalletNumber, mCharacterToAdd_int >> 4,
			mCharacterToAdd_int & 15);

	Answer = "RAMV_";
	Answer += Buffer.substr(5, 2);
	Answer += "_";
	Answer += mCharacterToAdd;
	Answer += "\r\n";

	return Answer;

}

std::string Command_RDMV(std::string const& Buffer) {
	std::string Answer;

	int mPalletNumber = boost::lexical_cast<int>(Buffer.substr(5, 2)) + 1;
	int mPositionToDelete = boost::lexical_cast<int>(Buffer.substr(8, 3));

	StorageDeleteBrick(mPalletNumber, mPositionToDelete);

	Answer = "RDMV_";
	Answer += Buffer.substr(5, 2);
	Answer += "_";
	Answer += Buffer.substr(8, 3);
	Answer += "\r\n";

	return Answer;
}

std::string Command_RGUV(std::string const& Buffer) {
	std::string Answer;

	return Answer;
}

std::string Command_RPRV(std::string const& Buffer) {
	std::string Answer;
	//Get the number of pallets from the command
	int mNumberOfPallets = boost::lexical_cast<int>(Buffer.substr(5, 2));
	Answer = "RPRV_";
	Answer+=(boost::format("%02u")%mNumberOfPallets).str();
	// for every brick, add the UID, the number of bricks and the top brick
	for(int i=1;i<=mNumberOfPallets;i++){
		Answer+= "_";
		Answer+=StorageGetStoredUID(i);
		Answer+=char(StorageGetRaw(i,0));
		if(StorageGetNumberOfBricks(i)>0){
			Answer+=char(StorageGetRaw(i,StorageGetNumberOfBricks(i)));
		}else{
			Answer+=char(1);
		}
	}
	//Get the list of bricks on the line and add the number of bricks
	std::deque<std::array<long,4>> mListOfBricksOnTheLine = GetListOfBricksOnTheLine();
	Answer+= "_";
	Answer+= (boost::format("%02u")%(mListOfBricksOnTheLine.size())).str();
	//For every brick add the
	for(int i=1;i<=mListOfBricksOnTheLine.size();i++){
		Answer+= "_";
		Answer+=(boost::format("%06u")%(mListOfBricksOnTheLine.at(i-1).at(0))).str();//position
		Answer+=char(mListOfBricksOnTheLine.at(i-1).at(1));							 //grade and colour
		Answer+=(boost::format("%02u")%(mListOfBricksOnTheLine.at(i-1).at(2))).str();//assigned pallet
		Answer+=(boost::format("%02u")%(mListOfBricksOnTheLine.at(i-1).at(3))).str();//dni
	}
	//Common manipulator information
	Answer+="_";
	Answer+=(boost::format("%06u")%(RoboticArm::ActualValueOfTheLineEncoder)).str();

	//For every manipulator
	for(int i=1;i<=mNumberOfPallets/2;i++){
		Answer+="_";
		Answer+=(boost::format("%06u")%(DesiredRoboticArm(i)->ActualValueEncoder)).str();
		Answer+=(boost::format("%06u")%(DesiredRoboticArm(i)->ValueOfCatchDrop)).str();
	}
	Answer+="\r\n";
	return Answer;
}

bool readBool(std::string mString)
{
	if(mString=="0" || mString=="00")
		return false;
	else
		return true;
}
