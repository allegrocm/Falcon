//
//  ScreenShaker.h
//  StarWars
//
//  Created by Ken Kopecky on 2/19/14.
//  Copyright (c) 2014 VRAC. All rights reserved.
//


//shakes the screen.  we'll see if it makes people sick
#ifndef __StarWars__ScreenShaker__
#define __StarWars__ScreenShaker__

#include <iostream>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/PositionAttitudeTransform>

class ScreenShaker
{
public:
	ScreenShaker();
	osg::PositionAttitudeTransform* getPat()	{return _pat;}
	void activate(float amount);		//0 is none, 1 is a lot
	void update(float dt);
protected:
	osg::ref_ptr<osg::PositionAttitudeTransform> _pat;
	
	//we'll start out with this based on a damped spring model
	float _damping;		//damping constant
	float _k;			//spring constant
	float _strength;	//overall strength multiplier
	float _tTillShake;	//how long till our next shake
	float _tMin;		//min interval between shakes
	float _tMax;		//max interval between shakes
	float _minDistance;	//minimum distance from center for a shake
	float _maxDistance;	//maximum distance from center for a shake
	float _tTillSettle;	//time till we stop shaking
	int _shakes;		//how many times have we shaken this time?
	
	//for translational shaking
	osg::Vec3 _vel;		//current velocity
	osg::Vec3 _pos;		//current position
	osg::Vec3 _shakePos;	//where will this shake pull us?
	
	//and rotational shaking...
	osg::Quat _rotVel;		//rotational velocity
	osg::Quat _rot;			//current rotation
	osg::Quat _rotDest;		//where are we rotating to?
};

#endif /* defined(__StarWars__ScreenShaker__) */
