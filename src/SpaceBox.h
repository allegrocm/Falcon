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
#include <osg/PositionAttitudeTransform>

class TiXmlElement;		//for XML parsing


//an object that makes space interesting
class SpaceObject
{
public:
	enum ObjectType {PLANET, BILLBOARD, BILLBOARD_PLUS, MODEL, NUM_OBJECT_TYPES};		//billboard plus is additively blended
	ObjectType type;
	float size;	//height/radius
	osg::Vec3 pos;
	osg::Vec3 heading;		//euler angles.  only used for ships
	std::string texName;	//or model name
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
	std::vector<osg::MatrixTransform*> mCapitalShips;
	
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
	int getSystemNumber()	{return mCurrentSystem;}
	void nextSystem()
	{
		mCurrentSystem = (mCurrentSystem+1)%mSystems.size();
		loadSystem(mCurrentSystem);
	}
	osg::MatrixTransform* getNearGroup()	{return mNearGroup;}
	osg::MatrixTransform* getFarGroup()	{return mFarGroup;}
	std::string getSystemName()	{return mSystems[mCurrentSystem].name;}
	osg::MatrixTransform* getCapitalShip();		//get something we can spawn TIE fighers near!
	void setNavMatrix(osg::Matrix nav, float zOffset);
protected:
	osg::ref_ptr<osg::MatrixTransform> mRoot;
	osg::ref_ptr<osg::Group> mNearSystemStuff;			///where system objects go
	osg::ref_ptr<osg::MatrixTransform> mNearGroup;		//planets
	osg::ref_ptr<osg::MatrixTransform> mFarGroup;		//far-away things
	osg::ref_ptr<osg::Geode> mBox;		//what with the stars
	void addPlanet(std::string texture, osg::Vec3 pos, float radius);
	void addBillboard(std::string texture, osg::Vec3 pos, float height, bool additiveBlend);
	void addModel(SpaceObject o);
	std::vector<SpaceScene> mSystems;
	int mCurrentSystem;		//for cycling through the systems
	
};

#endif /* defined(__Millennium_Falcon__SpaceBox__) */
