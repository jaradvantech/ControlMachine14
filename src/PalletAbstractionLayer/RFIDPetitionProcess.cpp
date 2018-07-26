/*
 * RFIDPetitionProcess.cpp
 *
 *  Created on: Jan 29, 2018
 *      Author: andres
 */

/////////////////////////////////////////////////////////////////////////////
//
//Petition Layer. This layer crafts messages and puts them in the list
/////////////////////////////////////////////////////////////////////////////

#include <PalletAbstractionLayer/RFIDPetitionProcess.h>
#include <connexionRFID.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <PalletAbstractionLayer/Pallet.h>

#define TOTAL_GRADES 7
#define TOTAL_COLORS 15

inline const char * const RFIDBoolToString(bool b)
{
  return b ? "_01" : "_00";
}

void PrepareForPetitions_StartUp(){
	RFIDReaders_Configure();
	RFIDReaders_Start();
}

void PrepareForPetitions_Restart(){
		RFIDReaders_Start();
}

int PetitionOf_ConfigUnit(int RFIDServer,std::string address , int port,
	    bool FailSafe, bool RegisterOutputDriver1, bool RegisterOutputDriver2){
	//--------------------------------------------------------------------
	// STORE CONFIGURATION
	//--------------------------------------------------------------------
	RFIDReader_StoreRFIDReader_Config(RFIDServer, address, port,
			FailSafe, RegisterOutputDriver1, RegisterOutputDriver2);
	//---------------------------------------------------------
	//OPEN CONNEXION
	//---------------------------------------------------------
	if(RFIDReader_OpenConnexion(RFIDServer,address, port)){
		std::string message, answer;
		std::string framedmessage;
		//--------------------------------------------------------------------
		// MESSAGE CRAFTING
		//--------------------------------------------------------------------

		message+="_CU";
		message+=RFIDBoolToString(FailSafe);
		message+=RFIDBoolToString(RegisterOutputDriver1);
		message+=RFIDBoolToString(RegisterOutputDriver2);
		message+="_01_00_AS\r\n";
		framedmessage+=(boost::format("%01u") % RFIDServer).str();
		framedmessage+="1000_"; //1000 Means configuration of evaluation unit
		framedmessage+=(boost::format("%04u") % (message.length()+9)).str();
		framedmessage+=message;

		//ADD MESSAGE TO THE LIST.
		//THIS HAS TO BE PROTECTED BY MUTEX
		//START OF MUTEX PROTECTION
		EmergencyAddMessage(framedmessage);
		//END OF MUTEX PROTECTION
		return 1;
	}

	return 0;
	//return ConfigUnit_errors ? 0 : 1;


}

int PetitionOf_ConfigChannel(int RFIDServer, short Channel, short Mode, short unsigned DataHoldTime, short unsigned LengthOfTagBlock,
		   short unsigned NumberOfBlocksOnTag,  bool ActivateDataHoldTime){

	if((Channel==1 || Channel==2 || Channel==3 || Channel==4) &&
	   (Mode   ==1 || Mode   ==2 || Mode   ==3 || Mode   ==11 )&&
	   DataHoldTime<=2550 &&
	   (LengthOfTagBlock==4  || LengthOfTagBlock==8   || LengthOfTagBlock==16   ||
		LengthOfTagBlock==32 || LengthOfTagBlock==64  || LengthOfTagBlock==128  ||	LengthOfTagBlock==256) &&
	   NumberOfBlocksOnTag>=1 && NumberOfBlocksOnTag<=256)
	   {

		//--------------------------------------------------------------------
		// STORE CHANNEL CONFIGURATION
		//--------------------------------------------------------------------
		RFIDReader_StoreRFIDReader_Channel_Config(RFIDServer, Channel, Mode,
					DataHoldTime, LengthOfTagBlock, NumberOfBlocksOnTag, ActivateDataHoldTime);
		//--------------------------------------------------------------------
		// MESSAGE CRAFTING
		//--------------------------------------------------------------------
		std::string message, framedmessage;
		message+="_CI_";
		message+=(boost::format("%02u") % Channel).str();
		message+="_";
		message+=(boost::format("%02u") % Mode).str();
		message+="_";
		message+=(boost::format("%04u") % DataHoldTime).str();
		message+="_";
		message+=(boost::format("%03u") % LengthOfTagBlock).str();
		message+="_";
		message+=(boost::format("%03u") % NumberOfBlocksOnTag).str();
		message+="_01_01_";
		message+=(boost::format("%02u") % ActivateDataHoldTime).str();
		message+="\r\n";

		framedmessage+=(boost::format("%01u") % RFIDServer).str();
		framedmessage+="1010_"; //1010 Means configuration of channel
		framedmessage+=(boost::format("%04u") % (message.length()+9)).str();
		framedmessage+=message;
		EmergencyAddMessage(framedmessage);

		return 1;

	} else {
		std::cout << "Incorrect parameters, no action was performed: " << Channel << std::endl;
		std::cout << "+-----------------------------------------------------" <<std::endl;
		return 0;
	}
}

int PetitionOf_ReadUID(int RFIDServer, short Channel){
	if(Channel==1 || Channel==2 || Channel==3 || Channel==4){
		std::string message,framedmessage;

		//--------------------------------------------------------------------
		// MESSAGE CRAFTING
		//--------------------------------------------------------------------

		message+="_RU_";
		message+=(boost::format("%02u") % Channel).str();
		message+="\r\n";

		framedmessage+=(boost::format("%01u") % RFIDServer).str();
		framedmessage+="1020_"; //1020 Means read UID
		framedmessage+=(boost::format("%04u") % (message.length()+9)).str();
		framedmessage+=message;
		AddMessage(framedmessage);

		return 1;
	}else{
		std::cout << "Incorrect parameters, no action was performed: " << Channel << std::endl;
		std::cout << "+-----------------------------------------------------" <<std::endl;
		return 0;
	}
}

int PetitionOf_ReadAllMemory(int RFIDServer, short Channel)
{
	IndividualPallet* mPallet=DesiredPallet(RFIDServer*4+(Channel));
	if(boost::equals(mPallet->UID,"0000000000000000")){
		std::cout << "Try to read from where there is no pallet : " <<  Channel << std::endl; //DEBUG
		std::cout << "+-----------------------------------------------------" <<std::endl;
		return 4; //4 means that it tried to perform a format operation where there is no pallet.
	}
	if (Channel == 1 || Channel == 2 || Channel == 3 || Channel == 4) {
		//--------------------------------------------------------------------
		// MESSAGE CRAFTING
		//--------------------------------------------------------------------
		std::string message, framedmessage;

		message += "_RD_";
		message += (boost::format("%02u") % Channel).str();
		message += "_00000_0112\r\n";

		framedmessage+=(boost::format("%01u") % RFIDServer).str();
		framedmessage+="1030_"; //1030 Means read the whole memory
		framedmessage+=(boost::format("%04u") % (message.length()+9)).str();
		framedmessage+=message;
		AddMessage(framedmessage);
		return 1;
	} else {
		std::cout << "Incorrect parameters, no action was performed: "
				<< Channel << std::endl;
		std::cout << "+-----------------------------------------------------"
				<< std::endl;
		return 0;
	}

}

int PetitionOf_FormatMemory(int RFIDServer, short Channel)
{
	IndividualPallet* mPallet=DesiredPallet(RFIDServer*4+(Channel));
	if(boost::equals(mPallet->UID,"0000000000000000")){
		std::cout << "Try to format where there is no pallet : " <<  Channel << std::endl; //DEBUG
		std::cout << "+-----------------------------------------------------" <<std::endl;
		return 4; //4 means that it tried to perform a formate operation where there is no pallet.
	}
	if(Channel==1 || Channel==2 || Channel==3 || Channel==4)
	{
		//--------------------------------------------------------------------
		// MESSAGE CRAFTING
		//--------------------------------------------------------------------
		std::string message, framedmessage;

		message+="_WR_";
		message+=(boost::format("%02u") % Channel).str();
		message+="_00000_0112_";
		message+=char(17);
		for(int i=1;i<112;i++){
			message+=char(1);
		}
		message+="\r\n";

		framedmessage+=(boost::format("%01u") % RFIDServer).str();
		framedmessage+="1040_"; //1040 Means format the whole memory
		framedmessage+=(boost::format("%04u") % (message.length()+9)).str();
		framedmessage+=message;
		AddMessage(framedmessage);

		mPallet->Brick[0]=17;
		for(int i=1;i<112;i++){					//For each byte starting at position 0, so: for all memory positions
			mPallet->Brick[i]=1;		//Write in the computer's memory the value used for empty
		}
	}else{
		std::cout << "Incorrect parameters, no action was performed: " << Channel << std::endl;
		std::cout << "+-----------------------------------------------------" <<std::endl;
		return 0;
	}
	return 0;
}

int PetitionOf_AddBrick(int RFIDServer, short Channel,int Grade, int Colour)
{
	IndividualPallet* mPallet=DesiredPallet(RFIDServer*4+(Channel));
	if(mPallet->Brick[0]>=100+17){

		std::cout << "The limit of 100 bricks has been reached: " <<  Channel << std::endl; //DEBUG
		std::cout << "+-----------------------------------------------------" <<std::endl;
		return 3; //3 means that the limit has been reached
	}
	if(boost::equals(mPallet->UID,"0000000000000000")){
		std::cout << "Try to add brick where there is no pallet : " <<  Channel << std::endl; //DEBUG
		std::cout << "+-----------------------------------------------------" <<std::endl;
		return 4; //4 means that it tried to perform an add operation where there is no pallet.
	}

	if((Channel==1 || Channel==2 || Channel==3 || Channel==4) &&
		Grade  >=0 && Grade  <=TOTAL_GRADES && Colour >=0 && Colour <=TOTAL_COLORS )
	{

		//--------------------------------------------------------------------
		// MESSAGE CRAFTING: Writes the brick information
		//--------------------------------------------------------------------
		std::string message, framedmessage;
		short NumberOfBricks = mPallet->Brick[0]-17;
		message+="_WR_";
		message+=(boost::format("%02u") % Channel).str();
		message+="_00000_";
		message+=(boost::format("%04u") % (NumberOfBricks + 2)).str(); // Number of bytes to write, so, it's NumberOfBricks + 1 + The brick to add
		message+="_";
		message+=char(mPallet->Brick[0]+1);//Add the coded number of bricks +1
		for(int i=1;i<=NumberOfBricks;i++){
			message+=char(mPallet->Brick[i]);//Add the previous bricks

		}
		message+=char(((Grade+1)<<4)+(Colour+1)); //Finally, the current brick
		message+="\r\n";

		framedmessage+=(boost::format("%01u") % RFIDServer).str();
		framedmessage+="1050_"; //1050 I
		framedmessage+=(boost::format("%04u") % (message.length()+9)).str();
		framedmessage+=message;
		AddMessage(framedmessage);

		mPallet->Brick[0]+=1;											 //Increase the computers memory byte 0 AKA number of bricks in one
		mPallet->Brick[mPallet->Brick[0]-17]=((Grade+1)<<4)+(Colour+1); //Go to the memory position of the new brick and write the information
		return 1;
	}else{
		std::cout << "Incorrect parameters, no action was performed: " << Channel << std::endl;
		std::cout << "+-----------------------------------------------------" <<std::endl;
		return 0;
	}
}

int PetitionOf_DeleteBrick(int RFIDServer, short Channel,int PositionToDelete)
{
	IndividualPallet* mPallet=DesiredPallet(RFIDServer*4+(Channel));
	if(boost::equals(mPallet->UID,"0000000000000000")){
		std::cout << "Try to add brick where there is no pallet : " <<  Channel << std::endl; //DEBUG
		std::cout << "+-----------------------------------------------------" <<std::endl;
		return 4; //4 means that it tried to perform an add operation where there is no pallet.
	}
	if(mPallet->Brick[0]<=17){
		std::cout << "Attempt to perform a delete operation on an empty pallet: " <<  Channel << std::endl; //DEBUG
		std::cout << "+-----------------------------------------------------" <<std::endl;
		return 3; //3 means that the limit has been reached
	}
	if((Channel==1 || Channel==2 || Channel==3 || Channel==4))
	{

		//--------------------------------------------------------------------
		// MESSAGE CRAFTING: Writes the brick information
		//--------------------------------------------------------------------
		std::string message, framedmessage;
		short NumberOfBricks = mPallet->Brick[0]-17;

		message+="_WR_";
		message+=(boost::format("%02u") % Channel).str();
		message+="_00000_";
		message+=(boost::format("%04u") % (NumberOfBricks + 1)).str(); // Number of bytes to write, so, it's NumberOfBricks + 1
		message+="_";
		message+=char(mPallet->Brick[0]-1);//Add the coded number of bricks -1
		for(int i=1;i<PositionToDelete;i++){
			message+=char(mPallet->Brick[i]);//Add the previous bricks
		}
		for(int i=PositionToDelete;i<=NumberOfBricks;i++){
			message+=char(mPallet->Brick[i+1]);//Skip the one to delete
		}
		message+="\r\n";

		framedmessage+=(boost::format("%01u") % RFIDServer).str();
		framedmessage+="1060_"; //1060 Delete
		framedmessage+=(boost::format("%04u") % (message.length()+9)).str();
		framedmessage+=message;
		AddMessage(framedmessage);

		mPallet->Brick[0]-=1;											 //Decrease the computers memory byte 0 AKA number of bricks in one

		for(int i=PositionToDelete;i<=NumberOfBricks;i++){
			mPallet->Brick[i]=mPallet->Brick[i+1];//Skip the one to delete
		}
		return 1;
	}
	else
	{
		std::cout << "Incorrect parameters, no action was performed: " << Channel << std::endl;
		std::cout << "+-----------------------------------------------------" <<std::endl;
		return 0;
	}
}

int PetitionOf_EditBrick(int RFIDServer, short Channel, int Position, int Grade, int Colour)
{
	IndividualPallet* mPallet=DesiredPallet(RFIDServer*4+(Channel));
	if(boost::equals(mPallet->UID,"0000000000000000")){
		std::cout << "Try to edit brick where there is no pallet : " <<  Channel << std::endl; //DEBUG
		std::cout << "+-----------------------------------------------------" <<std::endl;
		return 4; //4 means that it tried to perform an add operation where there is no pallet.
	}
	if(Position==0){
		std::cout << "Attempt to edit null brick" <<  Channel << std::endl; //DEBUG
		std::cout << "+-----------------------------------------------------" <<std::endl;
		return 3; //3 means that the limit has been reached
	}
	if((Channel==1 || Channel==2 || Channel==3 || Channel==4) &&
		Grade  >=0 && Grade  <=TOTAL_GRADES && Colour >=0 && Colour <=TOTAL_COLORS )
	{

		//--------------------------------------------------------------------
		// MESSAGE CRAFTING: Writes the brick information
		//--------------------------------------------------------------------
		std::string message, framedmessage;
		short NumberOfBricks = mPallet->Brick[0]-17;

		message+="_WR_";
		message+=(boost::format("%02u") % Channel).str();
		message+="_00000_";
		message+=(boost::format("%04u") % (NumberOfBricks + 1)).str(); // Number of bytes to write, so, it's NumberOfBricks + 1
		message+="_";
		message+=char(mPallet->Brick[0]-1);//Add the coded number of bricks -1
		for(int i=1;i<Position;i++){
			message+=char(mPallet->Brick[i]);//Add the previous bricks
		}
			message+=char(((Grade+1) << 4) + Colour +1); //insert new brick

		for(int i=Position;i<=NumberOfBricks;i++){
			message+=char(mPallet->Brick[i+1]);//Skip that one and add every other
		}
		message+="\r\n";

		framedmessage+=(boost::format("%01u") % RFIDServer).str();
		framedmessage+="1060_"; //1060 Delete
		framedmessage+=(boost::format("%04u") % (message.length()+9)).str();
		framedmessage+=message;
		AddMessage(framedmessage);

		mPallet->Brick[Position]=((Grade+1) << 4) + Colour +1;
		return 1;

	}
	else
	{
		std::cout << "Incorrect parameters, no action was performed: " << Channel << std::endl;
		std::cout << "+-----------------------------------------------------" <<std::endl;
		return 0;
	}
	return 1;
}

int PetitionOf_InsertBrick(int RFIDServer, short Channel,int position, int Grade, int Colour)
{
	IndividualPallet* mPallet=DesiredPallet(RFIDServer*4+(Channel));
	if(mPallet->Brick[0]>=100+17){

		std::cout << "The limit of 100 bricks has been reached: " <<  Channel << std::endl; //DEBUG
		std::cout << "+-----------------------------------------------------" <<std::endl;
		return 3; //3 means that the limit has been reached
	}
	if(boost::equals(mPallet->UID,"0000000000000000")){
		std::cout << "Try to insert brick where there is no pallet : " <<  Channel << std::endl; //DEBUG
		std::cout << "+-----------------------------------------------------" <<std::endl;
		return 4; //4 means that it tried to perform an add operation where there is no pallet.
	}

	//RBS; arreglar gran problema de indices, luego continuar con esto.

	if((Channel==1 || Channel==2 || Channel==3 || Channel==4) &&
		Grade  >=0 && Grade  <=TOTAL_GRADES && Colour >=0 && Colour  <=TOTAL_COLORS ) {

		//--------------------------------------------------------------------
		// MESSAGE CRAFTING: Writes the brick information
		//--------------------------------------------------------------------
		std::string message, framedmessage;
		short NumberOfBricks = mPallet->Brick[0]-17;

		//Move right every brick from insert position to the end
		for(int i=NumberOfBricks; i>position; i--)
		{
			mPallet->Brick[i+1] = mPallet->Brick[i];
		}
		//Insert new brick
		mPallet->Brick[position+1] = ((Grade+1)<<4)+(Colour+1);
		//Update count
		NumberOfBricks++;
	    mPallet->Brick[0] += 1;

		message+="_WR_";
		message+=(boost::format("%02u") % Channel).str();
		message+="_00000_";
		message+=(boost::format("%04u") % (NumberOfBricks+1)).str(); // Number of bytes to write
		message+="_";

		//Add all the data
		for(int i=0; i<(NumberOfBricks+1); i++)
		{
			message+=char(mPallet->Brick[i]);
		}
		message+="\r\n";

		framedmessage+=(boost::format("%01u") % RFIDServer).str();
		framedmessage+="1050_"; //1050 I
		framedmessage+=(boost::format("%04u") % (message.length()+9)).str();
		framedmessage+=message;
		AddMessage(framedmessage);

		return 1;
	}else{
		std::cout << "Incorrect parameters, no action was performed: " << Channel << std::endl;
		std::cout << "+-----------------------------------------------------" <<std::endl;
		return 0;
	}

}
