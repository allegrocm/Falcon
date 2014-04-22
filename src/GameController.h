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
	float elapsedTime;
	int shotsHit;			//how many times the we've hit the enemy.  For calculating accuracy
	
	void reset()
	{
		score = shotsFired = elapsedTime = shotsHit = 0;
		health = 15;
	}
	

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
	void start();
	void end();
	void update(float dt);
	void enemyWasKilled(Spacecraft* c);		//called whenever an enemy dies
	void enemyWasHit(Spacecraft* c);		//called whenever an enemy is hit
	Stats& getStats()	{return mStats;}
protected:
	float mTime;
	GameMode mMode;
	Stats mStats;					//holds score, etc
};
#endif /* defined(__Millennium_Falcon__GameController__) */