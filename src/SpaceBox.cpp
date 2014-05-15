//
//  SpaceBox.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky on 4/8/14.
//
//

#include "Util.h"
#include "SpaceBox.h"

#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/Texture2D>
#include <osg/Image>
#include <osgDB/ReadFile>
#include <osg/Depth>
#include <osg/Camera>


using namespace osg;

SpaceBox::SpaceBox()
{
	
	mRoot = new MatrixTransform();
	Image* img = osgDB::readImageFile("data/textures/Hyades.jpg");
	if(!img)
		Util::logError("Couldn't open Hyades.jpg!");
	Texture2D* tex = new Texture2D(img);


	mRoot->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex);
	mRoot->getOrCreateStateSet()->setMode(GL_LIGHTING, false);
	mRoot->getOrCreateStateSet()->setAttribute(new Depth(Depth::LESS, 0, 1, false));		//don't write to the depth buffer

	osg::Box* cube = new Box(Vec3(), 10000);
	ShapeDrawable* sd = new ShapeDrawable(cube);
	Geode* g = new Geode();
	g->addDrawable(sd);

	//we render the spacebox to a separate camera so we can have it very, very far away without it
	//interfering with close-range depth testing
	Camera* c = new Camera;
	c->setName("Spacebox Camera");
	mRoot->addChild(c);
	c->addChild(g);
	c->setRenderOrder(Camera::NESTED_RENDER, -1);
}
