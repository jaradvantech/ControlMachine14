/*
 * DB_AbstractionLayer.cpp
 *
 *  Created on: Jul 26, 2018
 *      Author: root
 */

#include <DB_AbstractionLayer/DB_AbstractionLayer.h>

//When we call Execute we must provide a Query and a void function that accepts a sql::ResultSet* as argument to process the result of the query.
//in case that it is an INSERT or we are not interested in process the result, we can pass nullptr as argument.
MySQLAdapter MySQL_Manager;
MySQLAdapter* getMySQL_Manager(){return &MySQL_Manager;}

