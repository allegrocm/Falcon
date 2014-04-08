//
//  Spacecraft.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky II on 3/7/14.
//
//

#include "Spacecraft.h"


Spacecraft::Spacecraft()
{
	mIsEnemy = true;
	mDead = false;
}


Spacecraft::~Spacecraft()
{
	///eh...
}

bool Spacecraft::update(float dt)
{
	GameObject::update(dt);
	return !mDead;
}

bool Spacecraft::shoot()
{
	return false;
}


