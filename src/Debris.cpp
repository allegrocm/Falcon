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
Debris::Debris(osg::Node* model, osg::Vec3 generalDirection, float directionDelta, float speed,
	float angularSpeed, float modelScale)
	:GameObject()
{
	mOtherPat = new PositionAttitudeTransform();
	mPat->addChild(mOtherPat);
	mFireAmount = 1.0;
	if(model)
		mOtherPat->addChild(model);
	
	//final direction/velocity
	mVel = Util::vectorInCone(generalDirection, directionDelta);
	mVel.normalize();
	mVel *= speed;
	
	
	mOmega = angularSpeed / 57.3;
	mSpinAxis = Vec3(-1.0+2.0*Util::loggedRandom("Debris")/RAND_MAX, -1.0+2.0*Util::loggedRandom("Debris")/RAND_MAX, -1.0+2.0*Util::loggedRandom("Debris")/RAND_MAX);
	mSpinAxis.normalize();
	mLife = 2.0;
}

void Debris::setTransformAndOffset(Matrixf transform, Vec3 offset)
{
	//reparent so our pat is in the middle of two transforms
//	MatrixTransform* pre = new MatrixTransform;
//	pre->setName("Pre-Transform");
//	MatrixTransform* post = new MatrixTransform;
//	post->setName("Post-Transform");
//	pre->addChild(mPat);
//	mRoot->removeChild(mPat);
//	mRoot->addChild(pre);
//	post->addChild(mPat->getChild(0));
//	mPat->removeChild(mPat->getChild(0));
//	mPat->addChild(post);
//	pre->setMatrix(Matrix::translate(offset*-1));
//	post->setMatrix(Matrix::translate(offset));
	
//	setPos(offset);		//adjust so we don't translate when we set our pivot point
//	Matrixf xlate = Matrixf::translate(offset);
	setTransform(transform);
	mOtherPat->setPivotPoint(-offset);
	mOtherPat->setPosition(-offset);
//	printf("node:\n");
//	Util::printNodeHierarchy(mRoot);


	
}


bool Debris::update(float dt)
{
//	return true;
	GameObject::update(dt);
	mOtherPat->setAttitude(Quat(mOmega * dt, mSpinAxis) * mOtherPat->getAttitude());
	setPos(getPos() + mVel * dt);
	
	//lots of little explosions as we die
	FalconApp::instance().getFX()->makeExplosion(getPos() + Util::randomVector()*Util::random(0.2, 2.5) * mFireAmount, .75 *  mFireAmount, 0.2 / mFireAmount, 1.0 / mFireAmount);
	return(mAge < mLife);

}


void Debris::drawDebug()
{
	Vec3 pivot = mOtherPat->getPivotPoint();
//	printf("pivot at %.2f, %.2f, %.2f\n", pivot.x(), pivot.y(), pivot.z());
	glPushMatrix();
		glMultMatrixf(getTransform().ptr());
		glBegin(GL_LINES);
			glColor3f(1, 1, 1);
			glVertex3f(pivot.x(), pivot.y(), pivot.z());
			glVertex3f(0, 0, 0);
			float size = 2;
			for(int i = 0; i < 3; i++)
			{
				glColor3f(i==0, i==1, i==2);
				glVertex3f(0, 0, 0);
				glVertex3f(size*(i==0), size*(i==1), size*(i==2));
			}
		glEnd();
	glPopMatrix();
	

}

