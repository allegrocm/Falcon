//
//  Bloom.h
//  Millennium Falcon
//
//  Created by Ken Kopecky on 5/27/14.
//
//

#ifndef __Millennium_Falcon__Bloom__
#define __Millennium_Falcon__Bloom__
#include "CameraThatRendersAQuad.h"
#include "PrerenderCamera.h"
#include <osg/MatrixTransform>
#include <iostream>

#include "Layers.h"

//sets up bloom, given a scene and its root

//nothing wills how up unless you set its mask to include GLOW_LAYER and NOT NON_GLOW_LAYER
class BloomController
{
public:
	BloomController();
	
	osg::Group* getScene()				{return mScene;}	//put scene objects into this group (things that will either glow or occlude glow)
	osg::Group* getRoot()				{return mRoot;}		//add this as a child to the scenegraph root
	
	//parameters
	void setGlowAmount(float f);							//pixels :-/
	void setGlowGain(float f);

protected:
	osg::ref_ptr<osg::Group>			mScene;				//holds things that glow or occlude glow
	osg::ref_ptr<osg::Group>			mRoot;				//holds our cameras.  add this to the scenegraph root
	osg::ref_ptr<osg::Uniform>			mGlowAmount;		//pixels
	osg::ref_ptr<osg::Uniform>			mGlowGain;			//multiply the light!
	osg::ref_ptr<osg::Uniform>			mGlowRadius;		//used for sampling the glow.  not adjustable
	
	osg::ref_ptr<osg::Camera>			mNonGlowCamera;		//renders our non-glowing things in the scene
	osg::ref_ptr<osg::Camera>			mGlowCamera;		//renders our glowing things in the scene
	
};

#endif /* defined(__Millennium_Falcon__Bloom__) */
