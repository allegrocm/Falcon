//
//  EnemyController.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky on 4/17/14.
//
//

#include "EnemyController.h"
#include "FalconApp.h"
#include "Spacecraft.h"
#include "StupidPlaceholderShip.h"
#include "Defaults.h"
#include "Util.h"
#include "GameController.h"
#include "EventAudio.h"

#include <stdlib.h>

using namespace osg;
EnemyController& EnemyController::instance()
{
	return *(FalconApp::instance().getEnemyController());
}

EnemyController::EnemyController(bool TIENode)
{
	mEnemyPlayers.push_back(new EnemyPlayer);
//	if(TIENode)
		mPlayer = mEnemyPlayers.back();
	//else
	//	mPlayer = NULL;
	reset();

}

void EnemyController::reset()
{
//just add a few generic spaceships for now
	int numToMake = 3;

	mLeftToSpawn = 10;
	mMaxEnemies = 7;
	
	Defaults::instance().getValue("InitialShipCount", numToMake);
	Defaults::instance().getValue("demoBattleTotalShipCount", mLeftToSpawn);
	Defaults::instance().getValue("demoBattleMaxShipCount", mMaxEnemies);


	for(int i = 0; i < numToMake; i++)
	{
		StupidPlaceholderShip* sps = new StupidPlaceholderShip();
		sps->setCircleOrigin(Vec3(-85 + 20 * i, 15, -100));
		addShip(sps);
	}
}

void EnemyController::update(float dt)
{
	for(size_t i = 0; i < mEnemies.size(); i++)
	{
	
		//delete enemies that have died
		if(mEnemies[i]->update(dt) == false)
		{
			FalconApp::instance().getModelGroup()->removeChild(mEnemies[i]->getRoot());		//remove the enemy from the scenegraph
			delete mEnemies[i];
			mEnemies[i] = mEnemies.back();
			mEnemies.pop_back();
		}

	}
	
	//do we have any enemy players that have no one to control?  get them someone!
	for(size_t i = 0; i < mEnemyPlayers.size(); i++)
	{
		mEnemyPlayers[i]->update(dt);
		if(mEnemyPlayers[i]->getShip() == NULL && mEnemyPlayers[i]->isReadyForShip())
		{
			//find a ship for this player
			Spacecraft* enemy = NULL;
			for(size_t i = 0; i < mEnemies.size(); i++)
			{
				if(mEnemies[i]->getPlayer() == NULL)
				{
					enemy = mEnemies[i];
				}
				
			}
				
			//link them up!
			if(enemy)
			{
				mEnemyPlayers[i]->setShip(enemy);
				enemy->setPlayer(mEnemyPlayers[i]);
			}
		}
		
	}
	
	//should we spawn new enemies?  Don't spawn unless we're actually playing
	if(mLeftToSpawn && GameController::instance().getMode() == GameController::MAIN_GAME)
	{
		int diff = mMaxEnemies - mEnemies.size();		//how many more enemies can we put in play?
		float chance = 0.4 * diff;
		
		//never let us have no enemies
		if(!mEnemies.size())
			chance = 10000;
		if(1.0 * rand() /  RAND_MAX < chance * dt)
		{
			//spawn a ship!
			mLeftToSpawn--;
			StupidPlaceholderShip* sps = new StupidPlaceholderShip();
			sps->setCircleOrigin(Vec3(Util::random(-40, 40), Util::random(-10, 40), Util::random(-50, -150)));
			sps->update(0);		//send a zero update so this ship positions itself immediately
			addShip(sps);
			printf("Spawn ship!\n");
			EventAudio::instance().eventHappened("spawnEnemy");
		}
		
	}
	
}

void EnemyController::addShip(Spacecraft* s)
{
	//check if it's already here
	for(size_t i = 0; i < mEnemies.size(); i++)
		if(mEnemies[i] == s) return;
	mEnemies.push_back(s);
	FalconApp::instance().getModelGroup()->addChild(s->getRoot());		//remove the enemy from the scenegraph
}

bool EnemyController::isDone()
{
	return (mEnemies.size() == 0 && mLeftToSpawn <= 0);
}


void EnemyController::drawDebug()
{
	for(size_t i = 0; i < mEnemies.size(); i++)
		mEnemies[i]->drawDebug();
}

void EnemyController::killAll()
{
	for(size_t i = 0; i < mEnemies.size(); i++)
	{
		mEnemies[i]->explode();
	}
	mLeftToSpawn = 0;
	
}

void EnemyController::setEnemyInput(int which, EnemyControlInput i)
{
	if(which < mEnemyPlayers.size())
		mEnemyPlayers[which]->setInput(i);
}
