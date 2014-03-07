//
//  Spacecraft.h
//  Millennium Falcon
//
//  Created by Ken Kopecky II on 3/7/14.
//
//

#ifndef __Millennium_Falcon__Spacecraft__
#define __Millennium_Falcon__Spacecraft__

#include <iostream>
#include "GameObject.h"

class Spacecraft: public GameObject
{
public:
	Spacecraft();
	virtual ~Spacecraft();
	virtual bool update(float dt);		//return false if this should be destroyed
	virtual bool shoot();				//return false if we couldn't shoot for some reason (e.g., out of ammo)
	virtual bool isEnemy()			{return mIsEnemy;}		//friend or foe?
protected:
	//basic things might go here, like ammo count, firing rate, speed/acceleration/turning stats, allegiance, etc
	bool			mIsEnemy;
};

#endif /* defined(__Millennium_Falcon__Spacecraft__) */
