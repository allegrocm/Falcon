//
//  StupidPlaceholderShip.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky II on 3/7/14.
//
//
#include "Falcon.h"
#include "Util.h"
#include <osg/LOD>
#include "StupidPlaceholderShip.h"
#include "Debris.h"
#include "FalconApp.h"
#include "ParticleFX.h"
#include "Defaults.h"
#include "ROM.h"
#include "GameController.h"
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osgUtil/Optimizer>
#include "Layers.h"
#include "Bullet.h"

using namespace osg;


StupidPlaceholderShip::StupidPlaceholderShip()
{
	mCenter = Vec3(-50, 15, -100);		//feet!
	mRadius = 50 + rand()%50;
	mOffset = 6.28 * rand() / RAND_MAX;
	//load a ship model.  we can also pre-transform the model into our coordinate system
//	MatrixTransform* red = Util::loadModel("data/models/tief3DS/TIEFReduced.3DS", 1.0, -90);
	MatrixTransform* nbest = Util::loadModel("data/models/tief3DS/TIEF.3DS", 1.0, -90);
	MatrixTransform* n = Util::loadModel("data/models/tief3DS/TIEF_50.3ds", 1.0, -90);
	MatrixTransform* lod = Util::loadModel("data/models/tief3DS/TIEF_10.3ds", 1.0, -90);
	

	//use an LOD to reduce render time
	osg::LOD* l = new LOD();
//	l->addChild(g, 0, 100000);
	//set the LOD for farther away rendering, and the main model for close up
	l->addChild(lod, 150, 100000);
	l->addChild(n, 50, 150);
	l->addChild(nbest, 0, 50);
//	l->addChild(red, 0, 2000);
	Util::cullSmallGeodes(n, 1.0);
	Util::cullSmallGeodes(lod, 2.0);
	osgUtil::Optimizer* o = new osgUtil::Optimizer();
	if(n)
		o->optimize(n);
	if(lod)
		o->optimize(lod);
//	Util::printNodeHierarchy(n);


	//add a sphere so it's easier to hit the far-away version
	Sphere* sphere = new Sphere(Vec3(0, 0, 0), ROM::TIE_HITBOX_SIZE);
	ShapeDrawable* sd = new ShapeDrawable(sphere);
	Geode* g = new Geode;
	g->addDrawable(sd);

	mPat->addChild(g);
	g->setNodeMask(1 << COLLISION_LAYER);			//don't draw the sphere


	//n->addChild(g);
	mPat->addChild(l);
//	n = Util::loadModel("data/models/TieWing.3ds", 10.0, -90,0,0, Vec3(8, 0, 0));
//	mPat->addChild(n);
	setName("Placeholder Ship");
//	for(int i = 0; i < 50; i++)
	{
		Vec4 center = Util::getNodeCG(n, n);
		center /= center.w();
	}
	mHP = 4;
//	printf("COG:  %.2f, %.2f, %.2f\n", center.x(), center.y(), center.z());
	
	std::string engineSound;
	if(Defaults::instance().getValue("placeholderShipSound", engineSound))
	{
		mEngineSound = KSoundManager::instance()->play3DSound(std::string("data/sounds/") + engineSound, 0.5, 1000, 1000, 1000, true, 30);
	}
	
	Vec3 pos = Vec3(Util::random(-200.0, 200), Util::random(0.0, 200.0), -500);
	this->setPos(pos);
	mSpeed = 100;
	this->setVel(Vec3(0, 0, -mSpeed));
	this->mMovingAway = true;
	this->mTurning = false;

	this->targetPosition = Vec3();
	this->mCurrentTurnTime = 0;
	this->mTimeToTurn = 0;
	mTimeTillShoot = Util::random(0.0, 2.0);
	mGun = ROM::TIE_FIGHTER_GUN;
}


bool StupidPlaceholderShip::update(float dt)
{


/*	
	//Fly in circle
	float radius = mRadius;
	float dTheta = -1.0;
//	dTheta = 0;		//hack for explosion testing
	float theta = mAge * dTheta + mOffset;
	Vec3 pos = mCenter + Vec3(cosf(theta)*radius, 0, sinf(theta)*radius);
	Vec3 forward = Vec3(-sinf(theta), 0, cosf(theta)) * dTheta * radius;
	mVel = forward;
	
	setPos(pos);
	
	if(forward.length())
	{
		forward.normalize();
		setForward(forward);
	}
*/
	
	this->setPos(this->getVel() * dt + this->getPos());
	Vec3 desiredDirection = this->targetPosition - this->getPos();
	desiredDirection.normalize();
	Vec3 adjustedVelocity = this->getVel() + (desiredDirection * mSpeed - this->getVel()) * dt; //dt when coded was .01
	adjustedVelocity.normalize();

	Vec3 error = desiredDirection * mSpeed - this->getVel();
//	std::cout << "Velocity change: " << error.x() << ", " << error.y() << ", " << error.z() << "\n";
	if(abs(error.x()) < 1 && abs(error.y()) < 1 && abs(error.z()) < 1 && Util::random(0, 100) < 50) {
		this->shoot();
	}
	this->setVel(adjustedVelocity*mSpeed);

	FalconApp& app = FalconApp::instance();
	Falcon* falcon = app.getFalcon();
	Vec3 distanceToFalcon = falcon->getPos() - this->getPos();
	
	if(mMovingAway) {
		if(distanceToFalcon.length() < 600) {
			//nothing special
		} else {
//			std::cout << "Turning around to attack!\n";
			mMovingAway = false;
			mTurning = true;
			mTimeTillShoot = Util::random(0.0, 2.0);		//attack after a random amount of time
			this->targetPosition = falcon->getPos();
			this->mCurrentTurnTime = 0;
			this->mTimeToTurn = 3;
		}
	} else {
		if(distanceToFalcon.length() > 200)		//flying toward the Falcon
		{
			Vec3 falconDir = distanceToFalcon;
			falconDir.normalize();
			float dot = falconDir * getForward();		//get the dot product of our direction with the millennium falcon
			if(dot > 0.9)		//are we mostly heading towards the falcon?  OPEN FIRE
			{
//				printf("time till shoot:  %.2f\n", mTimeTillShoot);
				mTimeTillShoot -= dt;		//but wait for our shoot timer to run out
				if(mTimeTillShoot <= 0)
				{
					shoot();
				}
			}
	
			
			
		} else {
//			std::cout << "Turning around to retreat!\n";
			mMovingAway = true;
			mTurning = true;
			float theta = Util::random(0.0, 3.14159265);
			float phi = Util::random(0.0, 6.28318531);
			Vec3 target = Vec3(500*cosf(theta), 500*sinf(theta), 500*cosf(phi));
			this->targetPosition = target;
			this->mCurrentTurnTime = 0;
			this->mTimeToTurn = 3;
			
		}
	}
	
	setForward(mVel);
	bool up = Spacecraft::update(dt);
	return up;
}

void StupidPlaceholderShip::wasHit(Bullet* b)
{
	GameController::instance().enemyWasHit(this);
	mHP--;
	if(mHP <= 0)
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
	
	KSoundManager::instance()->play3DSound(std::string("data/sounds/") + ROM::PLACEHOLDER_EXPLOSION_SOUND, 1, getPos().x(), getPos().y(), getPos().z(), false, 100);
//	printf("my mat:\n");
//	Util::printMatrix(getTransform());
	osg::MatrixTransform* debrisRoot = Util::loadModel("data/models/tief3DS/TieFighterDebris.3DS", 1.0, -90);
	std::vector<Debris*> debris = explodeSection(debrisRoot, 1, 3, .5, .5, getTransform(), debrisRoot, 5);
	for(size_t i = 0; i < debris.size(); i++)
	{
		Debris* h = debris[i];
//		printf("Debris mat:\n");
//		Util::printMatrix(h->getTransform());
//		h->setTransformAndOffset(h->getTransform() * getTransform(), Vec3());
		h->setVel(h->getVel() + mVel);
//		h->setFireAmount(0);
//		h->setLife(100);
		FalconApp::instance().addThis(h);
//		Util::printMatrix(Util::getCumulativeTransform(h->getRoot()->getChild(0)->asGroup()->getChild(0), Util::loadModel("data/models/tief3DS/TieFighterDebris.3DS", 1.0, -90)));
	}
	
	if(mEngineSound)
	{
		KSoundManager::instance()->stopSound(mEngineSound);
		KSoundManager::instance()->letSoundDie(mEngineSound);
	}
	/*
	//explode with debris
	Debris* h = new Debris("data/models/tief3DS/TieWing.3ds", Vec3(1, 0, 0), 45, 20.0 + 40.0 * rand() / RAND_MAX, 360);
	h->setTransformAndOffset(getTransform(), Vec3(5, 0, 0));
	h->setVel(h->getVel() + mVel);			//add in our own velocity to the debris
	FalconApp::instance().addThis(h);

	//explode with debris
	h = new Debris("data/models/tief3DS/TieWing2.3ds", Vec3(-1, 0, 0), 45, 20.0 + 40.0 * rand() / RAND_MAX, 360);
	h->setTransformAndOffset(getTransform(), Vec3(-4, 0, 0));
	h->setVel(h->getVel() + mVel);			//add in our own velocity to the debris
	FalconApp::instance().addThis(h);
	
	//explode with debris
	h = new Debris("data/models/tief3DS/TieDebris3.3ds", Vec3(-1, 0, 0), 180, 20.0 + 40.0 * rand() / RAND_MAX, 360, 1);
	h->setTransformAndOffset(getTransform(), Vec3(0, 0, 0));
	h->setVel(h->getVel() + mVel);			//add in our own velocity to the debris
	FalconApp::instance().addThis(h);

	h = new Debris("data/models/tief3DS/TieDebris4.3ds", Vec3(-1, 0, 0), 180, 20.0 + 40.0 * rand() / RAND_MAX, 360, 1);
	h->setTransformAndOffset(getTransform(), Vec3(1, 0, 1));
	h->setVel(h->getVel() + mVel);			//add in our own velocity to the debris
	h->setFireAmount(0.35);		//this little guy doesn't have as much fire behind it
	FalconApp::instance().addThis(h);
*/

	mDead = true;
	GameController::instance().enemyWasKilled(this);

}

void StupidPlaceholderShip::drawDebug()
{


}

bool StupidPlaceholderShip::shoot()
{
	if(!mGun.fire()) return false;
	

	Bullet* b = new Bullet();
	b->mShooter = this;
	//align the new bullet more or less with the wand, since that's our turret
	//each of the four barrels has a different position
	int whichBarrel = mGun.mBurstCounter%2;
	Vec3 barrelPos = Vec3(2.5 * (-1 + 2*(whichBarrel%2)), 0, -4) * getTransform();
	Vec3 fireDir = getForward();
	fireDir.normalize();
//	b->setTransform(wand);
	b->setPos(barrelPos);
	b->setForward(fireDir);
	b->mIsEnemy = true;

	float speed = ROM::FALCON_LASER_SPEED;
//	printf("new bullet at %.2f, %.2f, %.2f\n", wand.ptr()[8], wand.ptr()[9], wand.ptr()[10]);
	b->mVel = b->getForward() * speed;
//	FalconApp::instance().getBullets().push_back(b);
	FalconApp::instance().addThis(b);
//	printf("shoot from %.2f, %.2f, %.2f\n", barrelPos.x(), barrelPos.y(), barrelPos.z());
	KSoundManager::instance()->play3DSound(std::string("data/sounds/") + mGun.mFireSound, mGun.mFireVolume,
		getPos().x(), getPos().y(), getPos().z(), false, 200);
	return true;
	
}


