//
//  RadarScreen.h
//  Millennium Falcon
//
//  Created by Ken Kopecky on 4/17/14.
//
//

#ifndef __Millennium_Falcon__RadarScreen__
#define __Millennium_Falcon__RadarScreen__

#include <osg/Drawable>
#include "PrerenderCamera.h"
#include "ScreenImage.h"
class RadarScreen;

//a helper class to draw the radar screen in the scene
class RSDrawable : public osg::Drawable
{
public:
	//HUDDrawable();
	virtual const char* className()	{return "RadarScreenDrawable";}
	virtual void drawImplementation(osg::RenderInfo& renderInfo) const;	//the draw function
	RadarScreen* radar;			//we grab info from the radar screen to draw
	virtual osg::Object* cloneType()	const 	{return new RSDrawable;}
	virtual osg::Object* clone(const osg::CopyOp& copyop) const {return new RSDrawable;}
	virtual osg::BoundingBox computeBound() const;
};

//draws to a texture
class RadarScreen : public ScreenImage
{
public:
	RadarScreen();
	~RadarScreen();
	void draw();
	void update(float dt);
	osg::Texture2D* getTexture();
	osg::Vec3 worldToRadar(osg::Vec3 worldPos);		//convert worldspace to radar screen space
	
	PrerenderCamera* getCamera()	{return mCamera;}
protected:
	osg::ref_ptr<PrerenderCamera> mCamera;
	osg::ref_ptr<RSDrawable> mDrawable;
	osg::Matrixf mRadarMatInverse;		//usually the inverse wand matrix I guess.  For converting things to radarspace
//	osg::ref_ptr<RSDrawable*
};
#endif

