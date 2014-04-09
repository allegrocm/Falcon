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

};

#endif /* defined(__Millennium_Falcon__SpaceBox__) */