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
protected:
	std::vector<Spacecraft*> mEnemies;
	

};

#endif /* defined(__Millennium_Falcon__EnemyController__) */
