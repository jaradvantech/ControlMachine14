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
#include <boost/spirit/home/support/detail/hold_any.hpp>
#include <boost/any.hpp>
#include "ConfigParser.h"



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

std::vector<int> ConfigParser::GetArmPositions()
{
	return ReadLineAndAddToVector<int>(1);
}

void ConfigParser::SetArmPositions(std::vector<int> ArmPositions)
{
	ReadVectorAndAddToTheFile<int>(ArmPositions,1);
}


std::vector<std::string> ConfigParser::GetServerIPs()
{

	return ReadLineAndAddToVector<std::string>(2);
}

void ConfigParser::SetServerIPs(std::vector<std::string> ServerIPs)
{
	ReadVectorAndAddToTheFile<std::string>(ServerIPs,2);
}


std::vector<int>ConfigParser::GetServerPorts()
{
	return ReadLineAndAddToVector<int>(3);
}

void ConfigParser::SetServerPorts(std::vector<int> ServerPorts)
{
	ReadVectorAndAddToTheFile<int>(ServerPorts,3);
}


std::vector<int>ConfigParser::GetManipulatorModes()
{
	return ReadLineAndAddToVector<int>(4);
}

void ConfigParser::SetManipulatorModes(std::vector<int> ManipulatorModes)
{
	ReadVectorAndAddToTheFile<int>(ManipulatorModes,4);
}


int ConfigParser::GetPackagingGrade()
{
	return ReadLineAndAddToVector<int>(5).at(0);
}

int ConfigParser::GetPackagingColor()
{
	return ReadLineAndAddToVector<int>(6).at(0);
}

void ConfigParser::SetCurrentPackagingGrade(int PackagingGrade)
{
	std::vector<int> TempVector={PackagingGrade};
	ReadVectorAndAddToTheFile<int>(TempVector,5);
}

void ConfigParser::SetCurrentPackagingColor(int PackagingColor)
{
	std::vector<int> TempVector={PackagingColor};
	ReadVectorAndAddToTheFile<int>(TempVector,6);
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

