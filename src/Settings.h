/*
 *  Settings.h
 *  VeldtMonitor
 *
 *  Created by Kenneth Kopecky on 12/11/11.
 *  Copyright 2011 Orange Element Studios. All rights reserved.
 *
 */

#ifndef SETTINGS_THAT_USE_XML
#define SETTINGS_THAT_USE_XML

#include <string>

#include "KenXML.h"


namespace Settings
{
	bool loadFile(std::string name);
	bool saveFile();
	TiXmlNode* __settingsNode();

template<typename t> bool get(std::string valName, t& value)
	{
		if(!__settingsNode()) return false;
		TiXmlNode* which = __settingsNode()->FirstChild(valName);
		if(!which)
		{
			printf("Unable to find setting called %s\n", valName.c_str());
			return false;
		
		
		}
		return KenXML::getValueFromNode(valName, __settingsNode(), value);
	
	}
	
	
	template<typename t> void set(std::string valName, t value)
	{
		if(!__settingsNode()) return;
		TiXmlNode* newOne = KenXML::makeElementWithNameAndValue(valName, value);
//		printf("adding setting for %s\n", valName.c_str());
		TiXmlNode* which = __settingsNode()->FirstChild(valName);		
		if(which)	//remove this one if it exists.  we'll be overwriting it
			__settingsNode()->RemoveChild(which);
		__settingsNode()->LinkEndChild(newOne);
	
	}
	

	std::string &__settingsName();
}
#endif
