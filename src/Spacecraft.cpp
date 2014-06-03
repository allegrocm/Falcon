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
	
	//send our position and velocity to the SoundManager so we can have stereo and doppler and all that good stuff
	Vec3 pos = getPos();
	Vec3 vel = getVel();
	
	mGun.update(dt);
	
	//shoot if our gun is firing bursts
	if(mGun.canAutofire())
	{
//		printf("autofire shot %i\n", mGun.mBurstCounter+1);
		shoot();
	}
	if(mEngineSound)
		KSoundManager::instance()->setSound3DInfo(mEngineSound, pos.x(), pos.y(), pos.z(), vel.x(), vel.y(), vel.z());
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

