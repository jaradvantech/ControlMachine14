/*
 * ConfigParser.h
 *
 *  Created on: Mar 19, 2018
 *      Author: RBS
 */

#ifndef CONFIGPARSER_H_
#define CONFIGPARSER_H_

#include <string>

class ConfigParser {
	public:
		ConfigParser();
		ConfigParser(std::string);
		void SetConfigFilename(std::string);
		std::vector<int> GetArmPositions();
		std::vector<int> GetServerPorts();
		std::vector<std::string> GetServerIPs();
		std::vector<int>GetManipulatorModes();
		int GetPackagingGrade();
		int GetPackagingColor();
		void SetManipulatorModes(std::vector<int>);
		int SetCurrentPackagingGrade(int);
		int SetCurrentPackagingColor(int);
		int SaveConfig(std::vector<int> ArmPositionsVector, std::vector<std::string> RFIDIPAddresses, std::vector<int> RFIDPorts);

	private:
		std::string _Filename;
		template<typename T> std::vector<T>		ReadLineAndAddToVector(int line);

};


#endif /* CONFIGPARSER_H_ */
