//
//  EnemyController.h
//  Millennium Falcon
//
//  Created by Ken Kopecky on 4/17/14.
//
//

#ifndef __Millennium_Falcon__EnemyController__
#define __Millennium_Falcon__EnemyController__

#include <iostream>
#include <vector>

#include "EnemyPlayer.h"

//manages spawning of enemies

class Spacecraft;

class EnemyController
{
public:
	static EnemyController& instance();		//return the primary enemycontroller in the FalconApp
	EnemyController(bool isTIENode);
	void update(float dt);
	std::vector<Spacecraft*> getShips()	{return mEnemies;}
	void addShip(Spacecraft* s);
	bool isDone();			//everything dead and no more spawning?
	void drawDebug();
	void reset();
	void killAll();			//kill all the current enemy ships
	void setEnemyInput(int which, EnemyControlInput i);		//for controlling enemy players
	int getShipsLeftToSpawn()		{return mLeftToSpawn;}
	EnemyPlayer* getPlayer()					{return mPlayer;}
	virtual void spawnEnemy(bool initing = false);				//make an enemy.  maybe make it human-controlled
protected:
	std::vector<Spacecraft*> mEnemies;
	int mMaxEnemies;			//max in play
	int mLeftToSpawn;			//how many more enemies will we be spawning
	
	
	//for...MULTIPLAYER!
	std::vector<EnemyPlayer*>				mEnemyPlayers;
	EnemyPlayer*							mPlayer;				//if this node is used for an enemy POV, which enemy is it?
};

#endif /* defined(__Millennium_Falcon__EnemyController__) */
