//
//  TIERadar.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky II on 6/12/14.
//
//

#include "TIEComputer.h"
#include "PrerenderCamera.h"
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
#include "GameController.h"


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
	mAITextFlashTime = 0;
	
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
	
	mAIText = new osgText::Text();

	mAIText->setColor(ROM::VADER_DISPLAY_COLOR);

	mAIText->setMaximumWidth(TIEScreenAspect);
	mAIText->setAlignment(osgText::TextBase::CENTER_CENTER);
	mAIText->setText("Hi Ken");
	mAIText->setCharacterSize(0.05);
	mAIText->setPosition(Vec3(0, 0.43, 0));
	std::string where = osgDB::findDataFile("../Models/tarzeau_ocr_a.ttf");
//	std::string where = "../Models/arial.ttf";
	mAIText->setFont(where.c_str());

	
	Geode* textGeode = new Geode();
	textGeode->addDrawable(mAIText);
	mCamera->addChild(textGeode);
	
	for(int i = 0; i < 2; i++)
	{
		mFalconImage[i] = new ScreenImage();
//		mFalconImage[i]->transform->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		mCamera->addChild(mFalconImage[i]->transform);
		mFalconImage[i]->setImage("data/textures/falconOutlineInvertedGlowy.png");
		mFalconImage[i]->setHeight(0.125);

		mFalconImage[i]->transform->getOrCreateStateSet()->setAttribute(new BlendFunc(GL_SRC_ALPHA, GL_ONE));
		mFalconImage[i]->setColor(ROM::VADER_DISPLAY_COLOR);
		mFalconImage[i]->setPos(Vec3(10, 10, 10));	//start with these off-screen
		
		mArrowImage[i] = new ScreenImage();
		mCamera->addChild(mArrowImage[i]->transform);
		mArrowImage[i]->setImage("data/textures/TIERadarArrow2.png");
		mArrowImage[i]->setHeight(.2);
		mArrowImage[i]->setAspect(0.5);
		mArrowImage[i]->setColor(ROM::VADER_DISPLAY_COLOR);


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
	updateStatusText(dt);
	if(!playerShip)
	{
		//we'll respawn soon...right?
		
		return true;
	}
	updateRadar(dt);
	updateDamageDisplay();
	return true;
}

void TIEComputer::updateRadar(float dt)
{
	Spacecraft* playerShip = getShip();

	Matrix falconMat = Matrix();		//the falcon's always in the middle of the world
	Matrix shipMat = playerShip->getTransform();
	shipMat.invert(shipMat);
	
	
	//find where the Falcon is relative to the TIE fighter
	
	float yaw = atan2f(shipMat.ptr()[12], -shipMat.ptr()[14]) * 57.3;
	float pitch = atan2f(shipMat.ptr()[13], -shipMat.ptr()[14]) * 57.3;
//	printf("yaw:  %.2f   pitch:  %.2f\n", yaw, pitch);
	float dist = playerShip->getPos().length();		//how far away are we?
	
	//move our falcon images alternately
	mFlashTime -= dt;
	float flashPeriod = 1.0 / ROM::TIE_RADAR_UPDATE_RATE;
	float radarFOV = ROM::TIE_RADAR_FOV;
	float malt = 0.3 / radarFOV;		//use the relative directions to position the Falcon icon on the radar

	//fade the two falcons in and out alternately to give a CRT effect
	float timeSinceSwitch = (flashPeriod - mFlashTime);
	float fadeInTime = 0.3 * flashPeriod;
	float fadeIn = timeSinceSwitch / fadeInTime;		//0->1 and beyond
	
	float fadeOutTime = 0.8 * flashPeriod;
	float fadeOut = 1.0 - timeSinceSwitch / fadeOutTime;									//1->0 and less
	bool offScreen = (sqrtf(yaw * yaw + pitch * pitch) > radarFOV);
	if(offScreen)	yaw = 100;		//keep off-screen if not near the center
	if(fadeIn > 1) fadeIn = 1;

	if(fadeOut < 0) fadeOut = 0;
	Vec4 colorFadeOut(ROM::VADER_DISPLAY_COLOR.x(), ROM::VADER_DISPLAY_COLOR.y(), ROM::VADER_DISPLAY_COLOR.z(), fadeOut);
	Vec4 colorFadeIn(ROM::VADER_DISPLAY_COLOR.x(), ROM::VADER_DISPLAY_COLOR.y(), ROM::VADER_DISPLAY_COLOR.z(), fadeIn);

	int currentImage = mCurrentFalconImage%2;
	int otherImage = (mCurrentFalconImage+1)%2;

	mFalconImage[currentImage]->setColor(colorFadeOut);
	mFalconImage[otherImage]->setColor(colorFadeIn);
	if(offScreen)
	{
		mArrowImage[currentImage]->setColor(colorFadeOut);
		mArrowImage[otherImage]->setColor(colorFadeIn);
	}


	if(mFlashTime < 0)		//time to start a new fade in cycle
	{

		//the radar isn't quite centered
		float dx = -0.023;
		float dy = 0;

		mFalconImage[currentImage]->setPos(Vec3(yaw*malt + dx, pitch * malt + dy, 0));
		mFalconImage[currentImage]->setColor(Vec4());	//start the image black when it fades in
		mArrowImage[currentImage]->setColor(Vec4());	//start the image black when it fades in

		mFlashTime = flashPeriod;

		//flash a locating arrow if we can't see the falcon
		if(offScreen && dist > 50)
		{
			float arrowRadius = 0.2;		//for positioning the little arrow
			float arrowAngle = atan2f(shipMat.ptr()[13], shipMat.ptr()[12]);
			
			//quantize the arrowangle
			arrowAngle *= 18.0 / (3.14159 * 2.0);
//			printf("aa:  %f ", arrowAngle);
			arrowAngle = (int)arrowAngle;
//			printf(" then %.2f ", arrowAngle);
			arrowAngle *= 3.14159 * 2.0 / 18.0;
//			printf("finally %.2f\n", arrowAngle);
			mArrowImage[currentImage]->setPos(Vec3(arrowRadius * cosf(arrowAngle) + dx, arrowRadius * sinf(arrowAngle) + dy, 0));
			mArrowImage[currentImage]->setAngle(arrowAngle * 57.3 - 90);
		}
		else
			mArrowImage[currentImage]->setPos(Vec3(100, 0, 0));
		mCurrentFalconImage++;
	}
	
}


void TIEComputer::updateStatusText(float dt)
{
	//pulse the AI or other warning text
	std::string statusText = "";
	Spacecraft* playerShip = getShip();
	EnemyPlayer* player = EnemyController::instance().getPlayer();
	if(!playerShip)
	{
		if(GameController::instance().getMode() != GameController::MAIN_GAME)
		{
			statusText = "Waiting for the game to start...";
		}
		else
		if(player && EnemyController::instance().getShips().size())
			statusText = Util::stringWithFormat("Destroyed!  Waiting to respawn in %.2f...", player->deadTimer());
		else
			statusText = "You have failed to kill the Millennium Falcon!";
	}
	else if(playerShip->getPlayer()->AIControl)
		statusText = "AUTOPILOT";
	else if(playerShip->getHP() < ROM::VADER_TIE_HP / 3)
		statusText = "WARNING:  HULL CRITICAL";
	mAIText->setText(statusText);
	mAITextFlashTime += dt;
	float textFlashRate = 1.0;
	float fadeInTime = 0.2;
	float steadyTime = 0.3;
	float fadeOutTime = 0.4;
	if(mAITextFlashTime > textFlashRate) mAITextFlashTime -= textFlashRate;
	float tFlash = mAITextFlashTime / textFlashRate;
	
	float brite = 1.0;
	if(tFlash < fadeInTime)
		brite = tFlash / fadeInTime;
	else if(tFlash > fadeInTime + steadyTime)
		brite = 1.0 - ((tFlash-fadeInTime-steadyTime) / fadeOutTime);
	if(brite < 0) brite = 0;
	mAIText->setColor(Vec4(ROM::VADER_DISPLAY_COLOR.x(), ROM::VADER_DISPLAY_COLOR.y(), ROM::VADER_DISPLAY_COLOR.z(), brite));


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

