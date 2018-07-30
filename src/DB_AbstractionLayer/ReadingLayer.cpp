/*
 * ReadingLayer.cpp
 *
 *  Created on: Jul 27, 2018
 *      Author: root
 */

#include <DB_AbstractionLayer/ReadingLayer.h>
#include <DB_AbstractionLayer/DB_AbstractionLayer.h>
#include <vector>
#include <string>

sql::ResultSet *queryResult;
bool dataIsReady = false;

//add WHERE __ ____
void requestLogsByDate(std::string from, std::string to)
{

	std::string statement = "SELECT COUNT(idoperations_log), DATE(operations_log.Timestamp)";
	statement += "FROM operations_log ";
	statement += "GROUP BY DATE(operations_log.Timestamp)";
	getMySQL_Manager()->Execute(statement, &onQueryResult);
}

void requestLogsByGrade(std::string from, std::string to)
{
	std::string statement = "SELECT COUNT(idoperations_log), grade ";
	statement += "FROM operations_log ";
	statement += "GROUP BY ooperations_log.grade";
	getMySQL_Manager()->Execute(statement, &onQueryResult);
}

void requestLogsByColor(std::string from, std::string to)
{
	std::string statement = "SELECT COUNT(idoperations_log), grade ";
	statement += "FROM operations_log ";
	statement += "GROUP BY operations_log.color";
	getMySQL_Manager()->Execute(statement, &onQueryResult);
}

bool getQueryResult(std::vector<std::string> *characteristic, std::vector<int> *qty, int &total_rows, int &total_qty)
{
	bool successful_result = false;
	if(dataIsReady)
	{
		dataIsReady = false;
		total_rows = queryResult->rowsCount();
		total_qty = 0;
		characteristic->clear();
		qty->clear();
		while(queryResult->next())
		{
			qty->push_back(queryResult->getInt(1)); //First column (qty with this characteristic)
			characteristic->push_back(queryResult->getString(2)); //second column Characteristic (color, grade, date produced, etc)
			total_qty += queryResult->getInt(1);
		}
		successful_result = true;
	}
	return successful_result;
}

void onQueryResult(sql::ResultSet *sql_ResultSet)
{
	queryResult=sql_ResultSet;
	dataIsReady = true;
}

