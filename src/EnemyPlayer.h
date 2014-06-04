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
	EnemyControlInput()	{trigger = false;  xAxis = 0; yAxis = 0; thrustAxis = 0; button1 = 0; button2 = 0;}
	int trigger;
	int button1;
	int button2;
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
	bool isReadyForShip() {return (!mShip && mDeadTimer <= 0);}			//ready to get into a new ship?
protected:
	Spacecraft* mShip;
	EnemyControlInput mInput;
	float mDeadTimer;				//how long we been dead?

};
#endif /* defined(__Millennium_Falcon__EnemyPlayer__) */
