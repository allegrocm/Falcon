//
//  Hyperspace.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky on 4/30/14.
//
//
#include <osg/Geode>
#include <osg/Geometry>

#include "Hyperspace.h"
#include "Util.h"
#include <stdlib.h>

using namespace osg;

Hyperspace::Hyperspace()
{
	//start by creating a bunch of "stars"
	int stars = 2500;
	osg::Geode* geode = new Geode();
	Geometry* geom = new Geometry;
	geode->addDrawable(geom);
	Vec4Array* colors = new Vec4Array();
	colors->push_back(Vec4(1, 1, 1, 1));
	geom->setColorArray(colors);
	geom->setColorBinding(Geometry::BIND_OVERALL);
	Vec3Array* verts = new Vec3Array();
	geom->setVertexArray(verts);
	geom->addPrimitiveSet(new DrawArrays(PrimitiveSet::TRIANGLES, 0, stars*3));
	geom->getOrCreateStateSet()->setMode(GL_LIGHTING, false);
	for(int i  = 0; i < stars; i++)
	{
		float radius = Util::random(10, 150);
		float theta = 6.28 * rand() / RAND_MAX;
		float s = cosf(theta);
		float c = sinf(theta);
		Vec3 localUp(-s, c, 0);
		float width = Util::random(0.1, 0.25) * radius / 25;;
		float length = Util::random(10, 30) * radius / 20;
		float z = 0;
		Vec3 basePos(c*radius, s*radius, z+length*0.5);
		Vec3 zDir(0, 0, 1);
		verts->push_back(basePos + zDir * length * 0.5 + localUp * width * 0);
//		verts->push_back(basePos + zDir * length * 0.5 - localUp * width * 0.5);
		verts->push_back(basePos - zDir * length * 0.5 - localUp * width * 0.5);
		verts->push_back(basePos - zDir * length * 0.5 + localUp * width * 0.5);
	}
	mPat->setPosition(Vec3(0, 0, -250));
	mPat->addChild(geode);
	mHSTime = -1;
	update(0);
	
}

bool Hyperspace::update(float dt)
{
	mAge += dt;
	if(mHSTime >= 0)
	{
		mHSTime += dt;
		mPhase = mHSTime / 2.0;
	}
	else
	{
		mPhase = 1.0;
	}
//	mPhase= mAge / 2.0;
	//just loop our animation for now

	mPhase -= (int)mPhase;
	
	mPat->setScale(Vec3(1, 1, 10.0 * mPhase * mPhase));
	float z = -250;
	float moveStart = 0.5;
	
	if(mHSTime > 2) z = 10000;
	//start moving towards the camera after a certain amount of time
	if(mPhase > moveStart)
	{
		float movement = (mPhase-moveStart) / (1.0-moveStart);
		movement *= movement * movement;
		z += movement * 500;
	}
	mPat->setPosition(Vec3(0, 0, z));
	return true;
}

void Hyperspace::go()
{
	mHSTime = 0.0;
}

