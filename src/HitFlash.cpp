//
//  HitFlash.cpp
//  StarWars
//
//  Created by Ken Kopecky on 3/12/14.
//  Copyright (c) 2014 VRAC. All rights reserved.
//

#include "HitFlash.h"
#include "FalconApp.h"
#include <osg/BlendFunc>
using namespace osg;

HitFlash::HitFlash()
{
	_fadeTime = 0.25;
	_alpha = 1.0;
	_pat = new PositionAttitudeTransform();
	_switch = new Switch();
	_pat->addChild(_switch);
	
	//shape must be attached to a geode
	Geode* g = new Geode();
	_switch->addChild(g);
	g->getOrCreateStateSet()->setAttribute(new BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	g->getOrCreateStateSet()->setMode(GL_BLEND, true);
	g->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	_shape = new Sphere(Vec3(0, 3, 0), 5);
	_drawable = new ShapeDrawable(_shape);
	g->addDrawable(_drawable);
	setColor(Vec4(1.0, 0, 0, 0.5));
}

void HitFlash::setColor(Vec4 c)
{
	_drawable->setColor(c);
	_switch->setValue(0, (c.w() > 0));
	
}

void HitFlash::update(float dt)
{
	Matrix head = FalconApp::instance().getHeadMatrix();
	_pat->setPosition(Vec3(head.ptr()[12], head.ptr()[13], head.ptr()[14]));
	_alpha -= dt / _fadeTime;
	if(_alpha < 0) _alpha = 0;
	setColor(Vec4(1.0, 0.2, 0.2, _alpha));
}

void HitFlash::activate()
{
	_alpha = 1.0;		//start the fadeout process
	setColor(Vec4(1.0, 0.2, 0.2, _alpha));
}




