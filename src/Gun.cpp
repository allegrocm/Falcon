#include "Gun.h"



Gun::Gun()
{
	mFireTimer = 0;
	mBurstTimer = 0;
	mBurstDelay = 1.25;
	mShotDelay = 0.15;
	mBurstCounter = 0;
	mShotsPerBurst = 4;
	mAutoburst = false;
}

void Gun::update(float dt)
{
	mFireTimer -= dt;
	mBurstTimer -= dt;
	
	//once our burst times out, we can fire again
	if(mBurstTimer <= 0)
		mBurstCounter = 0;
}

bool Gun::fire()
{
	if(mFireTimer > 0 || mBurstCounter >= mShotsPerBurst)
	{
		return false;
	}
	
	if(!mBurstCounter)
	{
		//did we just start a burst?
		mBurstTimer = mBurstDelay;
	}
	mBurstCounter++;
	mFireTimer = mShotDelay;
	
	return true;
	
	
}

bool Gun::canAutofire()
{
	if(mBurstCounter > 0 && mFireTimer <= 0 && mBurstCounter < mShotsPerBurst) return true;
	return false;
}


