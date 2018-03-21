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

std::vector<int> ConfigParser::GetArmPositions()
{
	return ReadLineAndAddToVector<int>(1);
}



std::vector<std::string> ConfigParser::GetServerIPs()
{

	return ReadLineAndAddToVector<std::string>(2);
}

std::vector<int>ConfigParser::GetServerPorts()
{
	return ReadLineAndAddToVector<int>(3);
}

int ConfigParser::SaveConfig(std::vector<int> ArmPositionsVector, std::vector<std::string> RFIDIPAddresses, std::vector<int> RFIDPorts)
{
	/*
	//save types:

	//Get the old configuration
	//Prepare the old configuration in an easy to iterate way
	std::vector<boost::spirit::hold_any> OldVectorsToOperate;
	OldVectorsToOperate.push_back(GetArmPositions());
	OldVectorsToOperate.push_back(GetServerIPs());
	OldVectorsToOperate.push_back(GetServerPorts());

	//Prepare the new configuration in an easy to iterate way
	std::vector<boost::any>  NewVectorsToOperate;
	NewVectorsToOperate.push_back(ArmPositionsVector);
	NewVectorsToOperate.push_back(RFIDIPAddresses);
	NewVectorsToOperate.push_back(RFIDPorts);


	//Prepare the new configuration in an easy to iterate way

	std::ofstream OutputStream(_Filename);
	if(OutputStream.is_open())
	{
		//loop through all what we have to write
		for(uint i=0;i<NewVectorsToOperate.size()-1;i++){//For every vector to operate
			for(uint j=0;j<boost::any_cast<std::vector<boost::any>>(NewVectorsToOperate[i]).size()-1;j++){//For every item in the vector to operate
				OutputStream<<boost::any_cast<std::vector<boost::spirit::hold_any>>(NewVectorsToOperate[i])[j] << ",";
			}
			OutputStream<<",\n";
		}
		OutputStream<<"\n";
		OutputStream.close();
	}
	else
	{
		std::cout << "Error: can't write file " << _Filename << std::endl;
		return 0;
	}
	return 1;
	*/
}

template<typename T> std::vector<T>
ConfigParser::ReadLineAndAddToVector(int line)
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
