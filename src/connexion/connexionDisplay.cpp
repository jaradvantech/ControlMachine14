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
#include "JSON_FindAndAdd.h"

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
	RoboticArm* mArm = getArm(selectedArm);

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
	AnswerWriter->Key("alarmArray");
	AnswerWriter->Int(abs(mArm->AlarmArray));
	AnswerWriter->Key("manipulatorRepositionState");
	AnswerWriter->Int(mArm->ManipulatorRepositionState);
	AnswerWriter->Key("actualValueEncoder");
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
	AnswerWriter->EndObject();
}


/*
 * RBS; PLC Write DAtaposition
 */
void Command_PWDA(const rapidjson::Document& DOC_in, rapidjson::Writer<rapidjson::StringBuffer>* AnswerWriter)
{
	int selectedArm=1; //RBS default 1, so we can start and stop the motor without sending this parameter.
	FindAndAddTo(DOC_in, "selectedArm", &selectedArm);
	RoboticArm* mArm = getArm(selectedArm);

	//These names were poorly translated from chinese. It is a nightmare, but not our fault
	FindAndAddTo(DOC_in, "SBD", &mArm->StorageBinDirection);
	FindAndAddTo(DOC_in, "MR", &mArm->ManipulatorReset);
	FindAndAddTo(DOC_in, "SBFA", &mArm->StorageBinFullA);
	FindAndAddTo(DOC_in, "SBFB", &mArm->StorageBinFullB);
	FindAndAddTo(DOC_in, "BCRSA", &mArm->BarCodeReadStateA);
	FindAndAddTo(DOC_in, "BCRSB", &mArm->BarCodeReadStateB);
	FindAndAddTo(DOC_in, "MM", &mArm->ManipulatorMode);
	FindAndAddTo(DOC_in, "VV", &mArm->VacuumValve);
	FindAndAddTo(DOC_in, "MFB", &mArm->ManualForwardBackward);
	FindAndAddTo(DOC_in, "MLR", &mArm->ManualLeftRight);
	FindAndAddTo(DOC_in, "MUD", &mArm->ManualUpDown);
	FindAndAddTo(DOC_in, "COD", &mArm->CatchOrDrop);
	FindAndAddTo(DOC_in, "WTDWT", &mArm->WhatToDoWithTheBrick);
	FindAndAddTo(DOC_in, "PZA", &mArm->PulseZAxis);
	FindAndAddTo(DOC_in, "VOCD", &mArm->ValueOfCatchDrop);
	FindAndAddTo(DOC_in, "CE", &mArm->CommunicationExchange);
	FindAndAddTo(DOC_in, "ITT", &mArm->TestPattern);
	FindAndAddTo(DOC_in, "TMD", &mArm->TransmissionManualDebugging);
	FindAndAddTo(DOC_in, "PCS", &mArm->PCState);
	FindAndAddTo(DOC_in, "ADD", &mArm->Z_AxisDeceletationDistance);
	FindAndAddTo(DOC_in, "ZASV", &mArm->Z_AxisStandbyValue);
	FindAndAddTo(DOC_in, "TPOX_AGB", &mArm->ThePulseOfX_AxisGoBackToTheWaitingPositionInAdvance);
	FindAndAddTo(DOC_in, "TPOX_AAD", &mArm->ThePulseOfZ_AxisAdvanceDownInAdvance);

	DOC_in.IsNull();
	AnswerWriter->StartObject();
	AnswerWriter->Key("command_ID");
	AnswerWriter->String("PWDA");
	AnswerWriter->Key("MFBFeedback");
	AnswerWriter->Int(mArm->ManualForwardBackward);
	AnswerWriter->Key("MLRFeedback");
	AnswerWriter->Int(mArm->ManualLeftRight);
	AnswerWriter->Key("MUDFeedback");
	AnswerWriter->Int(mArm->ManualUpDown);
	AnswerWriter->Key("encoderFeedback");
	AnswerWriter->Int(mArm->ActualValueOfTheLineEncoder);
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
		AnswerWriter->Int(StorageGetRaw(selectedPallet, i+1)); //RBS +1 because 0 represents just the number of bricks
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
	int selectedPallet=1, valueToAdd=0, position=0, color=0, grade=0;
	FindAndAddTo(DOC_in, "selectedPallet", &selectedPallet);
	FindAndAddTo(DOC_in, "color", &color);
	FindAndAddTo(DOC_in, "grade", &grade);

	FindAndAddTo(DOC_in, "position", &position);

	//StorageAddBrick(selectedPallet, grade, color);
	StorageInsertBrick(selectedPallet, position, grade, color);

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

void Command_REMV(const rapidjson::Document& DOC_in, rapidjson::Writer<rapidjson::StringBuffer>* AnswerWriter)
{
	int selectedPallet=0, positionToEdit = 1, newGrade = 1, newColor = 1;
	FindAndAddTo(DOC_in, "palletNumber", &selectedPallet);
	FindAndAddTo(DOC_in, "positionToEdit", &positionToEdit);
	FindAndAddTo(DOC_in, "newGrade", &newGrade);
	FindAndAddTo(DOC_in, "newColor", &newColor);

	StorageEditBrick(selectedPallet, positionToEdit, newGrade, newColor);

	DOC_in.IsNull();
	AnswerWriter->StartObject();
	AnswerWriter->Key("command_ID");
	AnswerWriter->String("REMV");
	AnswerWriter->Key("reply");
	AnswerWriter->String("OK");
	AnswerWriter->EndObject();
}

/*
 * Rifd Periodic Request Values RBS: Actually, all line info ( ⚆ _ ⚆ )
 */
void Command_RPRV(const rapidjson::Document& DOC_in, rapidjson::Writer<rapidjson::StringBuffer>* AnswerWriter)
{
	int NumberOfPallets = 2 * getTotalArms();

	DOC_in.IsNull();
	AnswerWriter->StartObject();

	//Write Pallet Information
	AnswerWriter->Key("command_ID");
	AnswerWriter->String("RPRV");
	AnswerWriter->Key("palletInformation");
	AnswerWriter->StartArray();
	for(int i=1; i<=NumberOfPallets; i++)
	{
			AnswerWriter->StartObject();
			AnswerWriter->Key("palletUID");
			AnswerWriter->String(StorageGetStoredUID(i).c_str());
			AnswerWriter->Key("numberOfBricks");
			AnswerWriter->Int(StorageGetNumberOfBricks(i));
			AnswerWriter->Key("topBrick");
			if(StorageGetNumberOfBricks(i) > 0)
				AnswerWriter->Int(StorageGetRaw(i, StorageGetNumberOfBricks(i)));
			else
				AnswerWriter->Int(-1);
			AnswerWriter->EndObject();
	}
	AnswerWriter->EndArray();

	//Bricks on the line
	std::deque<Brick> mListOfBricksOnTheLine = Algorithm::Get::Bricks_On_The_Line();
	std::vector<int> brickIndexes = Algorithm::Get::IndexesOfBricksOnLine(mListOfBricksOnTheLine);
	AnswerWriter->Key("bricksOnTheLine");
	AnswerWriter->StartArray();
	//RBS This list only contains non-type0 bricks.
	for(unsigned int i=0; i<brickIndexes.size(); i++)
	{
		if(mListOfBricksOnTheLine.at(brickIndexes.at(i)).DNI) //to make sure never happens
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
		else
		{
			std::cout << "INTERNAL ERRROR, DNI 0 assigned to something" << std::endl;
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
			AnswerWriter->Key("currentXEncoderValue");
			AnswerWriter->Int(getArm(i+1)->ActualValueEncoder);
			AnswerWriter->Key("valueOfCatchDrop");
			AnswerWriter->Int(getArm(i+1)->ValueOfCatchDrop);
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
	ConfigParser config(CONFIG_FILE);
	std::vector<int> modes;
	int color=1, grade=1; //ConfigParser.getManipulatorNumber();
	DOC_in.IsNull();

	FindAndAddTo(DOC_in, "color", &color);
	FindAndAddTo(DOC_in, "grade", &grade);

	//Update both, running program and config file
	config.SetCurrentPackagingColor(boost::lexical_cast<int>(color));
	Algorithm::Set::CurrentPackagingColor(boost::lexical_cast<int>(color));
	config.SetCurrentPackagingGrade(boost::lexical_cast<int>(grade));
	Algorithm::Set::CurrentPackagingGrade(boost::lexical_cast<int>(grade));

	for(unsigned int i=0; i<DOC_in["modes"].Size(); i++)
		modes.push_back(DOC_in["modes"][i].GetInt()); //RBS TODO this may crash the entire machine if a bad JSON is received
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

	ConfigParser config(CONFIG_FILE);
	std::vector<int> modes = config.GetManipulatorModes();
	int Manipulators = getTotalArms();

	DOC_in.IsNull();
	AnswerWriter->StartObject();
	AnswerWriter->Key("command_ID");
	AnswerWriter->String("ALGC");
	AnswerWriter->Key("currentGrade");
	AnswerWriter->Int(config.GetPackagingGrade());
	AnswerWriter->Key("currentColor");
	AnswerWriter->Int(config.GetPackagingColor());
	AnswerWriter->Key("totalArms");
	AnswerWriter->Int(getTotalArms());
	AnswerWriter->Key("manipulatorModes");
	AnswerWriter->StartArray();
	for(int i=0; i<Manipulators; i++)
	{
		AnswerWriter->Int(modes.at(i));
	}
	AnswerWriter->EndArray();

	AnswerWriter->EndObject();
}

/*
 * Check for new alarms
 */
void Command_CHAL(const rapidjson::Document& DOC_in, rapidjson::Writer<rapidjson::StringBuffer>* AnswerWriter)
{
	int Manipulators = getTotalArms();

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
		AnswerWriter->Int(abs(getArm(i+1)->AlarmArray));
	}
	AnswerWriter->EndArray();

	AnswerWriter->EndObject();
}

//Save CAlibration Parameters
void Command_SCAP(const rapidjson::Document& DOC_in, rapidjson::Writer<rapidjson::StringBuffer>* AnswerWriter)
{
	DOC_in.IsNull();
	int SBV=0, AGBTTWPIA, NumberOfArms=0;
	FindAndAddTo(DOC_in, "armNumber", &NumberOfArms);
	FindAndAddTo(DOC_in, "SBV", &SBV);
	FindAndAddTo(DOC_in, "AGBTTWPIA", &AGBTTWPIA);

	ConfigParser config(CONFIG_FILE);
	//Implement: save arms to config now. THIS is where the number is stored.

	std::vector<int> ArmPositions;
	for(int i=0;i<NumberOfArms;i++)
	{
		ArmPositions.push_back(DOC_in["positions"][i].GetInt());
	}
	config.SetArmPositions(ArmPositions);

	std::vector<int> MechanicalParameters;
	MechanicalParameters.push_back(AGBTTWPIA);
	MechanicalParameters.push_back(SBV);
	config.SetMechanicalParameters(MechanicalParameters);

	//STOP PROGRAM AFTER SAVING THIS CRITICAL CONFIGURATION
	//Resources not released, but the entire machine will be restarted after this.
	std::cout << "Configuration received from tablet, shutting server down." << std::endl;
	exit(0);
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
		std::cout << "Changing WhetherOrNotPutTheTileTo_16 to ENABLED" << std::endl;

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

		std::cout << "DISABLED WhetherOrNotPutTheTileTo_16: Force input: " << forceInput << ", Force Output: " << forceOutput << std::endl;
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

	std::cout << "New debug order: " << std::endl;
	std::cout << ">>>What: " << type << " (0=to pallet, 1=to line)"<< std::endl;
	std::cout << ">>>Where: " << pallet << " (Pallet number)"<< std::endl;
	std::cout << ">>>When: " << position << " (Encoder units)"<< std::endl;

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
	FindAndAddTo(DOC_in, "pallet", &destination_pallet);
	Algorithm::Set::forced_pallet(destination_pallet);
	std::cout << "Brick forced to pallet " << destination_pallet << std::endl;

	//Reply
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

	AnswerWriter->Key("Manipulator_State");
	AnswerWriter->StartArray();
	for(unsigned int i=0;i<Manipulator_Fixed_Position.size();i++){
		AnswerWriter->StartObject();
		AnswerWriter->Key("WhatToDoWithTheBrick");
		AnswerWriter->Int(getArm(i+1)->WhatToDoWithTheBrick);
		AnswerWriter->Key("CatchOrDrop");
		AnswerWriter->Int(getArm(i+1)->CatchOrDrop);
		AnswerWriter->Key("ValueOfCatchDrop");
		AnswerWriter->Int(getArm(i+1)->ValueOfCatchDrop);
		AnswerWriter->EndObject();
	}
	AnswerWriter->EndArray();
AnswerWriter->EndObject();
}

/*
 * RBS Get ConFiGuration
 */
void Command_GCFG(const rapidjson::Document& DOC_in, rapidjson::Writer<rapidjson::StringBuffer>* AnswerWriter)
{
	DOC_in.IsNull();

	ConfigParser config(CONFIG_FILE);
	std::vector<int> RFID_ports = config.GetServerPorts();
	std::vector<std::string> RFID_addresses = config.GetServerIPs();
	int totalRFIDservers = RFID_addresses.size();

	AnswerWriter->StartObject();
	AnswerWriter->Key("command_ID");
	AnswerWriter->String("GCFG");
	AnswerWriter->Key("RFID_servers");
	//AnswerWriter->Int(config.GetNumberOfArms());
	AnswerWriter->Int(3);
	AnswerWriter->Key("RFID_ports");
	AnswerWriter->StartArray();
	for(int i=0; i<totalRFIDservers; i++)
	{
		AnswerWriter->Int(RFID_ports.at(i));
	}
	AnswerWriter->EndArray();

	AnswerWriter->Key("RFID_addresses");
	AnswerWriter->StartArray();
	for(int i=0; i<totalRFIDservers; i++)
	{
		AnswerWriter->String(RFID_addresses.at(i).c_str());
	}
	AnswerWriter->EndArray();

	AnswerWriter->Key("PLC_address");
	AnswerWriter->String(config.GetPLCAddress().c_str());

	AnswerWriter->EndObject();
}

/*
 * RBS Set ConFiGuration
 */
void Command_SCFG(const rapidjson::Document& DOC_in, rapidjson::Writer<rapidjson::StringBuffer>* AnswerWriter)
{
	DOC_in.IsNull();
	std::vector<int> RFID_ports;
	std::vector<std::string> RFID_addresses;
	std::string PLC_address;
	ConfigParser config(CONFIG_FILE);

	FindAndAddTo(DOC_in, "PLC_address", &PLC_address);
	config.SetPLCAddress(PLC_address);

	for(unsigned int i=0; i<DOC_in["RFID_addresses"].Size(); i++)
		RFID_addresses.push_back(DOC_in["RFID_addresses"][i].GetString());
	config.SetServerIPs(RFID_addresses);

	for(unsigned int i=0; i<DOC_in["RFID_ports"].Size(); i++)
		RFID_ports.push_back(DOC_in["RFID_ports"][i].GetInt());
	config.SetServerPorts(RFID_ports);

	AnswerWriter->StartObject();
	AnswerWriter->Key("command_ID");
	AnswerWriter->String("SCFG");
	AnswerWriter->Key("reply");
	AnswerWriter->String("OK");
	AnswerWriter->EndObject();
}

/*
 * Query Database ReQuest
 */
void Command_QDRQ(const rapidjson::Document& DOC_in, rapidjson::Writer<rapidjson::StringBuffer>* AnswerWriter)
{
	DOC_in.IsNull();
	int fromYear, fromMonth, fromDay, toYear, toMonth, toDay;

	FindAndAddTo(DOC_in, "fromYear", &fromYear);
	FindAndAddTo(DOC_in, "fromMonth", &fromMonth);
	FindAndAddTo(DOC_in, "fromDay", &fromDay);
	FindAndAddTo(DOC_in, "toYear", &toYear);
	FindAndAddTo(DOC_in, "toMonth", &toMonth);
	FindAndAddTo(DOC_in, "toDay", &toDay);





	AnswerWriter->StartObject();
	AnswerWriter->Key("command_ID");
	AnswerWriter->String("QDRQ");

	AnswerWriter->EndObject();
}


/*
 * Get Manual mode Info
 */
void Command_GMMI(const rapidjson::Document& DOC_in, rapidjson::Writer<rapidjson::StringBuffer>* AnswerWriter)
{
	DOC_in.IsNull();
	ConfigParser config(CONFIG_FILE);
	RoboticArm* mArm = getArm(1);

	AnswerWriter->StartObject();
	AnswerWriter->Key("command_ID");
	AnswerWriter->String("GMMI");
	AnswerWriter->Key("lineRunning");
	if(mArm->TransmissionManualDebugging == true && mArm->PCState ==3)
		AnswerWriter->Bool(true);
	else
		AnswerWriter->Bool(false);

	AnswerWriter->Key("manualModes");
	AnswerWriter->StartArray();
	for(int i=0; i<config.GetNumberOfArms(); i++)
	{
		AnswerWriter->Bool(getArm(i+1)->ManipulatorMode==MANUAL);
	}
	AnswerWriter->EndArray();

	AnswerWriter->EndObject();
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
	//std::cout << "Received: " << Message << std::endl;
	//one document for input
    rapidjson::Document DOC_in;
    rapidjson::StringBuffer Answer_JSON;
    rapidjson::Writer<rapidjson::StringBuffer> writer(Answer_JSON);
    rapidjson::ParseResult result;
    result = DOC_in.Parse(Message.c_str());
    if(!result)
    {
    	 std::cout << "Message is not JSON or is corrupt: " << std::endl;
    	 return "";
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
        else if(boost::equals(command_ID, "REMV")) Command_REMV(DOC_in, &writer);
        else if(boost::equals(command_ID, "RPRV")) Command_RPRV(DOC_in, &writer);
        else if(boost::equals(command_ID, "ALSC")) Command_ALSC(DOC_in, &writer);
        else if(boost::equals(command_ID, "ALGC")) Command_ALGC(DOC_in, &writer);
        else if(boost::equals(command_ID, "CHAL")) Command_CHAL(DOC_in, &writer);
        else if(boost::equals(command_ID, "SCAP")) Command_SCAP(DOC_in, &writer);
        else if(boost::equals(command_ID, "WADI")) Command_WADI(DOC_in, &writer);
        else if(boost::equals(command_ID, "PLRD")) Command_PLRD(DOC_in, &writer);
        else if(boost::equals(command_ID, "FOTP")) Command_FOTP(DOC_in, &writer);
        else if(boost::equals(command_ID, "GDIS")) Command_GDIS(DOC_in, &writer);
        else if(boost::equals(command_ID, "SCFG")) Command_SCFG(DOC_in, &writer);
        else if(boost::equals(command_ID, "GCFG")) Command_GCFG(DOC_in, &writer);
        else if(boost::equals(command_ID, "QDRQ")) Command_QDRQ(DOC_in, &writer);
        else if(boost::equals(command_ID, "GMMI")) Command_GMMI(DOC_in, &writer);
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



