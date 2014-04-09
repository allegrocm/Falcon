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

//the Millennium Falcon.  Will eventually include an interior, maybe can take damage, whatever


class Falcon : public Spacecraft
{
public:
	Falcon();
	void fire();			//pewpewpew!
	void aimAt(osg::Vec3 location);			//point the gun at a specific spot
protected:
	osg::Vec3 mAimTarget;		//where is the gun aiming?
};
#endif /* defined(__Millennium_Falcon__Falcon__) */
