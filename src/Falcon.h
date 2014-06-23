//
//  Falcon.h
//  Millennium Falcon
//
//  Created by Ken Kopecky II on 3/7/14.
//
//

#ifndef __Millennium_Falcon__Falcon__
#define __Millennium_Falcon__Falcon__

#include "Spacecraft.h"
#include "Falcon.h"
#include "Gun.h"

//the Millennium Falcon.  Will eventually include an interior, maybe can take damage, whatever

class Hyperspace;
class Falcon : public Spacecraft
{
public:
	Falcon();
	bool shoot();			//pewpewpew!
	bool update(float dt);
	void aimAt(osg::Vec3 location);			//point the gun at a specific spot
	osg::PositionAttitudeTransform* getAimedPart()	{return mAimedPart;}		//for connecting things to the turret
	void jump();		//initiate hyperjump!
	virtual void wasHit(Bullet* b, osg::Vec3 hitPos);

	//shoot bullets at the enemy player
	void updateAutoTurret(float dt);

protected:

	osg::Vec3 mAimTarget;		//where is the gun aiming?
	osg::ref_ptr<osg::PositionAttitudeTransform> mAimedPart;	//this is the part that moves when we aim
	osg::ref_ptr<osg::PositionAttitudeTransform> mUpperAimedPart;	//the turret part, basically
	Hyperspace* mHyperspace;
	Gun mAutoTurret;	//under-belly auto turret.  may not have any graphical rep.  fires at the enemy player
	float mTurretVisibleTime;		//how long has the turret been able to see the enemy player?
};
#endif /* defined(__Millennium_Falcon__Falcon__) */
