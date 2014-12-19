//
//  VaderTIE.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky II on 6/12/14.
//
//

#include "VaderTIE.h"
#include "Util.h"
#include "FalconApp.h"
#include "EnemyPlayer.h"
#include <osg/LOD>
#include <osgUtil/Optimizer>
#include "Layers.h"
#include "Bullet.h"
#include <osgUtil/IntersectVisitor>
#include <osg/ShapeDrawable>
#include "ROM.h"
#include "Defaults.h"
#include "ParticleFX.h"
#include "ScreenImage.h"
using namespace osg;

VaderTIE::VaderTIE()
:StupidPlaceholderShip()
{
//	loadTIEModel();
	mHP = ROM::VADER_TIE_HP;
	
	//distribute our "zone HP' evenly so it shows our araes getting damaged on the computer
	for(int i = 0; i < 3; i++)
		mZoneHP[i] = ROM::VADER_TIE_HP/3;
	mTopSpeed = ROM::VADER_TIE_MAX_SPEED;
	setName("Vader's TIE");
}

void VaderTIE::loadTIEModel()
{
//	StupidPlaceholderShip::loadTIEModel();
//	printf("Load thai model:  schmancy\n");
	//load a ship model.  we can also pre-transform the model into our coordinate system
//	MatrixTransform* red = Util::loadModel("data/models/tief3DS/TIEFReduced.3DS", 1.0, -90);
	MatrixTransform* nbest = Util::loadModel("data/models/vadertie3DS/vadertie.3ds", 1.1 / 25.4, 90, 0, 180, Vec3(-.08, 0, 0));
//	FalconApp::instance().getModelGroup()->addChild(nbest);
//	MatrixTransform* nbest = Util::loadModel("data/models/vadertie3DS/vadertie.3ds", 1.0, -90);
//	MatrixTransform* lod = Util::loadModel("data/models/tief3DS/TIEF_10.3ds", 1.0, -90);
	
	//use an LOD to reduce render time
	osg::LOD* l = new LOD();
//	l->addChild(g, 0, 100000);
	//set the LOD for farther away rendering, and the main model for close up
//	l->addChild(lod, 150, 100000);
//	l->addChild(n, 50, 150);
	l->addChild(nbest, 0, 50000);
//	l->addChild(red, 0, 2000);
//	Util::cullSmallGeodes(n, 1.0);
//	Util::cullSmallGeodes(lod, 2.0);
	osgUtil::Optimizer* o = new osgUtil::Optimizer();
	if(nbest)
		o->optimize(nbest);
//	if(lod)
//		o->optimize(lod);
//	Util::printNodeHierarchy(n);


	//add a sphere so it's easier to hit the far-away version
	Sphere* sphere = new Sphere(Vec3(0, 0, 0), ROM::VADER_HITBOX_SIZE);
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
		if(isLocalEnemy())
		{
			mEngineSound = KSoundManager::instance()->playSound(std::string("data/sounds/") + engineSound, 0, 0, true);
		}
		else
			mEngineSound = KSoundManager::instance()->play3DSound(std::string("data/sounds/") + engineSound, 0.75, 1000, 1000, 1000, true, 80);
	}


	//add an aiming reticle
	int numRings = 3;
	if(FalconApp::instance().tieNode1())
	{
		for(int i = 0; i < numRings; i++)
		{
			ScreenImage* reticle1 = new ScreenImage(true);
			setLayer(reticle1->transform, COLLISION_LAYER, false);
			
			//don't know why this works
			//putting anything in the transparent bin seems to make it vanish
			//maybe something to do with the bloom effect
			reticle1->transform->getOrCreateStateSet()->setRenderingHint(StateSet::OPAQUE_BIN);
			reticle1->setImage(Util::findDataFile("data/textures/reticleSingle.png"));
			reticle1->setPos(Vec3(0, -2, -100 * (1+i)));
			reticle1->setHeight(2);
			reticle1->transform->getOrCreateStateSet()->setMode(GL_LIGHTING, GL_FALSE);
			mPat->addChild(reticle1->transform);
		}
	}

}

void VaderTIE::wasHit(Bullet* b, osg::Vec3 hitPos)
{
	printf("LogRDM Vader got hit at %.4f,%i hp left random = %i, pos = %.2f, %.2f, %.2f\n", FalconApp::instance().getTime(), mHP, rand()%1000,
		getPos().x(), getPos().y(), getPos().z());
	StupidPlaceholderShip::wasHit(b, hitPos);
	FalconApp::instance().getFX()->makeExplosion(hitPos, 1.0);
	//which zone did this hit?
	Matrix inv = getTransform();
	inv.invert(inv);

	Vec3 relPos = hitPos * inv;			//transform into local space
	
	float cutoff = 2.5;				//how far to the side before a hit is on the wing?
	int zone = 1;				//center!
	if(relPos.x() > cutoff)		//right
	{
		zone = 2;
	}
	else if(relPos.x() < -cutoff) zone = 0;		//left
//	printf("hit!  rel position:  %.2f.  zone %i\n", relPos.x(), zone);
	
	//play a sound to commemorate this occasion
	float pan = relPos.x();
	if(pan < -1) pan = -1;
	if(pan > 1) pan = 1;
	
	
	mZoneHP[zone] -= b->mDamage;
	if(isLocalEnemy())
	{
		//play the rumble sound!
		KSoundManager::instance()->playSound(std::string("data/sounds/") + "BoomShort.wav", 1, 0, false);
		
		//and other sounds
		KSoundManager::instance()->playSound(std::string("data/sounds/") + ROM::VADER_HIT_SOUND, 1, pan, false);
		if(mHP == 0)
		{
			KSoundManager::instance()->playSound(std::string("data/sounds/") + ROM::VADER_DIE_SOUND, 1, 0, false);
		}
	}
}

