//
//  Falcon.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky II on 3/7/14.
//
//

#include "Falcon.h"
#include "Util.h"
#include "Bullet.h"
#include "FalconApp.h"

using namespace osg;

Falcon::Falcon()
{
	
	//load a ship model.  we can also pre-transform the model into our coordinate system
	float scaleFactor = 0.56 * 8.4;			//found through trial, error, and online MF specs
	MatrixTransform* n = Util::loadModel("data/models/falcon3DS/milfalcon.3ds", scaleFactor, -90, 180, 0, Vec3(0, -8, 0));
	
	mPat->addChild(n);
	
}


void Falcon::fire()
{
	Bullet* b = new Bullet();
	b->mShooter = this;
	//align the new bullet more or less with the wand, since that's our turret
	Matrix wand = FalconApp::instance().getWandMatrix();

	mAimTarget = Vec3(wand.ptr()[12], wand.ptr()[13], wand.ptr()[14]) +
		Vec3(wand.ptr()[8], wand.ptr()[9], wand.ptr()[10]) * -100;
	Vec3 shootFrom = getTransform() * Vec3(0, -2, 0);
	Vec3 fireDir = mAimTarget - shootFrom;			//actual direction our shot will travel
	fireDir.normalize();
	b->setTransform(wand);
	b->setPos(shootFrom);
	b->setForward(fireDir);
	b->mIsEnemy = false;
	float speed = 500.0;
//	printf("new bullet at %.2f, %.2f, %.2f\n", wand.ptr()[8], wand.ptr()[9], wand.ptr()[10]);
	b->mVel = b->getForward() * speed;
//	FalconApp::instance().getBullets().push_back(b);
	FalconApp::instance().addThis(b);
}

void Falcon::aimAt(Vec3 target)
{
	mAimTarget = target;
	
}
