//
//  GameController.h
//  Millennium Falcon
//
//  Created by Ken Kopecky on 4/21/14.
//
//

#ifndef __Millennium_Falcon__GameController__
#define __Millennium_Falcon__GameController__

#include <iostream>

class Spacecraft;

//a stats holder class
struct Stats
{
	int score;
	int shotsFired;
	int health;
	int maxHealth;
	float elapsedTime;
	int shotsHit;			//how many times the we've hit the enemy.  For calculating accuracy
	
	void reset();

};
//manages the state of the game, score, enemy spawning, etc

class GameController
{
public:
	enum GameMode {
		PRE_GAME,			//before the game begins.  The program will start in this mode
		PRE_LEVEL,			//level prep.  hit go here to begin.  maybe an opening cinematicish thing
		MAIN_GAME,			//kill kill kill!
		POST_LEVEL,			//stats, scores, etc
		GAME_OVER,			//u die plz try aken
		NUM_GAME_MODES};	//dunno if all these will be used yet
	static GameController& instance();

	GameController();
	void startGame();
	void end();
	void update(float dt);
	void enemyWasKilled(Spacecraft* c);		//called whenever an enemy dies
	void enemyWasHit(Spacecraft* c);		//called whenever an enemy is hit
	Stats& getStats()	{return mStats;}
	bool isSwitching()					{return mSwitchTime >0;}		//are we already getting ready to switch game modes?
	GameMode	getMode()				{return mMode;}
	void preGame(float dt);				//do pregame mode stuff
	void mainGame(float dt);
	bool modeTimeJustPassed(float val);	//did we cross this time value at this timestep?  for triggering events
	bool canPlaySound()	{return (mSoundTimer < 0);}
	void justPlayedSound()	{mSoundTimer = 5;}	//did we play a sound elsewhere that we don't want Han talking over?
	void reset();
	void falconLost();			// ):
	bool vaderWon()	{return mWeLost;}
protected:
	float mTime;
	float mJumpTime;			//we've initialized hyperjump.  how long till it happens?
	
	GameMode mMode;
	Stats mStats;					//holds score, etc
	float mSwitchTime;				//if this is >0, we're getting ready to switch to a new game mode
	float mModeTime;				//how long have we been in this mode?
	float mLastDT;					//supports the modeTimeJustPassed() function
	float mSoundTimer;	//don't play other random sounds till this is < 0
	bool mWeLost;				//did the falcon lose?
};
#endif /* defined(__Millennium_Falcon__GameController__) */
