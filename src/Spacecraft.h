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
#include "KSoundManager.h"
#include "Gun.h"

class Bullet;
class EnemyPlayer;		//enemy ships can be controlled by players
class Spacecraft: public GameObject
{
public:
	Spacecraft();
	virtual ~Spacecraft();
	virtual bool update(float dt);		//return false if this should be destroyed
	virtual bool shoot();				//return false if we couldn't shoot for some reason (e.g., out of ammo)
	virtual bool isEnemy()			{return mIsEnemy;}		//friend or foe?
	virtual void wasHit(Bullet* b, osg::Vec3 hitPos)		{}
	virtual osg::Vec3 getVel()	{return mVel;}
	virtual void setVel(osg::Vec3 v)	{mVel = v;}
	virtual int getHP()					{return mHP;}
	virtual int getScore()				{return mScore;}
	virtual void kill();				//blow it up
	virtual void explode()				{}
	virtual void setPlayer(EnemyPlayer* p)		{mPlayer = p;}
	virtual EnemyPlayer* getPlayer()			{return mPlayer;}
	virtual void loadTIEModel()	{printf("SPS load thai\n");}
protected:
	//basic things might go here, like ammo count, firing rate, speed/acceleration/turning stats, allegiance, etc
	bool			mIsEnemy;
	bool mDead;		//should we be deleted now?
	osg::Vec3 mVel;		//velocity
	SkySound* mEngineSound;
	int mScore;				//score for blowing this up
	int mHP;				//hitpoints
	Gun mGun;
	EnemyPlayer*			mPlayer;
};


#endif /* defined(__Millennium_Falcon__Spacecraft__) */
