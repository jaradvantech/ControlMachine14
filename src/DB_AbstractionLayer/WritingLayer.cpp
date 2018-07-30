/*
 * WritingLayer.cpp
 *
 *  Created on: Jul 27, 2018
 *      Author: root
 */

#include <DB_AbstractionLayer/DB_AbstractionLayer.h>
#include <boost/lexical_cast.hpp>

void InsertOperation(int Color_Of_Brick, int Grade_Of_Brick, int Pallet_Position, int Operation_Type, std::string Pallet_UID)
{
	std::string Query="INSERT INTO operations_log (Timestamp,Color_Of_Brick,Grade_Of_Brick,Grade_Of_Brick,Pallet_Position,Operation_Type,Pallet_UID) VALUES (CURRENT_TIMESTAMP()";
	Query+=",'";
	Query+=boost::lexical_cast<std::string>(Color_Of_Brick);
	Query+="'";

	Query+=",'";
	Query+=boost::lexical_cast<std::string>(Grade_Of_Brick);
	Query+="'";

	Query+=",'";
	Query+=boost::lexical_cast<std::string>(Pallet_Position);
	Query+="'";

	Query+=",'";
	Query+=boost::lexical_cast<std::string>(Operation_Type);
	Query+="'";

	Query+=",'";
	Query+=Pallet_UID;
	Query+="'";

	Query+=")";
	getMySQL_Manager()->Execute(Query,
					nullptr);
}

