/*
 *  Defaults.h
 *  DefaultsTest
 *
 *  Created by Ken Kopecky on 7/22/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

//holds Defaults for apps that are stored and accessed via an std::map
#ifndef DefaultsKENHHH
#define DefaultsKENHHH
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <climits>
#include <cctype>

//a shortcut
#define getDefault(name, val) Defaults::instance().getValue(name, val)

class Defaults
{
public:  
	static bool load(std::string file) {Defaults::instance().mFileName = file; return Defaults::instance().reload();}
	static Defaults& instance()	{static Defaults s; return s;}
	//Defaults(std::string file, bool &success);
	bool reload();
	bool getValue(std::string name, int& value);
	bool getValue(std::string name, float &value);
	bool getValue(std::string name, std::string &value);
	bool getValue(std::string name, bool &value); 
	void printAll();
	
protected:
	Defaults()	{mLoadedSomething = false; mDefaultsFileWasFound = false;}
	~Defaults();
	std::string mFileName;
	std::map<std::string, std::string> mData;
	
	//name and type of Defaults we weren't able to load
	std::map<std::string, std::string> mDefaultsWeCouldntFind;
	bool mLoadedSomething;	//did we ever TRY to load a file?
	bool mDefaultsFileWasFound;
	
	void checkForEarlySettingRequest();


};

//helper function prececlarations
void toUpper(std::string& s);

std::vector<std::string>tokenize( const std::string& str, const std::string& delimiters );
std::vector<std::string> quoteTokenize(	const std::string& str );
//pull spaces and things out of the ends of a string
std::string cleanUp(std::string in);
float stringToFloat(const std::string& str);

#endif
