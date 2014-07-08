//
//  Spacecraft.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky II on 3/7/14.
//
//

#include "Spacecraft.h"
#include "KSoundManager.h"

using namespace osg;

Spacecraft::Spacecraft()
{
	mIsEnemy = true;
	mDead = false;
	mEngineSound = NULL;
	mHP = 2;
	mScore = 10;
	mPlayer =  NULL;
}


Spacecraft::~Spacecraft()
{
	///eh...
}

bool Spacecraft::update(float dt)
{
	GameObject::update(dt);
	
	
	mGun.update(dt);
	
	//shoot if our gun is firing bursts
	if(mGun.canAutofire())
	{
//		printf("autofire shot %i\n", mGun.mBurstCounter+1);
		shoot();
	}
	return !mDead;
}

bool Spacecraft::shoot()
{
	return false;
}

void Spacecraft::kill()
{
	explode();
}

