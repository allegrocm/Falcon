//
//  EnemyPlayer.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky on 6/2/14.
//
//

#include "EnemyPlayer.h"
#include "Spacecraft.h"
#include "Util.h"
#include "FalconApp.h"

EnemyPlayer::EnemyPlayer()
{
	mShip = NULL;
	mDeadTimer = 0;
	AIControl = false;
}


void EnemyPlayer::update(float dt)
{
	mDeadTimer -= dt;
}

void EnemyPlayer::setInput(EnemyControlInput i)
{
	i.noInput = false;
	mInput = i;
}

void EnemyPlayer::shipDied()
{
	mDeadTimer = 5.0;
	mShip = NULL;
}

void EnemyPlayer::setShip(Spacecraft* ship)
{

	if(ship == mShip) return;
	mShip = ship;
//	ship->setPlayer(this);
	
}

