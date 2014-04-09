//
//  Debris.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky II on 4/7/14.
//
//

#include "Debris.h"
#include "Util.h"
#include "FalconApp.h"
#include "ParticleFX.h"

using namespace osg;
Debris::Debris(std::string modelName, osg::Vec3 generalDirection, float directionDelta, float speed,
	float angularSpeed)
	:GameObject()
{
	MatrixTransform* model = Util::loadModel(modelName, 10.0, -90);
	if(model)
		mPat->addChild(model);
	
	if(0.01 > generalDirection.length()) generalDirection = Vec3(1, 0, 0);		//construct a coordinate system around our direction
	generalDirection.normalize();
	Vec3 perp1 = generalDirection ^ Vec3(0.5, 0.2, 0.1);
	if(perp1.length() < 0.1)
		perp1 = generalDirection ^ Vec3(0.0, 0.5, 0.5);
	
	perp1.normalize();
	
	Vec3 perp2 = generalDirection ^ perp1;
	perp2.normalize();
	
	
	//now point our general direction somewhere along this cone we've made
	float phi = directionDelta * (-1.0 + 2.0 * rand() / RAND_MAX) / 57.3;

	float s = sinf(phi);
	float c = cosf(phi);
	
	float theta = 6.28 * rand() / RAND_MAX;
	float st = sinf(theta);
	float ct = cosf(theta);
	
	
	//final direction/velocity
	mVel = generalDirection * c + (perp1 * ct + perp2 * st) * s;
	mVel.normalize();
	mVel *= speed;
	
	
	mOmega = angularSpeed / 57.3;
	mSpinAxis = Vec3(-1.0+2.0*rand()/RAND_MAX, -1.0+2.0*rand()/RAND_MAX, -1.0+2.0*rand()/RAND_MAX);
	mSpinAxis.normalize();
	mLife = 2.0;
}

void Debris::setTransformAndOffset(Matrixf transform, Vec3 offset)
{

	setTransform(transform);
	mPat->setPivotPoint(offset);
	setPos(getPos()+offset);		//adjust so we don't translate when we set our pivot point
	
}


bool Debris::update(float dt)
{
//	return true;
	GameObject::update(dt);
	mPat->setAttitude(Quat(mOmega * dt, mSpinAxis) * mPat->getAttitude());
	setPos(getPos() + mVel * dt);
	
	//lots of little explosions as we die
	FalconApp::instance().getFX()->makeExplosion(getPos() + Util::randomVector()*Util::random(0.2, 2.5), .75, 0.2, 1.0);
	return(mAge < mLife);

}

