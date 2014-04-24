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
#include "ComputerScreen.h"
#include <stdio.h>

GameController& GameController::instance()
{
	return *FalconApp::instance().getGameController();
}

GameController::GameController()
{
	mTime = 0;
	mMode = PRE_GAME;
	mSwitchTime = 0;
	mModeTime = 0;
	mStats.reset();
}



void GameController::update(float dt)
{
	mTime += dt;
	
	mSwitchTime -= dt;
	
	if(mMode == PRE_GAME)
	{
		preGame(dt);
	}
	else if(mMode == MAIN_GAME)
	{
		mainGame(dt);
	}
	
	
	mModeTime += dt;
	
	if(mSwitchTime + dt > 0 && mSwitchTime <= 0)		//is it time to switch modes?
	{
		if(mMode == PRE_GAME)
		{
			startGame();
		}
		
	}
	
}

void GameController::startGame()
{
	mMode = MAIN_GAME;
	mStats.reset();
	mModeTime = 0;
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

void GameController::preGame(float dt)
{
	//the start of pregame
	if(mModeTime == 0)
	{
		printf("Pregame started!\n");
		FalconApp::instance().getScreen()->setStatusText("All systems normal");
	}
	
	//button 1 will start the fight!
	if(FalconApp::instance().getButton(1) == FalconApp::TOGGLE_ON && !isSwitching())
	{
		mSwitchTime = 3.0;
		FalconApp::instance().getScreen()->setStatusText("Incoming enemies detected!");
	}
}


void GameController::mainGame(float dt)
{
	//the start of pregame
	if(mModeTime == 0)
	{
		printf("Main Game started!\n");
		FalconApp::instance().getScreen()->setStatusText("UNDER ATTACK");
	}
	


}




