//
//  TIERadar.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky II on 6/12/14.
//
//

#include "TIEComputer.h"
#include "PreRenderCamera.h"
#include "Util.h"
#include "FalconApp.h"
#include <osgDB/FileUtils>
#include <osg/BlendFunc>
#include "ScreenImage.h"
#include <osg/Depth>
#include <osg/AlphaFunc>
#include "Defaults.h"
#include "ROM.h"
#include "EnemyController.h"
#include "Spacecraft.h"
#include "VaderTIE.h"

using namespace osg;
TIEComputer::TIEComputer()
{
	float TIEScreenAspect = 1.3333;
#ifndef USE_VRJ
	//only use the 1.333 aspect with Juggler.  otherwise, computer standardish 16:10
	TIEScreenAspect = 1.6;
#endif
	mCamera = new PrerenderCamera(1024, 1, GL_RGBA);
	mCamera->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	mPat->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
//	mPat->getOrCreateStateSet()->setAttribute(new BlendFunc(GL_SRC_ALPHA, GL_ONE));
	mCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	mCamera->setProjectionMatrix(osg::Matrix::ortho2D(-.5 * TIEScreenAspect,.5 * TIEScreenAspect,-.5,.5));
	mCamera->setClearColor(Vec4(0, .0, .0, 1));
	mPat->getOrCreateStateSet()->setMode(GL_BLEND, true);
//	mPat->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	mCurrentFalconImage = 0;
	mFlashTime = 0;

	//add the background image
	ScreenImage* image = new ScreenImage();
	mCamera->addChild(image->transform);
	image->setImage("data/textures/TIEPuterMask.png");
	image->setHeight(1);
//	image->setColor(Vec4(0.7, 0.7, 1.0, 1));
	image->setPos(Vec3(0, 0, .5));		//put this in front of the other images
	image->transform->getOrCreateStateSet()->setRenderingHint(osg::StateSet::OPAQUE_BIN);
	//everything is masked by the PuterMask, so the radar doesn't go beyond its bounds

	//now add the little radar screen overlay
	image = new ScreenImage();
	mCamera->addChild(image->transform);
	image->setImage("data/textures/TIEPuterRaderOverlay.png");
	image->setHeight(1);
//	image->setColor(Vec4(0.7, 0.7, 1.0, 1));
//	image->setPos(Vec3(0, 0, 1));		//put this in front of the other images
	
	mArrowImage = new ScreenImage();
//	mCamera->addChild(mArrowImage->transform);
	mArrowImage->setImage("data/textures/TIEPuter.png");
	mArrowImage->setHeight(1);
	mArrowImage->setColor(Vec4(1, 1, 1.0, 1));

	for(int i = 0; i < 2; i++)
	{
		mFalconImage[i] = new ScreenImage();
//		mFalconImage[i]->transform->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		mCamera->addChild(mFalconImage[i]->transform);
		mFalconImage[i]->setImage("data/textures/falconOutlineInvertedGlowy.png");
		mFalconImage[i]->setHeight(0.125);

		mFalconImage[i]->transform->getOrCreateStateSet()->setAttribute(new BlendFunc(GL_SRC_ALPHA, GL_ONE));
		mFalconImage[i]->setColor(Vec4(1, .25, .25, 1));
		mFalconImage[i]->setPos(Vec3(10, 10, 10));	//start with these off-screen
	}


	//load three seperate images to show damage to different regions of the ship
	const char* names[] = {"VaderTIESchematicLeft.png", "VaderTIESchematicCenter.png", "VaderTIESchematicRight.png"};
	for(int i = 0; i < 3; i++)
	{
		mDamageImage[i] = new ScreenImage();
		mDamageImage[i]->transform->getOrCreateStateSet()->setRenderingHint(osg::StateSet::OPAQUE_BIN);
		mCamera->addChild(mDamageImage[i]->transform);
		mDamageImage[i]->setImage(std::string("data/textures/") + names[i]);
		mDamageImage[i]->setHeight(0.125);
		mDamageImage[i]->setPos(Vec3(-0.5, 0.3, 0));
		mDamageImage[i]->transform->getOrCreateStateSet()->setAttribute(new BlendFunc(GL_SRC_ALPHA, GL_ONE));
		mDamageImage[i]->setColor(Vec4(0, 1, 0, 1));
	}
	
}

bool TIEComputer::update(float dt)
{
	mAge += dt;
	//find out where our ship is
	Spacecraft* playerShip = getShip();
	if(!playerShip)
	{
		//we'll respawn soon...right?
		
		return true;
	}
	Matrix falconMat = Matrix();		//the falcon's always in the middle of the world
	Matrix shipMat = playerShip->getTransform();
	shipMat.invert(shipMat);
	
	
	//find where the Falcon is relative to the TIE fighter
	
	float yaw = atan2f(shipMat.ptr()[12], -shipMat.ptr()[14]) * 57.3;
	float pitch = atan2f(shipMat.ptr()[13], -shipMat.ptr()[14]) * 57.3;
//	float dist = playerShip->getPos().length();		//how far away are we?
//	printf("MFYaw:  %.2f, %.2f\n", yaw, pitch);
	
	float malt = 0.025;		//use the relative directions to position the Falcon icon on the radar
	
	
	//move our falcon images alternately
	mFlashTime -= dt;
	float flashPeriod = 0.2;
	
	//fade the two falcons in and out alternately to give a CRT effect
	float timeSinceSwitch = (flashPeriod - mFlashTime);
	float fadeIn = timeSinceSwitch * 5;		//0->1 and beyond
	float fadeOut = 1.0 - 3.0 * timeSinceSwitch;									//1->0 and less
	
	if(fabs(yaw * malt) > 0.5 )	yaw = 10;		//keep off-screen if not near the center
	if(fadeIn > 1) fadeIn = 1;

	if(fadeOut < 0) fadeOut = 0;
	mFalconImage[mCurrentFalconImage%2]->setColor(Vec4(1, .25, .25, fadeOut));
	mFalconImage[(mCurrentFalconImage+1)%2]->setColor(Vec4(1, .25, .25, fadeIn));
//	printf("fade:  %.2f, %.2f\n", fadeIn, fadeOut);
	if(mFlashTime < 0)
	{
//		printf("Move %i!\n", mCurrentFalconImage%2);
		mFalconImage[mCurrentFalconImage%2]->setPos(Vec3(yaw*malt, pitch * malt, 0));
		mFalconImage[mCurrentFalconImage%2]->setColor(Vec4());	//start the image black when it fades in
		mCurrentFalconImage++;
		mFlashTime = flashPeriod;
	}
	
	
	updateDamageDisplay();
	return true;
}

void TIEComputer::updateDamageDisplay()
{
	VaderTIE* vt = dynamic_cast<VaderTIE*>(getShip());
	if(!vt)
	{
		return;
	}
	
	int maxZoneHP = ROM::VADER_TIE_HP/3;
	for(int i = 0; i < 3; i++)
	{
		int currentZoneHP = vt->getZoneHP(i);
		float percent = (float)currentZoneHP / (float)maxZoneHP;
		
		Vec4 healthColor(0, 1, 0, 1);

		//start off green, fade to yellow, then red if we're very badly damaged
		float pulseTime = 0;
		if(percent > 0.5)
		{
			healthColor.x() = (1.0 - percent) * 2.0;
		}
		else
		{
			healthColor.x() = 1.0;
			healthColor.y() = 2.0 * percent;
		}
		
		if(percent < 0.25)		//getting bad.  start pulsing or something
		{
			pulseTime = mAge;
			float pulse = 0.75 + 0.25 * cosf(pulseTime * 12.0);
			healthColor.x() *= pulse;
			healthColor.y() *= pulse;
		}
		
		mDamageImage[i]->setColor(healthColor);

	
	}
	
	
}

Spacecraft* TIEComputer::getShip()
{
	EnemyPlayer* player = EnemyController::instance().getPlayer();
	if(!player) return NULL;		//no player?  we can't do anything
	Spacecraft* playerShip = player->getShip();
	if(!playerShip)
	{
		//TODO:  show something indicating if we'll respawn?
		return NULL;
	}

	return playerShip;
}

