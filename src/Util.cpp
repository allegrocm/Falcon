
/*
 *  Util.cpp
 *
 *  Created by Kenneth Kopecky on 5/22/13.
 *  Copyright 2013 Iowa State University. All rights reserved.
 *
 */

// OSG Headers
#include <osg/Geode>
#include <osg/Geometry>
#include <osgDB/FileUtils>
#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>

// STL Headers
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

// Project Headers
#include "Util.h"

using namespace osg;
namespace  Util
{

std::vector<std::string> _loggedErrors;

void printNodeHierarchy(osg::Node* node)
{
	if(!node)
	{
		printf("%s:  null node supplied!\n", __FUNCTION__);
		return;
	}
	std::string name = node->getName();
	static int recursion = 0;
	static int line = 0;
	static int smalls = 0;
	if(!recursion)
	{
		smalls = 0;
		line = 0;

	}
		line++;
	int maxRecursion = 10;
	
	if(recursion >= maxRecursion)
	{
		
		return;
	}
	recursion++;

	//append dashes to the name to show the hierarchical structure
//	printf("%04i:", line);
	for(int i = 0; i < recursion - 1; i++)
		printf("-");
	printf("%s: %s", name.c_str(), node->className());
	
	//if we're not going to dive into this node, show its child count
	if(recursion >= maxRecursion)
	{
		if(node->asGroup())
		{
			int chillun = node->asGroup()->getNumChildren();		
		
			printf("--->%i children", chillun);
		}
	}
	
	if(std::string("Geode") == node->className())
	{
		float geodeSize = getNodeCG(node, node).w();
		printf("--area:  %.3f", geodeSize);
		if(geodeSize < 0.25)
		{
			smalls++;
			printf("->SMALL!");
		}
	}
	printf("\n");
	
	//for geodes, show the drawables
	if(std::string("Geode") == node->className())
	{
		osg::Geode* gio = (osg::Geode*)(node);
		for (unsigned int j = 0; j < gio->getNumDrawables(); ++j)
		{
			for(int i = 0; i < recursion; i++)
				printf("-");
			
			osg::Drawable* drawable = gio->getDrawable(j);
			// Cast the drawable to a geometry object.
			osg::Geometry* geom = dynamic_cast<osg::Geometry*>(drawable);
			const osg::Vec3Array* verts = (const osg::Vec3Array*)(geom->getVertexArray());
			if(verts != NULL)
			{

				printf("-Geometry--%i vertices\n", (int)verts->size());
			}
		}
	}
	
	//if this isn't a group, grab its bounding box and return it
	if(!node->asGroup() || recursion >= maxRecursion)
	{ 
		recursion--;
		return;
	}
	//otherwise dive in, child by child
	else
	{
		int kids = node->asGroup()->getNumChildren();
		
		for(int i = 0; i < kids; i++)
		{
			printNodeHierarchy(node->asGroup()->getChild(i));
		}
	}
	recursion--;
	if(!recursion)
	{
		printf("%i small geodes detected\n", smalls);
	}
	
	return;
}


void cullSmallGeodes(osg::Node* node, float threshold)
{
	if(!node)
	{
		printf("%s:  null node supplied!\n", __FUNCTION__);
		return;
	}
	static int recursion = 0;
	static int smalls = 0;
	static int geodes = 0;
	if(!recursion)
	{
		smalls = 0;
		geodes = 0;
	}

	int maxRecursion = 10;
	
	if(recursion >= maxRecursion)
	{
		
		return;
	}
	recursion++;

	
	if(std::string("Geode") == node->className())
	{
		geodes++;
		float geodeSize = getNodeCG(node, node).w();
//		printf("Geodearea:  %.3f", geodeSize);
		if(geodeSize < threshold)
		{
			smalls++;
			node->getParent(0)->removeChild(node);
//			printf("->SMALL!");
			//this is small.  de-parent it
			recursion--;
			return;
		}
	}
	
	//if this isn't a group, grab its bounding box and return it
	if(!node->asGroup() || recursion >= maxRecursion)
	{ 
		recursion--;
		return;
	}
	//otherwise dive in, child by child
	else
	{
		int kids = node->asGroup()->getNumChildren();
		
		for(int i = 0; i < kids; i++)
		{
			cullSmallGeodes(node->asGroup()->getChild(i), threshold);
		}
	}
	recursion--;
	if(!recursion)
	{
		if(smalls)
			printf("%i of %i geodes removed from %s\n", smalls, geodes, node->getName().c_str());
	}
	
	return;
}



osg::Node* findNodeWithName(osg::Group* group, std::string nameToFind)
{
	if(!group)
	{
		printf("%s:  null node supplied!\n", __FUNCTION__);
		return NULL;
	}
	
	std::string name = group->getName();
	if(name == nameToFind) return group;		//is this it?  then return it!
	
	//otherwise, dive in and look in our children
	int kids = group->getNumChildren();
	
	for(int i = 0; i < kids; i++)
	{
		osg::Group* g = group->getChild(i)->asGroup();
		if(g)
		{
			osg::Node* n = findNodeWithName(g, nameToFind);
			if(n) return n;
		}
	}
	return NULL;		//if we're here, we failed to find it ):  
}

void logError(const char* format, ...)
{
	char stringData[2024];
	va_list args;
	va_start(args, format);
	vsnprintf(stringData, 2024, format, args);
	int length = strlen(stringData);
	
	//remove \n's from the string cuz we're adding our own
	if(length > 0 && stringData[length-1] == '\n')
		stringData[length-1] = 0;


	printf("Error:  %s\n", stringData);
		
	_loggedErrors.push_back(stringData);
}

void printErrors()
{
	//let us know if there were no errors, just to be sure
	if(!_loggedErrors.size())
	{
		std::cout << "---No Problems Reported---" << std::endl;
		return;
	}
	
	
	//otherwise print out each error that we got
	std::cout << "-------------Runtime Errors and Warnings-------------" << std::endl;
	for(size_t i = 0; i < _loggedErrors.size(); i++)
	{
		std::cout << _loggedErrors[i] << std::endl;
	}
	
}

std::string findDataFile(std::string name)
{
	std::string found = osgDB::findDataFile(name);
	if(found == "")
		Util::logError("Couldn't find the file %s", name.c_str());
	return found;
}


//store all our loaded models so we don't have to re-load them
std::map<std::string, osg::ref_ptr<osg::Node> > gLoadedModels;

MatrixTransform* loadModel(std::string name, float scale, float rotX, float rotY, float rotZ, osg::Vec3 translate)
{
	__FUNCTION_HEADER__

	Node* n = NULL;
	
	//did we already load (or try to load) this one?
	std::map<std::string, osg::ref_ptr<osg::Node> >::iterator i;
	bool haz = false;
	for(i = gLoadedModels.begin(); i != gLoadedModels.end(); i++)
	{
		if(i->first == name)
		{
			haz = true;
//			printf("We already have %s\n", name.c_str());
			n = i->second;
		}
	}
	

	if(!haz)		//if we didn't try to laod it before, do so now
	{
		n = osgDB::readNodeFile(findDataFile(name));
		gLoadedModels[name] = n;
		if(!n)
			logError("Unable to load model %s\n", name.c_str());
		else	printf("Loaded %s\n", name.c_str());
	}
	if(!n) return NULL;

	MatrixTransform* m = new MatrixTransform();
	m->setName(name);
	m->addChild(n);
	
	Matrix mat = Matrix::scale(scale, scale, scale);
	mat = mat * Matrix::rotate(rotX / 57.296, Vec3(1, 0, 0));
	mat = mat * Matrix::rotate(rotY / 57.296, Vec3(0, 1, 0));
	mat = mat * Matrix::rotate(rotZ / 57.296, Vec3(0, 0, 1));
	mat = mat * Matrix::translate(translate);
	m->setMatrix(mat);
	return m;
}

float random(float minVal, float maxVal) 	{return minVal + (maxVal-minVal) * rand() / RAND_MAX;}
osg::Vec3 randomVector()
{
	Vec3 v(random(-1, 1), random(-1, 1), random(-1,1));
	v.normalize();
	return v;
}

void printMatrix(Matrixf m)
{
	for(int i = 0; i < 4; i++)
		printf("%.3f	%.3f	%.3f	%.3f\n", m.ptr()[i], m.ptr()[i+4], m.ptr()[i+8], m.ptr()[i+12]);
}

std::vector<OSGTri> turnGeodeToTriangles(osg::Geode &node)
{
	std::vector<OSGTri> tris;
	
	//printf("found %i drawables\n", node.getNumDrawables());
	for(unsigned int i = 0; i < node.getNumDrawables(); i++)
	{
		osg::Drawable* drawable = node.getDrawable(i);

		// Cast the drawable to a geometry object.
		osg::Geometry* geom = dynamic_cast<osg::Geometry*>(drawable);
		if(!geom) continue;
		// Copy the geometry object's vertex and texCoord data
		const osg::Vec3Array* vertexArray = 
			(const osg::Vec3Array*)(geom->getVertexArray());
		const osg::Vec2Array* texArray = 
			(const osg::Vec2Array*)(geom->getTexCoordArray(0));

		for (unsigned int j = 0; j < geom->getNumPrimitiveSets(); ++j)
		{
			osg::PrimitiveSet* primSet = geom->getPrimitiveSet(j);

			if (1)
			{
				const char* mode = NULL;
				
				switch(primSet->getMode())
				{
					case 0:
						mode = "GL_POINTS";
						break;
					case 1:
						mode = "GL_LINES";
						break;
					case 2:
						mode = "GL_LINE_STRIP";
						break;
					case 3:
						mode = "GL_LINE_LOOP";
						break;
					case GL_TRIANGLES:
						mode = "GL_TRIANGLES";
						break;
					case GL_TRIANGLE_STRIP:
						mode = "GL_TRIANGLE_STRIP";
						break;
					case 6:
						mode = "GL_TRIANGLE_FAN";
						break;
					case 7:
						mode = "GL_QUADS";
						break;
					case 8:
						mode = "GL_QUAD_STRIP";
						break;
					case 9:
						mode = "GL_POLYGON";
						break;
					default:
						break;
				}

/*				std::cout << "Primitive set number: " << j 
					<< ", OpenGL mode: " << mode 
					<< ", numIndices: " << primSet->getNumIndices() 
					<< std::endl;
*/
			}

			switch(primSet->getMode())
			{
				case GL_TRIANGLES:
					for (unsigned int k = 0; k < primSet->getNumIndices(); k += 3)
					{
						osg::Vec3 verts[3];
						osg::Vec2 texes[3];
						//grab the 3 verts and make 3 edges out of them
						for(int v = 0; v < 3 && primSet->getNumIndices() > k + v; v++)
						{
							verts[v] = vertexArray->at(primSet->index(k + v));				
							if(texArray)
								texes[v] = texArray->at(primSet->index(k+v));
						}
						tris.push_back(OSGTri(verts[0], verts[1], verts[2]));
						tris.back().setTexes(texes[0], texes[1], texes[2]);
					}
					break;
				case GL_TRIANGLE_STRIP:
					for (unsigned int k = 0; k < primSet->getNumIndices() - 2; k += 2)
					{
						osg::Vec3 verts[4];
						osg::Vec2 texes[4];						
						//grab the 4 verts and make 4 edges out of them
						for(int v = 0; v < 4 && primSet->getNumIndices() > k + v; v++)
						{
							verts[v] = vertexArray->at(primSet->index(k + v));
							if(texArray)
								texes[v] = texArray->at(primSet->index(k+v));

						}
						
						tris.push_back(OSGTri(verts[0], verts[1], verts[2]));
						tris.back().setTexes(texes[0], texes[1], texes[2]);
						if(primSet->getNumIndices() > k + 3)
						{
							tris.push_back(OSGTri(verts[3], verts[1], verts[2]));
							tris.back().setTexes(texes[3], texes[1], texes[2]);
						}
						
					}	
					
					break;
					
				case GL_QUADS:
					for (unsigned int k = 0; k < primSet->getNumIndices() - 3; k += 4)
					{
						osg::Vec3 verts[4];
						osg::Vec2 texes[4];
						//grab the 4 verts and make 4 edges out of them
						for(int v = 0; v < 4; v++)
						{
							verts[v] = vertexArray->at(primSet->index(k + v));
							//printf("Quad vert %i (%i):  %.2f, %.2f, %.2f\n",v, k+v, verts[v][0], verts[v][1], verts[v][2]);
							if(texArray)
								texes[v] = texArray->at(primSet->index(k+v));

						}
						tris.push_back(OSGTri(verts[0], verts[1], verts[2]));
						tris.back().setTexes(texes[0], texes[1], texes[2]);
						tris.push_back(OSGTri(verts[2], verts[3], verts[0]));
						tris.back().setTexes(texes[2], texes[3], texes[0]);
					}	
					break;
				default:
					std::cout << "ERROR: Cannot convert primitive set of mode " 
						<< primSet->getMode() << " to a physical mesh." << std::endl;
					break;
			}
		}
	}
	return tris;
}


std::vector<OSGTri> turnNodeToTriangles(osg::Node* node)
{
	std::vector<OSGTri> tris;
	if(std::string(node->className()) == std::string("Geom"))
		printf("found a geom\n");
	else if(node->asGroup())		//recurse for groups
	{
		Util::logError("Group send to turnNodeToTriangles.  Shouldn't happen in this app!\n");
		for(unsigned int kid = 0; kid < node->asGroup()->getNumChildren(); kid++)
		{
			std::vector<OSGTri> moreTris = turnNodeToTriangles(node->asGroup()->getChild(kid));
			while(moreTris.size())
			{
				tris.push_back(moreTris.back());
				moreTris.pop_back();
			}

		}
	
	
	}
	//if this is a geode, use a different function
	else if(std::string(node->className()) == std::string("Geode"))
	{
		osg::Geode* geode = (osg::Geode*)(node);
		return turnGeodeToTriangles(*geode);
	}

	return tris;
}


float areaOfTriangle(osg::Vec3 v0, osg::Vec3 v1, osg::Vec3 v2)
{
	osg::Vec3 n1 = v1 - v0;
	osg::Vec3 n2 = v2 - v0;
	osg::Vec3 cross = n1 ^ (n2);
	return cross.length() / 2;
}

std::map<osg::Node*, osg::Vec4> gCOGCache;			//cache this so we only need to calculate it once
osg::Vec4 getNodeCG(osg::Node* node, osg::Node* topLevel)
{
	if(!node)
	{
		Util::logError("GetNodeCG received NULL node!\n");
		return Vec4(0, 0, 0, 1);	
	}

	static Matrixf cumulative;
	__FUNCTION_HEADER__
	Vec4 soFar;		//use homogeneous coordinates so we know what to divide by
	if(node->asGroup())
	{
		ScopedBlock sb("GroupCG");
		Matrixf lastCumulative = cumulative;			//store the last cumulative so we can go back to it
		cumulative = Util::getTransform(node) * cumulative;			//add in the cumulative transform
		for(int i = 0; i < node->asGroup()->getNumChildren(); i++)
		{
			Node* child = node->asGroup()->getChild(i);
			Vec4 more = getNodeCG(child, topLevel);
			Vec3 pos(more.x(), more.y(), more.z());
//			printf("CG for %s:  %.2f, %.2f, %.2f\n", child->getName().c_str(), more.x(), more.y(), more.z());
			//transform the data from this node into local space for the topLevel object
//			Matrixf xform = Util::getTransform(child);
//			xform = xform * soFar;
			pos = pos * cumulative;
			more.x() = pos.x();
			more.y() = pos.y();
			more.z() = pos.z();
//			printf("Transform to:  %.2f, %.2f, %.2f (%.2f weight)\n", more.x(), more.y(), more.z(), more.w());
			soFar += more;
		}
		cumulative = lastCumulative;		//pop the cumulative matrix
		
	}
	else
	{
		ScopedBlock sb("NodeCG");
		
		//do we already have this one?
		std::map<osg::Node*, osg::Vec4>::iterator iter;
		for(iter = gCOGCache.begin(); iter != gCOGCache.end(); ++iter)
		{
			if(iter->first == node) return iter->second;
		}
		
		//if this isn't a group, turn it into triangles!
		std::vector<OSGTri> tris = turnNodeToTriangles(node);
		for(size_t i = 0; i < tris.size(); i++)
		{
			float area = tris[i].getArea();
			Vec3 center = tris[i].getCenter() * area;
			soFar += Vec4(center.x(), center.y(), center.z(), area);
		}
		
		//put this in our cache if it's not there already
		gCOGCache[node] = soFar;
		
	}
	
	return soFar;
}

osg::Matrixf getTransform(osg::Node* n)
{
	if(!n->asGroup()) return Matrixf();
	MatrixTransform* mt = dynamic_cast<MatrixTransform*>(n);
	PositionAttitudeTransform* pat = dynamic_cast<PositionAttitudeTransform*>(n);
	if(mt) return mt->getMatrix();
	if(pat)
	{
		Matrixf currentXForm;
		Vec3 pos = pat->getPosition();
		pat->getAttitude().get(currentXForm);
		currentXForm.ptr()[12] = pos.x();
		currentXForm.ptr()[13] = pos.y();
		currentXForm.ptr()[14] = pos.z();
		return  currentXForm;
	}
	
	return Matrixf();
}


osg::Matrixf getCumulativeTransform(osg::Node* from, osg::Node* to)
{
	__FUNCTION_HEADER__
	if(!from || !to) return Matrixf();
//	MatrixList mats = from->getWorldMatrices(to);
//	if(!mats.size()) return Matrixf();

	NodePathList nodePaths = from->getParentalNodePaths(to);
	
//	printf("there are %i paths to %s\n", nodePaths.size(), to->getName().c_str());
	if(!nodePaths.size()) return Matrixf();
	Matrixf mat;
	NodePath p = nodePaths.back();

	//get the path that ends in our "to" node.
	for(int j = 0; j < nodePaths.size(); j++)
	{
		if(nodePaths[j][0] == to)
		{
			//found it.  multiply out the matrices!
			Matrixf final;
			for(int k = 0; k < nodePaths[j].size(); k++)
			{
				final = getTransform(nodePaths[j][k]) * final;
			}
//			printf("path length:  %i\n", nodePaths[j].size());
//			printf("______________using______________\n");
//			printMatrix(mats[j]);
//			printf("____________compare to___________\n");
//			printMatrix(final);
//			return mats[j];
			return final;

		}
		
//		p = nodePaths[j];
//		printf("a path:\n");
//		for(int i = 0; i < p.size(); i++)
//			printf("%s\n", p[i]->getName().c_str());
//		printf("---------------\n");
		
	}
//	return mats[0];
//	
//	for(size_t i = 0; i < mats.size(); i++)
//	{
////		printf("-----------matrix for %s--------------\n", p[i]->getName().c_str());
//		printMatrix(mats[i]);		printf("-----------------------------\n");
////		mat = mats[i] * mat;
//		mat *= mats[i];
//	}
	return mat;

}

std::string stringWithFormat(const char* format, ...)
{
	char stringData[2048];
	va_list args;
	va_start(args, format);
	vsprintf(stringData, format, args);
	return stringData;
}

void deCull(osg::Node* n)
{
	n->setCullingActive(false);
	if(n->asGroup())
	{
		for(int i = 0; i < n->asGroup()->getNumChildren(); i++)
			deCull(n->asGroup()->getChild(i));
	}
	
}


		
}

Vec3 OSGTri::getCenter()
{
	return (v0 + v1 + v2) / 3.0;
}

float OSGTri::getArea()	{return Util::areaOfTriangle(v0, v1, v2);}
float OSGTri::getTextureArea()	
	{return Util::areaOfTriangle(Vec3(t0[0], t0[1], 0), Vec3(t1[0],t1[1], 0), Vec3(t2[0],t2[1],0));}
	
	



ScopedBlock::ScopedBlock(std::string name)
{
	mBlockName = name;
	PROFILER.beginBlock(mBlockName); 
//	printf("begin %s\n", mBlockName.c_str());			
}

ScopedBlock::~ScopedBlock()
{
	PROFILER.endBlock(mBlockName);
//	printf("end %s\n", mBlockName.c_str());
}


