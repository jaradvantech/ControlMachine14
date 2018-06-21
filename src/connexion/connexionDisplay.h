/*
 * connexionDisplay.h
 *
 *  Created on: Dec 22, 2017
 *      Author: Jose Andres Grau Martinez
 */

#include <string>

#ifndef CONNEXION_CONNEXIONDISPLAY_H_
#define CONNEXION_CONNEXIONDISPLAY_H_

std::string ProcessCommand(std::string const& bufferRead, bool ServerIsReady);

void InitDisplayParser();


#endif /* CONNEXION_CONNEXIONDISPLAY_H_ */
