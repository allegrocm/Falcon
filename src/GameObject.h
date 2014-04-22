//
//  GameObject.h
//  PaddleboatQuest
//
//  Created by Ken Kopecky II on 1/27/14.
//
//

#ifndef __PaddleboatQuest__GameObject__
#define __PaddleboatQuest__GameObject__
#ifdef _WIN32
#include <windows.h>
#include <gl/glut.h>
#endif

#include <osg/PositionAttitudeTransform>
#include <iostream>

class Debris;
//the base class for things that are in the game

class GameObject
{
public:
	GameObject();
	virtual ~GameObject();					//destroy and remove all traces from the scenegraph

	//timestep update.  return false if this needs to be deleted and removed
	virtual bool update(float dt){ mAge += dt;  return true;}
	
	//the root is a group, but it also contains a positionAttitudeTransform
	//just in case certain objects want to position things globally
	//in general, load models and put them in the pat.
	virtual osg::Group* getRoot()	{return mRoot;}
	
	
	//-----------some handy setters, getters, etceters
	//set/get position
	virtual osg::Vec3 getPos()	{return mPat->getPosition();}
	virtual void setPos(osg::Vec3 p)		{mPat->setPosition(p);}
	
	//set/get transform by matrix
	virtual osg::Matrixf getTransform();
	virtual void setTransform(osg::Matrixf mat);
	
	//set/get rotation by quat
	virtual osg::Quat getQuat()	{return mPat->getAttitude();}
	virtual void setQuat(osg::Quat q)	{mPat->setAttitude(q);}
	
	//----------other directioneering things
	
	//forward is the NEGATIVE Z-AXIS!
	//aim the front of this guy forward, attempting to preserve the other axes
	virtual void setForward(osg::Vec3 f);
	virtual osg::Vec3 getForward();
	std::string getName()							{return mName;}
	void setName(std::string s);
	virtual float getAge()							{return mAge;}
	
	//break a 3D model apart according to its hierarchy to the stated depth
	//each piece might split into its sub pieces, or might vanish instead of becoming debris
	//called recursively
	virtual std::vector<Debris*> explodeSection(osg::Group* n, int depth, int maxDepth, float splitChance, float vanishChance, osg::Matrixf currentTransform, osg::Node* root, int maxPieces = 5);
	virtual void drawDebug();		//for...you guessed it...debugging!
protected:
	osg::ref_ptr<osg::Group>						mRoot;
	osg::ref_ptr<osg::PositionAttitudeTransform>	mPat;		//used for positioning/orienting
	float											mAge;		//how old is this?  Can be used for timing and control
	std::string										mName;
};



#endif /* defined(__PaddleboatQuest__GameObject__) */
