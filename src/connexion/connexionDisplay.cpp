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
#include <algorithms/algorithm_v2.h>
#include "ConfigParser.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

inline const char * const BoolToString(bool b)
{
	return b ? "1" : "0";
}
bool readBool(const std::string & mString)
{
	if(mString=="0" || mString=="00")
		return false;
	else
		return true;
}
/* 6/7/2018 JAGM Functions for the transition to JSON Based on Control Machine 18*/

bool FindAndAddTo(const rapidjson::Document& DOC, std::string Key, int* Variable)
{
	static rapidjson::Value::ConstMemberIterator itr;
	itr = DOC.FindMember(Key.c_str());
	if(itr !=DOC.MemberEnd()) {
		*Variable=itr->value.GetInt();
		return true;
	}
	else return false;
}

bool FindAndAddTo(const rapidjson::Document& DOC, std::string Key, std::string* Variable)
{
	static rapidjson::Value::ConstMemberIterator itr;
	itr = DOC.FindMember(Key.c_str());
	if(itr !=DOC.MemberEnd()) {
		*Variable=itr->value.GetString();
		return true;
	}
	else return false;
}

bool FindAndAddTo(const rapidjson::Document& DOC, std::string Key, bool* Variable)
{
	static rapidjson::Value::ConstMemberIterator itr;
	itr = DOC.FindMember(Key.c_str());
	if(itr !=DOC.MemberEnd()) {
		*Variable=itr->value.GetBool();
		return true;
	}
	else return false;
}
/*Functions for the transition to JSON Based on Control Machine 18*/



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
 * Values marked as 'X' won't be changed
 */
std::string Command_PWDA(std::string const& Buffer)
{
	int mArmNumber = boost::lexical_cast<int>(Buffer.substr(8,2));
	RoboticArm* mArm = DesiredRoboticArm(mArmNumber);

	std::cout << Buffer << std::endl;

	if(boost::contains(Buffer.substr(11,1), "X") == false)
		mArm->StorageBinDirection = readBool(Buffer.substr(11,1));

	if(boost::contains(Buffer.substr(12,1), "X") == false)
		mArm->ManipulatorReset = readBool(Buffer.substr(12,1));

	if(boost::contains(Buffer.substr(13,1), "X") == false)
		mArm->StorageBinFullA = readBool(Buffer.substr(13,1));

	if(boost::contains(Buffer.substr(14,1), "X") == false)
		mArm->StorageBinFullB = readBool(Buffer.substr(14,1));

	if(boost::contains(Buffer.substr(15,1), "X") == false)
		mArm->BarCodeReadStateA = readBool(Buffer.substr(15,1));

	if(boost::contains(Buffer.substr(16,1), "X") == false)
		mArm->BarCodeReadStateB = readBool(Buffer.substr(16,1));

	if(boost::contains(Buffer.substr(17,1), "X") == false)
		mArm->ManipulatorMode = readBool(Buffer.substr(17,1));

	if(boost::contains(Buffer.substr(18,1), "X") == false)
		mArm->VacuumValve = readBool(Buffer.substr(18, 1));

	if(boost::contains(Buffer.substr(20,3), "X") == false)
		mArm->ManualForwardBackward = boost::lexical_cast<short>(Buffer.substr(20,3));

	if(boost::contains(Buffer.substr(24,3), "X") == false)
		mArm->ManualLeftRight = boost::lexical_cast<short>(Buffer.substr(24,3));

	if(boost::contains(Buffer.substr(28,3), "X") == false)
		mArm->ManualUpDown = boost::lexical_cast<short>(Buffer.substr(28,3));

	if(boost::contains(Buffer.substr(32,3), "X") == false)
		mArm->CatchOrDrop =  boost::lexical_cast<short>(Buffer.substr(32,3));

	if(boost::contains(Buffer.substr(36,3), "X") == false)
		mArm->WhatToDoWithTheBrick = boost::lexical_cast<short>(Buffer.substr(36,3));

	if(boost::contains(Buffer.substr(40,6), "X") == false)
		mArm->PulseZAxis = boost::lexical_cast<int>(Buffer.substr(40,6));

	if(boost::contains(Buffer.substr(47,6), "X") == false)
		mArm->ValueOfCatchDrop = boost::lexical_cast<long>(Buffer.substr(47,6));

	if(boost::contains(Buffer.substr(54,1), "X") == false)
		mArm->CommunicationExchange = readBool(Buffer.substr(54,1));

	if(boost::contains(Buffer.substr(55,1), "X") == false)
		mArm->TestPattern = readBool(Buffer.substr(55,1));

	if(boost::contains(Buffer.substr(56,1), "X") == false)
		mArm->InquiryTheTile = readBool(Buffer.substr(56,1));

	if(boost::contains(Buffer.substr(57,1), "X") == false)
		mArm->TransmissionManualDebugging = readBool(Buffer.substr(57,1));

	if(boost::contains(Buffer.substr(59,3), "X") == false)
		mArm->PCState = (short) boost::lexical_cast<int>(Buffer.substr(59,3));

	if(boost::contains(Buffer.substr(63,6), "X") == false)
		mArm->Z_AxisDeceletationDistance = boost::lexical_cast<int>(Buffer.substr(63,6));

	if(boost::contains(Buffer.substr(70,6), "X") == false)
		mArm->Z_AxisStandbyValue = boost::lexical_cast<int>(Buffer.substr(70,6));

	if(boost::contains(Buffer.substr(77,6), "X") == false)
		mArm->ThePulseOfX_AxisGoBackToTheWaitingPositionInAdvance = boost::lexical_cast<int>(Buffer.substr(77,6));

	if(boost::contains(Buffer.substr(84,6), "X") == false)
		mArm->ThePulseOfZ_AxisAdvanceDownInAdvance = boost::lexical_cast<int>(Buffer.substr(84,6));

	return Buffer;
}

std::string Command_RGMV(std::string const& Buffer)
{
	std::string Answer;
	int mPalletNumber = boost::lexical_cast<int>(Buffer.substr(5, 2)) + 1;
	//StorageReadAllMemory(mPalletNumber);
	Answer = "RGMV_";
	Answer += Buffer.substr(5, 2); //mPalletNumber
	Answer += "_";

	for (int i = 0; i <= StorageGetNumberOfBricks(mPalletNumber); i++)
	{

		Answer += char(StorageGetRaw(mPalletNumber, i));
	}
	Answer += "\r\n";

	return Answer;
}

std::string Command_RFMV(std::string const& Buffer)
{
	std::string Answer;

	int mPalletNumber = boost::lexical_cast<int>(Buffer.substr(5, 2)) + 1;
	StorageFormatMemory(mPalletNumber);
	//StorageReadAllMemory(mPalletNumber);
	Answer = "RFMV_";
	Answer += Buffer.substr(5, 2);
	Answer += "\r\n";

	return Answer;
}

std::string Command_RAMV(std::string const& Buffer)
{

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

std::string Command_RDMV(std::string const& Buffer)
{
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

std::string Command_RGUV(std::string const& Buffer)
{
	std::string Answer;

	return Answer;
}

std::string Command_RPRV(std::string const& Buffer)
{
	std::string Answer;
	//Get the number of pallets from the command
	int mNumberOfPallets = boost::lexical_cast<int>(Buffer.substr(5, 2));
	Answer = "RPRV_";
	Answer+=(boost::format("%02u")%mNumberOfPallets).str();
	// for every brick, add the UID, the number of bricks and the top brick
	for(int i=1;i<=mNumberOfPallets;i++){
		Answer+= "_";
		Answer+=StorageGetStoredUID(i);
		/*
		 * Be careful with this char(), if it is 0, the string will be terminated and everything will crash.
		 */
		Answer+=char(StorageGetRaw(i,0));
		if(StorageGetNumberOfBricks(i)>0)
		{
			Answer+=char(StorageGetRaw(i,StorageGetNumberOfBricks(i)));
		}else
		{
			Answer+=char(1);
		}
	}
	//Get the list of bricks on the line and add the number of bricks
	std::deque<Brick> mListOfBricksOnTheLine = Algorithm::Get::Bricks_Before_The_Line();
	Answer+= "_";

	//RBS get list of bricks on line and iterate for every brick
	std::vector<int> BricksOnLine = Algorithm::Get::IndexesOfBricksOnLine(mListOfBricksOnTheLine);
	Answer+= (boost::format("%02u")%(BricksOnLine.size())).str();
	for(unsigned int i=0; i<BricksOnLine.size(); i++)
	{
		//std::cout << i << std::endl;
		if(mListOfBricksOnTheLine.at(BricksOnLine.at(i)).Type!=0)
		{
		Answer+= "_";
		Answer+=(boost::format("%06u")%(mListOfBricksOnTheLine.at(BricksOnLine.at(i)).Position)).str();//position
		Answer+=char(mListOfBricksOnTheLine.at(BricksOnLine.at(i)).Type);							 //grade and colour
		Answer+=(boost::format("%02u")%(mListOfBricksOnTheLine.at(BricksOnLine.at(i)).AssignedPallet)).str();//assigned pallet
		Answer+=(boost::format("%02u")%(mListOfBricksOnTheLine.at(BricksOnLine.at(i)).DNI)).str();//dni
			//std::cout << Answer << std::endl;
		}
	}

	//For every manipulator
	std::vector<Brick> mListOfBricksTakenByManipulators = Algorithm::Get::Manipulator_TakenBrick();

	for(unsigned int i=0; i<mListOfBricksTakenByManipulators.size(); i++)
	{
		Answer+="_";
		Answer+=(boost::format("%06u")%(DesiredRoboticArm(i+1)->ActualValueEncoder)).str();
		Answer+=(boost::format("%06u")%(DesiredRoboticArm(i+1)->ValueOfCatchDrop)).str();
		Answer+=(boost::format("%06u")%(mListOfBricksTakenByManipulators.at(i).Position)).str();
		Answer+=char(mListOfBricksTakenByManipulators.at(i).Type);
		Answer+=(boost::format("%02u")%(mListOfBricksTakenByManipulators.at(i).AssignedPallet)).str();
		Answer+=(boost::format("%02u")%(mListOfBricksTakenByManipulators.at(i).DNI)).str();
	}

	//Common manipulator information
	Answer+="_";
	Answer+=(boost::format("%06u")%(RoboticArm::ActualValueOfTheLineEncoder)).str();

	Answer+="\r\n";
	return Answer;
}

//ALgorithm Save Config
std::string Command_ALSC(std::string Buffer)
{
	ConfigParser config("/etc/unit14/unit14.conf");
	std::vector<int> modes;
	int Manipulators = 5; //ConfigParser.getManipulatorNumber();

	//Update both, running program and config file
	config.SetCurrentPackagingColor(boost::lexical_cast<int>(Buffer.substr(8,2)));
	Algorithm::Set::CurrentPackagingColor(boost::lexical_cast<int>(Buffer.substr(8,2)));

	config.SetCurrentPackagingGrade(boost::lexical_cast<int>(Buffer.substr(11,3)));
	Algorithm::Set::CurrentPackagingGrade(boost::lexical_cast<int>(Buffer.substr(11,3)));

	for(int i=0; i<Manipulators; i++)
		modes.push_back(boost::lexical_cast<int>(Buffer.substr((15+i),1)));
	config.SetManipulatorModes(modes);
	Algorithm::Set::ManipulatorModes(modes);

	/*
	 * code for weights
	 */
	std::string Answer=Buffer;
	Answer+="\n";

	return Answer;
}

//ALgorithm Get Config
std::string Command_ALGC(std::string Buffer)
{
	ConfigParser config("/etc/unit14/unit14.conf");
	std::vector<int> modes;
	int Manipulators = 5; //ConfigParser.getManipulatorNumber();

	std::string Answer = "ALGC_14_";
	Answer += config.GetPackagingGrade();
	Answer += "_";
	Answer += config.GetPackagingColor();
	Answer += "_";

	for(int i=0; i<Manipulators; i++)
		Answer += "_"; //(whatever)

	return Answer;
}
//Check for new alarms
std::string Command_CHAL(std::string const& Buffer)
{
	std::string Answer;
	int Manipulators = 5; //ConfigParser.getManipulatorNumber();

	Answer += "CHAL_14_";
	Answer += (boost::format("%05u") % abs(RoboticArm::EquipmentAlarmArray)).str();

	for(int i=1; i<=Manipulators; i++)
	{
		Answer += "_";
		Answer += (boost::format("%05u") % abs(DesiredRoboticArm(i)->AlarmArray)).str();
	}
	Answer += "\r\n";

	return Answer;
}

std::string Command_SCAP(std::string const& Buffer)
{
	std::string Answer;

	ConfigParser config("/etc/unit14/unit14.conf");
	//SCAP
	//_
	//05_000000_000000_000000_000000_000000\r\n

	int NumberOfArms = boost::lexical_cast<int>(Buffer.substr(5, 2));
	std::vector<int> ArmPositions;

	for(int i=0;i<NumberOfArms;i++)
	{
		ArmPositions.push_back(boost::lexical_cast<int>(Buffer.substr(8+i*7, 6)));
	}
	Algorithm::Set::ManipulatorFixedPosition(ArmPositions);
	config.SetArmPositions(ArmPositions);
	Answer = "SCAP\r\n";

	return Answer;
}

/*
 * RBS: Write Advanced Debug Information
 */
std::string Command_WADI(std::string const& Buffer)
{
	if(Buffer.substr(5,1) == "1")
	{
		//WhetherOrNotPutTheTileTo is enabled
		Algorithm::Set::enable_WhetherOrNotPutTheTileTo_16(true);

	}
	else
	{
		//WhetherOrNotPutTheTileTo is disabled
		Algorithm::Set::enable_WhetherOrNotPutTheTileTo_16(false);

		//wheterornot16 disabled, force output
		if(Buffer.substr(6,1) == "1")
			Algorithm::Set::force_output(true);
		else
			Algorithm::Set::force_output(false);

		//wheterornot16 disabled, force input
		if(Buffer.substr(7,1) == "1")
			Algorithm::Set::force_input(true);
		else
			Algorithm::Set::force_input(false);
	}

	return "WADI\r\n";
}

/*
 * RBS PLace ORder
 */
std::string Command_PLRD(std::string const& Buffer)
{

	int type = boost::lexical_cast<int>(Buffer.substr(5, 6));  //type
	int position = boost::lexical_cast<int>(Buffer.substr(12, 6)); //position (as a brick, it's relative to PS4)
	int pallet = boost::lexical_cast<int>(Buffer.substr(19, 6));//pallet

	Brick brick(type,position,pallet,0);//DNI is meaningless here
	Algorithm::Set::order(brick);
	return "PLRD\r\n";
}

/*
 * RBS FOrce To Pallet...
 */
std::string Command_FOTP(std::string const& Buffer)
{
	int destination_pallet = boost::lexical_cast<int>(Buffer.substr(5, 2));

	Algorithm::Set::forced_pallet(destination_pallet);

	std::cout << "forced to: " << destination_pallet << std::endl;

	return "FOTP\r\n";
}

void Command_GDIS(const rapidjson::Document& DOC_in, rapidjson::Writer<rapidjson::StringBuffer>* AnswerWriter){

	//Get debug internal state

	DOC_in.IsNull(); //removes a warning. I hate warnings.

	std::deque<int> Available_DNI_List = Algorithm::Get::Available_DNI_List();
	OrderManager Manipulator_Order_List = Algorithm::Get::Manipulator_Order_List();
	std::deque<Brick> Bricks_Before_The_Line = Algorithm::Get::Bricks_Before_The_Line();
	std::deque<Brick> Bricks_On_The_Line = Algorithm::Get::Bricks_On_The_Line();

	std::vector<int> Bricks_Ready_For_Output = Algorithm::Get::Bricks_Ready_For_Output();
	std::vector<int> Manipulator_Fixed_Position = Algorithm::Get::Manipulator_Fixed_Position();
	std::vector<int> Manipulator_Modes = Algorithm::Get::Manipulator_Modes();
	std::vector<int> Pallet_LowSpeedPulse_Height_List = Algorithm::Get::Pallet_LowSpeedPulse_Height_List();
	std::vector<Brick> Manipulator_TakenBrick = Algorithm::Get::Manipulator_TakenBrick();

	AnswerWriter->StartObject();			// Between StartObject()/EndObject(),

	AnswerWriter->Key("command_ID");		// output a key,
	AnswerWriter->String("GDIS");			// follow by a value.

	AnswerWriter->Key("Available_DNI_List");
	AnswerWriter->StartArray();
	for(unsigned int i=0;i<Available_DNI_List.size();i++) AnswerWriter->Int(Available_DNI_List.at(i));
	AnswerWriter->EndArray();

	AnswerWriter->Key("Manipulator_Order_List");
	AnswerWriter->StartArray();
	for(unsigned int i=0;i<Manipulator_Order_List.NumberOfManipulators();i++){
		AnswerWriter->StartObject();			// Between StartObject()/EndObject(),
		for (unsigned int j=0;j<Manipulator_Order_List.atManipulator(i)->NumberOfOrders();j++){
			AnswerWriter->Key("What");
			AnswerWriter->Bool(Manipulator_Order_List.atManipulator(i)->getOrder_byIndex(j)->What);
			AnswerWriter->Key("When");
			AnswerWriter->Int(Manipulator_Order_List.atManipulator(i)->getOrder_byIndex(j)->When);
			AnswerWriter->Key("Where");
			AnswerWriter->Bool(Manipulator_Order_List.atManipulator(i)->getOrder_byIndex(j)->Where);
		}
		AnswerWriter->EndObject();
	}
	AnswerWriter->EndArray();

	AnswerWriter->Key("Bricks_Before_The_Line");
	AnswerWriter->StartArray();
	for(unsigned int i=0;i<Bricks_Before_The_Line.size();i++){
		AnswerWriter->StartObject();			// Between StartObject()/EndObject(),
		AnswerWriter->Key("DNI");
		AnswerWriter->Int(Bricks_Before_The_Line.at(i).DNI);
		AnswerWriter->Key("AssignedPallet");
		AnswerWriter->Int(Bricks_Before_The_Line.at(i).AssignedPallet);
		AnswerWriter->Key("Position");
		AnswerWriter->Int(Bricks_Before_The_Line.at(i).Position);
		AnswerWriter->Key("Type");
		AnswerWriter->Int(Bricks_Before_The_Line.at(i).Type);
		AnswerWriter->EndObject();
	}
	AnswerWriter->EndArray();

	AnswerWriter->Key("Bricks_On_The_Line");
	AnswerWriter->StartArray();
	for(unsigned int i=0;i<Bricks_On_The_Line.size();i++){
		AnswerWriter->StartObject();			// Between StartObject()/EndObject(),
		AnswerWriter->Key("DNI");
		AnswerWriter->Int(Bricks_On_The_Line.at(i).DNI);
		AnswerWriter->Key("AssignedPallet");
		AnswerWriter->Int(Bricks_On_The_Line.at(i).AssignedPallet);
		AnswerWriter->Key("Position");
		AnswerWriter->Int(Bricks_On_The_Line.at(i).Position);
		AnswerWriter->Key("Type");
		AnswerWriter->Int(Bricks_On_The_Line.at(i).Type);
		AnswerWriter->EndObject();
	}
	AnswerWriter->EndArray();

	AnswerWriter->Key("Bricks_Ready_For_Output");
	AnswerWriter->StartArray();
	for(unsigned int i=0;i<Bricks_Ready_For_Output.size();i++) AnswerWriter->Int(Bricks_Ready_For_Output.at(i));
	AnswerWriter->EndArray();

	AnswerWriter->Key("Manipulator_Fixed_Position");
	AnswerWriter->StartArray();
	for(unsigned int i=0;i<Manipulator_Fixed_Position.size();i++) AnswerWriter->Int(Manipulator_Fixed_Position.at(i));
	AnswerWriter->EndArray();


	AnswerWriter->Key("Manipulator_Modes");
	AnswerWriter->StartArray();
	for(unsigned int i=0;i<Manipulator_Modes.size();i++) AnswerWriter->Int(Manipulator_Modes.at(i));
	AnswerWriter->EndArray();

	AnswerWriter->Key("Pallet_LowSpeedPulse_Height_List");
	AnswerWriter->StartArray();
	for(unsigned int i=0;i<Pallet_LowSpeedPulse_Height_List.size();i++) AnswerWriter->Int(Pallet_LowSpeedPulse_Height_List.at(i));
	AnswerWriter->EndArray();

	AnswerWriter->Key("Manipulator_TakenBrick");
	AnswerWriter->StartArray();
	for(unsigned int i=0;i<Manipulator_TakenBrick.size();i++){
		AnswerWriter->StartObject();			// Between StartObject()/EndObject(),
		AnswerWriter->Key("DNI");
		AnswerWriter->Int(Manipulator_TakenBrick.at(i).DNI);
		AnswerWriter->Key("AssignedPallet");
		AnswerWriter->Int(Manipulator_TakenBrick.at(i).AssignedPallet);
		AnswerWriter->Key("Position");
		AnswerWriter->Int(Manipulator_TakenBrick.at(i).Position);
		AnswerWriter->Key("Type");
		AnswerWriter->Int(Manipulator_TakenBrick.at(i).Type);
		AnswerWriter->EndObject();
	}
	AnswerWriter->EndArray();
	AnswerWriter->Key("Manipulator_State");
	AnswerWriter->StartArray();
	for(unsigned int i=0;i<Manipulator_Fixed_Position.size();i++){
		AnswerWriter->StartObject();
		AnswerWriter->Key("WhatToDoWithTheBrick");
		AnswerWriter->Int(DesiredRoboticArm(i+1)->WhatToDoWithTheBrick);
		AnswerWriter->Key("CatchOrDrop");
		AnswerWriter->Int(DesiredRoboticArm(i+1)->CatchOrDrop);
		AnswerWriter->Key("ValueOfCatchDrop");
		AnswerWriter->Int(DesiredRoboticArm(i+1)->ValueOfCatchDrop);
		AnswerWriter->EndObject();
	}
	AnswerWriter->EndArray();
	AnswerWriter->EndObject();
}





std::string ProcessCommand(std::string const& bufferRead, bool ServerIsReady)
{
	std::string bufferWrite;
	//TODO RBS, replace contains by substring(0,4) and be more specific when catching exceptions .-.
	if (boost::contains(bufferRead, "PGSI") && ServerIsReady)
	{
		try {bufferWrite = Command_PGSI(bufferRead);}
		catch(...){
			std::cout << "Command_PGSI exception: bad syntax"<< std::endl;
			bufferWrite = "Error_PGSI\r\n";
		}
	}
	else if (boost::contains(bufferRead,"PWDA") && ServerIsReady)
	{
		try {bufferWrite = Command_PWDA(bufferRead);}
		catch(...){
			std::cout << "Command_PWDA exception: bad syntax"<< std::endl;
			bufferWrite = "Error_PWDA\r\n";
		}
	}
	else if (boost::contains(bufferRead,"RGMV") && ServerIsReady)
	{
		try {bufferWrite = Command_RGMV(bufferRead);}
		catch(...){
			std::cout << "Command_RGMV exception: bad syntax"<< std::endl;
			bufferWrite = "Error_RGMV\r\n";
		}
	}
	else if (boost::contains(bufferRead,"RFMV") && ServerIsReady)
	{
		try {bufferWrite = Command_RFMV(bufferRead);}
		catch(...){
			std::cout << "Command_RFMV exception: bad syntax"<< std::endl;
			bufferWrite = "Error_RFMV\r\n";
		}
	}
	else if (boost::contains(bufferRead,"RAMV") && ServerIsReady)
	{
		try {bufferWrite = Command_RAMV(bufferRead);}
		catch(...){
			std::cout << "Command_RAMV exception: bad syntax"<< std::endl;
			bufferWrite = "Error_RAMV\r\n";
		}
	}
	else if (boost::contains(bufferRead,"RDMV") && ServerIsReady)
	{
		try {bufferWrite = Command_RDMV(bufferRead);}
		catch(...){
			std::cout << "Command_RDMV exception: bad syntax"<< std::endl;
			bufferWrite = "Error_RDMV\r\n";
		}
	}
	else if (boost::contains(bufferRead,"RPRV") && ServerIsReady)
	{
		try {bufferWrite = Command_RPRV(bufferRead);}
		catch(...){
			std::cout << "Command_RPRV exception: bad syntax"<< std::endl;
			std::cout << bufferRead << std::endl;
		}
	}
	else if (boost::contains(bufferRead,"ALSC") && ServerIsReady)
	{
		try {bufferWrite = Command_ALSC(bufferRead);}
		catch(...){
			std::cout << "Command_ALSC exception: bad syntax"<< std::endl;
			bufferWrite = "Error_ALSC\r\n";
		}
	}
	else if (boost::contains(bufferRead,"CHAL") && ServerIsReady)
	{
		try {bufferWrite = Command_CHAL(bufferRead);}
		catch(...){
			std::cout << "Command_CHAL exception: bad syntax"<< std::endl;
			bufferWrite = "Error_CHAL\r\n";
		}
	}
	else if (boost::contains(bufferRead,"SCAP") && ServerIsReady)
	{
		try {bufferWrite = Command_SCAP(bufferRead);}
		catch(...){
			std::cout << "Command_SCAP exception: bad syntax"<< std::endl;
			bufferWrite = "Error_SCAP\r\n";
		}
	}
	else if (boost::contains(bufferRead,"WADI") && ServerIsReady)
	{
		try {bufferWrite = Command_WADI(bufferRead);}
		catch(...){
			std::cout << "Command_WADI exception: bad syntax"<< std::endl;
			bufferWrite = "Error_WADI\r\n";
		}
	}
	else if (boost::contains(bufferRead,"PLRD") && ServerIsReady)
	{
		try {bufferWrite = Command_PLRD(bufferRead);}
		catch(...){
			std::cout << "Command_PLRD exception: bad syntax"<< std::endl;
			bufferWrite = "Error_PLRD\r\n";
		}
	}
	else if (boost::contains(bufferRead,"FOTP") && ServerIsReady)
	{
		try {bufferWrite = Command_FOTP(bufferRead);}
		catch(...){
			std::cout << "Command_FOTP exception: bad syntax"<< std::endl;
			bufferWrite = "Error_FOTP\r\n";
		}
	}

	else if (boost::contains(bufferRead,"PING"))
	{
		/* RBS 20/03/2018
		 * Used to test connection. If this fails, the display app
		 * will try reestablishing the communication.
		 */

		bufferWrite = "PING_PLC14_echo_reply\r\n";

		std::cout << ".";
		fflush(stdout);
	}

	/*7/6/2018 JAGM: This piece of code below implements JSON over the old stringbanging method. */

	else if (boost::contains(bufferRead,"GDIS") && ServerIsReady) /* This line to be removed, see ControlMachine18 for reference */
	{
	    rapidjson::Document DOC_in;
	    rapidjson::StringBuffer Answer_JSON;
	    rapidjson::Writer<rapidjson::StringBuffer> writer(Answer_JSON);
	    rapidjson::ParseResult result;

	    result = DOC_in.Parse(bufferRead.c_str());
	    if(!result)
	    {
	    	 std::cout << "Message is not JSON or is corrupt: " << std::endl;
	    	 return "ERROR_MESSAGECORRUPT\n";
	    }

	    // 2. Modify it by DOM.
	    std::string command_ID;
	    bool command_ID_check = FindAndAddTo(DOC_in, "command_ID", &command_ID);
	    if(command_ID_check)
	    //try //Yup, it's dirty. But we don't want to risk that the machine stops working by a corrupt packet
	    {
	        if(boost::equals(command_ID, "GDIS")) Command_GDIS(DOC_in, &writer);
	        else std::cout << "Unknown command: " << command_ID << std::endl;
	    }
	    //catch( ... )
	    else
	    {
	    	return "command_ID not found\n";
	    	std::cout << command_ID << " Bad Syntax" << std::endl;
	    }
	    // 3. Stringify the DOM output

	    return ((std::string)Answer_JSON.GetString()) + "\n";
	}

	else
	{
		bufferWrite = "Error_unknown_CMD\r\n";
	}

	return bufferWrite;
}



