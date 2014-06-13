//
//  VaderTIE.h
//  Millennium Falcon
//
//  Created by Ken Kopecky II on 6/12/14.
//
//

#ifndef __Millennium_Falcon__VaderTIE__
#define __Millennium_Falcon__VaderTIE__

#include "StupidPlaceholderShip.h"

//Vader's TIE.  Controlled by a human!

class VaderTIE : public StupidPlaceholderShip
{
public:
	VaderTIE();
	virtual void loadTIEModel();			//load our Vader prototype TIE fighter
	void wasHit(Bullet* b, osg::Vec3 hitPos);
	int getZoneHP(int which) {return mZoneHP[which];}		//hit points for a particular part of this ship
protected:
	int mZoneHP[3];		//hitpoints for our left, center, right
};

#endif /* defined(__Millennium_Falcon__VaderTIE__) */
