/*
 * DB_AbstractionLayer.cpp
 *
 *  Created on: Jul 26, 2018
 *      Author: root
 */

#include <DB_AbstractionLayer.h>

MySQLAdapter MySQL_Manager;
MySQLAdapter* getMySQL_Manager(){return &MySQL_Manager;}



void InsertRegister()
{
	MySQL_Manager.Execute("INSERT INTO operations_log (Timestamp,Type_Of_Brick,Pallet_Position,Operation_Type,Pallet_UID) VALUES ('2014-01-05 08:35:12','1','1','4','1')",
					nullptr);
	MySQL_Manager.Execute("INSERT INTO operations_log (Timestamp,Type_Of_Brick,Pallet_Position,Operation_Type,Pallet_UID) VALUES ('2014-01-05 08:35:12','1','2','5','1')",
						nullptr);
	MySQL_Manager.Execute("INSERT INTO operations_log (Timestamp,Type_Of_Brick,Pallet_Position,Operation_Type,Pallet_UID) VALUES ('2014-01-05 08:35:12','1','3','6','1')",
						nullptr);
}


void processSelect(sql::ResultSet *sql_ResultSet){
	while (sql_ResultSet->next()) {
	    std::cout << "\t... MySQL replies: "  ;
	    /* Access column data by alias or column name */
	    std::cout << sql_ResultSet->getInt("idoperations_log");
	    std::cout << "\t... MySQL says it again: ";
	    /* Access column data by numeric offset, 1 is the first column */
	    std::cout << sql_ResultSet->getInt(1) << std::endl;
	  }
}

void RetrieveRegister()
{
	MySQL_Manager.Execute("SELECT * FROM operations_log LIMIT 0, 1000",&processSelect );
}

