
/*
 *  Util.h
 *
 *  Created by Kenneth Kopecky on 5/22/13.
 *  Copyright 2013 Iowa State University. All rights reserved.
 *
 */

#ifndef __UTIL__
#define __UTIL__
#ifdef _WIN32
#include <windows.h>
#include <gl/glut.h>
#endif

#include <iostream>
#include <osg/MatrixTransform>
#include "quickprof.h"
#include <osg/Geometry>

//for tracking crashes when we don't have an easily accessible stacktrace
//uncomment this line for tons of info to be dumped into the console and/or log file
//#define TraceLine(s) std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << s << std::endl;
#define TraceLine(s) 



//			For profiling, put __FUNCTION_HEADER__ at the top of major functions
//			This can be called dozens of times per frame without any real overhead
//			but void calling it, say, 50,000 times per second.  That'll slow us down
//			You can also use __FUNCTION_HEADER2__(functionName) if the functio name isn't specific enough
//			finally, you can do ScopedBlock(name) inside {} to time individual parts of functions easily

#define __FUNCTION_HEADER__ ScopedBlock __SCOPER__(__FUNCTION__);  
#define __FUNCTION_HEADER2__(name) ScopedBlock __SCOPER__(std::string(__FUNCTION__)+name);  


//starts a block and then stops it when it goes out of scope
//useful for profiling individual functions
	class ScopedBlock
	{
	public:
		ScopedBlock(std::string name);
		~ScopedBlock();
		std::string mBlockName;
	};



//holds a triangle for field calculations
class OSGTri
{
public:
	OSGTri(){}
	OSGTri(osg::Vec3 V0, osg::Vec3 V1, osg::Vec3 V2){v0 = V0; v1 = V1; v2 = V2;}
	void setTexes(osg::Vec2 t, osg::Vec2 tt, osg::Vec2 ttt)	{t0 = t; t1 = tt; t2 = ttt;}
	osg::Vec3 v0;
	osg::Vec3 v1;
	osg::Vec3 v2;
	
	osg::Vec2 t0;
	osg::Vec2 t1;
	osg::Vec2 t2;
	
	float getArea();
	float getTextureArea();
	osg::Vec3 getCenter();
};


//some utility functions!  We'll add more as we need them
namespace Util
{


	//error reporting function.  Re-prints important messages when the program is about to shut down
	//use this from anywhere in the program for when something happens that needs attention
	void logError(const char* format, ...);
	void printMatrix(osg::Matrixf m);
	void printNodeHierarchy(osg::Node* n);							//for looking at a node's structure within your app
	void cullSmallGeodes(osg::Node* n, float threshold);
	osg::Node* findNodeWithName(osg::Group* g, std::string name);	//find a node in this group with the given name

	std::string findDataFile(std::string name);						//error reporting version of OSG"s finddatafile function
	
	
	//number generation
	float random(float minVal, float maxVal);
	osg::Vec3 randomVector();			//a random unit vector
	//this loads a model with the given name, and scales, rotates, and translates it (in the order of the arguments) to a useable state
	//handy for importing downloaded models which are usually way way way too big
	//(rotations are in degrees!)
	osg::MatrixTransform*		loadModel(std::string name, float scale, float rotX = 0, float rotY = 0, float rotZ = 0, osg::Vec3 translate = osg::Vec3());

	//print out logged errors.  This is usually called right before the program exits
	void printErrors();

	void clearLoadedModels();		//calling this before exit may help us exit more smoothly
	std::string stringWithFormat(const char* format, ...);
	osg::Matrixf getCumulativeTransform(osg::Node* from, osg::Node* to);		//transform from into to's space
	//for finding the "center" of an object
	float areaOfTriangle(osg::Vec3 v0, osg::Vec3 v1, osg::Vec3 v2);
	std::vector<OSGTri> turnNodeToTriangles(osg::Node* node);
	osg::Vec4 getNodeCG(osg::Node* node, osg::Node* topLevel);	//get the center of gravity of this node relative to the topLevel node
	
	osg::Matrixf getTransform(osg::Node* n);				//gets matrix from a matrixtransform, or turns a PAT into a matrix
	void deCull(osg::Node* n);					//recursively turn off culling.  experimenting with using no culling
};
#endif /* defined(__OSGSample__Util__) */

