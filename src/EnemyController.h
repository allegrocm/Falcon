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

//manages spawning of enemies

class Spacecraft;

class EnemyController
{
public:
	static EnemyController& instance();		//return the primary enemycontroller in the FalconApp
	EnemyController();
	void update(float dt);
	std::vector<Spacecraft*> getShips()	{return mEnemies;}
	void addShip(Spacecraft* s);
	bool isDone();			//everything dead and no more spawning?
	void drawDebug();
protected:
	std::vector<Spacecraft*> mEnemies;
	int mMaxEnemies;			//max in play
	int mLeftToSpawn;			//how many more enemies will we be spawning
	
	

};

#endif /* defined(__Millennium_Falcon__EnemyController__) */
