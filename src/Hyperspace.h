//
//  Hyperspace.h
//  Millennium Falcon
//
//  Created by Ken Kopecky on 4/30/14.
//
//

#ifndef __Millennium_Falcon__Hyperspace__
#define __Millennium_Falcon__Hyperspace__

#include <iostream>
#include "GameObject.h"

//hyperspace effect is composed of lots of stars that suddenly lengthen and shoot away

class Hyperspace : public GameObject
{
public:
	Hyperspace();
	bool update(float dt);
	void go();
	bool done();		//have we finished the jump?

protected:
	float mHSTime;				//used to time hyperspace things
	float mPhase;			//what phase of the jump are we in?

};

#endif /* defined(__Millennium_Falcon__Hyperspace__) */
