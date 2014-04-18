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

using namespace osg;
EnemyController& EnemyController::instance()
{
	return *(FalconApp::instance().getEnemyController());
}

EnemyController::EnemyController()
{
	//just add a few generic spaceships for now
	int numToMake = 5;
	Defaults::instance().getValue("InitialShipCount", numToMake);
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
	
}

void EnemyController::addShip(Spacecraft* s)
{
	//check if it's already here
	for(size_t i = 0; i < mEnemies.size(); i++)
		if(mEnemies[i] == s) return;
	mEnemies.push_back(s);
	FalconApp::instance().getModelGroup()->addChild(s->getRoot());		//remove the enemy from the scenegraph
}



