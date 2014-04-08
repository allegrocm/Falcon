//
//  Bullet.h
//  Millennium Falcon
//
//  Created by Ken Kopecky II on 4/4/14.
//
//

#ifndef __Millennium_Falcon__Bullet__
#define __Millennium_Falcon__Bullet__

#include "GameObject.h"

//a Bullet is the base class for anything fired from a weapon

class Bullet : public GameObject
{
public:
	Bullet();
	virtual bool update(float dt);
	virtual void addGeometry();
	virtual bool checkHit();		//did we hit something of the same affiliation as our target?
	virtual void peter();			//poof or vanish or whatever this does when it doesn't hit anything
	virtual void explode(osg::Vec3 whereExactly);			//kboom!  Pass in the exact hit position

//these are pretty much invariant so I'm gonna be a rebel and make them public
//I don't feel like putting in a bunch of getters that will only be used once
	osg::Vec3				mVel;			//velocity
	GameObject*				mTarget;
	GameObject*				mShooter;
	float					mLife;		//how long can this last before it peters out?
	int						mDamage;	//how hard does this hit?
	bool					mIsEnemy;	//was this fired by an enemy?
};


#endif /* defined(__Millennium_Falcon__Bullet__) */
