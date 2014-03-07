//
//  StupidPlaceholderShip.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky II on 3/7/14.
//
//

#include "StupidPlaceholderShip.h"
#include "Util.h"

using namespace osg;


StupidPlaceholderShip::StupidPlaceholderShip()
{
	mCenter = Vec3(-200, 50, -400);		//feet!
	
	//load a ship model.  we can also pre-transform the model into our coordinate system
	MatrixTransform* n = Util::loadModel("data/models/tieF3DS/TIEF.3DS", 1.0, -90);
	
	mPat->addChild(n);
	
}


bool StupidPlaceholderShip::update(float dt)
{
	bool up = Spacecraft::update(dt);
	float radius = 200;
	float dTheta = -1.0;
	float theta = mAge * dTheta;
	Vec3 pos = mCenter + Vec3(cosf(theta)*radius, 0, sinf(theta)*radius);
	Vec3 forward = Vec3(-sinf(theta), 0, cosf(theta)) * dTheta;
	forward.normalize();
	
	setPos(pos);
	
	setForward(forward);

	return up;
}


