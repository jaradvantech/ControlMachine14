/*
 * ConfigParser.cpp
 *
 *  Created on: Mar 19, 2018
 *      Author: RBS
 */

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include "ConfigParser.h"

ConfigParser::ConfigParser()
{
}

ConfigParser::ConfigParser(std::string mFilename)
{
	this->Filename = mFilename;
}

void ConfigParser::SetConfigFilename(std::string mFilename)
{
	this->Filename = mFilename;
}

int ConfigParser::GetArmNumber()
{
	std::ifstream InputStream(Filename);
	int retval = -1;
	if(InputStream.is_open())
	{
		std::string Line;

		//First line is number of arms
		std::getline(InputStream, Line);
		retval = boost::lexical_cast<int>(Line);

		InputStream.close();
	}
	else
		std::cout << "GetArmNumber(): Can't access config file " << Filename << std::endl;

	return retval;
}

int ConfigParser::GetServerNumber()
{
	std::ifstream InputStream(Filename);
	int retval = -1;
	if(InputStream.is_open())
	{
		std::string Line;

		//Skip one line
		std::getline(InputStream, Line);
		//Second line is the number of RFID servers
		std::getline(InputStream, Line);
		retval = boost::lexical_cast<int>(Line);

		InputStream.close();
	}
	else
		std::cout << "GetServerNumber(): Can't access config file " << Filename << std::endl;

	return retval;
}

int * ConfigParser::GetServerPorts()
{
	std::ifstream InputStream(Filename);
	int * ServerPortsArray = NULL;

	if(InputStream.is_open())
	{
		std::string CurrentValue, Line;

		//Skip two lines
		for(int i=0; i<2; i++)
			std::getline(InputStream, Line);
		//third line is the RFID server ports in use, separated by commas
		std::getline(InputStream, Line);

		//Create sstream object from read line to ease parsing.
		std::stringstream LineToParse(Line);

		//Create array and fill with data from file
		int ServerNumber = GetServerNumber();
		ServerPortsArray = new int[ServerNumber];
		for(int i=0; i<ServerNumber; i++)
		{
			std::getline(LineToParse, CurrentValue, ',');
			ServerPortsArray[i] = boost::lexical_cast<int>(CurrentValue);
		}
		InputStream.close();
	}
	else
		std::cout << "GetServerPorts(): Can't access config file " << Filename << std::endl;

	return ServerPortsArray;
}

std::string * ConfigParser::GetServerIPs()
{
	std::ifstream InputStream(Filename);
	std::string * ServerIPArray = NULL;

	if(InputStream.is_open())
	{
		std::string CurrentValue, Line;

		//Skip three lines
		for(int i=0; i<3; i++)
			std::getline(InputStream, Line);
		//4th line is the RFID server IP's, separated by commas
		std::getline(InputStream, Line);

		//Create sstream object from read line to ease parsing.
		std::stringstream LineToParse(Line);

		//Create array and fill with data from file
		int ServerNumber = GetServerNumber();
		ServerIPArray = new std::string[ServerNumber];
		for(int i=0; i<ServerNumber; i++)
		{
			std::getline(LineToParse, CurrentValue, ',');
			ServerIPArray[i] = CurrentValue;
		}
		InputStream.close();
	}
	else
		std::cout << "GetServerIPs(): Can't access config file " << Filename << std::endl;

	return ServerIPArray;
}
/* TODO; implement

int SaveConfig(std::string Filename)
{
	std::ofstream OutputStream(Filename);
	int retVal = 0;

	if(OutputStream.is_open())
	{
		//Save number of arms
		OutputStream << ArmNumber << std::endl;

		//Save number of servers
		OutputStream << RFIDServers << std::endl;

		//Save Ports
		for(int i=0; i<ArmNumber; i++)
		{
			OutputStream << std::to_string(RFIDPorts[i]) << ",";
		}
		OutputStream << std::endl;

		//Save IP's
		for(int i=0; i<ArmNumber; i++)
		{
			OutputStream << RFIDIPAddresses[i] << ",";
		}
		OutputStream << std::endl;


		OutputStream.close();
	}

	return retVal;
}
*/

