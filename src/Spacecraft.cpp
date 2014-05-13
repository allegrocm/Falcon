//
//  Spacecraft.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky II on 3/7/14.
//
//

#include "Spacecraft.h"


using namespace osg;

Spacecraft::Spacecraft()
{
	mIsEnemy = true;
	mDead = false;
	mEngineSound = NULL;
	mHP = 2;
	mScore = 10;
}


Spacecraft::~Spacecraft()
{
	///eh...
}

bool Spacecraft::update(float dt)
{
	GameObject::update(dt);
	
	//send our position and velocity to the SoundManager so we can have stereo and doppler and all that good stuff
	Vec3 pos = getPos();
	Vec3 vel = getVel();
	

	
	return !mDead;
}

bool Spacecraft::shoot()
{
	return false;
}


