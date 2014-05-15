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
#include "KSoundManager.h"
#include "Falcon.h"
#include "ROM.h"
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
//		printf("Pregame started!\n");
		FalconApp::instance().getScreen()->setStatusText("All systems normal");
		KSoundManager::instance()->fadeIntoSong(1.0, std::string("data/sounds/") + ROM::MAIN_MUSIC);
		//set up text for the computer screen
		FalconApp::instance().getScreen()->setButtonText(0, "Begin");
		FalconApp::instance().getScreen()->setButtonText(1, "");
		FalconApp::instance().getScreen()->setButtonText(2, "");
		FalconApp::instance().getScreen()->setButtonText(3, "");
	}
	
	//button 1 will start the fight!
	if(FalconApp::instance().getButton(1) == FalconApp::TOGGLE_ON && !isSwitching())
	{
		srand(mTime * 100);		//use the time we start to seed the RNG
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
		mJumpTime = 3;
		

	}
	
	if(modeTimeJustPassed(1.5))		//play random Han sound when we start!
	{
		std::string hanStartPhrases[] = {"han/get_out.wav", "han/goodluck.wav", "han/welltake.wav"};
		int numPhrases = sizeof(hanStartPhrases)/sizeof(std::string);
		std::string phrase = hanStartPhrases[rand()%numPhrases];
		KSoundManager::instance()->playSound(std::string("data/sounds/")+phrase, 1.0, -1);
		justPlayedSound();
	}
	
	if(EnemyController::instance().isDone())
	{
		mJumpTime -= dt;
		if(mJumpTime < 0 && mJumpTime + dt >= 0)		//did jumptime just pass zero?
		{
			FalconApp::instance().getFalcon()->jump();
		}
		
	}
	
}


void Stats::reset()
{
	score = shotsFired = elapsedTime = shotsHit = 0;
	health = maxHealth = ROM::FALCON_HITPOINTS;
}


