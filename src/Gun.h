

#ifndef GUN_TIMER_DOT_H
#define GUN_TIMER_DOT_H

//handles timing of guns and burst fire, etc
class Gun
{
public:
	Gun();
	void update(float dt);
	bool fire();			//returns true if we were able to spawn a shot.  restarts timers, so hold onto this return value
	bool canAutofire();		//returns true if we've started a burst and autoburst is true.  does NOT restart shot timers
	//parameters related to firing our weapon
	//shots are fired in bursts...pewpewpew....pewpewpew....
	float mFireTimer;			//how long till our next bullet can be fired
	float mBurstTimer;			//how long till we can begin a new burst?
	float mBurstDelay;			//time between bursts
	float mShotDelay;			//initial time between bullet fires
	int mBurstCounter;			//how many shots have been fired this burst?
	int mShotsPerBurst;			//how many total shots per burst?
	bool mAutoburst;			//fires a full set of shots when burst is intiated

};

#endif

