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
	EnemyControlInput()	{trigger = false;  xAxis = 0; yAxis = 0; thrustAxis = 0; button1 = 1; button2 = 0; noInput = true;}
	int trigger;
	int button1;
	int button2;
	bool noInput;		//true if there's not actually a controller on there
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

	//are we temporarily under AI control? due to no controller or flying in an illegal place or putting on autopilot
	//used for display on the computer
	bool AIControl;
	float deadTimer()	{return mDeadTimer;}
protected:
	Spacecraft* mShip;
	EnemyControlInput mInput;
	float mDeadTimer;				//how long we been dead?

};
#endif /* defined(__Millennium_Falcon__EnemyPlayer__) */
