/*
 * connexionMySQL.cpp
 *
 *  Created on: Jul 25, 2018
 *      Author: andres
 */


#include <connexionMySQL.h>
#include <stdlib.h>
#include <iostream>
#include <deque>
#include <pthread.h>

bool MySQLAdapter::Connect()
{
        bool connected = (sql_connection != nullptr ) && (sql_connection->isValid() || sql_connection->reconnect());

        if (!connected)
        {
    		std::string IP = "tcp://";
    		IP+= _IP;
    		IP+= ":";
    		IP+="3306";
    		const sql::SQLString host = IP;
    		const sql::SQLString user = "root";
    		const sql::SQLString pass = "drogo";

    		sql_connection = sql_driver->connect(host, user, pass);
            connected = sql_connection->isValid();
            if(connected){
        		bool isInfoSchemaUsed;
        		sql_connection->getClientOption("metadataUseInfoSchema", (void *) &isInfoSchemaUsed);

        		int defaultStmtResType;
        		int defaultPStmtResType;
        		sql_connection->getClientOption("defaultStatementResultType", (void *) &defaultStmtResType);
        		sql_connection->getClientOption("defaultPreparedStatementResultType", (void *) &defaultPStmtResType);
        		std::cout << std::endl;
        		std::cout << "+-----------------------------------------------------" << std::endl;
        		std::cout << "| Connected to database on " <<_IP<< std::endl;
        		std::cout << "| isInfoSchemaUsed: " <<isInfoSchemaUsed << std::endl;
        		std::cout << "| defaultStatementResultType: " <<defaultStmtResType << std::endl;
        		std::cout << "| defaultPreparedStatementResultType: " <<defaultPStmtResType << std::endl;
        		std::cout << "+-----------------------------------------------------" << std::endl;
            }
        }
        if (connected){
        	return true;
        }
        return false;
}

void MySQLAdapter::OnErrorWipeClean(sql::SQLException &e){

	std::cout << "# ERR: SQLException in " << __FILE__;
	std::cout << "(" << __FUNCTION__ << ") on line "
	<< __LINE__ << std::endl;
	std::cout << "# ERR: " << e.what();
	std::cout << " (MySQL error code: " << e.getErrorCode();
	std::cout << ", SQLState: " << e.getSQLState() <<
	" )" << std::endl;
		//delete sql_connection;
		//delete sql_driver;
		//delete sql_stmt;
		sql_connection = nullptr;
		sql_driver = nullptr;
		sql_stmt = nullptr;

}

void MySQLAdapter::InitMySQL(std::string IP, std::string DBName)
{
	_IP = IP;
	_DBName = DBName;
	RealInitMySQL();
}

void MySQLAdapter::RealInitMySQL(){
	try {
		// Connect to the MySQL test database
		sql_driver = get_driver_instance();
		// Connect to the MySQL test database
		bool connect = Connect();
		if(connect == true)
		{
			sql_connection->setSchema(_DBName);

			sql_stmt = sql_connection->createStatement();
		}
		else{
			std::cout << "No way, it should have thrown" << std::endl;
		}
		//stmt->execute("INSERT INTO operations_log (Timestamp,Type_Of_Brick,Pallet_Position,Operation_Type,Pallet_UID) VALUES ('2014-01-05 08:35:12','1','1','1','1')");
		//stmt->execute("CREATE TABLE test(id INT)");

	} catch (sql::SQLException &e) {
		OnErrorWipeClean(e);
	}

}


//The idea is to create a new detached thread every time an insert has to be done.
//As operations in the DB must be atomic, we'll have a list of queries to execute

void* MySQLAdapter::QueryThread(void* Arg)
{

	pthread_detach((unsigned long)pthread_self());
	Thread_Semaphore_mutex.lock();
	bool ThereAreMoreQueries = true;
		while(ThereAreMoreQueries)
		{
			//AQUIRE LOCK HERE
			std::lock_guard < std::mutex > lockGuard(List_Of_Queries_mutex);
			std::cout<<List_Of_Queries.size()<< std::endl;
			QueryStructure Query = List_Of_Queries.at(0);
			List_Of_Queries.pop_front();
			std::cout<<List_Of_Queries.size()<< std::endl;
			if(List_Of_Queries.size()==0) ThereAreMoreQueries=false;

			//RELEASE LOCK HERE
			 lockGuard.~lock_guard();
			if(Connect())
			{
				try
				{
					sql_ResultSet = sql_stmt->executeQuery(Query.Query);
				}
				catch(sql::SQLException &e)
				{
					if(e.getErrorCode() !=0){
						std::cout << "# ERR: SQLException in " << __FILE__;
						std::cout << "(" << __FUNCTION__ << ") on line "
						<< __LINE__ << std::endl;
						std::cout << "# ERR: " << e.what();
						std::cout << " (MySQL error code: " << e.getErrorCode();
						std::cout << ", SQLState: " << e.getSQLState() <<
						" )" << std::endl;
					}
				}
				if(Query.FunctionToProcessQuery != nullptr) Query.FunctionToProcessQuery(sql_ResultSet);
			}
		}
	Thread_Semaphore=false;
	Thread_Semaphore_mutex.unlock();
	return nullptr;
}

void MySQLAdapter::Execute(std::string Query, void(*foo)(sql::ResultSet*))
{
	QueryStructure structure;
	structure.Query=Query;
	structure.FunctionToProcessQuery = foo;
	//AQUIRE LOCK HERE
	std::lock_guard < std::mutex > lockGuard(List_Of_Queries_mutex);
	List_Of_Queries.push_back(structure);
	//RELEASE LOCK HERE
	 lockGuard.~lock_guard();





	 if(Thread_Semaphore==false)
	 {
			Thread_Semaphore_mutex.lock();
			Thread_Semaphore=true;
			pthread_create(&MySQLThread, NULL, &QueryThread_wrapper, this);
			Thread_Semaphore_mutex.unlock();
	 }
}

void* MySQLAdapter::QueryThread_wrapper(void* object)
{
    reinterpret_cast<MySQLAdapter*>(object)->QueryThread((void*) 0);
    return nullptr;
}

