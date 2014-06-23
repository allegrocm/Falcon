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

class SpaceBox
{
public:
	SpaceBox();
	osg::MatrixTransform* getRoot()	{return mRoot;}
protected:
	osg::ref_ptr<osg::MatrixTransform> mRoot;
	void addPlanet(std::string texture, osg::Vec3 pos, float radius);
	void addBillboard(std::string texture, osg::Vec3 pos, float height);
};

#endif /* defined(__Millennium_Falcon__SpaceBox__) */
