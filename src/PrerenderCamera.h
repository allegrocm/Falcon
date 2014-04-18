/*
 *  PrerenderCamera.h
 *  OSGEnhancer
 *
 *  Created by Kenneth Kopecky on 11/4/09.
 *  Copyright 2009 Orange Element Studios. All rights reserved.
 *
 */

//a camera that renders to an fbo.  easy enough

#ifndef PRERENDERERERERCAMERA
#define PRERENDERERERERCAMERA
#include <osg/Camera>
#include <osg/Geode>
#include <osg/Program>
#include <osg/Texture2D>

class PrerenderCamera : public osg::Camera
{
public:
	virtual const char* className()	const {return "PrerenderCamera";}
	PrerenderCamera(int texSize, int targets, int format, bool useDepth = false);
	void setShader(osg::Program* shader);
	osg::Texture2D* getTargetTexture(int t);
	int getNumTargets()	{return mNumTargets;}
	void addQuadGeometry();	//used for rendering a quad from an input texture
	void addInputTexture(osg::Texture2D* tex, std::string uniformName, int texUnit);
protected:
	int mNumTargets;
	int mTexSize;
	osg::ref_ptr<osg::Texture2D> mTargetTexes[16];
};

#endif
