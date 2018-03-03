/*
 * connexionDisplay.h
 *
 *  Created on: Dec 22, 2017
 *      Author: Jose Andres Grau Martinez
 */

#include <string>

#ifndef CONNEXION_CONNEXIONDISPLAY_H_
#define CONNEXION_CONNEXIONDISPLAY_H_

std::string Command_RGMV(std::string const& Buffer);

std::string Command_RFMV(std::string const& Buffer);

std::string Command_RAMV(std::string const& Buffer);

std::string Command_RDMV(std::string const& Buffer);

std::string Command_RGUV(std::string const& Buffer);

std::string Command_RPRV(std::string const& Buffer);

#endif /* CONNEXION_CONNEXIONDISPLAY_H_ */
