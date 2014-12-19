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
#include "VaderTIE.h"

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
#ifdef RNG_LOGGING
	//printf("LogRDM Bullet %i Pos:  %.2f, %.2f, %.2f\n", mObjectID, getPos().x(), getPos().y(), getPos().z());
#endif

	if(checkHit()) return false;
#ifdef RNG_LOGGING
	if(mAge >= mLife)
		printf("LogRDM Bullet %i disappearing\n", mObjectID);
#endif
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
			//friendly fire doesn't affect Vader's TIE.
			//this is partly because the camera view for Vader's tie apparently allows it to be hit by bullets near the origin!
			if(dynamic_cast<VaderTIE*>(ships[i]) && mIsEnemy)
			{
				printf("************Avoiding invalid hit on TIE fighter!\n");
				continue;
			}
			
			//sometimes a laser will "hit" a ship that's very far away from it.
			//Seems to only happen when laser direction and ship-to-target are almost completely parallel
			Vec3 meToHit = hitPos-pos;
			Vec3 shipToHit = hitPos-ships[i]->getPos();
			meToHit.normalize();
			shipToHit.normalize();
			if(fabs(meToHit * shipToHit) > 0.999)
			{
				printf("***********Detected parallel laser and ship-to-hit, might be a bad collision\n");
				continue;
			}
#ifdef RNG_LOGGING
			printf("LogRDM Bullet %i just hit %s, cast length %.2f, isE = %i, age = %.2f\n", 
				mObjectID, ships[i]->getName().c_str(), length, mIsEnemy, mAge);
			printf("direction:  %.2f, %.2f, %.2f\n", dir.x(), dir.y(), dir.z());
			float distToShip = (pos-ships[i]->getPos()).length();
			printf("LogRDM dist:  %.2f.  Target dist:  %.2f\n", (pos-hitPos).length(), distToShip);
			if(distToShip > length)
			{
			//	printf("LogDRM ******************************* bullet hit invalid target!\n");
			}
#endif
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

