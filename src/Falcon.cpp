//
//  Falcon.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky II on 3/7/14.
//
//
#ifdef _WIN32
#include <windows.h>
#include <gl/glut.h>
#endif

#include "Falcon.h"
#include "Util.h"
#include "Bullet.h"
#include "FalconApp.h"
#include "ROM.h"
#include "KSoundManager.h"
<<<<<<< HEAD
#include "ScreenImage.h"
#include "Hyperspace.h"
=======
#include <stdarg.h>
>>>>>>> 483d5e78385500af3251ce47e5f0ededdc022066

using namespace osg;

Falcon::Falcon()
{
	
	//load a ship model.  we can also pre-transform the model into our coordinate system
	float scaleFactor = 0.56 * 8.4;			//found through trial, error, and online MF specs
	MatrixTransform* n = Util::loadModel("data/models/falcon3DS/milfalcon.3ds", scaleFactor, -90, 180, 0, Vec3(0, -8, 0));
	
	mPat->addChild(n);
	mFireTimer = 0;
	mFireRate = 6;
	mAimedPart = new PositionAttitudeTransform();

	//make some rings with which to aim
	int numRings = 3;
	for(int i = 0; i < numRings; i++)
	{
		ScreenImage* reticle1 = new ScreenImage();
		reticle1->setImage(Util::findDataFile("data/textures/reticleSingle.png"));
		reticle1->setPos(Vec3(0, 0, -50 * (1+i)));
		reticle1->setHeight(2);
		reticle1->transform->getOrCreateStateSet()->setMode(GL_LIGHTING, GL_FALSE);
		mAimedPart->addChild(reticle1->transform);
	}
	
	mPat->addChild(mAimedPart);
	mAimedPart->setPosition(Vec3(0, 4, 0));
	
	mHyperspace = new Hyperspace();
	mPat->addChild(mHyperspace->getRoot());
	
}

bool Falcon::update(float dt)
{
	mFireTimer -= dt;
	Matrix wand = FalconApp::instance().getWandMatrix();
	mAimedPart->setAttitude(wand.getRotate());
	mHyperspace->update(dt);
	return true;

}



void Falcon::fire()
{
//	printf("Fire!  (timer:  %.3f\n", mFireTimer);
	if(mFireTimer > 0)
	{
		return;		//can't shoot yet
	}
	
	mFireTimer = 1.0 / mFireRate;
	Bullet* b = new Bullet();
	b->mShooter = this;
	//align the new bullet more or less with the wand, since that's our turret
	Matrix wand = FalconApp::instance().getWandMatrix();

	mAimTarget = Vec3(wand.ptr()[12], wand.ptr()[13], wand.ptr()[14]) +
		Vec3(wand.ptr()[8], wand.ptr()[9], wand.ptr()[10]) * -100;
	Vec3 shootFrom = getTransform() * Vec3(0, 4, 0);
	Vec3 fireDir = mAimTarget - shootFrom;			//actual direction our shot will travel
	fireDir.normalize();
	b->setTransform(wand);
	b->setPos(shootFrom);
	b->setForward(fireDir);
	b->mIsEnemy = false;
	float speed = ROM::FALCON_LASER_SPEED;
	//printf("soundvolume: %f\n", ROM::FALCON_FIRE_VOLUME);
	KSoundManager::instance()->playSound(std::string("data/sounds/") + ROM::FALCON_FIRE_SOUND, ROM::FALCON_FIRE_VOLUME, 0);
//	printf("new bullet at %.2f, %.2f, %.2f\n", wand.ptr()[8], wand.ptr()[9], wand.ptr()[10]);
	b->mVel = b->getForward() * speed;
//	FalconApp::instance().getBullets().push_back(b);
	FalconApp::instance().addThis(b);
}

void Falcon::aimAt(Vec3 target)
{
	mAimTarget = target;
	
}
