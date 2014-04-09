//
//  Falcon.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky II on 3/7/14.
//
//
#include <Windows.h>
#include "Falcon.h"
#include "Util.h"

using namespace osg;

Falcon::Falcon()
{
	
	//load a ship model.  we can also pre-transform the model into our coordinate system
	float scaleFactor = 0.56 * 8.4;			//found through trial, error, and online MF specs
	MatrixTransform* n = Util::loadModel("data/models/falcon3DS/milfalcon.3ds", scaleFactor, -90, 180, 0, Vec3(0, -8, 0));
	
	mPat->addChild(n);
	
}
