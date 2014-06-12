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
#include <osgUtil/IntersectVisitor>

using namespace osg;


StupidPlaceholderShip::StupidPlaceholderShip()
{
	setGlows(false);
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


	//...maybe not.  we'll leave the engine glow out for now
	//now add the engine glow as another sphere
	mEngineGlow = new Sphere(Vec3(0, 0, -15), 3.0);
	sd = new ShapeDrawable(mEngineGlow);
	sd->setColor(Vec4(.6, .7, 1.0, 1.0) * 0.25);
	g = new Geode;
	g->addDrawable(sd);
	g->setNodeMask(1 << GLOW_ONLY_LAYER);			//only glow.  not used for anything else
//	mPat->addChild(g);
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
		mEngineSound = KSoundManager::instance()->play3DSound(std::string("data/sounds/") + engineSound, 0.75, 1000, 1000, 1000, true, 80);
	}
	
	Vec3 pos = Vec3(Util::random(-200.0, 200), Util::random(0.0, 200.0), -500);
	setPos(pos);
	mSpeed = 100;
	setVel(Vec3(0, 0, -mSpeed));
	mMovingAway = true;
	mTurning = false;

	mTargetPosition = Vec3();
	mCurrentTurnTime = 0;
	mTimeToTurn = 0;
	mTimeTillShoot = Util::random(0.0, 2.0);
	mGun = ROM::TIE_FIGHTER_GUN;
}

void StupidPlaceholderShip::playerControl(float dt)
{
	EnemyControlInput input = mPlayer->getInput();
	if(input.button1)		//autopilot
	{
		AIControl(dt);
		return;
	}
	//printf("player control!  %.2f, %.2f, %.2f, (%i)\n", input.xAxis, input.yAxis, input.thrustAxis, input.trigger);
	float yawSpeed = 90.0 * input.xAxis * dt;
	float pitchSpeed = 60.0 * input.yAxis * dt;
	Vec3 vel = getVel();
	Matrix current = getTransform();
	Matrix pitchRot;
	pitchRot.setRotate(osg::Quat(pitchSpeed / -57.3, Util::xAxis(current)));
	Matrix yawRot;
	yawRot.setRotate(osg::Quat(-yawSpeed / 57.3, Util::zAxis(current)));
//	Vec3 up = getUp();
//	setTransform(pitchRot * yawRot * current);
//	vel = pitchRot * yawRot * vel;
	Vec3 pos = getPos();
	setTransform(current * yawRot);
	setPos(pos);
	vel = getForward() * pitchRot;
	vel.normalize();
	vel *= (input.thrustAxis + 1.0) * 0.5 * mSpeed;
//	Util::printMatrix(yawRot);
//	printf("Forward:  %f, %f, %f\n", vel.x(), vel.y(), vel.z());
	setVel(vel);
	if(input.trigger)
		shoot();
}


void StupidPlaceholderShip::AIControl(float dt)
{
	Vec3 desiredDirection = mTargetPosition - getPos();
	desiredDirection.normalize();
	Vec3 adjustedVelocity = getVel() + (desiredDirection * mSpeed - getVel()) * dt; //dt when coded was .01
	adjustedVelocity.normalize();

	Vec3 error = desiredDirection * mSpeed - getVel();
//	std::cout << "Velocity change: " << error.x() << ", " << error.y() << ", " << error.z() << "\n";
	if(abs(error.x()) < 1 && abs(error.y()) < 1 && abs(error.z()) < 1 && Util::random(0, 100) < 50) {
//		shoot();
	}
	setVel(adjustedVelocity*mSpeed);

	FalconApp& app = FalconApp::instance();
	Falcon* falcon = app.getFalcon();
	Vec3 distanceToFalcon = falcon->getPos() - getPos();
	
	if(mMovingAway) {
		if(distanceToFalcon.length() < 600) {
			//nothing special
		} else {
//			std::cout << "Turning around to attack!\n";
			mMovingAway = false;
			mTurning = true;
			mTimeTillShoot = Util::random(0.0, 2.0);		//attack after a random amount of time
			mTargetPosition = falcon->getPos();
			mCurrentTurnTime = 0;
			mTimeToTurn = 3;
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
			float theta = Util::random(-0.0, 6.28);
			float phi = Util::random(-1, 2.5);
			Vec3 target = Vec3(500*cosf(theta) * cosf(phi), 500*sinf(phi), -500*sinf(theta) * cosf(phi));
			mTargetPosition = target;
			mCurrentTurnTime = 0;
			mTimeToTurn = 3;
			
		}
	}

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
	if(mPlayer)
		playerControl(dt);
	else
		AIControl(dt);
	setPos(getVel() * dt + getPos());

	setForward(mVel);
	bool up = Spacecraft::update(dt);
	
	if(isHittingFalcon())
		explode();
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
	mHP = 0;
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
	if(mPlayer)
		mPlayer->shipDied();
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
	SkySound* ss =
		KSoundManager::instance()->play3DSound(std::string("data/sounds/") + mGun.mFireSound, mGun.mFireVolume,
			getPos().x(), getPos().y(), getPos().z(), false, 200);
	
	//no doppler effect on laser sounds.  it's weird.
	KSoundManager::instance()->setSoundDopplerLevel(ss, 0);
	return true;
	
}

bool StupidPlaceholderShip::isHittingFalcon()
{
	osgUtil::IntersectVisitor iv;
	
	Vec3 dir = getForward();
	Vec3 pos = getPos();
	float length = 3;
	float travelLength = 0;
	
	//if our length is shorter than our velocity * dt, lengthen it so we don't pass through something
	if(travelLength > length)
		length = travelLength;
	//make a line segment representing the laser beam
	ref_ptr<LineSegment> seg = new LineSegment(pos - dir * length * 0.5, pos + dir * length * 0.5);
	iv.setTraversalMask(1 << COLLISION_LAYER);		//DON'T check collisions with other lazer beams
	iv.addLineSegment(seg.get());
//	printf("Seg:  %.2f, %.2f, %.2f\n", pos.x(), pos.y(), pos.z());
	//dunno if this is the best way to do it or not, but we're gonna just check each ship individually
	std::vector<Spacecraft*> ships;
	Falcon* f = FalconApp::instance().getFalcon();

	ships.push_back((Spacecraft*)f);


	
	bool hitSometing = false;		//stop after any collision
	Vec3 hitPos;
	for(size_t i = 0; i < ships.size() && !hitSometing; i++)
	{
//		iv.reset();
		ships[i]->getRoot()->accept(iv);
		osgUtil::IntersectVisitor::HitList& hitList = iv.getHitList(seg.get());
		if(hitList.size())		//if there's any size in the hitlist, we HIT something!
		{
		
			return true;
		}
	}
	
	return false;
}



