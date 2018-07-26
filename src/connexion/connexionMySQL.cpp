/*
 * connexionMySQL.cpp
 *
 *  Created on: Jul 25, 2018
 *      Author: andres
 */


#include <connexionMySQL.h>

//#include "mysqlx"
//#include "mysql_connexion.h"


#include <stdlib.h>
#include <iostream>
#include "jdbc/mysql_connection.h"
#include <jdbc/cppconn/driver.h>
#include <jdbc/mysql_driver.h>
#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/prepared_statement.h>


void TestMySQL(){


	try {


		sql::Driver *driver = get_driver_instance();
		std::string IP = "tcp://";
		IP+= "192.168.0.151";
		IP+= ":";
		IP+="3306";
		const sql::SQLString host = IP;
		const sql::SQLString user = "root";
		const sql::SQLString pass = "drogo";

		sql::Connection *con = driver->connect(host, user, pass);

		bool isInfoSchemaUsed;
		con->getClientOption("metadataUseInfoSchema", (void *) &isInfoSchemaUsed);

		int defaultStmtResType;
		int defaultPStmtResType;
		con->getClientOption("defaultStatementResultType", (void *) &defaultStmtResType);
		con->getClientOption("defaultPreparedStatementResultType", (void *) &defaultPStmtResType);
		std::cout << std::endl;
		std::cout << "+-----------------------------------------------------" << std::endl;
		std::cout << "| Connected to database on " <<host<< std::endl;
		std::cout << "| isInfoSchemaUsed: " <<isInfoSchemaUsed << std::endl;
		std::cout << "| defaultStatementResultType: " <<defaultStmtResType << std::endl;
		std::cout << "| defaultPreparedStatementResultType: " <<defaultPStmtResType << std::endl;
		std::cout << "+-----------------------------------------------------" << std::endl;
		// Connect to the MySQL test database
		//con->setSchema("test");

		//stmt = con->createStatement();
		//stmt->execute("DROP TABLE IF EXISTS test");
		//stmt->execute("CREATE TABLE test(id INT)");
		//delete stmt;

		//delete res;

		//delete pstmt;
		//delete con;

	} catch (sql::SQLException &e) {
	std::cout << "# ERR: SQLException in " << __FILE__;
	std::cout << "(" << __FUNCTION__ << ") on line "
	<< __LINE__ << std::endl;
	std::cout << "# ERR: " << e.what();
	std::cout << " (MySQL error code: " << e.getErrorCode();
	std::cout << ", SQLState: " << e.getSQLState() <<
	" )" << std::endl;
	}
}


