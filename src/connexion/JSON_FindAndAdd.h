/*
 * JSON_FindAndAdd.h
 *
 *  Created on: Jun 28, 2018
 *      Author: RBS
 */

#ifndef CONNEXION_JSON_FINDANDADD_H_
#define CONNEXION_JSON_FINDANDADD_H_

#include <iostream>
#include "JSON_FindAndAdd.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

bool FindAndAddTo(const rapidjson::Document& DOC, std::string Key, short* Variable);
bool FindAndAddTo(const rapidjson::Document& DOC, std::string Key, int* Variable);
bool FindAndAddTo(const rapidjson::Document& DOC, std::string Key, long* Variable);
bool FindAndAddTo(const rapidjson::Document& DOC, std::string Key, std::string* Variable);
bool FindAndAddTo(const rapidjson::Document& DOC, std::string Key, bool* Variable);

#endif /* CONNEXION_JSON_FINDANDADD_H_ */
