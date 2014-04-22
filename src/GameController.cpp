//
//  GameController.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky on 4/21/14.
//
//

#include "GameController.h"
#include "FalconApp.h"
#include "EnemyController.h"
#include "Spacecraft.h"


GameController& GameController::instance()
{
	return *FalconApp::instance().getGameController();
}

GameController::GameController()
{
	mTime = 0;
	mMode = MAIN_GAME;
}



void GameController::update(float dt)
{
	mTime += dt;
}

void GameController::start()
{
	mMode = MAIN_GAME;
	mStats.reset();
}

void GameController::end()
{
	mMode = POST_LEVEL;
}

void GameController::enemyWasKilled(Spacecraft* c)
{
	mStats.score += c->getScore();

}

void GameController::enemyWasHit(Spacecraft* c)
{
	mStats.shotsHit++;
}





