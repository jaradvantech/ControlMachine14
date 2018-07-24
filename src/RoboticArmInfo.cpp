/*
 * RoboticArmInfo.cpp
 *
 *  Created on: Dec 13, 2017
 *      Author: Jose Andres Grau Martinez
 */

#include <RoboticArmInfo.h>
#include <algorithms/algorithm_v2.h>
#include <pthread.h>
#include "ConfigParser.h"

int totalNumberOfArms;
std::vector<RoboticArm*> RoboticArms;

RoboticArmConfigInfo RoboticArm::_ConfigInfo;

/*Arm-specific variables*/
bool RoboticArm::CommunicationExchange;
bool RoboticArm::TestPattern=1;
bool RoboticArm::InquiryTheTile;
bool RoboticArm::TransmissionManualDebugging;
short RoboticArm::PCState=1;
int RoboticArm::Z_AxisDeceletationDistance=1200;
int RoboticArm::Z_AxisStandbyValue=400;
int RoboticArm::ThePulseOfX_AxisGoBackToTheWaitingPositionInAdvance=500; //Yup bitches. I'm serious, in the manual had this name and I decided to keep it.
int RoboticArm::ThePulseOfZ_AxisAdvanceDownInAdvance=6500;

/*Common variables*/
bool RoboticArm::TheQueueOfPhotosensor_1;
bool RoboticArm::TheQueueOfPhotosensor_2;
bool RoboticArm::TheQueueOfPhotosensor_3;
bool RoboticArm::TheQueueOfPhotosensor_4;
bool RoboticArm::StationInterlock_16;
bool RoboticArm::WhetherOrNotPutTheTileTo_16;
int RoboticArm::EquipmentAlarmArray;
short RoboticArm::TileGrade;
short RoboticArm::ChangeColor;
short RoboticArm::SystemState;
long RoboticArm::ActualValueOfTheLineEncoder;
long RoboticArm::EnterTheTileStartingCodeValue;


RoboticArm::RoboticArm(){

    /*DATA READ FROM THE PLC*/
    HasDischarged=0; 				//0:Yes  1:No
    PhotosensorOfManipulator=0;		//0:Yes  1:No
    ManipulatorStatePosition=0;		//0:Standby 1:No Standby
    DischargedTheBrickConfirm=0;	//?????????????????
    LeftStorageBinSecurity=0;		//0:No  1:Alarm Looking from the beginning to the end
    RightStorageBinSecurity=0;		//0:No  1:Alarm Looking from the beginning to the end
    AlarmArray=0;					//Array 0-15 bool
    ManipulatorRepositionState=0; 	//0:Rest 1:Done 2:Alarm
    ActualValueEncoder=0; 			//

    /*DATA TO BE SENT TO THE PLC*/
    StorageBinDirection=0;			//0:A 1:B Not used anymore
	ManipulatorReset=0;				//0:Reset 1:Done?
	StorageBinFullA=1;				//0:Yes 1:No
	StorageBinFullB=1;				//0:Yes 1:No
	BarCodeReadStateA=1;			//0:NO 1:YES
	BarCodeReadStateB=1;			//0:NO 1:YES
    ManipulatorMode=1;				//0:Manual 1:Automatic
    VacuumValve=0;					//0:Open 1:Close
    ManualForwardBackward=0;		//0:Stop 1:Forward 2:Backward
	ManualLeftRight=0;				//0:Stop 1:Left 2:Right
	ManualUpDown=0;					//0:Stop 1:Up 2:Down
	WhatToDoWithTheBrick=0;			//0:Wait 1:Right Side 2:Left side
	CatchOrDrop=0;					//0:Stop 1:Catch 2:Drop
	PulseZAxis=5000;				//High speed to low speed
	ValueOfCatchDrop=0;				//Pulse of grab target
}


//Configure where to get the data from
bool RoboticArm::Setup(TS7Client *SetClient,
	int SetAmmountOfArms,
	int SetIODBReading,
	int SetIODBWriting,
	int Padding,
	int SetCommonBytesReading,
	int SetCommonBytesWriting,
	int SetReadingStartByte,
	int SetReadingLength,
	int SetWritingStartByte,
	int SetWritingLength){

	bool Problem=true;
	if(
		SetReadingLength>0 && //Avoids empty readings
		SetWritingLength>0
	  ) Problem=false;

	if(!Problem)
	{
		_ConfigInfo.Client=SetClient;							//PLC CLient that controls this arm
		_ConfigInfo.AmmountOfArms=SetAmmountOfArms;			//Amount arms controlled by the PLC
		_ConfigInfo.IODBReading=SetIODBReading;				//DB that holds the PLC information that the PC has to read
		_ConfigInfo.IODBWriting=SetIODBWriting;				//DB that holds the PLC information that the PC has to write
		_ConfigInfo.Padding=Padding;						//Spare bytes between data.
		_ConfigInfo.CommonBytesReading=SetCommonBytesReading;//Common bytes on the Reading DB
		_ConfigInfo.CommonBytesWriting=SetCommonBytesWriting;//Common bytes on the Writing DB
		_ConfigInfo.ReadingStartByte=SetReadingStartByte; 	//Number of the position where the reading variables are located at the DB. For the first Arm.
		_ConfigInfo.ReadingLength=SetReadingLength;			//Number of bytes that have to be red. For every Arm
		_ConfigInfo.WritingStartByte=SetWritingStartByte;	//Number of the position where the writing variables are located at the DB.For the first Arm.
		_ConfigInfo.WritingLength=SetWritingLength;			//Number of bytes that have to be written. For every Arm

		return 0; //0 means no error at SetUp
	}
	else
	{
		return -1;
	}
}

//RBS: what is all of this? can I delete it?

//--------------------------------------------------------------------
// PerformReading stores at the public variables of the object the values that are read from the PLC
//--------------------------------------------------------------------
/*
int RoboticArm::PerformLocalReading(){
	byte Buffer[_ConfigInfo.ReadingLength];
	int res = _ConfigInfo.Client->DBRead(_ConfigInfo.IODB,_ConfigInfo.ReadingStartByte,_ConfigInfo.ReadingLength,&Buffer);

    HasDischarged=S7_GetBitAt(Buffer, 0, 0);
    PhotosensorOfManipulator=S7_GetBitAt(Buffer, 0, 1);
    ManipulatorStatePosition=S7_GetBitAt(Buffer, 0, 2);
    DischargedTheBrickConfirm=S7_GetBitAt(Buffer, 0, 3);
    LeftStorageBinSecurity=S7_GetBitAt(Buffer, 0, 4);
    RightStorageBinSecurity=S7_GetBitAt(Buffer, 0, 5);
    AlarmArray=S7_GetWordAt(Buffer, 2);
    ManipulatorRepositionState=Buffer[4];
    ActualValueEncoder=S7_GetDIntAt(Buffer, 6);

    return res;
}
*/
//--------------------------------------------------------------------
// PerformWriting stores at the PLC the public variables of the object that are set somewhere else in the code
//--------------------------------------------------------------------
/*
int RoboticArm::PerformLocalWriting(){
	byte Buffer[_ConfigInfo.WritingLength];

	S7_SetBitAt(Buffer, 0, 0, StorageBinDirection);
	S7_SetBitAt(Buffer, 0, 1, ManipulatorReset);
	S7_SetBitAt(Buffer, 0, 2, StorageBinFullA);
	S7_SetBitAt(Buffer, 0, 3, StorageBinFullB);
	S7_SetBitAt(Buffer, 0, 4, BarCodeReadStateA);
	S7_SetBitAt(Buffer, 0, 5, BarCodeReadStateB);
	S7_SetBitAt(Buffer, 0, 6, ManipulatorMode);
	S7_SetBitAt(Buffer, 0, 7, VacuumValve);
	S7_SetBitAt(Buffer, 0, 7, VacuumValve);
	S7_SetByteAt(Buffer, 1, ManualForwardBackward);
	S7_SetByteAt(Buffer, 2, ManualLeftRight);
	S7_SetByteAt(Buffer, 3, ManualUpDown);
	S7_SetByteAt(Buffer, 4, CatchOrDrop);
	S7_SetByteAt(Buffer, 5, WhatToDoWithTheBrick);
	S7_SetIntAt(Buffer, 6, PulseZAxis);
	S7_SetDIntAt(Buffer, 8, ValueOfCatchDrop);
	int res = _ConfigInfo.Client->DBWrite(_ConfigInfo.IODB,_ConfigInfo.WritingStartByte,_ConfigInfo.WritingLength,Buffer);
    return res;
}
*/

RoboticArmConfigInfo RoboticArm::GetConfig(){
	return _ConfigInfo;
}


//PerformGlobalReading stores at the PLC the public variables of the object that are set somewhere else in the code
int PerformGlobalReading()
{
	byte Buffer[RoboticArm::GetConfig().CommonBytesReading +
				RoboticArm::GetConfig().AmmountOfArms *
				(RoboticArm::GetConfig().Padding + RoboticArm::GetConfig().ReadingLength)];

	int res = RoboticArm::GetConfig().Client->DBRead(RoboticArm::GetConfig().IODBReading,
			0,
			RoboticArm::GetConfig().CommonBytesReading +
			RoboticArm::GetConfig().AmmountOfArms *
			(RoboticArm::GetConfig().Padding + RoboticArm::GetConfig().ReadingLength),
			&Buffer);
	//----------------------//
	//Get common information//
	//----------------------//
123 678


	RoboticArm::TheQueueOfPhotosensor_1=S7_GetBitAt(Buffer, 0, 0);
	RoboticArm::TheQueueOfPhotosensor_2=S7_GetBitAt(Buffer, 0, 1);
	RoboticArm::TheQueueOfPhotosensor_3=S7_GetBitAt(Buffer, 0, 2);
	RoboticArm::TheQueueOfPhotosensor_4=S7_GetBitAt(Buffer, 0, 3);
	RoboticArm::StationInterlock_16=S7_GetBitAt(Buffer, 0, 4);
	RoboticArm::WhetherOrNotPutTheTileTo_16=S7_GetBitAt(Buffer, 0, 5);
	RoboticArm::EquipmentAlarmArray=revert16bitWordEndianness(S7_GetWordAt(Buffer,2));
	RoboticArm::TileGrade=Buffer[4];
	RoboticArm::ChangeColor=Buffer[5];
	RoboticArm::SystemState=Buffer[6];
	RoboticArm::EnterTheTileStartingCodeValue=S7_GetDIntAt(Buffer,8);
	RoboticArm::ActualValueOfTheLineEncoder=S7_GetDIntAt(Buffer,12);

	for(int j=0;j<RoboticArm::GetConfig().AmmountOfArms;j++){
		int StartByteOfThisArm =RoboticArm::GetConfig().CommonBytesReading + RoboticArm::GetConfig().Padding +
				j*(RoboticArm::GetConfig().ReadingLength + RoboticArm::GetConfig().Padding);
		RoboticArms.at(j)->HasDischarged=S7_GetBitAt(Buffer, 0+StartByteOfThisArm, 0);
		RoboticArms.at(j)->PhotosensorOfManipulator=S7_GetBitAt(Buffer, 0+StartByteOfThisArm, 1);
		RoboticArms.at(j)->ManipulatorStatePosition=S7_GetBitAt(Buffer, 0+StartByteOfThisArm, 2);
		RoboticArms.at(j)->DischargedTheBrickConfirm=S7_GetBitAt(Buffer, 0+StartByteOfThisArm, 3);
		RoboticArms.at(j)->LeftStorageBinSecurity=S7_GetBitAt(Buffer, 0+StartByteOfThisArm, 4);
		RoboticArms.at(j)->RightStorageBinSecurity=S7_GetBitAt(Buffer, 0+StartByteOfThisArm, 5);
		RoboticArms.at(j)->AlarmArray=revert16bitWordEndianness(S7_GetWordAt(Buffer, 2+StartByteOfThisArm)); //RBS changed from (Buffer, 2+StartByteOfThisArm)
		RoboticArms.at(j)->ManipulatorRepositionState=Buffer[4+StartByteOfThisArm];
		RoboticArms.at(j)->ActualValueEncoder=S7_GetDIntAt(Buffer, 6+StartByteOfThisArm);
	}

	pthread_cond_signal(&read_condition);
	return res;
}
//--------------------------------------------------------------------
// PerformGlobalWriting stores at the PLC the public variables of the object that are set somewhere else in the code
//--------------------------------------------NUMBEROFARMS------------------------
int PerformGlobalWriting(){
	byte Buffer[RoboticArm::GetConfig().CommonBytesWriting +
				RoboticArm::GetConfig().AmmountOfArms *
				(RoboticArm::GetConfig().Padding + RoboticArm::GetConfig().WritingLength)];
	S7_SetBitAt(Buffer, 0, 0, RoboticArm::CommunicationExchange);
	S7_SetBitAt(Buffer, 0, 1, RoboticArm::TestPattern);
	S7_SetBitAt(Buffer, 0, 2, RoboticArm::InquiryTheTile);
	S7_SetBitAt(Buffer, 0, 3, RoboticArm::TransmissionManualDebugging);
	S7_SetByteAt(Buffer, 1, RoboticArm::PCState);
	S7_SetIntAt(Buffer, 2, RoboticArm::Z_AxisDeceletationDistance);
	S7_SetIntAt(Buffer, 4, RoboticArm::Z_AxisStandbyValue);
	S7_SetIntAt(Buffer, 6, RoboticArm::ThePulseOfX_AxisGoBackToTheWaitingPositionInAdvance);
	S7_SetIntAt(Buffer, 8, RoboticArm::ThePulseOfZ_AxisAdvanceDownInAdvance);

	for(int j=0; j<totalNumberOfArms; j++)
	{
		int StartByteOfThisArm =RoboticArm::GetConfig().CommonBytesWriting + RoboticArm::GetConfig().Padding +
				j*(RoboticArm::GetConfig().WritingLength + RoboticArm::GetConfig().Padding);

		S7_SetBitAt(Buffer, 0+StartByteOfThisArm, 0, RoboticArms.at(j)->StorageBinDirection);
		S7_SetBitAt(Buffer, 0+StartByteOfThisArm, 1, RoboticArms.at(j)->ManipulatorReset);
		S7_SetBitAt(Buffer, 0+StartByteOfThisArm, 2, RoboticArms.at(j)->StorageBinFullA);
		S7_SetBitAt(Buffer, 0+StartByteOfThisArm, 3, RoboticArms.at(j)->StorageBinFullB);
		S7_SetBitAt(Buffer, 0+StartByteOfThisArm, 4, RoboticArms.at(j)->BarCodeReadStateA);
		S7_SetBitAt(Buffer, 0+StartByteOfThisArm, 5, RoboticArms.at(j)->BarCodeReadStateB);
		S7_SetBitAt(Buffer, 0+StartByteOfThisArm, 6, RoboticArms.at(j)->ManipulatorMode);
		S7_SetBitAt(Buffer, 0+StartByteOfThisArm, 7, RoboticArms.at(j)->VacuumValve);
		S7_SetByteAt(Buffer, 1+StartByteOfThisArm, RoboticArms.at(j)->ManualForwardBackward);
		S7_SetByteAt(Buffer, 2+StartByteOfThisArm, RoboticArms.at(j)->ManualLeftRight);
		S7_SetByteAt(Buffer, 3+StartByteOfThisArm, RoboticArms.at(j)->ManualUpDown);
		S7_SetByteAt(Buffer, 4+StartByteOfThisArm, RoboticArms.at(j)->WhatToDoWithTheBrick);
		S7_SetByteAt(Buffer, 5+StartByteOfThisArm, RoboticArms.at(j)->CatchOrDrop);
		S7_SetIntAt(Buffer,  6+StartByteOfThisArm, RoboticArms.at(j)->PulseZAxis);
		S7_SetDIntAt(Buffer, 8+StartByteOfThisArm, RoboticArms.at(j)->ValueOfCatchDrop);
	}
	int res = RoboticArm::GetConfig().Client->DBWrite(RoboticArm::GetConfig().IODBWriting,
			0,
			RoboticArm::GetConfig().CommonBytesWriting +
			RoboticArm::GetConfig().AmmountOfArms *
			(RoboticArm::GetConfig().Padding + RoboticArm::GetConfig().WritingLength),
												 Buffer);
	return res;
}


// PerformGlobalWriting stores at the PLC the public variables of the object that are set somewhere else in the code
void initGlobalArms(TS7Client *Client)
{
	ConfigParser config(CONFIG_FILE);
	int TPOX_AGBTTWPIA = config.GetMechanicalParameters().at(0);
	int SVB = config.GetMechanicalParameters().at(1);
	totalNumberOfArms = config.GetNumberOfArms();


	for(short i=0; i<totalNumberOfArms; i++)
	{
		RoboticArms.push_back(new RoboticArm());
		RoboticArms.at(i)->ThePulseOfX_AxisGoBackToTheWaitingPositionInAdvance = TPOX_AGBTTWPIA;
		RoboticArms.at(i)->Z_AxisStandbyValue = SVB;
	}

	RoboticArm::Setup(Client,
				totalNumberOfArms,
				2,   //DBRead
				1,   //DBWrite
				16,  //Padding bytes
				16,  //Common Reading bytes
				10,  //Common Writing bytes
				32,  //Starting Reading Arms
				10,  //Reading Bytes Per Arm
				26,  //Starting Writing Arms
				12); //Writing Bytes Per Arm
}

RoboticArm * getArm(int armIndex){
	return RoboticArms.at(armIndex-1);
}

int getTotalArms(void)
{
	return totalNumberOfArms;
}

/*
 * RBS Needed to match the order of the bytes read form the PLC
 * Siemens S7 PLC CPU is big-endian, while out processor is little endian
 */
uint16_t revert16bitWordEndianness(uint16_t inputWord)
{
	/*
	 * Input:
	 * UINT16 = (Byte1 Byte2)
	 * UINT16 = (Byte2 Byte1)
	 */
	return (inputWord & 0x00FF) << 8 | (inputWord & 0xFF00) >> 8;
}


