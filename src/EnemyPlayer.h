//
//  EnemyPlayer.h
//  Millennium Falcon
//
//  Created by Ken Kopecky on 6/2/14.
//
//

#ifndef __Millennium_Falcon__EnemyPlayer__
#define __Millennium_Falcon__EnemyPlayer__

#include <iostream>
class Spacecraft;


class EnemyControlInput
{
	public:
	//enemy controls are simple!
	EnemyControlInput()	{trigger = false;  xAxis = 0; yAxis = 0; thrustAxis = 0;}
	bool trigger;
	float xAxis;
	float yAxis;
	float thrustAxis;

};

//EnemyPlayer is a way for normal enemy ships to be controlled by a human player

class EnemyPlayer
{
public:
	EnemyPlayer();
	void update(float dt);
	void setInput(EnemyControlInput i);
	Spacecraft* getShip()			{return mShip;}
	void setShip(Spacecraft* ship);
	EnemyControlInput getInput()	{return mInput;}
	void shipDied();				//called by the spaceship we control
protected:
	Spacecraft* mShip;
	EnemyControlInput mInput;

};
#endif /* defined(__Millennium_Falcon__EnemyPlayer__) */
