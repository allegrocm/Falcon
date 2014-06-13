//
//  ScreenImage.h
//  Millennium Falcon
//
//  Created by Ken Kopecky on 4/16/14.
//
//

#ifndef __Millennium_Falcon__ScreenImage__
#define __Millennium_Falcon__ScreenImage__

#include <iostream>
#include <osg/PositionAttitudeTransform>
#include <osg/Image>
#include <osg/Geometry>

//a little class for displaying an image on our screen with a quad
class ScreenImage
{
public:
	ScreenImage();
	osg::ref_ptr<osg::PositionAttitudeTransform> transform;
	void setImage(std::string name);
	void setHeight(float s);
	void setPos(osg::Vec3 p);
	void setTopLeft(osg::Vec3 p);
	void setColor(osg::Vec4 c);
	void setAspect(float s);		//set aspect ratio of the image (width / height)
	void setVisible(bool b);
	void setAngle(float degrees);			//rot8!
protected:
	float aspect;			//our image's aspect ratio.  used for setting our scale
	float scale;			//our current scale
	osg::ref_ptr<osg::Geometry>	geometry;		//needed so we can adjust color
	bool visible;
};


#endif /* defined(__Millennium_Falcon__ScreenImage__) */
