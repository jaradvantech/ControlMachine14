/*
 * DB_AbstractionLayer.h
 *
 *  Created on: Jul 26, 2018
 *      Author: root
 */

#ifndef DB_ABSTRACTIONLAYER_H_
#define DB_ABSTRACTIONLAYER_H_
#include <connexionMySQL.h>



MySQLAdapter* getMySQL_Manager();
void InsertRegister();
void RetrieveRegister();

#endif /* DB_ABSTRACTIONLAYER_H_ */
