//
//  Bloom.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky on 5/27/14.
//
//

#include "Bloom.h"
#include <osg/BlendFunc>
#include <osg/Depth>
#include "ShaderManager.h"
#include "Layers.h"
#include "Defaults.h"

using namespace osg;

#define prerenderTextureSize 1024

BloomController::BloomController()
{
	CameraThatRendersAQuad* bloomCam = new CameraThatRendersAQuad;		//this camera overlays the blurred glow onto our scene as bloom
	mRoot = new Group();
	mScene = new Group;
	BlendFunc* funk = new BlendFunc(GL_ONE, GL_ONE);	//the compositing camera does addditive blending with the glowverlay
	bloomCam->getOrCreateStateSet()->setAttribute(funk);

	bloomCam->getOrCreateStateSet()->setMode(GL_BLEND, true);
	mRoot->addChild(bloomCam);
//	bloomCam->setViewport(100, 100, 500, 500);


	bloomCam->setClearMask(0);
	//this is needed to make glow show up on the Oculus for some reason.  normally it would be POST_RENDER
#ifdef OCULUS
	bloomCam->setRenderOrder(Camera::NESTED_RENDER, 999);
#else
	bloomCam->setRenderOrder(Camera::POST_RENDER);
#endif
	bloomCam->getOrCreateStateSet()->setAttribute(new osg::Depth(Depth:: LESS, 0, 1, false));	//don't write to the depth buffer


	//using two-stage blurring (once along x, then once along y) is faster than blurring along x and y simultaneously
	PrerenderCamera* glowBlendCam1 = new PrerenderCamera(prerenderTextureSize, 1, GL_RGBA);
	PrerenderCamera* glowBlendCam2 = new PrerenderCamera(prerenderTextureSize, 1, GL_RGBA);
	glowBlendCam1->addQuadGeometry();			//set this up as a render-from-texture-input camera
	glowBlendCam2->addQuadGeometry();			//set this up as a render-from-texture-input camera
	mNonGlowCamera = new osg::Camera;		//the glow scene renders in our pre-render camera
	mGlowCamera = new osg::Camera;
//	Camera* gc = _glowCamera;
	mGlowCamera->setCullingMode(0);			//don't do any non-node-mask culling
	mNonGlowCamera->setCullingMode(0);			//don't do any non-node-mask culling
	PrerenderCamera* blendInputCam = new PrerenderCamera(prerenderTextureSize, 1, GL_RGBA);
//	glowBlendCam1->setRenderOrder(Camera::NESTED_RENDER, 0);
//	glowBlendCam2->setRenderOrder(Camera::NESTED_RENDER, 1);
//	blendInputCam->setRenderOrder(Camera::NESTED_RENDER, 2);
//	gc = blendInputCam;
	mNonGlowCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//noglow draws first and clears everything
	mGlowCamera->setClearMask(GL_COLOR_BUFFER_BIT);							//glow camera draws second, only clears color
	mNonGlowCamera->setRenderOrder(osg::Camera::NESTED_RENDER, 4);
	mNonGlowCamera->setClearColor(Vec4(1, 1, 0, 1));
	ShaderManager::applyShaderToNode("data/shaders/JustDepth", mNonGlowCamera);		//use a shader to make everything the noglow camera draws black
	mGlowCamera->setClearColor(Vec4(1, 1, 1, 1));
	mGlowCamera->setRenderOrder(osg::Camera::NESTED_RENDER, 5);
	
	//we're using culling modes to separate the glowing and non-glowing objects.  set 0x1 for nonglow, and 0x2 for glow (node->setNodeMask(0x1);)
	mNonGlowCamera->addChild(mScene);			//things that don't glow
	mNonGlowCamera->setCullMask(1 << NON_GLOW_LAYER);
	mGlowCamera->addChild(mScene);			//things in the scene do glow
	mGlowCamera->setCullMask(1 << GLOW_LAYER | 1 << GLOW_ONLY_LAYER);

	glowBlendCam1->addInputTexture(blendInputCam->getTargetTexture(0), "glowInput", 0);	//glowblend 1 users the raw glowy objects as input
	glowBlendCam2->addInputTexture(glowBlendCam1->getTargetTexture(0), "glowInput", 0);	//glowblend2 uses the output of glowblend1 as input
	ShaderManager::applyShaderToNode("data/shaders/LinearBlur", glowBlendCam1);		//this shader blurs the bloom!
	ShaderManager::applyShaderToNode("data/shaders/LinearBlur", glowBlendCam2);		//this shader blurs the bloom!
	blendInputCam->addChild(mNonGlowCamera);
	blendInputCam->addChild(mGlowCamera);
	mGlowRadius = new Uniform("blurDelta", 1.0f / prerenderTextureSize);
	mGlowAmount	= new Uniform("blurAmount", 5.0f * prerenderTextureSize / 512);
	mGlowAmountV	= new Uniform("blurAmount", 5.0f * prerenderTextureSize / 512);
	mGlowGain = new Uniform("glowGain", 1.5f);
	float glowAmount = 5.6;
	float glowGain = 1.1;
	Defaults::instance().getValue("glowAmount", glowAmount);
	Defaults::instance().getValue("glowGain", glowGain);

	mGlowAmount->set(glowAmount);
	mGlowGain->set(glowGain);
	mGlowAmountV->set(glowAmount * 16.0 / 9.0);		//default to HDTV aspecct
	glowBlendCam1->getOrCreateStateSet()->addUniform(new Uniform("blurDirection", Vec2(1, 0)));		//horizontal blur
	glowBlendCam2->getOrCreateStateSet()->addUniform(new Uniform("blurDirection", Vec2(0, 1)));		//vertical blur
	glowBlendCam1->getOrCreateStateSet()->addUniform(mGlowRadius);
	glowBlendCam1->getOrCreateStateSet()->addUniform(mGlowAmount);
	glowBlendCam2->getOrCreateStateSet()->addUniform(mGlowRadius);
	glowBlendCam2->getOrCreateStateSet()->addUniform(mGlowAmountV);
//	glowBlendCam2->getOrCreateStateSet()->addUniform(new Uniform("blurAmount", glowAmount * 2 ));
	glowBlendCam1->getOrCreateStateSet()->addUniform(mGlowGain);
	glowBlendCam2->getOrCreateStateSet()->addUniform(mGlowGain);

	mRoot->addChild(blendInputCam);
	mRoot->addChild(glowBlendCam1);
	mRoot->addChild(glowBlendCam2);
	bloomCam->setTexture(glowBlendCam2->getTargetTexture(0));
}

void BloomController::setGlowAmount(float f)
{
	//maintain aspect ratio adjustment when we do this
	float v1, v2;
	mGlowAmount->get(v1);
	mGlowAmountV->get(v2);
	float aspect = v2/v1;
	mGlowAmount->set(f);
	mGlowAmountV->set(f*aspect);
}

void BloomController::setAspectRatio(float f)
{
	//vertical blur amount is horizontal blur amount * aspect ratio
	float v;
	mGlowAmount->get(v);
	mGlowAmountV->set(v * f);
}

void BloomController::setGlowGain(float f)	{mGlowGain->set(f);}


