/*
 * JSON_FindAndAdd.cpp
 *
 *  Created on: Jun 28, 2018
 *      Author: RBS
 */

#include <iostream>
#include "JSON_FindAndAdd.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

bool FindAndAddTo(const rapidjson::Document& DOC, std::string Key, short* Variable)
{
	static rapidjson::Value::ConstMemberIterator itr;
	itr = DOC.FindMember(Key.c_str());
	if(itr !=DOC.MemberEnd()) {
		*Variable=short(itr->value.GetInt());
		return true;
	}
	else {
		std::cout << "Json parser: Can't find member " << Key << std::endl;
		return false;
	}
}

bool FindAndAddTo(const rapidjson::Document& DOC, std::string Key, int* Variable)
{
	static rapidjson::Value::ConstMemberIterator itr;
	itr = DOC.FindMember(Key.c_str());
	if(itr !=DOC.MemberEnd()) {
		*Variable=itr->value.GetInt();
		return true;
	}
	else {
		std::cout << "Json parser: Can't find member " << Key << std::endl;
		return false;
	}
}

bool FindAndAddTo(const rapidjson::Document& DOC, std::string Key, long* Variable)
{
	static rapidjson::Value::ConstMemberIterator itr;
	itr = DOC.FindMember(Key.c_str());
	if(itr !=DOC.MemberEnd()) {
		*Variable=long(itr->value.GetInt());
		return true;
	}
	else {
		std::cout << "Json parser: Can't find member " << Key << std::endl;
		return false;
	}
}

bool FindAndAddTo(const rapidjson::Document& DOC, std::string Key, std::string* Variable)
{
	static rapidjson::Value::ConstMemberIterator itr;
	itr = DOC.FindMember(Key.c_str());
	if(itr !=DOC.MemberEnd()) {
		*Variable=itr->value.GetString();
		return true;
	}
	else {
		std::cout << "Json parser: Can't find member " << Key << std::endl;
		return false;
	}
}

bool FindAndAddTo(const rapidjson::Document& DOC, std::string Key, bool* Variable)
{
	static rapidjson::Value::ConstMemberIterator itr;
	itr = DOC.FindMember(Key.c_str());
	if(itr !=DOC.MemberEnd()) {
		*Variable=itr->value.GetBool();
		return true;
	}
	else {
		std::cout << "Json parser: Can't find member " << Key << std::endl;
		return false;
	}
}


