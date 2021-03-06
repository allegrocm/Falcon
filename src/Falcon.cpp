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

#include <stdarg.h>

#include "Falcon.h"
#include "Util.h"
#include "Bullet.h"
#include "FalconApp.h"
#include "ROM.h"
#include "KSoundManager.h"
#include "ScreenImage.h"
#include "Hyperspace.h"
#include "ParticleFX.h"
#include "GameController.h"
#include "EventAudio.h"
#include "EnemyController.h"
#include "ShaderManager.h"

using namespace osg;

Falcon::Falcon()
{
	setGlows(false);
	setName("Millennium Falcon");
	//load a ship model.  we can also pre-transform the model into our coordinate system
	float scaleFactor = 0.56 * 8.4;			//found through trial, error, and online MF specs
	MatrixTransform* n = Util::loadModel("data/models/falcon3DS/milfalcon.3ds", scaleFactor, -90, 180, 0, Vec3(0, -10, 0));
	Util::cullSmallGeodes(n, 1.0);
	mTurretVisibleTime = 0;
//	Util::printNodeHierarchy(n);
	mPat->addChild(n);
	mAimedPart = new PositionAttitudeTransform();
	mUpperAimedPart = new PositionAttitudeTransform();
//	osg::Node* turretNode = Util::findNodeWithName(n, "mf GunBod");
	osg::Node* turretNode = Util::loadModel("data/models/falcon3DS/laserbeamerthing.3ds", scaleFactor, -90, 180, 0, Vec3(0, -10, 0));

	//pull the chunks of turret out of the main model and put them in the aiming group
	std::string toRemove[] = {"mf GunBody", "mf GunBod1", "mf GunTube"};
	for(int i = 0; i < sizeof(toRemove) / sizeof(std::string); i++)
	{
		Node* turretPiece = Util::findNodeWithName(n, toRemove[i]);
		if(turretPiece)
		{
//			mAimedPart->addChild(turretPiece);
			turretPiece->getParent(0)->removeChild(turretPiece);
		}
		
	}
	
//	Util::printNodeHierarchy(turretNode);
	mUpperAimedPart->addChild(turretNode);
	//make some rings with which to aim
	int numRings = 3;
	for(int i = 0; i < numRings; i++)
	{
		ScreenImage* reticle1 = new ScreenImage();

		//don't know why this works
		//putting anything in the transparent bin seems to make it vanish
		//maybe something to do with the bloom effect
		reticle1->transform->getOrCreateStateSet()->setRenderingHint(StateSet::OPAQUE_BIN);
		reticle1->setImage(Util::findDataFile("data/textures/reticleSingle.png"));
		reticle1->setPos(Vec3(0, 0, -100 * (1+i)));
		reticle1->setHeight(2);
		reticle1->transform->getOrCreateStateSet()->setMode(GL_LIGHTING, GL_FALSE);
		mUpperAimedPart->addChild(reticle1->transform);
	}
	
	mPat->addChild(mAimedPart);
	mAimedPart->setPivotPoint(Vec3(0, 2, 0));
	mAimedPart->setPosition(Vec3(0, 5, 0));

	mPat->addChild(mUpperAimedPart);
	mUpperAimedPart->setPivotPoint(Vec3(0, -3, 0));
	mUpperAimedPart->setPosition(Vec3(0, 3, 0));


	
	mHyperspace = new Hyperspace();
	FalconApp::instance().getModelGroup()->addChild(mHyperspace->getRoot());
	
	mGun.mAutoburst = true;
	
	mGun.mFireSound = ROM::FALCON_FIRE_SOUND;
	mGun.mFireVolume = ROM::FALCON_FIRE_VOLUME;
	
	mAutoTurret = mGun;		//auto turret has same properties as the main one
	mAutoTurret.mShotDelay = ROM::LOWER_TURRET_FIRE_DELAY;
	mAutoTurret.mBurstDelay = ROM::LOWER_TURRET_BURST_DELAY;
	
	mGlowUniform = new Uniform("glowColor", Vec4(1.0, 0.0, 0.0, 0.0));
	mPat->getOrCreateStateSet()->addUniform(mGlowUniform);
	mGlowTime = 0;
	
	mKlaxon = NULL;//
//	KSoundManager::instance()->stopSound(mKlaxon);
//	KSoundManager::instance()
//	ShaderManager::instance().applyShaderToNode("data/shaders/PerPixelLighting", n);
}

bool Falcon::update(float dt)
{
	mAge += dt;
	Matrix wand = FalconApp::instance().getWandMatrix();
	mAimedPart->setAttitude(wand.getRotate());
	mUpperAimedPart->setAttitude(wand.getRotate());

	mHyperspace->update(dt);
	mGun.update(dt);
	if(FalconApp::instance().zeroPlayerMode())
	{
		if(GameController::instance().getMode() == GameController::MAIN_GAME)
			shoot();		//automatically shoot in zero player mode
	}
	if(mGun.canAutofire()) shoot();		//if we've started a burst shot, finish it
	
	if(FalconApp::instance().lowerTurretAllowed())
		updateAutoTurret(dt);
	

	::Stats& stats = GameController::instance().getStats();
	float percent = (float)stats.health / (float)stats.maxHealth;
	float glowAmount = 0.5 - 0.3 * cosf(mGlowTime * 5.0);
	if(percent > .25)
	{
		mGlowTime = 0;
		glowAmount = 0;
		
	}
	else
		mGlowTime += dt;

	
	
	if(GameController::instance().getMode() == 0 || (percent > 0 && mHyperspace->done() == false))
	{
		//kill our klaxon if we've started over or begun to jump safely
		if(mKlaxon)
		{
//			printf("klax on!\n");
			KSoundManager::instance()->setSoundLoop(mKlaxon, false);
			if(KSoundManager::instance()->soundIsPlaying(mKlaxon) == false)
			{
//				printf("Klaxon is done!\n");
				KSoundManager::instance()->letSoundDie(mKlaxon);
				mKlaxon = NULL;
			}
			
		}
		glowAmount = 0;
	}
	mGlowUniform->set(Vec4(1.0, 0.05, 0.05, glowAmount * 0.25));
	return true;

}

void Falcon::updateAutoTurret(float dt)
{

	mAutoTurret.update(dt);

	

	Spacecraft* ship = FalconApp::getEnemyPlayerShip();
	if(!ship)
	{
		mTurretVisibleTime	= 0;
		return;
	}
	Vec3 turretPos(0, -25, 0);

	
	Vec3 pos = ship->getPos();
	Vec3 vel = ship->getVel();
	
	Vec3 hitPos;
	bool canSee = !checkRaycast(turretPos, pos-turretPos, hitPos);		//is the falcon between the turret and the ship?
//	if(canSee) printf("I can see you!\n"); else printf("Can't see the guy!\n");
	float speed = ROM::FALCON_LASER_SPEED * 0.5;
	
	//how long will it take the shot to arrive?
	float travelTime = (turretPos-pos).length() / speed;



	if(!canSee)
	{
		mTurretVisibleTime	= 0;		//reset the time if we can't see the ship
		return;		//lower turret is only active if Vader's TIE is below the ship
	}


	mTurretVisibleTime += dt;
	

	//if the turret's not ready, don't do anything further
	if(!mAutoTurret.canFire()) return;


	//wait a little bit before firing, as a courtesy.
	if(mTurretVisibleTime < ROM::LOWER_TURRET_GRACE_PERIOD) return;

	if(!mAutoTurret.fire()) return;



	//gun starts aiming in front and then catches down
	float deadliness = (mTurretVisibleTime - ROM::LOWER_TURRET_GRACE_PERIOD) / ROM::LOWER_TURRET_CATCHUP_TIME;
	if(deadliness > 1) deadliness = 1;
	float leadAmount = 2.0 - deadliness;

//	printf("fire with lead:  %.2f (viz: %.2f, grace:  %.2f)\n", leadAmount, mTurretVisibleTime, ROM::LOWER_TURRET_GRACE_PERIOD);
	//lead by that amount
	pos += vel * travelTime * leadAmount;


	Bullet* b = new Bullet();
#ifdef RNG_LOGGING
	printf("Falcon autoturret created bullet %i\n", b->mObjectID);
#endif

	b->mShooter = this;

	//each of the four barrels has a different position
	Vec3 fireDir = pos - turretPos;
	fireDir.normalize();
//	b->setTransform(wand);
	b->setPos(turretPos);
	b->setForward(Util::vectorInCone(fireDir, 4 * (1.0 - deadliness), 1.0 * (1.0-deadliness)));
	b->mIsEnemy = false;
//	printf("AT:  %.2f, %i, lead:  %.2f\n", mAutoTurret.mBurstTimer, mAutoTurret.mBurstCounter, leadAmount);
	//printf("soundvolume: %f\n", ROM::FALCON_FIRE_VOLUME);
	KSoundManager::instance()->play3DSound(std::string("data/sounds/") + mGun.mFireSound, 
		mGun.mFireVolume, b->getPos().x(), FalconApp::instance().getHeadMatrix().ptr()[13], b->getPos().z(),false, 50);
//	printf("new bullet at %.2f, %.2f, %.2f\n", wand.ptr()[8], wand.ptr()[9], wand.ptr()[10]);
	b->mVel = b->getForward() * speed;
	b->setColor(Vec4(1.0, .2, .1, 1.0));
//	FalconApp::instance().getBullets().push_back(b);
	FalconApp::instance().addThis(b);
}

bool Falcon::shoot()
{
	if(!mGun.fire()) return false;

	static int whichBarrel = 0;
	whichBarrel = (whichBarrel+1)%4;
	Bullet* b = new Bullet();
	#ifdef RNG_LOGGING
	printf("Falcon main turret created bullet %i\n", b->mObjectID);
#endif
	b->mShooter = this;
	//align the new bullet more or less with the wand, since that's our turret
	Matrix wand = FalconApp::instance().getWandMatrix();
	Matrix wandRotate = Matrix::rotate(wand.getRotate());
	mAimTarget = Vec3(wand.ptr()[12], wand.ptr()[13], wand.ptr()[14]) +
		Vec3(wand.ptr()[8], wand.ptr()[9], wand.ptr()[10]) * -500;

	//each of the four barrels has a different position
	Vec3 barrelPos = Vec3(1.0 * (-1 + 2*(whichBarrel%2)), 1.0 * (whichBarrel/2), 0) * wandRotate;
	Vec3 shootFrom = getTransform() * Vec3(0, 6, -2) * wandRotate;
	Vec3 fireDir = mAimTarget - shootFrom;			//actual direction our shot will travel
	
	fireDir.normalize();
	b->setTransform(wand);
	b->setPos(shootFrom + barrelPos);
	b->setForward(fireDir);
	b->mIsEnemy = false;

	float speed = ROM::FALCON_LASER_SPEED;
	//printf("soundvolume: %f\n", ROM::FALCON_FIRE_VOLUME);
	KSoundManager::instance()->play3DSound(std::string("data/sounds/") + mGun.mFireSound, 
		mGun.mFireVolume, b->getPos().x(), FalconApp::instance().getHeadMatrix().ptr()[13], b->getPos().z(),false, 50);
//	printf("new bullet at %.2f, %.2f, %.2f\n", wand.ptr()[8], wand.ptr()[9], wand.ptr()[10]);
	b->mVel = b->getForward() * speed;
	b->setColor(Vec4(1.0, .5, .3, 1.0));
//	FalconApp::instance().getBullets().push_back(b);
	FalconApp::instance().addThis(b);
	
	return true;
}

void Falcon::aimAt(Vec3 target)
{
	mAimTarget = target;
	
}

void Falcon::jump()
{

	//play a voice clip before we jump
	EventAudio::instance().eventHappened("preJump");
	mHyperspace->go();
}


void Falcon::wasHit(Bullet* b, osg::Vec3 hitPos)
{
	//we can't get damaged till the game has started
	if(GameController::instance().getMode() != GameController::MAIN_GAME) return;
	//reduce hitpoints
	::Stats& stats = GameController::instance().getStats();
	EventAudio::instance().eventHappened("falconHit");
	float oldPercent = 1.0 * (stats.health) / stats.maxHealth;
	stats.health--;
	printf("got hit at %.3f, health left:  %i\n", mAge, stats.health);

	float newPercent = 1.0 * stats.health / stats.maxHealth;
	
	if(oldPercent > 0.5 && newPercent <= 0.5)
	{
		EventAudio::instance().eventHappened("halfHealth");		//maybe play a sound when health gets low
	}

	if(oldPercent > 0.25 && newPercent <= 0.25)
	{
		EventAudio::instance().eventHappened("quarterHealth");		//maybe play a sound when health gets low
		EventAudio::instance().eventHappened("QuarterHealthVaderListener");		//maybe play a sound when health gets low
		//start playing our klaxon.  we'll kill it when it's time
		if(mKlaxon) KSoundManager::instance()->letSoundDie(mKlaxon);
		mKlaxon = KSoundManager::instance()->playSound("data/sounds/Buzzer3.wav", FalconApp::instance().tieNode1() ? 0:0.5, 0, true);
	}

	if(oldPercent > 0 && newPercent <= 0)
	{
		printf("No health left!--starting hyperspace jump!\n");
		EventAudio::instance().eventHappened("noHealth");
		mHyperspace->go();		//gotta go now!
		GameController::instance().falconLost();
	}

//	printf("Health:  %i\n", 	GameController::instance().getStats().health);
//	FalconApp::instance().getFX()->makeExplosion(b->getPos(), 1.5);
}

bool Falcon::isJumping()
{
	return !(mHyperspace->done());
}



