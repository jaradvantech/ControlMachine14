/*
 * ReadingLayer.h
 *
 *  Created on: Jul 27, 2018
 *      Author: root
 */

#ifndef DB_ABSTRACTIONLAYER_READINGLAYER_H_
#define DB_ABSTRACTIONLAYER_READINGLAYER_H_

#include <vector>
#include <string>
#include <DB_AbstractionLayer/DB_AbstractionLayer.h>

void requestLogsByDate(std::string from, std::string to);
void requestLogsByGrade(std::string from, std::string to);
void requestLogsByColor(std::string from, std::string to);
bool getQueryResult(std::vector<std::string> *characteristic, std::vector<int> *qty, int &total_rows, int &total_qty);
void onQueryResult(sql::ResultSet *sql_ResultSet);

#endif /* DB_ABSTRACTIONLAYER_READINGLAYER_H_ */
