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
#include "Layers.h"
#include "VaderTIE.h"
#include <stdlib.h>
#include "SpaceBox.h"

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
	mSwarm = new Group();
	mSwarm->setNodeMask(1 << BACKGROUND_LAYER);		//only show this on the background layer for speedz
	FalconApp::instance().getSpaceBox()->getNearGroup()->addChild(mSwarm);
	reset();

}

void EnemyController::reset()
{
	killAll();
//just add a few generic spaceships for now
	int numToMake = 3;

	int totalEnemies = 10;
	mMaxEnemies = 7;
	
	Defaults::instance().getValue("InitialShipCount", numToMake);
	Defaults::instance().getValue("demoBattleTotalShipCount", totalEnemies);
	Defaults::instance().getValue("demoBattleMaxShipCount", mMaxEnemies);

	populateSwarm(totalEnemies);
	
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
		if(!getShipsLeftToSpawn()) chance = 0;		///..unlesss there are none left to spawn

		//but always spawn if there's a human player needing a ship
		//is there a human player waiting to respawn?
		//if it's ready, spawn right away.  otherwise don't spawn at all
		if(!mPlayer->getShip() && (getShipsLeftToSpawn() || mEnemies.size()))
		{
			chance = mPlayer->isReadyForShip() * 1000;
		}
		
		if(1.0 * rand() /  RAND_MAX < chance * dt)
		{
			//spawn a ship!
			spawnEnemy();
		}
		
	}
	
	//if there's no active player and hasn't been for a while, spawn it anyway
	if(GameController::instance().getMode() == GameController::PRE_GAME)
	{
		if(mPlayer->isReadyForShip())
			spawnEnemy(true);
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
	
	//if we're resetting, make sure the enemy player has a ship
	if(mPlayer && mPlayer->getShip() == NULL && initing)
		playerForShip = mPlayer;
	
	Matrix spawnPos;
	
	//if there are ships left in our swarm, grab one of them
	if(getShipsLeftToSpawn())
	{
		int whichSpawn = rand()%getShipsLeftToSpawn();
		printf("spawning from position %i in the swarm of %i\n", whichSpawn, getShipsLeftToSpawn());
		Node* spawner = mSwarm->getChild(whichSpawn);
		MatrixTransform* spawnMat = dynamic_cast<MatrixTransform*>(spawner);
		spawnPos = spawnMat->getMatrix();
<<<<<<< HEAD
		spawnPos = FalconApp::instance().getPotentialSpawnPosition((playerForShip != NULL));
		
=======
>>>>>>> c21ba12bf057eaff21f9099d253027b94ed5252c
		mSwarm->removeChildren(whichSpawn, 1);		//pull this guy out of the swarm cuz he turned into a real ship
	}
	else
	{
		printf("no swarm left.  just spawning willy-nilly\n");
		spawnPos = FalconApp::instance().getPotentialSpawnPosition(true);
	}
	
	StupidPlaceholderShip* sps;

	if(playerForShip)
	{
		printf("Vader tie!\n");
		sps = new VaderTIE();
		sps->setPlayer(playerForShip);
		playerForShip->setShip(sps);
	}
	else sps = new StupidPlaceholderShip();
	sps->loadTIEModel();
	addShip(sps);
	sps->setTransform(spawnPos);
	sps->setVel(sps->getForward() * sps->getVel().length());		//set the initial velocity to forward
<<<<<<< HEAD
	static int spawnCount = 0;
	spawnCount++;
	printf("Enemy %i spawned at %.2f, %.2f, %.2f\n", spawnCount, spawnPos.ptr()[12], spawnPos.ptr()[13], spawnPos.ptr()[14]);
=======

>>>>>>> c21ba12bf057eaff21f9099d253027b94ed5252c
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
	return (mEnemies.size() == 0 && getShipsLeftToSpawn() <= 0);
}


void EnemyController::drawDebug()
{
	for(size_t i = 0; i < mEnemies.size(); i++)
		mEnemies[i]->drawDebug();
}

void EnemyController::killAll()
{
	printf("******** kill em all!\n");
	for(size_t i = 0; i < mEnemies.size(); i++)
	{
		mEnemies[i]->explode(true);
	}
	mSwarm->removeChildren(0, mSwarm->getNumChildren());
<<<<<<< HEAD
	FalconApp::instance().getPotentialSpawnPosition(true);
=======
	
>>>>>>> c21ba12bf057eaff21f9099d253027b94ed5252c
}

void EnemyController::setEnemyInput(int which, EnemyControlInput i)
{
	if(which < mEnemyPlayers.size())
		mEnemyPlayers[which]->setInput(i);
}

void EnemyController::populateSwarm(int howMany)
{
	printf("populating swarm with %i ships\n", howMany);
	//load up a TIE fighter for each spawn
	for(int i = 0; i < howMany; i++)
	{
		bool nearShip = (rand()%4 != 0);
<<<<<<< HEAD
		nearShip = false;		//HACK till we figure out how to spawn near capital ship and still be fun
=======
>>>>>>> c21ba12bf057eaff21f9099d253027b94ed5252c
		MatrixTransform* TIENode = Util::loadModel("data/models/tief3DS/TIEF_10.3ds", 1.0, -90);
		MatrixTransform* TIEXform = new MatrixTransform;
		
		Matrix mat = FalconApp::instance().getPotentialSpawnPosition(nearShip);
		TIEXform->setMatrix(mat);
		if(nearShip)					//if this wasn't near a ship, it's a "random" spawn, and doesn't need a placeholder graphic
			TIEXform->addChild(TIENode);
		
		mSwarm->addChild(TIEXform);
		TIEXform->getOrCreateStateSet()->setRenderingHint(osg::StateSet::OPAQUE_BIN);
	}

}
