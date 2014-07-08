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
	setGlows(true);
}

bool Bullet::update(float dt)
{
	GameObject::update(dt);
	 
	setPos(getPos() + mVel * dt);
//	printf("pos:  %.2f, %.2f, %.2f\n", getPos().x(), getPos().y(), getPos().z());
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
	
	Vec3 dir = getForward();
	Vec3 pos = getPos();
	float length = mLength;
	float travelLength = mVel.length() * TimeStep;
	
	//if our length is shorter than our velocity * dt, lengthen it so we don't pass through something
	if(travelLength > length)
		length = travelLength;

	//dunno if this is the best way to do it or not, but we're gonna just check each ship individually
	std::vector<Spacecraft*> ships = EnemyController::instance().getShips();
	bool canHitEnemy = (!mIsEnemy || mAge > 0.1);		//friendly fire activates after a moment so we dont' hit our own gun
	if(mIsEnemy)		//enemy shots only hit the falcon in their first moments
	{
		if(canHitEnemy == false)
			ships.clear();
		Falcon* f = FalconApp::instance().getFalcon();

		//ships.push_back((Spacecraft*)f);

		if(ROM::FALCON_HIT_DETECTION)
			ships.push_back((Spacecraft*)f);
	}
	
	bool hitSometing = false;		//stop after any collision
	Vec3 hitPos;
	for(size_t i = 0; i < ships.size() && !hitSometing; i++)
	{
		if(mIsEnemy && ships[i] == mShooter) continue;		//don't let the TIE's shots it itself
//		iv.reset();
		Vec3 hitPos;
		if(ships[i]->checkRaycast(pos-dir*length*0.5, dir*length, hitPos))
		{
//			printf("Bullet just hit %s, cast length %.2f\n", ships[i]->getName().c_str(), length);
//			printf("dist:  %.2f\n", (pos-hitPos).length());
			explode(hitPos);
			ships[i]->wasHit(this, hitPos);
			hitSometing = true;
			return true;
		}
//		else printf("Didn't hit %s\n", ships[i]->getName().c_str());
		
	
	}
	
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

