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


/*
std::string Command_PGSI(RoboticArm GlobalArm[],std::string const& Buffer) {
	std::string Answer;
	int mPlcNumber = boost::lexical_cast<int>(Buffer.substr(5, 2));
	int mRoboticArm = boost::lexical_cast<int>(Buffer.substr(8, 2));

	Answer = "PGSI_";
	Answer += Buffer.substr(5, 2); //mPlcNumber
	Answer += "_";
	Answer += Buffer.substr(8, 2); //mRoboticArm
	Answer += "_";
	Answer += TCPSBoolToString(GlobalArm[mRoboticArm].HasDischarged);
	Answer += "_";
	Answer += TCPSBoolToString(
			GlobalArm[mRoboticArm].PhotosensorOfManipulator);
	Answer += "_";
	Answer +=
			(boost::format("%01u") % GlobalArm[mRoboticArm].HasDischarged).str();
	Answer += (boost::format("%01u")
			% GlobalArm[mRoboticArm].PhotosensorOfManipulator).str();
	Answer += (boost::format("%01u")
			% GlobalArm[mRoboticArm].ManipulatorStatePosition).str();
	Answer += (boost::format("%01u")
			% GlobalArm[mRoboticArm].DischargedTheBrickConfirm).str();
	Answer += (boost::format("%01u")
			% GlobalArm[mRoboticArm].LeftStorageBinSecurity).str();
	Answer += (boost::format("%01u")
			% GlobalArm[mRoboticArm].RightStorageBinSecurity).str();
	Answer += "_";
	Answer +=
			(boost::format("%05u") % abs(GlobalArm[mRoboticArm].AlarmArray)).str();
	Answer += "_";
	Answer += (boost::format("%01u")
			% GlobalArm[mRoboticArm].ManipulatorRepositionState).str();
	Answer += "_";

	if (GlobalArm[mRoboticArm].ActualValueEncoder > 0)
		Answer += "0";
	else
		Answer += "1";

	Answer += (boost::format("%010u")
			% abs(GlobalArm[mRoboticArm].ActualValueEncoder)).str();

	Answer += "\r\n";
	return Answer;

}

std::string Command_PGAI(RoboticArm GlobalArm[],std::string const& Buffer) {
	std::string Answer;
	int mPlcNumber = boost::lexical_cast<int>(Buffer.substr(5, 2));
	int mRoboticArm = boost::lexical_cast<int>(Buffer.substr(8, 2));

	Answer = "PGAI_";
	Answer += Buffer.substr(5, 2); //mPlcNumber
	Answer += "_";
	Answer += Buffer.substr(8, 2); //mRoboticArm

	for (int i = 0; i < mRoboticArm; i++) {
		Answer += "_";
		Answer += TCPSBoolToString(GlobalArm[i].HasDischarged);
		Answer += "_";
		Answer += TCPSBoolToString(GlobalArm[i].PhotosensorOfManipulator);
		Answer += "_";

		Answer += (boost::format("%01u") % GlobalArm[i].HasDischarged).str();
		Answer += (boost::format("%01u")
				% GlobalArm[i].PhotosensorOfManipulator).str();
		Answer += (boost::format("%01u")
				% GlobalArm[i].ManipulatorStatePosition).str();
		Answer += (boost::format("%01u")
				% GlobalArm[i].DischargedTheBrickConfirm).str();
		Answer +=
				(boost::format("%01u") % GlobalArm[i].LeftStorageBinSecurity).str();
		Answer +=
				(boost::format("%01u") % GlobalArm[i].RightStorageBinSecurity).str();
		Answer += "_";
		Answer += (boost::format("%05u") % abs(GlobalArm[i].AlarmArray)).str();
		Answer += "_";
		Answer += (boost::format("%01u")
				% GlobalArm[i].ManipulatorRepositionState).str();
		Answer += "_";

		if (GlobalArm[i].ActualValueEncoder > 0)
			Answer += "0";
		else
			Answer += "1";

		Answer += (boost::format("%010u")
				% abs(GlobalArm[i].ActualValueEncoder)).str();
	}

	Answer += "\r\n";

	return Answer;
}
*/

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
