//
//  Debris.h
//  Millennium Falcon
//
//  Created by Ken Kopecky II on 4/7/14.
//
//

#ifndef __Millennium_Falcon__Debris__
#define __Millennium_Falcon__Debris__


//debris.  may be on fire.  Part of a destroyed ship, hurtling through space
#include "GameObject.h"

class Debris : public GameObject
{
public:
	//modelName:			//pretty self-explanatory
	//generalDirection:  the piece will fly in sort of this direction
	//directionDelta:  how far away from the general direction it can be off (in degrees)
	//speed:  how fast this will go
	//angularSpeed:  how fast this will spin about a random axis (degrees/sec)
	//axisOffset:  position of the axis of rotation in model space
	Debris(osg::Node* model, osg::Vec3 generalDirection, float directionDelta, float speed,
		float angularSpeed = 0, float modelScale = 10);
		
	//set the starting position of this, and the offset of its rotation axis
	void setTransformAndOffset(osg::Matrixf transform, osg::Vec3 offset);
	
	void setLife(float f)	{mLife = f;}
	virtual bool update(float dt);
	
	osg::Vec3 getVel()	{return mVel;}
	void setVel(osg::Vec3 v)	{mVel = v;}
	void setFireAmount(float f)	{mFireAmount = f;}
	virtual void drawDebug();
protected:
	osg::ref_ptr<osg::PositionAttitudeTransform> mOtherPat;		//local rotation
	osg::Vec3 mSpinAxis;
	float mOmega;
	float mLife;			//how long till this disappears
	osg::Vec3 mVel;
	float mFireAmount;		//how much fire do we put out?
};


#endif /* defined(__Millennium_Falcon__Debris__) */
