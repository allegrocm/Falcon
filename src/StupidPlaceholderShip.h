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

//this spaceship flies around in a circle and sometimes maybe shoots.
class StupidPlaceholderShip : public Spacecraft
{
public:
	StupidPlaceholderShip();
	void setCircleOrigin(osg::Vec3 p)	{mCenter = p;}
	bool update(float dt);			//flies around in a circle....
	void explode();
	void wasHit(Bullet* b);
	float mRadius;
	float mOffset;
	virtual void drawDebug();
	virtual bool shoot();
protected:

	bool mMovingAway;
	bool mTurning;
	osg::Vec3 targetPosition;
	float mCurrentTurnTime;
	float mTimeToTurn;
	float mTimeTillShoot;		//how long till we open fire once we're facing the falcon?
	float mSpeed;
	osg::Vec3 mCenter;			//center of our ship
	

};


#endif /* defined(__Millennium_Falcon__StupidPlaceholderShip__) */
