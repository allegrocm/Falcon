//
//  Bullet.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky II on 4/4/14.
//
//

#include "Bullet.h"
#include "Util.h"
#include "FalconApp.h"
#include "Spacecraft.h"
#include "ParticleFX.h"
#include "Debris.h"
#include "EnemyController.h"
#include "ROM.h"
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osgUtil/IntersectVisitor>
#include "Layers.h"
using namespace osg;

Bullet::Bullet()
{
	mTarget = NULL;
	mShooter = NULL;
	mLife = 5;
	mDamage = 1;
	mIsEnemy = false;
	mLength = ROM::FALCON_LASER_LENGTH;
	addGeometry();
}

bool Bullet::update(float dt)
{
	GameObject::update(dt);
	setPos(getPos() + mVel * dt);
	if(checkHit()) return false;
	return (mAge < mLife);
}

void Bullet::addGeometry()
{
	Cylinder* cyl = new Cylinder(Vec3(), 0.25, mLength);
//	cyl->setRotation(Quat(3.14159/2.0, Vec3(1, 0, 0)));
	mSD = new ShapeDrawable(cyl);
	mSD->setColor(Vec4(.4, .9, .4, 1));
	Geode* g = new Geode();
	g->addDrawable(mSD);
	mRoot->getOrCreateStateSet()->setMode(GL_LIGHTING, false);
	mPat->addChild(g);

}

bool Bullet::checkHit()
{
	__FUNCTION_HEADER__
	#define TimeStep 0.01
	osgUtil::IntersectVisitor iv;
	
	//TODO:  collisions
	Vec3 dir = getForward();
	Vec3 pos = getPos();
	float length = mLength;
	float travelLength = mVel.length() * TimeStep;
	
	//if our length is shorter than our velocity * dt, lengthen it so we don't pass through something
	if(travelLength > length)
		length = travelLength;
	//make a line segment representing the laser beam
	ref_ptr<LineSegment> seg = new LineSegment(pos - dir * length * 0.5, pos + dir * length * 0.5);
	iv.setTraversalMask(1 << COLLISION_LAYER);		//DON'T check collisions with other lazer beams
	iv.addLineSegment(seg.get());
//	printf("Seg:  %.2f, %.2f, %.2f\n", pos.x(), pos.y(), pos.z());
	//dunno if this is the best way to do it or not, but we're gonna just check each ship individually
	std::vector<Spacecraft*> ships = EnemyController::instance().getShips();
	
	if(mIsEnemy)		//enemy shots only hit the falcon
	{
		ships.clear();
		Falcon* f = FalconApp::instance().getFalcon();
#ifdef __APPLE__
#warning TODO:  get falcon collision detection back in
#endif
		if(ROM::FALCON_HIT_DETECTION)
			ships.push_back((Spacecraft*)f);
	}
	
	bool hitSometing = false;		//stop after any collision
	Vec3 hitPos;
	for(size_t i = 0; i < ships.size() && !hitSometing; i++)
	{
//		iv.reset();
		ships[i]->getRoot()->accept(iv);
		osgUtil::IntersectVisitor::HitList& hitList = iv.getHitList(seg.get());
		if(hitList.size())		//if there's any size in the hitlist, we HIT something!
		{
			hitPos = hitList[0].getWorldIntersectPoint();
			printf("Bullet just hit %s\n", ships[i]->getName().c_str());
			explode(hitPos);
			ships[i]->wasHit(this);
			hitSometing = true;
			return true;
		}
//		else printf("Didn't hit %s\n", ships[i]->getName().c_str());
		
	
	}
	
//	iv.setTraversalMask(0x4);		//DON'T check collisions with other lazer beams
/*
	//this is the segment that represents the our light saber.


	node->accept(iv);
	osgUtil::IntersectVisitor::HitList& hitList = iv.getHitList(seg.get());
	if(hitList.size())		//if there's any size in the hitlist, we HIT something!
	{
		hitPos = hitList[0];
	
		return true;
	}
*/

	return false;

}

void Bullet::peter()
{
	//TODO:  particle systems!
}

void Bullet::explode(Vec3 where)
{
	FalconApp::instance().getFX()->makeExplosion(where, 0.5);

}

void Bullet::setColor(Vec4 c)
{
	mSD->setColor(c);
}

