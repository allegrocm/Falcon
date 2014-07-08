//
//  StarDestroyer.h
//  Millennium Falcon
//
//  Created by Ken Kopecky II on 7/2/14.
//
//

#ifndef __Millennium_Falcon__StarDestroyer__
#define __Millennium_Falcon__StarDestroyer__

#include "Spacecraft.h"

//the star destroyer can act as a spawn point.
//can't be destroyed I don't think

class StarDestroyer : public Spacecraft
{
public:
	StarDestroyer();
	virtual bool update(float dt);
	void warp();		//move to a randomish position
protected:
};

#endif /* defined(__Millennium_Falcon__StarDestroyer__) */
