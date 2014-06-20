//
//  StupidPlaceholderShip.h
//  Millennium Falcon
//
//  Created by Ken Kopecky II on 3/7/14.
//
//

#ifndef __Millennium_Falcon__StupidPlaceholderShip__
#define __Millennium_Falcon__StupidPlaceholderShip__

#include "Spacecraft.h"
#include <osg/ShapeDrawable>


//this spaceship flies around in a circle and sometimes maybe shoots.
class StupidPlaceholderShip : public Spacecraft
{
public:
	StupidPlaceholderShip();
	virtual bool update(float dt);			//flies around in a circle....
	virtual void explode();
	virtual void wasHit(Bullet* b, osg::Vec3 hitPos);
	virtual void drawDebug();
	virtual bool shoot();
	virtual void AIControl(float dt, bool canFire = true);		//Shawn's control scheme.  can be activated by the player, but no shooting
	virtual void playerControl(float dt);	//if a player is controlling this ship
	virtual bool willHitFalcon(float distance);
	virtual void loadTIEModel();			//load our normal TIE fighter
	virtual bool isLocalEnemy();			//is this human-controlled on this node?
	virtual void pickNewDestination();		//fly somewhere that's not straight into the MF
protected:

	bool mMovingAway;
	bool mTurning;
	osg::Vec3 mTargetPosition;
	float mCurrentTurnTime;
	float mTimeToTurn;
	float mTimeTillShoot;		//how long till we open fire once we're facing the falcon?
	float mSpeed;				//current speed
	float mTopSpeed;			//max speed
	float mTargetSpeed;			//desired speed
	osg::ref_ptr<osg::Sphere> mEngineGlow;
};


#endif /* defined(__Millennium_Falcon__StupidPlaceholderShip__) */
