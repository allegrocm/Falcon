//
//  StupidPlaceholderShip.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky II on 3/7/14.
//
//

#include "StupidPlaceholderShip.h"
#include "Util.h"
#include "Debris.h"
#include "FalconApp.h"
#include "ParticleFX.h"

using namespace osg;


StupidPlaceholderShip::StupidPlaceholderShip()
{
	mCenter = Vec3(-50, 15, -100);		//feet!
	
	//load a ship model.  we can also pre-transform the model into our coordinate system
	MatrixTransform* n = Util::loadModel("data/models/tieF3DS/TIEF.3DS", 1.0, -90);
	
	Util::printNodeHierarchy(n);
	mPat->addChild(n);
//	n = Util::loadModel("data/models/TieWing.3ds", 10.0, -90,0,0, Vec3(8, 0, 0));
//	mPat->addChild(n);
	setName("Placeholder Ship");
	
}


bool StupidPlaceholderShip::update(float dt)
{
	bool up = Spacecraft::update(dt);
	float radius = 50;
	float dTheta = -1.0;
	dTheta = 0;		//hack for explosion testing
	float theta = mAge * dTheta;
	Vec3 pos = mCenter + Vec3(cosf(theta)*radius, 0, sinf(theta)*radius);
	Vec3 forward = Vec3(-sinf(theta), 0, cosf(theta)) * dTheta;
	mVel = forward;
	
	setPos(pos);
	
	if(forward.length())
	{
		forward.normalize();
		setForward(forward);
	}
	return up;
}

void StupidPlaceholderShip::wasHit(Bullet* b)
{
	explode();
}

void StupidPlaceholderShip::explode()
{
	//make scattered little explosions
	int count = rand()%2+3;
	for(int i = 0; i < count; i++)
	{
		Vec3 pos = Util::randomVector();
		pos.x() *= 5.0;
		pos.y() *= 3.0;
		pos.z() *= 3.0;


		FalconApp::instance().getFX()->makeExplosion(pos * getTransform(), Util::random(.5, 2.0));
	}
	
	//explode with debris
	Debris* h = new Debris("data/models/TieWing.3ds", Vec3(1, 0, 0), 45, 20.0 + 40.0 * rand() / RAND_MAX, 360);
	h->setTransformAndOffset(getTransform(), Vec3(5, 0, 0));
	h->setVel(h->getVel() + mVel);			//add in our own velocity to the debris
	FalconApp::instance().addThis(h);

	//explode with debris
	h = new Debris("data/models/TieWing2.3ds", Vec3(-1, 0, 0), 45, 20.0 + 40.0 * rand() / RAND_MAX, 360);
	h->setTransformAndOffset(getTransform(), Vec3(-4, 0, 0));
	h->setVel(h->getVel() + mVel);			//add in our own velocity to the debris

	FalconApp::instance().addThis(h);
	mDead = true;

}


