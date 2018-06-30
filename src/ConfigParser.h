/*
 * ConfigParser.h
 *
 *  Created on: Mar 19, 2018
 *      Author: RBS
 */

#ifndef CONFIGPARSER_H_
#define CONFIGPARSER_H_

#include <string>
#define CONFIG_FILE "/etc/unit14/unit14.conf"

class ConfigParser {
	public:
		ConfigParser();
		ConfigParser(std::string);
		void SetConfigFilename(std::string);

		int GetNumberOfArms();
		std::vector<int> GetArmPositions();
		void SetArmPositions(std::vector<int> ArmPositions);

		int GetNumberOfRFIDservers();
		std::vector<int> GetServerPorts();
		void SetServerPorts(std::vector<int> ServerPorts);
		std::vector<std::string> GetServerIPs();
		void SetServerIPs(std::vector<std::string> ServerIPs);

		std::vector<int> GetManipulatorModes();
		void SetManipulatorModes(std::vector<int> ManipulatorModes);

		int GetPackagingGrade();
		int GetPackagingColor();
		void SetCurrentPackagingGrade(int);
		void SetCurrentPackagingColor(int);

		std::string GetPLCAddress();
		void SetPLCAddress(std::string);

		std::vector<int> GetMechanicalParameters();
		void SetMechanicalParameters(std::vector<int> MechanicalParameters);

	private:
		std::string _Filename;
		template<typename T> std::vector<T>		ReadLineAndAddToVector(int line);
		template<typename T> void 				ReadVectorAndAddToTheFile(std::vector<T> VectorToAdd,int line);

};


#endif /* CONFIGPARSER_H_ */
