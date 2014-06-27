/*
 *  ShaderManager.cpp
 *  DP3D
 *
 *  Created by Kenneth Kopecky on 5/16/08.
 *  Copyright (C) 2004-2011 Ken Kopecky and Iowa State University
 *  All rights reserved.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *
 */

#include "ShaderManager.h"
#include <osgDB/FileUtils>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include "Util.h"
using namespace osg;

osg::Program* ShaderManager::loadProgram(std::string filename)
{	
	//if we've loaded this shader before, just return that
	//std::map<std::string, osg::ref_ptr<osg::Program > >:: iterator iter;
	if(mPrograms.find(filename) != mPrograms.end())
		return mPrograms[filename].get();

	//if we're here, we haven't loaded it
	std::string vertName = osgDB::findDataFile(filename + ".vert");
	std::string fragName = osgDB::findDataFile(filename + ".frag");
	mPrograms[filename] = new osg::Program;
	mPrograms[filename]->setName(filename);
	osg::Shader* vertShader = NULL;
	osg::Shader* fragShader = NULL;
	bool foundSomething = false;
	if(vertName != "")
	{
		vertShader = new osg::Shader(osg::Shader::VERTEX);
		vertShader->setName(vertName.c_str());
		vertShader->setShaderSource(loadAndProcessFile(vertName));
		mPrograms[filename]->addShader(vertShader);
		foundSomething = true;
	}
	else printf("Couldn't find %s\n", std::string(filename + ".vert").c_str());

	if(fragName != "")
	{
		fragShader = new osg::Shader(osg::Shader::FRAGMENT);
		fragShader->setName(fragName.c_str());
		fragShader->setShaderSource(loadAndProcessFile(fragName));	
		mPrograms[filename]->addShader(fragShader);
		foundSomething = true;
	
	}
	else printf("Couldn't find %s\n", std::string(filename + ".frag").c_str());
	
	if(!foundSomething)
		Util::logError("Couldn't find any shaders named %s\n", filename.c_str());
	return mPrograms[filename].get();
}

void ShaderManager::reloadShaders()
{
	std::map<std::string, osg::ref_ptr<osg::Program> >::iterator iter;
	printf("reloading all shaders...\n");
	for(iter = mPrograms.begin(); iter != mPrograms.end(); ++iter)
	{
		for(unsigned int i = 0; i < iter->second->getNumShaders(); i++)
		{
			osg::Shader* shader = iter->second->getShader(i);
			printf("reloading %s\n", shader->getName().c_str());
			shader->setShaderSource(loadAndProcessFile(shader->getName()));
		}	
	}
}

std::vector<std::string> fileToLines(std::string fileName)
{
	std::vector<std::string> lines;
	std::ifstream infile(fileName.c_str());						//try open the file
	if (!infile)
	{
		return lines;
	}
	std::string line;
	int lineNum = 0;
	while(infile.peek() != EOF)									//go through every line of the file
	{
		lineNum++;
		std::getline(infile, line);
		lines.push_back(line);
	}
	return lines;
}


std::string ShaderManager::loadAndProcessFile(std::string name)
{
	std::vector<std::string> lines = fileToLines(name);
	std::string wholeThing;
	if(!lines.size()) return "";
	
	for(unsigned int i = 0; i < lines.size(); i++)
	{
		std::string line = lines[i];
		if(line.find("#define") != std::string::npos)
		{
//			printf("%s is a preprocessor\n", line.c_str());
			char pname[256];
			pname[0] = NULL;
			char defn[256];
			defn[0] = NULL;
			int success = sscanf(line.c_str(), "#define %s %s", pname, defn);
			
			if(success == 2)		//this is a mutable definition.  we may have an override for it
			{
//				printf("scanned %i items and got %s = %s\n",success,  pname, defn);
				std::map<std::string, std::string>::iterator iter;
				for(iter = mDefines.begin(); iter != mDefines.end(); iter++)
				{
					if(std::string(pname) == iter->first)
					{
						//printf("replacing property %s (%s to %s)\n", pname, defn, iter->second.c_str());
						char newLine[1024];
						sprintf(newLine, "#define %s %s\n", pname, iter->second.c_str());
						line = newLine;
					}
				}
			}
		}
		wholeThing = wholeThing + line + "\n";	
	}
	
	return wholeThing;
}

void ShaderManager::addFragmentShaderToProgram(std::string programName, std::string fileName)
{
	Program* shader = loadProgram(programName);
	
	std::string fragName = osgDB::findDataFile(fileName);
	if(fragName != "")
	{
		Shader* moreShades = new Shader(Shader::FRAGMENT);	
		moreShades->setName(fragName.c_str());
		moreShades->setShaderSource(loadAndProcessFile(fragName));	
		shader->addShader(moreShades);
	}
	else printf("Couldn't find the shader file %s\n", fileName.c_str());
}

