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
#include "TCPServerLibrary.h"

//RBS TODO:these functions deserve their own file.
bool FindAndAddTo(const rapidjson::Document& DOC, std::string Key, short* Variable)
{
	static rapidjson::Value::ConstMemberIterator itr;
	itr = DOC.FindMember(Key.c_str());
	if(itr !=DOC.MemberEnd()) {
		*Variable=short(itr->value.GetInt());
		return true;
	}
	else return false;
}

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

bool FindAndAddTo(const rapidjson::Document& DOC, std::string Key, long* Variable)
{
	static rapidjson::Value::ConstMemberIterator itr;
	itr = DOC.FindMember(Key.c_str());
	if(itr !=DOC.MemberEnd()) {
		*Variable=long(itr->value.GetInt());
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


/*
 * PLC Get Status Information
 */
void Command_PGSI(const rapidjson::Document& DOC_in, rapidjson::Writer<rapidjson::StringBuffer>* AnswerWriter)
{
	DOC_in.IsNull();
	AnswerWriter->StartObject();
	AnswerWriter->Key("command_ID");
	AnswerWriter->String("PGSI");

	int selectedArm=1;
	FindAndAddTo(DOC_in, "selectedArm", &selectedArm);
	RoboticArm* mArm = DesiredRoboticArm(selectedArm);

	//Arm specific variables (for the selected Manipulator)
	AnswerWriter->Key("selectedArm");
	AnswerWriter->Int(selectedArm);
	AnswerWriter->Key("hasDischarged");
	AnswerWriter->Bool(mArm->HasDischarged);
	AnswerWriter->Key("photosensorOfManipulator");
	AnswerWriter->Bool(mArm->PhotosensorOfManipulator);
	AnswerWriter->Key("manipulatorStatePosition");
	AnswerWriter->Bool(mArm->ManipulatorStatePosition);
	AnswerWriter->Key("dischargedTheBrickConfirm");
	AnswerWriter->Bool(mArm->DischargedTheBrickConfirm);
	AnswerWriter->Key("leftStorageBinSecurity");
	AnswerWriter->Bool(mArm->LeftStorageBinSecurity);
	AnswerWriter->Key("rightStorageBinSecurity");
	AnswerWriter->Bool(mArm->RightStorageBinSecurity);
	AnswerWriter->Key("AlarmArray");
	AnswerWriter->Int(abs(mArm->AlarmArray));
	AnswerWriter->Key("ManipulatorRepositionState");
	AnswerWriter->Int(mArm->ManipulatorRepositionState);
	AnswerWriter->Key("ActualValueEncoder");
	AnswerWriter->Int(abs(mArm->ActualValueEncoder));

	//Common Variables
	AnswerWriter->Key("theQueueOfPhotosensor_1");
	AnswerWriter->Bool(mArm->TheQueueOfPhotosensor_1);
	AnswerWriter->Key("theQueueOfPhotosensor_2");
	AnswerWriter->Bool(mArm->TheQueueOfPhotosensor_2);
	AnswerWriter->Key("theQueueOfPhotosensor_3");
	AnswerWriter->Bool(mArm->TheQueueOfPhotosensor_3);
	AnswerWriter->Key("theQueueOfPhotosensor_4");
	AnswerWriter->Bool(mArm->TheQueueOfPhotosensor_4);
	AnswerWriter->Key("stationInterlock_16");
	AnswerWriter->Bool(mArm->StationInterlock_16);
	AnswerWriter->Key("whetherOrNotPutTheTileTo_16");
	AnswerWriter->Bool(mArm->WhetherOrNotPutTheTileTo_16);
	AnswerWriter->Key("equipmentAlarmArray");
	AnswerWriter->Int(abs(mArm->EquipmentAlarmArray));
	AnswerWriter->Key("tileGrade");
	AnswerWriter->Int(mArm->TileGrade);
	AnswerWriter->Key("changeColor");
	AnswerWriter->Int(mArm->ChangeColor);
	AnswerWriter->Key("systemState");
	AnswerWriter->Int(mArm->SystemState);
	AnswerWriter->Key("actualValueOfTheLineEncoder");
	AnswerWriter->Int(mArm->ActualValueOfTheLineEncoder);
	AnswerWriter->Key("enterTheTileStartingCodeValue");
	AnswerWriter->Int(mArm->EnterTheTileStartingCodeValue);
}


/*
 * RBS; PLC Write DAta
 */
void Command_PWDA(const rapidjson::Document& DOC_in, rapidjson::Writer<rapidjson::StringBuffer>* AnswerWriter)
{
	int selectedArm=0;
	FindAndAddTo(DOC_in, "selectedArm", &selectedArm);
	RoboticArm* mArm = DesiredRoboticArm(selectedArm);

	//These names were poorly translated from chinese. It is a nightmare, but not our fault
	FindAndAddTo(DOC_in, "StorageBinDirection", &mArm->StorageBinDirection);
	FindAndAddTo(DOC_in, "ManipulatorReset", &mArm->ManipulatorReset);
	FindAndAddTo(DOC_in, "StorageBinFullA", &mArm->StorageBinFullA);
	FindAndAddTo(DOC_in, "StorageBinFullB", &mArm->StorageBinFullB);
	FindAndAddTo(DOC_in, "BarCodeReadStateA", &mArm->BarCodeReadStateA);
	FindAndAddTo(DOC_in, "BarCodeReadStateB", &mArm->BarCodeReadStateB);
	FindAndAddTo(DOC_in, "ManipulatorMode", &mArm->ManipulatorMode);
	FindAndAddTo(DOC_in, "VacuumValve", &mArm->VacuumValve);
	FindAndAddTo(DOC_in, "ManualForwardBackward", &mArm->ManualForwardBackward);
	FindAndAddTo(DOC_in, "ManualLeftRight", &mArm->ManualLeftRight);
	FindAndAddTo(DOC_in, "ManualUpDown", &mArm->ManualUpDown);
	FindAndAddTo(DOC_in, "CatchOrDrop", &mArm->CatchOrDrop);
	FindAndAddTo(DOC_in, "WhatToDoWithTheBrick", &mArm->WhatToDoWithTheBrick);
	FindAndAddTo(DOC_in, "PulseZAxis", &mArm->PulseZAxis);
	FindAndAddTo(DOC_in, "ValueOfCatchDrop", &mArm->ValueOfCatchDrop);
	FindAndAddTo(DOC_in, "CommunicationExchange", &mArm->CommunicationExchange);
	FindAndAddTo(DOC_in, "TestPattern", &mArm->TestPattern);
	FindAndAddTo(DOC_in, "TransmissionManualDebugging", &mArm->TransmissionManualDebugging);
	FindAndAddTo(DOC_in, "PCState", &mArm->PCState);
	FindAndAddTo(DOC_in, "Z_AxisDeceletationDistance", &mArm->Z_AxisDeceletationDistance);
	FindAndAddTo(DOC_in, "ThePulseOfX_AxisGoBackToTheWaitingPositionInAdvance", &mArm->ThePulseOfX_AxisGoBackToTheWaitingPositionInAdvance);
	FindAndAddTo(DOC_in, "ThePulseOfZ_AxisAdvanceDownInAdvance", &mArm->ThePulseOfZ_AxisAdvanceDownInAdvance);

	DOC_in.IsNull();
	AnswerWriter->StartObject();
	AnswerWriter->Key("command_ID");
	AnswerWriter->String("PWDA");
	AnswerWriter->Key("manualForwardBackwardFeedback");
	AnswerWriter->Int(mArm->ManualForwardBackward);
	AnswerWriter->Key("manualLeftRightFeedback");
	AnswerWriter->Int(mArm->ManualLeftRight);
	AnswerWriter->Key("manualUpDownFeedback");
	AnswerWriter->Int(mArm->ManualUpDown);
	AnswerWriter->EndObject();
}

/*
 * Rfid Get Memory Values
 */
void Command_RGMV(const rapidjson::Document& DOC_in, rapidjson::Writer<rapidjson::StringBuffer>* AnswerWriter)
{
	int selectedPallet=1;
	FindAndAddTo(DOC_in, "palletNumber", &selectedPallet);

	DOC_in.IsNull();
	AnswerWriter->StartObject();
	AnswerWriter->Key("command_ID");
	AnswerWriter->String("RGMV");
	AnswerWriter->Key("palletNumber");
	AnswerWriter->Int(selectedPallet);
	AnswerWriter->Key("totalBricks");
	AnswerWriter->Int(StorageGetNumberOfBricks(selectedPallet));
	AnswerWriter->Key("memoryValues");
	AnswerWriter->StartArray();
	for(int i=0; i<StorageGetNumberOfBricks(selectedPallet); i++)
	{
		AnswerWriter->StartObject();
		AnswerWriter->Key("memoryValue");
		AnswerWriter->Int(StorageGetRaw(selectedPallet, i));
		AnswerWriter->EndObject();
	}
	AnswerWriter->EndArray();
	AnswerWriter->EndObject();
}

/*
 * Rfid Format Memory Values
 */
void Command_RFMV(const rapidjson::Document& DOC_in, rapidjson::Writer<rapidjson::StringBuffer>* AnswerWriter)
{
	int selectedPallet=1;
	FindAndAddTo(DOC_in, "selectedPallet", &selectedPallet);

	StorageFormatMemory(selectedPallet);

	DOC_in.IsNull();
	AnswerWriter->StartObject();
	AnswerWriter->Key("command_ID");
	AnswerWriter->String("RFMV");
	AnswerWriter->Key("reply");
	AnswerWriter->String("OK");
	AnswerWriter->EndObject();
}

/*
 * Rfid Add Memory Value
 */
void Command_RAMV(const rapidjson::Document& DOC_in, rapidjson::Writer<rapidjson::StringBuffer>* AnswerWriter)
{
	int selectedPallet=1, valueToAdd=0;
	FindAndAddTo(DOC_in, "selectedPallet", &selectedPallet);
	FindAndAddTo(DOC_in, "valueToAdd", &valueToAdd);

	StorageAddBrick(selectedPallet, valueToAdd >> 4, valueToAdd & 15);

	DOC_in.IsNull();
	AnswerWriter->StartObject();
	AnswerWriter->Key("command_ID");
	AnswerWriter->String("RAMV");
	AnswerWriter->Key("reply");
	AnswerWriter->String("OK");
	AnswerWriter->EndObject();
}

/*
 * Rfid Delete Memory Value
 */
void Command_RDMV(const rapidjson::Document& DOC_in, rapidjson::Writer<rapidjson::StringBuffer>* AnswerWriter)
{
	int selectedPallet=1, positionToDelete=0;
	FindAndAddTo(DOC_in, "selectedPallet", &selectedPallet);
	FindAndAddTo(DOC_in, "positionToDelete", &positionToDelete);

	StorageDeleteBrick(selectedPallet, positionToDelete);

	DOC_in.IsNull();
	AnswerWriter->StartObject();
	AnswerWriter->Key("command_ID");
	AnswerWriter->String("RDMV");
	AnswerWriter->Key("reply");
	AnswerWriter->String("OK");
	AnswerWriter->EndObject();
}

void Command_RGUV(const rapidjson::Document& DOC_in, rapidjson::Writer<rapidjson::StringBuffer>* AnswerWriter)
{
	//not implemented (?)
}

/*
 * Rifd Periodic Request Values RBS: Actually, all line info ( ⚆ _ ⚆ )
 */
void Command_RPRV(const rapidjson::Document& DOC_in, rapidjson::Writer<rapidjson::StringBuffer>* AnswerWriter)
{
	DOC_in.IsNull();
	AnswerWriter->StartObject();

	//Get the number of pallets from the command. //TODO RBS this should be detected automatically.
	int NumberOfPallets;
	FindAndAddTo(DOC_in, "numberOfPallets", &NumberOfPallets);

	//Write Pallet Information
	AnswerWriter->Key("palletInformation");
	AnswerWriter->StartArray();
	for(int i=0; i<NumberOfPallets; i++)
	{
			AnswerWriter->StartObject();
			AnswerWriter->Key("palletUID");
			AnswerWriter->String(StorageGetStoredUID(i).c_str());
			AnswerWriter->Key("numberOfBricks");
			AnswerWriter->Int(StorageGetNumberOfBricks(i));
			AnswerWriter->Key("topBrick");
			AnswerWriter->Int(StorageGetRaw(i, StorageGetNumberOfBricks(i)));
			AnswerWriter->EndObject();
	}
	AnswerWriter->EndArray();

	//Bricks on the line
	std::deque<Brick> mListOfBricksOnTheLine = Algorithm::Get::Bricks_On_The_Line();
	std::vector<int> brickIndexes = Algorithm::Get::IndexesOfBricksOnLine(mListOfBricksOnTheLine);
	AnswerWriter->Key("bricksOnTheLine");
	AnswerWriter->StartArray();
	for(unsigned int i=0; i<mListOfBricksOnTheLine.size(); i++)
	{
		if(mListOfBricksOnTheLine.at(brickIndexes.at(i)).Type!=0)
		{
			AnswerWriter->StartObject();
			AnswerWriter->Key("position");
			AnswerWriter->Int(mListOfBricksOnTheLine.at(brickIndexes.at(i)).Position);
			AnswerWriter->Key("type");
			AnswerWriter->Int(mListOfBricksOnTheLine.at(brickIndexes.at(i)).Type);
			AnswerWriter->Key("assignedPallet");
			AnswerWriter->Int(mListOfBricksOnTheLine.at(brickIndexes.at(i)).AssignedPallet);
			AnswerWriter->Key("DNI");
			AnswerWriter->Int(mListOfBricksOnTheLine.at(brickIndexes.at(i)).DNI);
			AnswerWriter->EndObject();
		}
	}
	AnswerWriter->EndArray();

	//Bricks that have been taken already
	std::vector<Brick> mListOfBricksTakenByManipulators = Algorithm::Get::Manipulator_TakenBrick();
	AnswerWriter->Key("bricksTakenByManipulators");
	AnswerWriter->StartArray();
	for(unsigned int i=0; i<mListOfBricksTakenByManipulators.size(); i++)
	{
		AnswerWriter->StartObject();
		AnswerWriter->Key("curentXEncoderValue");
		AnswerWriter->Int(DesiredRoboticArm(i+1)->ActualValueEncoder);
		AnswerWriter->Key("valueOfCatchDrop");
		AnswerWriter->Int(DesiredRoboticArm(i+1)->ValueOfCatchDrop);
		AnswerWriter->Key("position");
		AnswerWriter->Int(mListOfBricksTakenByManipulators.at(i).Position);
		AnswerWriter->Key("type");
		AnswerWriter->Int(mListOfBricksTakenByManipulators.at(i).Type);
		AnswerWriter->Key("assignedPallet");
		AnswerWriter->Int(mListOfBricksTakenByManipulators.at(i).AssignedPallet);
		AnswerWriter->Key("DNI");
		AnswerWriter->Int(mListOfBricksTakenByManipulators.at(i).DNI);
		AnswerWriter->EndObject();
	}
	AnswerWriter->EndArray();

	AnswerWriter->Key("currentEncoderValue");
	AnswerWriter->Int(RoboticArm::ActualValueOfTheLineEncoder);
	AnswerWriter->EndObject();
}

/*
 * ALgorithm Save Config
 */
void Command_ALSC(const rapidjson::Document& DOC_in, rapidjson::Writer<rapidjson::StringBuffer>* AnswerWriter)
{
	ConfigParser config("/etc/unit14/unit14.conf");
	std::vector<int> modes;
	int color=0, grade=0; //ConfigParser.getManipulatorNumber();
	DOC_in.IsNull();

	FindAndAddTo(DOC_in, "color", &color);
	FindAndAddTo(DOC_in, "grade", &grade);

	//Update both, running program and config file
	config.SetCurrentPackagingColor(boost::lexical_cast<int>(color));
	Algorithm::Set::CurrentPackagingColor(boost::lexical_cast<int>(color));
	config.SetCurrentPackagingGrade(boost::lexical_cast<int>(grade));
	Algorithm::Set::CurrentPackagingGrade(boost::lexical_cast<int>(grade));

	//for(Value::ConstValueIterator itr = modes.Begin; itr != modes.End();; ++itr)
	//	modes.pushBack(itr.GetInt());
	config.SetManipulatorModes(modes);
	Algorithm::Set::ManipulatorModes(modes);


	DOC_in.IsNull();
	AnswerWriter->StartObject();
	AnswerWriter->Key("command_ID");
	AnswerWriter->String("ALSC");
	AnswerWriter->Key("reply");
	AnswerWriter->String("OK");
	AnswerWriter->EndObject();
}

/*
 * ALgorithm Get Config
 */
void Command_ALGC(const rapidjson::Document& DOC_in, rapidjson::Writer<rapidjson::StringBuffer>* AnswerWriter)
{

	ConfigParser config("/etc/unit14/unit14.conf");
	std::vector<int> modes;
	int Manipulators = 5; //ConfigParser.getManipulatorNumber();

	DOC_in.IsNull();
	AnswerWriter->StartObject();
	AnswerWriter->Key("command_ID");
	AnswerWriter->String("ALGC");
	AnswerWriter->Key("currentGrade");
	AnswerWriter->Int(config.GetPackagingGrade());
	AnswerWriter->Key("currentColor");
	AnswerWriter->Int(config.GetPackagingColor());
	AnswerWriter->Key("manipulatorModes");
	AnswerWriter->StartArray();
	for(int i=0; i<Manipulators; i++)
	{
		AnswerWriter->StartObject();
		//AnswerWriter->Key(""); //TODO: Insert modes and other relevant information
		//AnswerWriter->String();
		AnswerWriter->EndObject();
	}
	AnswerWriter->EndArray();

	AnswerWriter->EndObject();
}

/*
 * Check for new alarms
 */
void Command_CHAL(const rapidjson::Document& DOC_in, rapidjson::Writer<rapidjson::StringBuffer>* AnswerWriter)
{
	//Get number of arms from ConfigParser config("/etc/unit14/unit14.conf");
	int Manipulators = 5; //ConfigParser.getManipulatorNumber();

	DOC_in.IsNull();
	AnswerWriter->StartObject();
	AnswerWriter->Key("command_ID");
	AnswerWriter->String("CHAL");
	AnswerWriter->Key("equipmentAlarms");
	AnswerWriter->Int(abs(RoboticArm::EquipmentAlarmArray));
	AnswerWriter->Key("manipulatorAlarms");
	AnswerWriter->StartArray();
	for(int i=0; i<Manipulators; i++)
	{
		AnswerWriter->StartObject();
		AnswerWriter->Key("alarmArray");
		AnswerWriter->Int(abs(DesiredRoboticArm(i+1)->AlarmArray));
		AnswerWriter->EndObject();
	}
	AnswerWriter->EndArray();

	AnswerWriter->EndObject();
}

//Save CAlibration Parameters
void Command_SCAP(const rapidjson::Document& DOC_in, rapidjson::Writer<rapidjson::StringBuffer>* AnswerWriter)
{
	int NumberOfArms=0;
	FindAndAddTo(DOC_in, "armNumber", &NumberOfArms);

	ConfigParser config("/etc/unit14/unit14.conf");
	//Implement: save arms to config now. THIS is where the number is stored.

	std::vector<int> ArmPositions;
	for(int i=0;i<NumberOfArms;i++)
	{
		ArmPositions.push_back(DOC_in["positions"][i].GetInt());
	}
	Algorithm::Set::ManipulatorFixedPosition(ArmPositions);
	config.SetArmPositions(ArmPositions);

	DOC_in.IsNull();
	AnswerWriter->StartObject();
	AnswerWriter->Key("command_ID");
	AnswerWriter->String("SCAP");
	AnswerWriter->Key("reply");
	AnswerWriter->String("OK");
	AnswerWriter->EndObject();
}

/*
 * RBS: Write Advanced Debug Information
 */
void Command_WADI(const rapidjson::Document& DOC_in, rapidjson::Writer<rapidjson::StringBuffer>* AnswerWriter)
{
	DOC_in.IsNull();
	bool enable16, forceOutput, forceInput;

	FindAndAddTo(DOC_in, "enable16", &enable16);
	FindAndAddTo(DOC_in, "forceOutput", &forceOutput);
	FindAndAddTo(DOC_in, "forceInput", &forceInput);

	if(enable16)
	{
		//WhetherOrNotPutTheTileTo is enabled
		Algorithm::Set::enable_WhetherOrNotPutTheTileTo_16(true);

	}
	else
	{
		//WhetherOrNotPutTheTileTo is disabled
		Algorithm::Set::enable_WhetherOrNotPutTheTileTo_16(false);

		//wheterornot16 disabled, force output
		if(forceOutput)
			Algorithm::Set::force_output(true);
		else
			Algorithm::Set::force_output(false);

		//wheterornot16 disabled, force input
		if(forceInput)
			Algorithm::Set::force_input(true);
		else
			Algorithm::Set::force_input(false);
	}

	//Reply
	DOC_in.IsNull();
	AnswerWriter->StartObject();
	AnswerWriter->Key("command_ID");
	AnswerWriter->String("WADI");
	AnswerWriter->Key("reply");
	AnswerWriter->String("OK");
	AnswerWriter->EndObject();
}

/*
 * RBS PLace ORder
 */
void Command_PLRD(const rapidjson::Document& DOC_in, rapidjson::Writer<rapidjson::StringBuffer>* AnswerWriter)
{
	DOC_in.IsNull();
	int type, position, pallet;

	FindAndAddTo(DOC_in, "type", &type);
	FindAndAddTo(DOC_in, "position", &position);
	FindAndAddTo(DOC_in, "pallet", &pallet);

	Brick brick(type,position,pallet,0); //DNI is meaningless here
	Algorithm::Set::order(brick);

	//Reply
	DOC_in.IsNull();
	AnswerWriter->StartObject();
	AnswerWriter->Key("command_ID");
	AnswerWriter->String("FOTP");
	AnswerWriter->Key("reply");
	AnswerWriter->String("OK");
	AnswerWriter->EndObject();
}

/*
 * RBS FOrce To Pallet...
 */
void Command_FOTP(const rapidjson::Document& DOC_in, rapidjson::Writer<rapidjson::StringBuffer>* AnswerWriter)
{
	DOC_in.IsNull();

	int destination_pallet = 0;
	FindAndAddTo(DOC_in, "ForceTo", &destination_pallet);
	Algorithm::Set::forced_pallet(destination_pallet);
	std::cout << "Brick forced to pallet " << destination_pallet << std::endl;

	//Reply
	DOC_in.IsNull();
	AnswerWriter->StartObject();
	AnswerWriter->Key("command_ID");
	AnswerWriter->String("FOTP");
	AnswerWriter->Key("reply");
	AnswerWriter->String("OK");
	AnswerWriter->EndObject();
}

/*
 * Get Debug Internal State
 */
void Command_GDIS(const rapidjson::Document& DOC_in, rapidjson::Writer<rapidjson::StringBuffer>* AnswerWriter){
	DOC_in.IsNull();

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

}

void Command_PING(const rapidjson::Document& DOC_in, rapidjson::Writer<rapidjson::StringBuffer>* AnswerWriter)
{
	DOC_in.IsNull();
	AnswerWriter->StartObject();

	AnswerWriter->Key("command_ID");
	AnswerWriter->String("PING");

	AnswerWriter->Key("reply");
	AnswerWriter->String("OK");

	AnswerWriter->EndObject();
}

std::string ProcessCommand(std::string Message)
{
	std::cout << "Received: " << Message << std::endl;
	//one document for input
    rapidjson::Document DOC_in;
    rapidjson::StringBuffer Answer_JSON;
    rapidjson::Writer<rapidjson::StringBuffer> writer(Answer_JSON);
    rapidjson::ParseResult result;
    result = DOC_in.Parse(Message.c_str());
    if(!result)
    {
    	 std::cout << "Message is not JSON or is corrupt: " << std::endl;
    	 return "ERROR_MESSAGECORRUPT\n";
    }

    // 2. Modify it by DOM.
    std::string command_ID;
    bool command_ID_check = FindAndAddTo(DOC_in, "command_ID", &command_ID);

    if(command_ID_check)
    {
        	 if(boost::equals(command_ID, "PWDA")) Command_PWDA(DOC_in, &writer);
        else if(boost::equals(command_ID, "PGSI")) Command_PGSI(DOC_in, &writer);
        else if(boost::equals(command_ID, "RGMV")) Command_RGMV(DOC_in, &writer);
        else if(boost::equals(command_ID, "RFMV")) Command_RFMV(DOC_in, &writer);
        else if(boost::equals(command_ID, "RAMV")) Command_RAMV(DOC_in, &writer);
        else if(boost::equals(command_ID, "RDMV")) Command_RDMV(DOC_in, &writer);
        else if(boost::equals(command_ID, "RPRV")) Command_RPRV(DOC_in, &writer);
        else if(boost::equals(command_ID, "ALSC")) Command_ALSC(DOC_in, &writer);
        else if(boost::equals(command_ID, "CHAL")) Command_CHAL(DOC_in, &writer);
        else if(boost::equals(command_ID, "SCAP")) Command_SCAP(DOC_in, &writer);
        else if(boost::equals(command_ID, "WADI")) Command_WADI(DOC_in, &writer);
        else if(boost::equals(command_ID, "PLRD")) Command_PLRD(DOC_in, &writer);
        else if(boost::equals(command_ID, "FOTP")) Command_FOTP(DOC_in, &writer);
        else if(boost::equals(command_ID, "GDIS")) Command_GDIS(DOC_in, &writer);
        else if(boost::equals(command_ID, "PING")) Command_PING(DOC_in, &writer);
        else std::cout << "Unknown command: " << command_ID << std::endl;
    }
    else
    {
    	std::cout << command_ID << " Bad Syntax" << std::endl;
    	return "command_ID not found\n";
    }

    // 3. Stringify the DOM output
    return ((std::string)Answer_JSON.GetString()) + "\n";
}

void InitDisplayParser()
{
	SetFunctionToProcessMessages(&ProcessCommand);
}



