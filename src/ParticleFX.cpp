//
//  ParticleFX.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky II on 4/5/14.
//
//

#include "ParticleFX.h"
#include <osgParticle/Particle>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/LinearInterpolator>
#include <osgParticle/FluidFrictionOperator>
#include <osgParticle/ModularProgram>
#include <osg/BlendFunc>
#include <osg/Depth>
#include "Layers.h"
#ifdef KENS_MIDI_CONTROLLER
#include "magicjoystick.h"
#endif

using namespace osg;

ParticleFX::ParticleFX()
{
	mRoot = new Group();
	Geode* g = new Geode;
	mRoot->addChild(g);
	osgParticle::ParticleSystemUpdater* psu = new osgParticle::ParticleSystemUpdater;
	mRoot->addChild(psu);
//	mRoot->setNodeMask(1 << GLOW_LAYER);
	mRoot->setNodeMask(1 << BACKGROUND_LAYER);				//particles don't glow or occlude glow
	for(int i = 0; i < NUM_PARTICLE_TYPES; i++)
	{
		mSystems[i] = new osgParticle::ParticleSystem();
	//
		mSystems[i]->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		mSystems[i]->getOrCreateStateSet()->setRenderBinDetails(2, "RenderBin");
		mSystems[i]->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		g->addDrawable(mSystems[i]);
		psu->addParticleSystem(mSystems[i]);
	}

	//set up damping for the explosion particle type
	// let's add a fluid operator to simulate air friction.
	osgParticle::FluidFrictionOperator *op3 = new osgParticle::FluidFrictionOperator;
	op3->setFluidToAir();
	op3->setFluidViscosity(.05);
	op3->setFluidDensity(2);
	osgParticle::ModularProgram* explosionProgram = new osgParticle::ModularProgram();
	explosionProgram->setParticleSystem(mSystems[EXPLOSION]);
	explosionProgram->addOperator(op3);
	mRoot->addChild(explosionProgram);
	mSystems[EXPLOSION]->setDefaultAttributes("data/textures/fireWhite.png");
//	mSystems[EXPLOSION]->setDefaultAttributes("data/textures/RingSS.png");
	mSystems[EXPLOSION]->getOrCreateStateSet()->setAttribute(new Depth(Depth::LESS, 0, 1, false));		//don't write to the depth buffer
	g->getOrCreateStateSet()->setAttribute(new BlendFunc(GL_SRC_ALPHA, GL_ONE));		//additive blending for the display hologram
	g->getOrCreateStateSet()->setMode(GL_BLEND, true);
}

void ParticleFX::update(float dt)
{
	//nothing yet...
}

void ParticleFX::makeExplosion(Vec3 pos, float size, float countMult, float lifeMult)
{

//	printf("boom at %.2f, %.2f, %.2f\n", pos.x(), pos.y(), pos.z());
	osgParticle::Particle basicParticle;
	basicParticle.setLifeTime(.75);
	basicParticle.setAlphaRange(osgParticle::rangef(0.0, 1.0));
//	basicParticle.setTextureTileRange(8, 1, 0, 7);

	float baseSize = 2.5*.16 * size;
	
	basicParticle.setColorInterpolator(new osgParticle::LinearInterpolator);
	float speedMult = 2.0*0.35 * size;
	int numParticles = 0.13*5*75 * size * countMult;
#ifdef KENS_MIDI_CONTROLLER


////	numParticles *= 5.0 * MIDIControl::slider(1);
//	baseSize *= 2.5 * MIDIControl::slider(2);
//	speedMult = 2.0 * MIDIControl::slider(3);
//				basicParticle.setLifeTime(2.0 * MIDIControl::slider(1) + 0.01);
//				basicParticle.setSizeRange(osgParticle::rangef(.1 * MIDIControl::slider(1), .2 * MIDIControl::slider(1)));
#endif
//	printf("Speed mult:  %.2f\n", speedMult);
	basicParticle.setSizeRange(osgParticle::rangef(4.0 * baseSize, 10.0 * baseSize));

	for(int i = 0; i < numParticles; i++)
	{
		basicParticle.setColorRange(osgParticle::range<Vec4>(Vec4(1.0, .9, 0.5, 1.0), Vec4(0.5, 0.2, 0.1, 0.0)));


		//local position of this particle within the explosion
		Vec3 offset( -1.0 + 2.0 * rand() / RAND_MAX, -1.0 + 2.0 * rand() / RAND_MAX, -1.0 + 2.0 * rand() / RAND_MAX);
		offset.normalize();
		offset *= 1.0 * rand() / RAND_MAX * size;
		osgParticle::Particle* party = mSystems[EXPLOSION]->createParticle(&basicParticle);
		party->setLifeTime((0.25 + 0.5 * rand() / RAND_MAX) * size * lifeMult);
		party->setPosition(pos + offset);
		party->setVelocity(offset * speedMult * (50.0 + 55.0 * rand() / RAND_MAX));
		party->setAngularVelocity(Vec3(0, 0, 2.0 * (-1.0 + 2.0 * rand() / RAND_MAX)));
//		printf("Made particle at %.2f, %.2f, %.2f\n", pos.x()+offset.x(), pos.y()+offset.y(), pos.z()+offset.z());
	}
}


