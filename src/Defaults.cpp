/*
 *  Defaults.cpp
 *  DefaultsTest
 *
 *  Created by Ken Kopecky on 7/22/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "Defaults.h"

#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <osgDB/fstream>
#include <vector>
#include <climits>
#include <cctype>
#include <assert.h>
#include <fstream>
#include <sstream>
#include <iostream>

using std::string;
using std::vector;
//using namespace std;
//utility functions...written by Joe Heiniger for the Radius project

void
toUpper(std::string& s)
{
	for (size_t i=0; i<s.size(); i++)
	{
		if (isalpha(s[i]) && !isupper(s[i]))
			s[i] = s[i] - 'a' + 'A';
	}
}

std::vector<std::string>
tokenize(	const string& str,  
			const string& delimiters )
{
	vector<string> tokens;
	// Skip delimiters at beginning.
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	string::size_type pos     = str.find_first_of(delimiters, lastPos);
	
	while (string::npos != pos || string::npos != lastPos)
	{
		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}

	return tokens;
}


std::vector<std::string>
quoteTokenize(	const std::string& str )
{
	std::vector<std::string> firstTokens = tokenize(str, "\"");
	std::vector<std::string> finalTokens;
	bool inLiteral = false;
	for (size_t i=0; i<firstTokens.size(); ++i)
	{
		if (!inLiteral)
		{
			std::vector<std::string> tempTokens = tokenize(firstTokens[i], " \t");
			finalTokens.insert(finalTokens.end(), tempTokens.begin(), tempTokens.end());
		}
		else
		{
			finalTokens.push_back(firstTokens[i]);
		}
		inLiteral = !inLiteral;
	}
	
	for(size_t i = 0; i < finalTokens.size(); i++)
	{
		//remove any fake newlines from the tokens and turn them real newlines
		size_t pos = finalTokens[i].find("\\n");
		while( pos != std::string::npos)
		{
//			printf("token going in:  %s\n", finalTokens[i].c_str());
			finalTokens[i] = finalTokens[i].substr(0, pos) + "\n" + finalTokens[i].substr(pos + 2);
//			printf("token going out:  %s\n", finalTokens[i].c_str());
			pos = finalTokens[i].find("\\n");
		}
	}
	
	return finalTokens;
}

float stringToFloat(const std::string& str)
{
	std::istringstream isstream(str);
	float t;
	isstream >> t;
	return t;
}

//ken's code begins here

//pull spaces and things out of the ends of a string
std::string cleanUp(std::string in)
{
	while(in.size() && (in[0] == ' ' || in[0] == '	'|| in[in.size() - 1] == '\n'))
		in.erase(0, 1);
	while(in.size() && (in[in.size() - 1] == ' ' || in[in.size() - 1] == '	' || in[in.size() - 1] == '\n' || in[in.size() - 1] == 13))
		in.erase(in.size() - 1, 1);

	return in;
}

Defaults::~Defaults()
{
	//upon quitting, print whatever Defaults we couldn't find

	if(mDefaultsWeCouldntFind.size())
	{	
		printf("\n--------Defaults Summary--------\n");
		//if we never loaded a file,but requested Defaults anyway...give a severe warning.  
		//the programmer needs to fix this asap
		if(!mLoadedSomething)
		{
			printf("Defaults Error!  Defaults were requested but none were found");
			printf(" because a Defaults file was never loaded!");
	
		}
		else
		{
			//put all these warnings in a pretty box since they may be the last
			//thing the user sees
			
			printf("The following Defaults were requested by the program, but\n");
			printf("were not found in the file %s:\n\n", mFileName.c_str());
			printf(" ___________________________________.____________________________ \n");
			printf("|               Name                |        Data Type           |\n");
			printf("|___________________________________|____________________________|\n");
			std::map<std::string, std::string>::iterator i;
			for(i = mDefaultsWeCouldntFind.begin(); i != mDefaultsWeCouldntFind.end(); ++i)
				printf("|%-35s|     %-23s|\n", i->first.c_str(), i->second.c_str());
			printf("|___________________________________|____________________________|\n\n");

		}
	}
	
	//in this case, we requested a file, but it wasn't found
	//this could be an mistake on the user's end
	if(mDefaultsFileWasFound == false && mLoadedSomething == true)
	{
		printf("Warning!  The Defaults file\n \"%s\" \nwasn't found. ", mFileName.c_str());
		printf(" Please make sure it exists.\n");
	
	}

}

bool Defaults::reload()
{
	mLoadedSomething = true; 
	mDefaultsWeCouldntFind.clear();
	mData.clear();
	osgDB::ifstream infile(mFileName.c_str());						//try open the file
	if (!infile)
	{
		printf("Unable to find Defaults file %s\n", mFileName.c_str());
		mDefaultsFileWasFound = false;
		return false;
	}
	mDefaultsFileWasFound = true;
	//printf("Defaults:  Loading %s...\n", mFileName.c_str());
	
	std::string line;
	int lineNum = 0;
	while(infile.peek() != EOF)									//go through every line of the file
	{
		lineNum++;
		std::getline(infile, line);
		//printf("%i:	%s\n", lineNum, line.c_str());
		line = cleanUp(line);
		if ("" == line || '/' == line[0] || '#' == line[0] || line[0] < 'A' || line[0] > 'z' )
			continue;


		std::vector<std::string> tokens = tokenize(line, "=");
		std::string tag = tokens[0];							//first token is the tag
		toUpper(tag);
		tag = cleanUp(tag);
		if(tokens.size() > 1)
		{
			std::pair<std::string, std::string> newEntry(tag, cleanUp(tokens[1]));
			mData.insert(newEntry);
		
		
		}
	}
	return true;
}

void Defaults::printAll()
{
	std::map<std::string, std::string>::iterator iter;
	for(iter = mData.begin(); iter != mData.end(); ++iter)
		printf("%s	=	%s\n", iter->first.c_str(), iter->second.c_str());


}

//issue a very stern warning if we request a setting before the file has been specified!
//this would be a programming error that must be fixed ASAP
void Defaults::checkForEarlySettingRequest()
{
	if(!mLoadedSomething)
	{
		bool Defaults_file_has_been_loaded_internally_before_requesting_a_setting = false;
		printf("\n\n********Internal Error********\n");
		printf("Requested a setting before we even tried to load a file.\n");
		printf("Please contact the support for this software.\n");
		printf("******************************\n");
		assert(Defaults_file_has_been_loaded_internally_before_requesting_a_setting == true);
	
	}

}

bool Defaults::getValue(std::string name, int& value)
{
	checkForEarlySettingRequest();
	std::string originalName = name;
	toUpper(name);
	std::string valString;
	if (mData.find(name) == mData.end())			//check to see if we've loaded this one already
	{
		mDefaultsWeCouldntFind[originalName] = "Integer Value";
		return false;
	}
	else
		valString = mData.find(name)->second;
	int ken;
	char str[48];
	sprintf(str, "%s", valString.c_str());
	sscanf(str, "%i", &ken);
	value = ken;
	return true;
}

bool Defaults::getValue(std::string name, float &value)
{
	checkForEarlySettingRequest();
	std::string originalName = name;
	toUpper(name);
	std::string valString;
	
	if (mData.find(name) == mData.end())			//check to see if we've loaded this one already
	{
		mDefaultsWeCouldntFind[originalName] = "Decimal Value";
		return false;
	}
	else
		valString = mData.find(name)->second;
	float ken;
	char str[48];
	sprintf(str, "%s", valString.c_str());
	sscanf(str, "%f", &ken);
	value = ken;
	return true;
}

bool Defaults::getValue(std::string name, bool &value)
{
	checkForEarlySettingRequest();
	std::string originalName = name;
	toUpper(name);
	std::string valString;
	
	if (mData.find(name) == mData.end())			//check to see if we've loaded this one already
	{
		mDefaultsWeCouldntFind[originalName] = "Boolean (true or false)";
		return false;
	}
	else
		valString = mData.find(name)->second;
	int ken;
	toUpper(valString);

	//check for true, false, yes, and no
	if("FALSE" == valString || valString == std::string("NO"))
	{
		value = false;

		return true;		//true, we found a value.  and it was false
	}
	
	if(valString == std::string("TRUE") || valString == std::string("YES"))
	{
		value = true;

		return true;		//true, we found a value.  and it was false
	}
	
	//if none of those were found, check for an integer value
	char str[48];
	sprintf(str, "%s", valString.c_str());
	sscanf(str, "%i", &ken);
	value = (ken != 0);
	return true;
}

bool Defaults::getValue(std::string name, std::string &value)
{
	checkForEarlySettingRequest();
	std::string originalName = name;
	toUpper(name);
	
	if (mData.find(name) == mData.end())			//check to see if we've loaded this one already
	{
	
		mDefaultsWeCouldntFind[originalName] = "Text String";
		return false;
	}
	else
	{
		value = mData.find(name)->second;
		
		//trim off starting and ending quotes
		if(value.size() >= 2)
		{
			if(value[0] == '"') value = value.substr(1);
			if(value[value.size()-1] == '"') value = value.substr(0, value.length()-1);
		}
	}
	return true;
}
