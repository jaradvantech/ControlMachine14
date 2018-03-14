/*
 * RFIDAnswerProcess.cpp
 *
 *  Created on: Jan 28, 2018
 *      Author: andres
 */


#include <PalletAbstractionLayer/RFIDAnswerProcess.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <PalletAbstractionLayer/Pallet.h>



/////////////////////////////////////////////////////////////////////////////
//Answer Layer, here the command will be processed
//
/////////////////////////////////////////////////////////////////////////////



int AnswerOfCU(std::string Answer, int RFIDServer){
	if(Answer.substr(13,2)=="00"){ //00 means no error
		std::cout << "Unit connected and configured OK " << RFIDServer << std::endl;
		std::cout << "+-----------------------------------------------------" <<std::endl;
		return 1;
	}else{
		std::cout << "One error (or more) has been detected, restart of the unit is recommended: " << RFIDServer << std::endl;
		std::cout << "+-----------------------------------------------------" <<std::endl;
		return 0;
	}
}

int AnswerOfCI(std::string Answer, int RFIDServer){
	if(Answer.substr(16,2)=="00"){
		std::cout << "Channel connected and configured OK. Server: "<< RFIDServer << "   Channel:" << Answer.substr(13,2) << std::endl;
		std::cout << "+-----------------------------------------------------" <<std::endl;
		return 1;
	}else{
		std::cout << "One error (or more) has been detected, restart of the unit is recommended: Server: " << RFIDServer << "   Channel:" << Answer.substr(13,2) << std::endl;
		std::cout << "+-----------------------------------------------------" <<std::endl;
		return 0;
	}
}

int AnswerOfRU(std::string Answer, int RFIDServer){

	std::string mUID;
	short UIDLength;
	short Channel;
	if(Answer.substr(16,2)!="00"){
		std::cout << "One error (or more) has been detected when reading the UID" << std::endl;
		return 0;
	}else{

		//--------------------------------------------------------------------
		// IF NO ERRORS PERFORM THE FOLLOWING OPERATIONS
		//--------------------------------------------------------------------

		UIDLength=boost::lexical_cast<short>(Answer.substr(19,2)); //Get the length of the UID
		mUID=Answer.substr(22,UIDLength*2);						  //Extract the UID

		Channel=boost::lexical_cast<short>(Answer.substr(13,2)); //Get the channel of the UID

		IndividualPallet* mPallet= DesiredPallet(RFIDServer*4+(Channel));

		if(mUID==""){
			mUID+=(boost::format("%016u")%"").str();		  //In case that there is no UID, continue as UID=0000000000000000
			//Not sure									  //And format the whole computer's memory with 01s
		}
		std::string PreviousUID=mPallet->UID;

		mPallet->UID=mUID;								  //Store in the computer memory the value of the UID

		if(!boost::equal(mPallet->UID,PreviousUID)){
			mPallet->UIDChanged=1;
			mPallet->UIDChanged_count=0;
		}
		if (mPallet->UIDChanged == 1
				&& boost::equal(mPallet->UID, PreviousUID)) {
			mPallet->UIDChanged_count += 1;
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------

		std::cout << "UID Red successfully. Server: " << RFIDServer <<" Channel: " <<  Channel << "  Tag: "<< mUID << std::endl;
		std::cout << "+-----------------------------------------------------" <<std::endl;
		return 1;
	}

}

int AnswerOfReadAllMemory(std::string Answer, int RFIDServer){

	if (Answer.substr(16, 2) != "00") {
		std::cout
				<< "One error (or more) has been detected when reading the memory"
				<< std::endl;
		std::cout
				<< "+-----------------------------------------------------"
				<< std::endl;
		return 0; // 0 means that there is some error in the message
	} else {

		//--------------------------------------------------------------------
		// IF NO ERRORS PERFORM THE FOLLOWING OPERATIONS
		//--------------------------------------------------------------------
		int Pos0 = 3;
		short Channel;
		Channel=boost::lexical_cast<short>(Answer.substr(13,2)); //Get the channel of the UID
		IndividualPallet* mPallet=DesiredPallet(RFIDServer*4+(Channel));

		Pos0 = (int) Answer.at(30); //store the number of bricks from the byte 0
		std::cout << "Pos0: " << Pos0 << std::endl; //Debug purpose
		if (Pos0 == 1) { //Memory Empty
			mPallet->Brick[0] = 17; //01 means Formated memory, treat it as a 17.
		} else {

			mPallet->Brick[0] = Pos0; //Save that value at the computer's memory at the position 0
		}
		for (int i = 1; i < 112; i++) {	//For each byte starting at position 1, so: for each brick
			mPallet->Brick[i] = (int) (Answer.at(30 + i));//Store the value at the computer's memory in the right position.
			//std::cout << "Brick info read "<<  i << " :"  <<  answer.at(20+i) << std::endl; //Debug purpose
		}
		std::cout << "NOB: " << Pos0 - 17 << std::endl; //Debug purpose
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------

		std::cout << "Memory Red successfully. Channel: " << Channel << std::endl;
		std::cout << "+-----------------------------------------------------"	<< std::endl;
		return 1;
	}


}

int AnswerOfFormatMemory(std::string Answer, int RFIDServer){
	if(Answer.substr(16,2)!="00"){
		std::cout << "One error (or more) has been detected when writing to the memory" << std::endl;
		std::cout << "+-----------------------------------------------------" <<std::endl;
		return 0;
	}else{
		//--------------------------------------------------------------------
		// IF NO ERRORS PERFORM THE FOLLOWING OPERATIONS
		//--------------------------------------------------------------------
		short Channel;
		Channel=boost::lexical_cast<short>(Answer.substr(13,2)); //Get the channel of the UID


		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		std::cout << "Memory formated successfully. Channel: " <<  Channel << std::endl;
		std::cout << "+-----------------------------------------------------" <<std::endl;
		//ReadAllMemory(Channel);
		return 1;
	}
}

int AnswerOfAddBrick(std::string Answer, int RFIDServer){
	if(Answer.substr(16,2)!="00"){
		std::cout << "One error (or more) has been detected when writing to the memory" << std::endl;
		std::cout << "+-----------------------------------------------------" <<std::endl;
		return 0; //0 means that there is some error in the message
	}else{
		//--------------------------------------------------------------------
		// IF NO ERRORS AT ALL, PERFORM THE FOLLOWING OPERATIONS
		//--------------------------------------------------------------------
		short Channel;

		Channel=boost::lexical_cast<short>(Answer.substr(13,2)); //Get the channel of the UID
			//std::cout << "Memory writting successfully. Brick N: " <<  Pallet[Channel-1]->Brick[0]-17 << std::endl;
			//std::cout << "Memory writting successfully. Brick Info: " <<  Pallet[Channel-1]->Brick[Pallet[Channel-1]->Brick[0]-17] << std::endl;

			std::cout << "Memory writting successfully. Channel: " <<  Channel << std::endl; //DEBUG
			std::cout << "+-----------------------------------------------------" <<std::endl;
			return 1;
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
	}
}

int AnswerOfDeleteBrick(std::string Answer, int RFIDServer){
	if(Answer.substr(16,2)!="00"){
		std::cout << "One error (or more) has been detected when writing to the memory" << std::endl;
		std::cout << "+-----------------------------------------------------" <<std::endl;
		return 0; //2 means that there is some error in the message
	}else{
		//--------------------------------------------------------------------
		// IF NO ERRORS AT ALL, PERFORM THE FOLLOWING OPERATIONS
		//--------------------------------------------------------------------
		short Channel;

		Channel=boost::lexical_cast<short>(Answer.substr(13,2)); //Get the channel of the UID
			//std::cout << "Memory writting successfully. Brick N: " <<  Pallet[Channel-1]->Brick[0]-17 << std::endl;
			//std::cout << "Memory writting successfully. Brick Info: " <<  Pallet[Channel-1]->Brick[Pallet[Channel-1]->Brick[0]-17] << std::endl;

			std::cout << "Memory writting successfully. Channel: " <<  Channel << std::endl; //DEBUG
			std::cout << "+-----------------------------------------------------" <<std::endl;
			return 1;
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
	}
}

int AnswerOfEditBrick(std::string Answer, int RFIDServer){

}

int AnswerOfInsertBrick(std::string Answer, int RFIDServer){

}

int processAnswer(std::string Answer, int RFIDServer){

	//ALWAYS:
	if(boost::equals(Answer.substr(0,4),"1000")){		//CONNEXION UNIT
		return AnswerOfCU(Answer, RFIDServer);

	}else if(boost::equals(Answer.substr(0,4),"1010")){//CONFIGURE CHANNEL
		return AnswerOfCI(Answer, RFIDServer);

	}else if(boost::equals(Answer.substr(0,4),"1020")){//READ UID
		return AnswerOfRU(Answer, RFIDServer);

	}else if(boost::equals(Answer.substr(0,4),"1030")){//READ ALL MEMORY
		return AnswerOfReadAllMemory(Answer, RFIDServer);

	}else if(boost::equals(Answer.substr(0,4),"1040")){//FORMAT MEMORY
		return AnswerOfFormatMemory(Answer, RFIDServer);

	}else if(boost::equals(Answer.substr(0,4),"1050")){//ADD BRICK
		return AnswerOfAddBrick(Answer, RFIDServer);

	}else if(boost::equals(Answer.substr(0,4),"1060")){//DELETE BRICK
		return AnswerOfDeleteBrick(Answer, RFIDServer);

	}else if(boost::equals(Answer.substr(0,4),"1070")){//EDIT BRICK
		return AnswerOfEditBrick(Answer, RFIDServer);

	}else if(boost::equals(Answer.substr(0,4),"1080")){//INSERT BRICK
		return AnswerOfInsertBrick(Answer, RFIDServer);
	}
	return 1;
}
