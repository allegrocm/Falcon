//
//  SpaceBox.h
//  Millennium Falcon
//
//  Created by Ken Kopecky on 4/8/14.
//
//

#ifndef __Millennium_Falcon__SpaceBox__
#define __Millennium_Falcon__SpaceBox__

#include <iostream>

//a skybox.  but in space

#include <osg/MatrixTransform>

class TiXmlElement;		//for XML parsing


//an object that makes space interesting
class SpaceObject
{
public:
	enum ObjectType {PLANET, BILLBOARD, BILLBOARD_PLUS, NUM_OBJECT_TYPES};		//billboard plus is additively blended
	ObjectType type;
	float size;	//height/radius
	osg::Vec3 pos;
	std::string texName;
	bool fromXML(TiXmlElement* e);
	SpaceObject(){type = PLANET; size = 500;}
};


//a set of objects
class SpaceScene
{
public:
	SpaceScene()
	{
		loaded = false;
		mFarThings = new osg::Group;
		mNearThings = new osg::Group;
	}
	std::vector<SpaceObject> objects;
	std::string name;
	bool loaded;
	osg::ref_ptr<osg::Group> mFarThings;
	osg::ref_ptr<osg::Group> mNearThings;
};


//displays a space scene and has a box of stars and stuff
class SpaceBox
{
public:
	SpaceBox();
	osg::MatrixTransform* getRoot()	{return mRoot;}
	
	void setViewerPos(osg::Vec3 p);		//repositions things around us so we don't fly into the background
	
	void reload();
	bool loadSystems(std::string fileName);
	void loadSystem(int which);
	void nextSystem()
	{
		mCurrentSystem = (mCurrentSystem+1)%mSystems.size();
		loadSystem(mCurrentSystem);
	}
	std::string getSystemName()	{return mSystems[mCurrentSystem].name;}
protected:
	osg::ref_ptr<osg::MatrixTransform> mRoot;
	osg::ref_ptr<osg::MatrixTransform> mNearGroup;		//planets
	osg::ref_ptr<osg::MatrixTransform> mFarGroup;		//far-away things
	void addPlanet(std::string texture, osg::Vec3 pos, float radius);
	void addBillboard(std::string texture, osg::Vec3 pos, float height, bool additiveBlend);
	std::vector<SpaceScene> mSystems;
	int mCurrentSystem;		//for cycling through the systems
	
};

#endif /* defined(__Millennium_Falcon__SpaceBox__) */
