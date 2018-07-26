/*
 * connexionMySQL.h
 *
 *  Created on: Jul 25, 2018
 *      Author: andres
 */
#include "jdbc/mysql_connection.h"
#include <jdbc/cppconn/driver.h>
#include <jdbc/mysql_driver.h>
#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <deque>
#include <mutex>
#include <thread>


class MySQLAdapter {
	private:
		sql::Driver *sql_driver; //this is the driver
		sql::Statement *sql_stmt;
		sql::Connection *sql_connection;
		sql::ResultSet *sql_ResultSet;
		std::string _IP;
		std::string _DBName;

		void RealInitMySQL();
		bool Connect();
		void OnErrorWipeClean(sql::SQLException &e);

		struct QueryStructure {
			public:
			std::string Query;
			void(*FunctionToProcessQuery)(sql::ResultSet*);
		};

		std::deque<QueryStructure> List_Of_Queries;
		pthread_t MySQLThread;

		void* QueryThread(void* Arg);
		//void Execute(std::string Query, void(*foo)(sql::ResultSet**));

		std::mutex List_Of_Queries_mutex;
		std::mutex Thread_Semaphore_mutex;
		bool Thread_Semaphore;
		static void* QueryThread_wrapper(void* object);
	public:
		void InitMySQL(std::string IP, std::string DBName);
		void Execute(std::string Query, void(*foo)(sql::ResultSet*));



};


