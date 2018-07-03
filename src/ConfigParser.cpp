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
#include <cmath>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/spirit/home/support/detail/hold_any.hpp>
#include <boost/any.hpp>
#include "ConfigParser.h"

#define PALLETS  2 //How many pallets every manipulator has
#define CHANNELS 4 //How many channels every RFIS server has

/* RBS. Config file description
 * Line 1: Number of arms in this machine
 * Line 2: Position of the manipulators in encoder units
 * Line 3: IPs of RFID servers
 * Line 4: Ports of RFID servers
 * Line 5: Array of manipulator modes
 * Line 6: Grade of tile being currently packaged
 * Line 7: Color of tile being currently packaged
 * Line 8: IP address(es) of the PLC(s)
 * Line 9: Mechanical parameters. 0=SetAxisGoBackToTheWaitingPositionInAdvance
 * 								  1=StandByValue
 * Line 10: ...
 */

ConfigParser::ConfigParser()
{
}

ConfigParser::ConfigParser(std::string mFilename)
{
	this->_Filename = mFilename;
}

void ConfigParser::SetConfigFilename(std::string mFilename)
{
	this->_Filename = mFilename;
}


//--------------------------------------------------
//Use this functions as template

int ConfigParser::GetNumberOfArms()
{
	return ReadLineAndAddToVector<int>(1).at(0);
}

int ConfigParser::GetNumberOfRFIDservers()
{
	return ceil(((double)GetNumberOfArms()*PALLETS)/CHANNELS);
}

std::vector<int> ConfigParser::GetArmPositions()
{
	//If configuration is not complete, fill with defaults to avoid crashings.
	std::vector<int> saved_positions = ReadLineAndAddToVector<int>(2);
	std::vector<int> total_positions;
	int total_arms = GetNumberOfArms();
	int total_saved_arm_positions = saved_positions.size();
	for(int i=0; i<total_arms; i++)
	{
		if(i<total_saved_arm_positions)
			total_positions.push_back(saved_positions.at(i));
		else
			total_positions.push_back(0); //Default
	}
	return total_positions;
}

void ConfigParser::SetArmPositions(std::vector<int> ArmPositions)
{
	ReadVectorAndAddToTheFile<int>(ArmPositions,2);
}


std::vector<std::string> ConfigParser::GetServerIPs()
{
	//If configuration is not complete, fill with defaults to avoid crashings.
	std::vector<std::string> saved_IPs = ReadLineAndAddToVector<std::string>(3);
	std::vector<std::string> total_IPs;
	int total_servers = GetNumberOfRFIDservers();
	int total_saved_IPs = saved_IPs.size();
	for(int i=0; i<total_servers; i++)
	{
		if(i<total_saved_IPs)
			total_IPs.push_back(saved_IPs.at(i));
		else
			total_IPs.push_back("192.168.0.0"); //Default
	}

	return total_IPs;
}

void ConfigParser::SetServerIPs(std::vector<std::string> ServerIPs)
{
	ReadVectorAndAddToTheFile<std::string>(ServerIPs,3);
}


std::vector<int>ConfigParser::GetServerPorts()
{
	//If configuration is not complete, fill with defaults to avoid crashings.
	std::vector<int> saved_ports = ReadLineAndAddToVector<int>(4);
	std::vector<int> total_ports;
	int total_servers = GetNumberOfRFIDservers();
	int total_saved_servers = saved_ports.size();
	for(int i=0; i<total_servers; i++)
	{
		if(i<total_saved_servers)
			total_ports.push_back(saved_ports.at(i));
		else
			total_ports.push_back(0); //Default
	}
	return total_ports;
}

void ConfigParser::SetServerPorts(std::vector<int> ServerPorts)
{
	ReadVectorAndAddToTheFile<int>(ServerPorts,4);
}


std::vector<int>ConfigParser::GetManipulatorModes()
{
	//If configuration is not complete, fill with defaults to avoid crashings.
	std::vector<int> saved_modes = ReadLineAndAddToVector<int>(5);
	std::vector<int> total_modes;
	int total_arms = GetNumberOfArms();
	int total_saved_modes = saved_modes.size();
	for(int i=0; i<total_arms; i++)
	{
		if(i<total_saved_modes)
			total_modes.push_back(saved_modes.at(i));
		else
			total_modes.push_back(0); //Default
	}
	return total_modes;
}

void ConfigParser::SetManipulatorModes(std::vector<int> ManipulatorModes)
{
	ReadVectorAndAddToTheFile<int>(ManipulatorModes,5);
}

//TODO TRYCATCH?
int ConfigParser::GetPackagingGrade()
{
	return ReadLineAndAddToVector<int>(6).at(0);
}

int ConfigParser::GetPackagingColor()
{
	return ReadLineAndAddToVector<int>(7).at(0);
}

void ConfigParser::SetCurrentPackagingGrade(int PackagingGrade)
{
	std::vector<int> TempVector={PackagingGrade};
	ReadVectorAndAddToTheFile<int>(TempVector,6);
}

void ConfigParser::SetCurrentPackagingColor(int PackagingColor)
{
	std::vector<int> TempVector={PackagingColor};
	ReadVectorAndAddToTheFile<int>(TempVector,7);
}

std::string ConfigParser::GetPLCAddress()
{
	std::string PLC_adddress; //TODO replace this with a function in config parser to load default parameters
	try
	{
		PLC_adddress = ReadLineAndAddToVector<std::string>(8).at(0);
	}
	catch(const std::out_of_range& exc)
	{
		PLC_adddress = "000.000.000.000"; //default
	}
	return PLC_adddress;
}

void ConfigParser::SetPLCAddress(std::string PLC_address)
{
	std::vector<std::string> TempVector={PLC_address};
	ReadVectorAndAddToTheFile<std::string>(TempVector,8);
}

std::vector<int> ConfigParser::GetMechanicalParameters()
{
	//If configuration is not complete, fill with defaults to avoid crashings.
	std::vector<int> saved_parameters = ReadLineAndAddToVector<int>(9);
	std::vector<int> total_parameters;
	int total_saved_parameters = saved_parameters.size();
	for(int i=0; i<2; i++)
	{
		if(i<total_saved_parameters)
			total_parameters.push_back(saved_parameters.at(i));
		else
			total_parameters.push_back(0); //Default
	}
	return total_parameters;
}

void ConfigParser::SetMechanicalParameters(std::vector<int> MechanicalParameters)
{
	ReadVectorAndAddToTheFile<int>(MechanicalParameters,9);
}

//--------------------------------------------------

template<typename T>
std::vector<T> ConfigParser::ReadLineAndAddToVector(int line)
{
	std::ifstream InputStream(_Filename);
	std::vector<T> VectorToReturn;

	if(InputStream.is_open())
	{
		std::string CurrentValue, Line;

		//Skip n lines
		for(int i=1; i<=line; i++)
					std::getline(InputStream, Line);
		//all is separated by commas
		//Create sstream object from read line to ease parsing.
		std::stringstream LineToParse(Line);

		//Populate vector with
		bool moreinfo=true;
		while(moreinfo)
		{
			std::getline(LineToParse, CurrentValue, ',');
			if(CurrentValue.size()==0)moreinfo=false; //if the thing that we are reading is between 2 commas, its size must be 0
			else VectorToReturn.push_back(boost::lexical_cast<T>(CurrentValue)); //The first server will be at the position 0 of the vector
		}
		InputStream.close();
	}
	else
		std::cout << "Can't access config file " << _Filename << std::endl;
	return VectorToReturn;
}

template<typename T>
void ConfigParser::ReadVectorAndAddToTheFile(std::vector<T> VectorToAdd,int line)
{
	//Read the whole original file
	//Form a string with all the lines from 0 to the line that we want to write
	//Append our line
	//Append to the string the lines from the next line till end
	//Overwrite the file
	std::ifstream InputStream(_Filename);
	std::string Line;
	std::string totalText;
	if(InputStream.is_open())
	{
		for(int i=1; i<line; i++)
		{
			std::getline(InputStream, Line);
			totalText+=Line;
			totalText+="\n";
		}
		std::getline(InputStream, Line);
		for(unsigned int i=0;i<VectorToAdd.size();i++)
		{
			totalText+=boost::lexical_cast<std::string>(VectorToAdd.at(i));
			totalText+=",";
		}
		totalText+=",";
		totalText+="\n";

		int i=line;
		while(std::getline(InputStream, Line))
		{
			totalText+=Line;
			totalText+="\n";
			i++;
		}
		InputStream.close();
	}

	std::ofstream OutputStream(_Filename);
	if(OutputStream.is_open())
	{
		OutputStream<<totalText;
		OutputStream.close();
	}
}

