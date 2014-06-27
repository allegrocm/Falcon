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
#include "VaderTIE.h"
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
		spawnEnemy(true);
//		bool playerNeedsShip = mPlayer->getShip() == NULL;
//		StupidPlaceholderShip* sps = new StupidPlaceholderShip(playerNeedsShip ? mPlayer : NULL);
//
//		addShip(sps);
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
	
	for(size_t i = 0; i < mEnemyPlayers.size(); i++)
	{
		mEnemyPlayers[i]->update(dt);
	}

		//should we spawn new enemies?  Don't spawn unless we're actually playing
	if(GameController::instance().getMode() == GameController::MAIN_GAME)
	{
		int diff = mMaxEnemies - mEnemies.size();		//how many more enemies can we put in play?
		float chance = 0.4 * diff;

		//never let us have no enemies
		if(!mEnemies.size())
			chance = 10000;
		if(!mLeftToSpawn) chance = 0;		///..unlesss there are none left to spawn

		//but always spawn if there's a human player needing a ship
		//is there a human player waiting to respawn?
		//if it's ready, spawn right away.  otherwise don't spawn at all
		if(!mPlayer->getShip() && (mLeftToSpawn || mEnemies.size()))
		{
			chance = mPlayer->isReadyForShip() * 1000;
		}
		
		if(1.0 * rand() /  RAND_MAX < chance * dt)
		{
			//spawn a ship!
			spawnEnemy();
		}
		
	}
	
}

void EnemyController::spawnEnemy(bool initing)
{
	EnemyPlayer* playerForShip = NULL;		//will this ship be controlled by a human?
	for(size_t i = 0; i < mEnemyPlayers.size() && !playerForShip; i++)
	{
		if(mEnemyPlayers[i]->getShip() == NULL && mEnemyPlayers[i]->isReadyForShip())
			playerForShip = mEnemyPlayers[i];
	}
	
	if(mLeftToSpawn != 0)
		mLeftToSpawn--;
	StupidPlaceholderShip* sps;
	if(playerForShip)
	{
		sps = new VaderTIE();
		sps->setPlayer(playerForShip);
		playerForShip->setShip(sps);
	}
	else sps = new StupidPlaceholderShip();
	sps->loadTIEModel();
	addShip(sps);
	printf("Spawn ship!\n");

	//events and things only happen if we're not initializing the controller
	if(!initing)
	{
		sps->update(0);		//send a zero update so this ship positions itself immediately
		EventAudio::instance().eventHappened("spawnEnemy");
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
