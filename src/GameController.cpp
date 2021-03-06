//
//  GameController.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky on 4/21/14.
//
//

#include <stdio.h>
#include <stdlib.h>


#include "GameController.h"
#include "FalconApp.h"
#include "EnemyController.h"
#include "Spacecraft.h"
#include "ComputerScreen.h"
#include "KSoundManager.h"
#include "Falcon.h"
#include "ROM.h"
#include "EventAudio.h"

GameController& GameController::instance()
{
	return *FalconApp::instance().getGameController();
}

GameController::GameController()
{
	reset();
	mWeLost = false;
}

void GameController::reset()
{
	printf("Resetting to pregame!\n");
	mTime = 0;
	mMode = PRE_GAME;
	mSwitchTime = 0;
	mModeTime = 0;
	mStats.reset();
	KSoundManager::instance()->setMusicVolume(ROM::MUSIC_VOLUME);
	mSoundTimer = 0;

	
}


bool GameController::modeTimeJustPassed(float t)
{
	if(mModeTime >= t && mModeTime - mLastDT < t) return true;
	return false;
}

void GameController::update(float dt)
{
	mLastDT = dt;
	mTime += dt;
	mSoundTimer -= dt;
	mSwitchTime -= dt;
	
	
	
	if(mSwitchTime + dt > 0 && mSwitchTime <= 0)		//is it time to switch modes?
	{
		printf("Switch mode!\n");
		if(mMode == PRE_GAME)
		{
			printf("To pre game!\n");
			startGame();
		}
		else if(mMode == MAIN_GAME)
		{
			printf("To Main game!\n");
			EnemyController::instance().reset();		
			reset();
			
		}
		return;
		
	}
	
	if(mMode == PRE_GAME)
	{
		preGame(dt);
	}
	else if(mMode == MAIN_GAME)
	{
		mainGame(dt);
	}
	
	
	mModeTime += dt;
	
}

void GameController::startGame()
{
	printf("Reset the game!\n");
	mMode = MAIN_GAME;
	mStats.reset();
	mModeTime = 0;
	mWeLost = false;

}

void GameController::end()
{
	mMode = POST_LEVEL;
}

void GameController::enemyWasKilled(Spacecraft* c)
{
	//maybe play a sound to congratz us
	if(EnemyController::instance().isDone())
	{
		EventAudio::instance().eventHappened("lastEnemyKilled");

	}
	if(!mWeLost)
		EventAudio::instance().eventHappened("EnemyKilled");
	mStats.score += c->getScore();
	printf("Enemy died at t = %.2f\n", mModeTime);
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
//		printf("Pregame started!\n");
		FalconApp::instance().getScreen()->setStatusText("Press the left gamepad button to begin");
		KSoundManager::instance()->fadeIntoSong(1.0, std::string("data/sounds/") + ROM::MAIN_MUSIC);
		//set up text for the computer screen
		FalconApp::instance().getScreen()->setButtonText(0, "Begin", "Begin");
		FalconApp::instance().getScreen()->setButtonText(1, "");
		FalconApp::instance().getScreen()->setButtonText(2, "");
		FalconApp::instance().getScreen()->setButtonText(3, "");
		FalconApp::instance().getScreen()->setIsUp(true);		//put the screen so we know to interact with it
		
	
	}
	
	
	//button 1 will start the fight!
	bool switchButton = FalconApp::instance().getButton(1) == FalconApp::TOGGLE_ON;

	//auto start in zero player mode
	if(FalconApp::instance().zeroPlayerMode())
	{
		switchButton = false;
		if(mModeTime > 5.0 && mModeTime - dt <= 5.0)
			switchButton = true;
	}
	
	if(switchButton && !isSwitching())
	{
		int seed = mTime * 100;
		static int times = 0;
		times++;
		//in zero player mode, use pre-existing seeds for no randomosity
		if(FalconApp::instance().zeroPlayerMode())
			seed = times;
		srand(seed);		//use the time we start to seed the RNG
		printf("Seed RNG to %i\n", seed);
		mSwitchTime = 3.0;
		FalconApp::instance().getScreen()->setStatusText("Incoming enemies detected!");
		FalconApp::instance().getScreen()->setButtonChangeText(0, "");
		KSoundManager::instance()->fadeIntoSong(2.0, std::string("data/sounds/") + ROM::COMBAT_MUSIC);
	}
}


void GameController::mainGame(float dt)
{
	//the start of pregame
	if(mModeTime == 0)
	{
		//printf("Main Game started!\n");
		FalconApp::instance().getScreen()->setStatusText("UNDER ATTACK");
		mJumpTime = 6;
		

	}
	
	if(modeTimeJustPassed(1.5))		//play random Han sound when we start!
	{
		EventAudio::instance().eventHappened("beginGame");
		FalconApp::instance().getScreen()->setIsUp(false);		//put the screen down
	}
	
	if(EnemyController::instance().isDone() && !mWeLost)
	{
		mJumpTime -= dt;
		if(mJumpTime < 3.0 && mJumpTime + dt >= 3.0)
		{
			//get us out of here!

			EventAudio::instance().eventHappened("timeToGo");
			FalconApp::instance().getScreen()->setStatusText("Initiating hyperspace jump...");
			printf("Getting ready for hyperspace....\n");

		}

		if(mJumpTime < 0 && mJumpTime + dt >= 0)		//did jumptime just pass zero?
		{
			FalconApp::instance().getFalcon()->jump();
			mSwitchTime = 10.0;
			printf("We won!  Going to hyperspace....\n");
		}


		
	}
	else if(mWeLost)		//did we lose?  run away
	{
		//mSwitchTime -= dt;
		//printf("We lost.  Switch time at %.2f....\n", mSwitchTime);
	}
	else
	{
		EventAudio::instance().eventHappened("randomFighting");	//triggered every frame
	}
	
	
	
}

void GameController::falconLost()
{
	mWeLost = true;
	mSwitchTime = 7.0;
	FalconApp::instance().getScreen()->setStatusText("WARNING:  HULL BREACH.");
	FalconApp::instance().getScreen()->setIsUp(true);		//put the screen back up
}


void Stats::reset()
{
	score = shotsFired = elapsedTime = shotsHit = 0;
	health = maxHealth = ROM::FALCON_HITPOINTS;
	EventAudio::instance().reset();
}


