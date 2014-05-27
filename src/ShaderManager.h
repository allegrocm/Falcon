/*
 *  ShaderManager.h
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

#ifndef KENOSGSHADERMANAGERH
#define KENOSGSHADERMANAGERH

#include <string>
#include <map>
#include <osg/Program>
#include <osg/Shader>
#include <osg/Uniform>
#include <osg/Node>
#include <stdio.h>

//class that manages OSG shaders for us.  returns osg::Programs
class ShaderManager
{
public:
	static osg::Program* loadShader(std::string filename)	{return ShaderManager::instance().loadProgram(filename); }
	static ShaderManager& instance()	{static ShaderManager s; return s;}

	//adds an additional source file to the code of a shader.  useful for sharing code between shaders
	void addFragmentShaderToProgram(std::string programName, std::string fileName);	
	static void applyShaderToNode(std::string filename, osg::Node* n, bool overrideOthers = false)
	{
		osg::Program* shader = loadShader(filename);
		n->getOrCreateStateSet()->setAttributeAndModes( shader, osg::StateAttribute::ON | (overrideOthers * osg::StateAttribute::OVERRIDE));
	}
	
	void reloadShaders();
	std::string loadAndProcessFile(std::string name);		//handle preprocessor defines in a file and replace them if we've overridden them
	osg::Program* loadProgram(std::string filename);
	
	//override #defined parameters in a shader
	void define(std::string pname, int value)	{char v[24]; sprintf(v,"%i",value);mDefines[pname] = v; reloadShaders();}
	void define(std::string pname, std::string value)	{mDefines[pname] = value; reloadShaders();}
protected:
	std::map<std::string, osg::ref_ptr<osg::Program> > mPrograms;
	std::map<std::string, std::string> mDefines;		//used to replace definitions included in files if we so choose
};

#endif
