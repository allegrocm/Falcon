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
#include "EnemyController.h"
using namespace osg;


StupidPlaceholderShip::StupidPlaceholderShip()
{
//	loadTIEModel();
	setGlows(false);
//	n = Util::loadModel("data/models/TieWing.3ds", 10.0, -90,0,0, Vec3(8, 0, 0));
//	mPat->addChild(n);
	setName("Placeholder Ship");
	mHP = 4;
//	printf("COG:  %.2f, %.2f, %.2f\n", center.x(), center.y(), center.z());
	
	float theta = Util::random(0, 6.28);
	float phi = Util::random(0.1, 1.0);
	float r = 900;
	Vec3 pos = Vec3(sinf(theta)*cosf(phi) * r, sinf(phi)*r, cosf(theta)*cosf(phi)*r);
	setPos(pos);
	setForward(Util::vectorInCone(pos * -1.0, 20, 10));
	mSpeed = mTopSpeed = mTargetSpeed = 100;
	mSpeed = 10;		//start with a low speed and spool up quickly
	setVel(getForward() * mSpeed);
	mMovingAway = false;
	mTurning = false;

	mTargetPosition = Vec3();
	mCurrentTurnTime = 0;
	mTimeToTurn = 0;
	mTimeTillShoot = Util::random(0.0, 2.0);
	mGun = ROM::TIE_FIGHTER_GUN;
}

void StupidPlaceholderShip::loadTIEModel()
{
	printf("Load thai model:  regular\n");
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
	
	std::string engineSound;
	if(Defaults::instance().getValue("placeholderShipSound", engineSound))
	{
//		printf("start your kengines\n");
		if(false && isLocalEnemy())
		{
			mEngineSound = KSoundManager::instance()->playSound(std::string("data/sounds/") + engineSound, 0, 0, true);
		}
		else
			mEngineSound = KSoundManager::instance()->play3DSound(std::string("data/sounds/") + engineSound, 0.75, 1000, 1000, 1000, true, 80);
	}


}

void StupidPlaceholderShip::playerControl(float dt)
{
	EnemyControlInput input = mPlayer->getInput();
	if(input.button1 || input.noInput)		//autopilot
	{
		AIControl(dt, input.noInput);		//can't shoot unless there is there-is-no-joystick input
		return;
	}
//	printf("PC\n");
	Matrix current = getTransform();

	Vec3 pos = getPos();
	float towardness = (pos / pos.length()) * Util::zAxis(current);		//are we facing the ship?
	float xzDist = sqrtf(pos.x() *pos.x() + pos.z()*pos.z());
//	printf("XZ:  %.2f, y:  %.2f, toward:  %.2 f\n", xzDist, pos.y(), towardness);

	//if the player flies too far away help them get pointed in the right direction
	if(pos.length() > 1500 || (mPlayer->AIControl && towardness < 0.9 && pos.length() > 1200))
	{
		AIControl(dt, false);
		return;
	}

	//are we hiding under the falcon?  that's not allowed
	//TODO:  maybe just have the falcon shoot at us.  makes more sense
	if(false && (pos.y() < 0 && (xzDist < -pos.y() * 3 || xzDist < 50)))
	{
		mTargetPosition = getPos() + Vec3(0, 50, 0);		//fly upward unless we're super close
		AIControl(dt, false);
		return;
	}
	
	mPlayer->AIControl = false;
	//printf("player control!  %.2f, %.2f, %.2f, (%i)\n", input.xAxis, input.yAxis, input.thrustAxis, input.trigger);
	float yawness = ROM::ENEMY_CONTROL_YAW_MIX;		//temporarily slide between x-to-roll and x-to-yaw
	float rollSpeed = 90 * input.xAxis * (1.0-yawness) * dt;;
	float yawSpeed = 90.0 * input.xAxis * yawness *  dt;
	float pitchSpeed = 60.0 * input.yAxis * dt;
	
	//we want to reduce roll slowly so that overall we stay with the y-axis uppish
	//coming in on the falcon at a weird roll is kind of disconcerting
	

	Vec3 vel = getVel();

	float currentRoll = current.ptr()[1];		//y-value of the x-axis indicates if we're rolled
	float unrollSpeed = ROM::VADER_UNROLL_RATE * currentRoll * dt;

	Matrix pitchRot;
	pitchRot.setRotate(osg::Quat(pitchSpeed / -57.3, Util::xAxis(current)));
	Matrix rollRot;
	rollRot.setRotate(osg::Quat(-rollSpeed / 57.3, Util::zAxis(current)));
	rollRot.setRotate(osg::Quat(-unrollSpeed, Util::zAxis(current)));
	Matrix yawRot;
	yawRot.setRotate(osg::Quat(-yawSpeed / 57.3, Util::yAxis(current)));

	
	setTransform(current * rollRot);
	setPos(pos);
	vel = getForward() * pitchRot * yawRot;
	vel.normalize();
	float thrust = (input.thrustAxis + 1.0) * 0.625;
	mTargetSpeed = thrust * mTopSpeed;
	
	//slow down slightly as we turn!
	float turnamount = sqrtf(input.xAxis * input.xAxis + input.yAxis * input.yAxis);
	mTargetSpeed *= 1.0 - 0.25 * turnamount;
	vel *= thrust * mSpeed;
	
	setVel(vel);
	if(input.trigger)
		shoot();
}


void StupidPlaceholderShip::AIControl(float dt, bool canFire)
{

	if(mPlayer)
		mPlayer->AIControl = true;

	Vec3 desiredDirection = mTargetPosition - getPos();
	desiredDirection.normalize();
//	printf("Dir: %.2f, %.2f, %.2f\n", desiredDirection.x(), desiredDirection.y(), desiredDirection.z());
//	printf("target pos:  %.2f, %.2f, %.2f\n", mTargetPosition.x(), mTargetPosition.y(), mTargetPosition.z());
	Vec3 adjustedVelocity = getVel() + (desiredDirection * mSpeed - getVel()) * dt; //dt when coded was .01
	adjustedVelocity.normalize();

//	Vec3 targetVel = desiredDirection * mSpeed;
	setVel(adjustedVelocity*mSpeed);

	FalconApp& app = FalconApp::instance();
	Falcon* falcon = app.getFalcon();
	Vec3 distanceToFalcon = falcon->getPos() - getPos();
	
	if(mMovingAway)
	{
		if(distanceToFalcon.length() < 600)
		{
			//nothing special
		}
		else
		{
//			std::cout << "Turning around to attack!\n";
			mMovingAway = false;
			mTurning = true;
			mTimeTillShoot = Util::random(0.0, 2.0);		//attack after a random amount of time
			mTargetPosition = falcon->getPos();
			mCurrentTurnTime = 0;
			mTimeToTurn = 3;
		}
	}
	else
	{
		if(distanceToFalcon.length() > ROM::TIE_WAVE_OFF_DISTANCE)		//flying toward the Falcon
		{
			Vec3 falconDir = distanceToFalcon;
			falconDir.normalize();
			float dot = falconDir * getForward();		//get the dot product of our direction with the millennium falcon
			if(dot > 0.9 && canFire)		//are we mostly heading towards the falcon?  OPEN FIRE
			{
//				printf("time till shoot:  %.2f\n", mTimeTillShoot);
				mTimeTillShoot -= dt;		//but wait for our shoot timer to run out
				if(mTimeTillShoot <= 0)
				{
					shoot();
				}
			}
	
		}
		else
		{
			
			ROM::TIE_WAVE_OFF_DISTANCE--;		//HACK:  testing how close TIE fighters can come before they hit
//			std::cout << "Turning around to retreat!  Next time at " << ROM::TIE_WAVE_OFF_DISTANCE << "\n";
			mMovingAway = true;
			mTurning = true;
			mCurrentTurnTime = 0;
			mTimeToTurn = 3;
			pickNewDestination();
		}
	}
	
	//are we on the verge of hitting the falcon?  wave off!
	if(false && willHitFalcon(150))
	{
		printf("too close!\n");
		//slow down and turn!
		mTargetSpeed = mTopSpeed * 0.15;
		mTargetPosition = Vec3(0, 600, 0);	//by default, go up
		if(getPos().y() < 0)		//below the falcon.  go down
		{
			printf("go down!\n");
			mTargetPosition = Vec3(0, -600, 0);
		}
		else
		{
			printf("go up!\n");
		}
		
	}
	else
		mTargetSpeed = mTopSpeed;
	

}


bool StupidPlaceholderShip::update(float dt)
{


	if(mPlayer)
		playerControl(dt);
	else
		AIControl(dt);
	setPos(getVel() * dt + getPos());

	setForward(mVel);
	//now adjust speed
	mSpeed += (mTargetSpeed - mSpeed) * dt * 0.5;
	bool up = Spacecraft::update(dt);

	//send our position and velocity to the SoundManager so we can have stereo and doppler and all that good stuff
	Vec3 pos = getPos();
	Vec3 vel = getVel();
	if(mEngineSound)
	{
		if(isLocalEnemy())
		{
			float speedPortion = mSpeed / mTopSpeed;
			KSoundManager::instance()->setSoundFrequency(mEngineSound, 44000 * speedPortion);
			KSoundManager::instance()->setSoundVolume(mEngineSound, 0.5 * speedPortion);
		}
		else
			KSoundManager::instance()->setSound3DInfo(mEngineSound, pos.x(), pos.y(), pos.z(), vel.x(), vel.y(), vel.z());
	}
	if(!mDead && willHitFalcon(3))		//are we in contact with the falcon?  sucks for us
	{
		printf("Hit falcon at waveoff distance of %.2f.  Moving away:  %i\n", ROM::TIE_WAVE_OFF_DISTANCE, mMovingAway);
		explode();
	}
		
		
	return up;
}

void StupidPlaceholderShip::wasHit(Bullet* b, osg::Vec3 hitPos)
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
	
	mDead = true;
	if(mPlayer)
	{
		printf("Player's ship died!\n");
		mPlayer->shipDied();
	}
	GameController::instance().enemyWasKilled(this);

}

void StupidPlaceholderShip::drawDebug()
{
	glBegin(GL_LINES);
		glColor3f(!mMovingAway, mMovingAway, 0);
		glVertex3fv(getPos().ptr());
		glVertex3fv(mTargetPosition.ptr());
	glEnd();

}

bool StupidPlaceholderShip::shoot()
{
	if(!mGun.fire()) return false;
	

	Bullet* b = new Bullet();
	b->mShooter = this;
	//align the new bullet more or less with the wand, since that's our turret
	//each of the four barrels has a different position
	int whichBarrel = mGun.mBurstCounter%2;
	Vec3 barrelPos = Vec3(1.0 * (-1 + 2*(whichBarrel%2)), -2, -4) * getTransform();
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

	//is this the darth vader tie?  its sounds are local
	if(isLocalEnemy())
	{

		KSoundManager::instance()->playSound(std::string("data/sounds/") + mGun.mFireSound, mGun.mFireVolume,
			(whichBarrel%2)? 0.75 : -0.75);
	
	}
	else
	{
		SkySound* ss =
			KSoundManager::instance()->play3DSound(std::string("data/sounds/") + mGun.mFireSound, mGun.mFireVolume,
				getPos().x(), getPos().y(), getPos().z(), false, 200);
		
		//no doppler effect on laser sounds.  it's weird.
		KSoundManager::instance()->setSoundDopplerLevel(ss, 0);
	}
	return true;
	
}

bool StupidPlaceholderShip::willHitFalcon(float distance)
{
	Vec3 dir = getForward();
	Vec3 pos = getPos();
	float length = distance;
	float travelLength = 0.01 * mSpeed;
	
	//if our length is shorter than our velocity * dt, lengthen it so we don't pass through something
	if(travelLength > length)
		length = travelLength;

	std::vector<Spacecraft*> ships;
	Falcon* f = FalconApp::instance().getFalcon();
	osg::Vec3 hitPos;
	if(f->checkRaycast(pos, pos+dir*distance, hitPos))
	{
			ROM::TIE_WAVE_OFF_DISTANCE += 10;
			return true;
	}
//	printf("no hit\n");
	return false;
}

bool StupidPlaceholderShip::isLocalEnemy()
{
	if(FalconApp::instance().tieNode1() && mPlayer) return true;
	return false;
}

void StupidPlaceholderShip::pickNewDestination()
{
	

	//sort of hacky:  temporarily modify our transform and check if we're about to hit the falcon
	Matrix current = getTransform();
	int tries = 0;
	while(tries < 10)
	{
		float theta = Util::random(-0.0, 6.28);
		float phi = Util::random(-1, 2.5);
		Vec3 target = Vec3(500*cosf(theta) * cosf(phi), 500*sinf(phi), -500*sinf(theta) * cosf(phi));
		mTargetPosition = target;
		
		//check!
//		printf("Destination:  %.2f, %.2f, %.2f\n", mTargetPosition.x(), mTargetPosition.y(), mTargetPosition.z());
		Vec3 fwd = mTargetPosition - getPos();
		fwd.normalize();
//		printf("Dir:  %.2f, %.2f, %.2f\n", fwd.x(), fwd.y(), fwd.z());
		Vec3 currentFwd = getForward();		//is this the same direction we're alrready going?  that's not good
//		printf("Current:  %.2f, %.2f, %.2f\n", currentFwd.x(), currentFwd.y(), currentFwd.z());
//		printf("Dot:  %.2f\n", fwd * currentFwd);
		setForward(fwd);
		tries++;		//if we hit enough tries, we're done, even if we're gonna fly into the poor Falcon
		if((fwd*currentFwd) < 0.8 && willHitFalcon(500) == false)		//not gonna hit it?  we're good to go!
		{

			break;
		}
//		printf("Bad destination!  ahhh!\n");

	}
//	printf("Found new direction after %i tries\n", tries);

	setTransform(current);		//restore our transform;
	
}

