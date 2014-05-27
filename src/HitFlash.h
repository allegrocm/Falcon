//
//  HitFlash.h
//  StarWars
//
//  Created by Ken Kopecky on 3/12/14.
//  Copyright (c) 2014 VRAC. All rights reserved.
//

#ifndef __StarWars__HitFlash__
#define __StarWars__HitFlash__

#include <iostream>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/PositionAttitudeTransform>
#include <osg/Switch>
#include <osg/ShapeDrawable>
#include <osg/Shape>

//provides visual feedback for when the player gets hit or health is low or something
class HitFlash
{
public:
	HitFlash();
	osg::PositionAttitudeTransform* getPat()	{return _pat;}
	void activate();		//make this flash
	void update(float dt);
	void setColor(osg::Vec4 c);
	void setRadius(float r);
protected:
	osg::ref_ptr<osg::PositionAttitudeTransform> _pat;
	

	float										_fadeTime;	//how long will this be visible?
	float										_alpha;		//how opaque at the moment?
	osg::ref_ptr<osg::ShapeDrawable>			_drawable;		//so we can change the color
	osg::ref_ptr<osg::Shape>					_shape;		//so we can change the radius
	osg::ref_ptr<osg::Switch>					_switch;		//hides and shows
};

#endif /* defined(__StarWars__HitFlash__) */
