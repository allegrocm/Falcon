/*
 *  CameraThatRendersAQuad.h
 *  DP3D
 *
 *  Created by Kenneth Kopecky on 6/10/08.
 *  Copyright 2008 Orange Element Studios. All rights reserved.
 *
 */

//Uses a quad as its input.  can be used to display a texture for debugging or as a GPGPU camera

#ifndef CAMERATHATRENDERSH
#define CAMERATHATRENDERSH
#include <osg/Camera>
#include <osg/Geode>
#include <osg/TextureRectangle>

class CameraThatRendersAQuad : public osg::Camera
{
public:
	virtual const char* className()	const {return "CameraThatRendersAQuad";}
	CameraThatRendersAQuad();		//creates a quad and sets up view matrices
	void setTexCoordLimits(float x0, float x1, float y0, float y1);
	void setTexture(osg::Texture* tex);
	osg::Geode* getGeode()	{return mGeode;}
protected:

	osg::Geode* mGeode;		//geode containing our quad
};

#endif
