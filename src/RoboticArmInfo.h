/*
 * RoboticArmInfo.h
 *
 *  Created on: Dec 13, 2017
 *      Author: Jose Andres Grau Martinez
 */

#ifndef ROBOTICARMINFO_H_
#define ROBOTICARMINFO_H_

#include "connexionPLC.h"
#include "connexionPLCdatahandler.h"
#include "stdio.h"

const int LOCAL_DB     = 0x01;
const int GLOBAL_DB    = 0x02;
const int DBTYPE = GLOBAL_DB;


struct RoboticArmConfigInfo{
	public:
	TS7Client *Client;		//PLC CLient that controls this arm
	StructInfoPlc InfoPlc;   //Info of the PLC that controls this arm
	int AmmountOfArms;
	int IODBReading;		//DB that holds the PLC information that the PC has to read
	int IODBWriting;		//DB that holds the PLC information that the PC has to write
	int Padding;			//Spare bytes between data.
	int CommonBytesReading; //Common bytes on the Reading DB
	int CommonBytesWriting; //Common bytes on the Writing DB
	int ReadingStartByte; 	//Number of the position where the reading variables are located at the DB. For the first Arm.
	int ReadingLength;		//Number of bytes that have to be red.
	int WritingStartByte;	//Number of the position where the writing variables are located at the DB.For the first Arm.
	int WritingLength;		//Number of bytes that have to be written.
};
class RoboticArm{
  private:

  public:
		// Remove the static keyword in order to use LocalArms.
		// Add the static keyword in order to use Global arms.
	static RoboticArmConfigInfo _ConfigInfo;
	static bool Setup(TS7Client *SetClient,
				int SetAmmountOfArms,
				int SetIODBReading,
				int SetIODBWriting,
				int Padding,
				int SetCommonBytesReading,
				int SetCommonBytesWriting,
				int SetReadingStartByte,
				int SetReadingLength,
				int SetWritingStartByte,
				int SetWritingLength);
    static RoboticArmConfigInfo GetConfig();
	//--------------------------------------------------------------------
    // DATA READ FROM THE PLC
	//--------------------------------------------------------------------
    static bool TheQueueOfPhotosensor_1;
    static bool TheQueueOfPhotosensor_2;
    static bool TheQueueOfPhotosensor_3;
    static bool TheQueueOfPhotosensor_4;
    static bool StationInterlock_16;
    static bool WhetherOrNotPutTheTileTo_16;
    static int EquipmentAlarmArray;
    static short TileGrade;
    static short ChangeColor;
    static short SystemState;
    static long ActualValueOfTheLineEncoder;
    static long EnterTheTileStartingCodeValue;
    //_________________________________________________________
    bool HasDischarged; 				//0:Yes  1:No
    bool PhotosensorOfManipulator;		//0:Yes  1:No
    bool ManipulatorStatePosition;		//0:Standby 1:No Standby
    bool DischargedTheBrickConfirm;		//?????????????????
    bool LeftStorageBinSecurity;		//0:No  1:Alarm Looking from the beginning to the end
    bool RightStorageBinSecurity;		//0:No  1:Alarm Looking from the beginning to the end
    int AlarmArray;						//Array 0-15 bool
    short ManipulatorRepositionState; 	//0:Rest 1:Done 2:Alarm
    long ActualValueEncoder; 			//
	//--------------------------------------------------------------------
    // DATA TO BE SENT TO THE PLC
	//--------------------------------------------------------------------
    static bool CommunicationExchange;
	static bool TestPattern;
	static bool InquiryTheTile;
	static bool TransmissionManualDebugging;
	static short PCState;
	static int Z_AxisDeceletationDistance;
	static int Z_AxisStandbyValue;
	static int ThePulseOfX_AxisGoBackToTheWaitingPositionInAdvance; //Yup bitches. I'm serious, in the manual had this name and I decided to keep it.
	static int ThePulseOfZ_AxisAdvanceDownInAdvance;
    //_________________________________________________________
    bool StorageBinDirection;			//0:A 1:B NOT USED
	bool ManipulatorReset;				//0:Reset 1:Done?
	bool StorageBinFullA;				//0:Yes 1:No
	bool StorageBinFullB;				//0:Yes 1:No
	bool BarCodeReadStateA;				//0:NO 1:YES
	bool BarCodeReadStateB;				//0:NO 1:YES
    bool ManipulatorMode;				//0:Manual 1:Automatic
    bool VacuumValve;					//0:Open 1:Close
    short ManualForwardBackward;		//0:Stop 1:Forward 2:Backward
	short ManualLeftRight;				//0:Stop 1:Left 2:Right
	short ManualUpDown;					//0:Stop 1:Up 2:Down
	short WhatToDoWithTheBrick;			//0:Idle 1:Right side 2:Left Side
	short CatchOrDrop;					//0:Stop 1:Catch 2:Drop
	int PulseZAxis;					//High speed to low speed
	long ValueOfCatchDrop;				//Pulse of grab target
	//--------------------------------------------------------------------
    RoboticArm();
};

int PerformGlobalReading();
int PerformGlobalWriting();
void initGlobalArms(TS7Client *Client);
RoboticArm * DesiredRoboticArm(int NumberOfTheArm);
int getTotalArms(void);

#endif /* ROBOTICARMINFO_H_ */
