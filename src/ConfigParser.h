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
		int GetArmNumber();
		int GetServerNumber();
		int * GetServerPorts();
		std::string * GetServerIPs();

	private:
		std::string Filename;
};


#endif /* CONFIGPARSER_H_ */
