/*
 * connexionMySQL.cpp
 *
 *  Created on: Jul 25, 2018
 *      Author: andres
 */

#include <connexionMySQL.h>

#ifdef COMPILE_WITH_MYSQL

#include <stdlib.h>
#include <iostream>
#include "jdbc/mysql_connection.h"
#include <jdbc/cppconn/driver.h>
#include <jdbc/mysql_driver.h>
#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/prepared_statement.h>


void dummyfunction(){


	try {
	sql::Driver *driver;
	sql::Connection *con;
	sql::Statement *stmt;
	//sql::ResultSet *res;
	//sql::PreparedStatement *pstmt;

	/* Create a connection */
	driver = sql::mysql::get_driver_instance();
	con = driver->connect("tcp://192.168.0.151:3306", "root", "drogo");
	/* Connect to the MySQL test database */
	con->setSchema("test");

	stmt = con->createStatement();
	stmt->execute("DROP TABLE IF EXISTS test");
	stmt->execute("CREATE TABLE test(id INT)");
	delete stmt;

	//delete res;

	//delete pstmt;
	delete con;

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



#endif


