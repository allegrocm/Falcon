//
//  ParticleFX.h
//  Millennium Falcon
//
//  Created by Ken Kopecky II on 4/5/14.
//
//

#ifndef __Millennium_Falcon__ParticleFX__
#define __Millennium_Falcon__ParticleFX__

#include <iostream>
#include <osgParticle/ParticleSystem>


class ParticleFX
{
public:
	ParticleFX();
	
	void update(float dt);
	enum ParticleType {EXPLOSION, SPARK, NUM_PARTICLE_TYPES};
	osg::Group* getRoot()	{return mRoot;}

	//make an explosion.  size influences the particle count, life, and overall bigness
	//tweek by adjusting countMult and lifeMult
	void makeExplosion(osg::Vec3 pos, float size, float countMult = 1.0, float lifeMult = 1.0);				//make a standard explosion, like a ship exploding
protected:
	osg::ref_ptr<osg::Group> mRoot;				//our OSG base node
	osg::ref_ptr<osgParticle::ParticleSystem>	mSystems[NUM_PARTICLE_TYPES];		//particle system!

};

#endif /* defined(__Millennium_Falcon__ParticleFX__) */
