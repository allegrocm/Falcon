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
#include <osg/BlendFunc>
#include <osg/Billboard>

using namespace osg;

SpaceBox::SpaceBox()
{
	__FUNCTION_HEADER__
	printf("Making spacebox...\n");
	mRoot = new MatrixTransform();
	Image* img = osgDB::readImageFile("data/textures/Hyades.jpg");
	if(!img)
		Util::logError("Couldn't open Hyades.jpg!");
	Texture2D* tex = new Texture2D(img);


	mRoot->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex);
	mRoot->getOrCreateStateSet()->setMode(GL_LIGHTING, false);
//	mRoot->getOrCreateStateSet()->setAttribute(new Depth(Depth::LESS, 0, 1, false));		//don't write to the depth buffer

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
	mRoot->setNodeMask(1 << NON_GLOW_LAYER);
	mRoot->getOrCreateStateSet()->setMode(GL_BLEND, true);
	
	//add a planet!
	addPlanet("data/textures/gasgiant_map42.jpg", Vec3(0, -5000, 0), 3500);
	addPlanet("data/textures/pluto.jpg", Vec3(1500, 700, 1200), 500);
	addBillboard("data/textures/Ssc2003-06c.jpg", Vec3(-3500, 1000, -2400), 1000);
}

void SpaceBox::addPlanet(std::string texture, osg::Vec3 pos, float radius)
{
	osg::Sphere* sphere = new Sphere(pos, radius);
	Geode* gee = new Geode();
	ShapeDrawable* sd = new ShapeDrawable(sphere);
	sd->setColor(Vec4(1, 1, 1, 1));
	sd->getOrCreateStateSet()->setRenderingHint(osg::StateSet::OPAQUE_BIN);
	gee->addDrawable(sd);
	mRoot->addChild(gee);
	Image* img = osgDB::readImageFile(texture);
	if(!img)
		Util::logError("Couldn't open texture %s!", texture.c_str());
	Texture2D* tex = new Texture2D(img);
	
	Sphere* sphere2 = new Sphere(pos, radius * 1.2);
	sd = new ShapeDrawable(sphere2);
	sd->setColor(Vec4(1, .0, .0, .25));
	sd->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
//	gee->addDrawable(sd);
	gee->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex);
	gee->getOrCreateStateSet()->setMode(GL_LIGHTING, true);
	gee->getOrCreateStateSet()->setMode(GL_LIGHT1, false);
//	gee->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
//	sd->getOrCreateStateSet()->setAttribute(new BlendFunc(GL_SRC_ALPHA, GL_ONE));
}


void SpaceBox::addBillboard(std::string texture, Vec3 pos, float height)
{

	//kludgy OSG stuff
	Billboard* b = new Billboard;
	b->setMode(Billboard::POINT_ROT_EYE	);
	Geometry* geom = new Geometry();
	Vec4Array* colors = new Vec4Array();
	colors->push_back(Vec4(1, 1, 1, 1));
	geom->setColorArray(colors);
	geom->setColorBinding(Geometry::BIND_OVERALL);
	float width = height * 1.5;
	Vec2Array* texes = new Vec2Array();
	texes->push_back(Vec2(0, 0));
	texes->push_back(Vec2(1, 0));
	texes->push_back(Vec2(1, 1));
	texes->push_back(Vec2(0, 1));
	geom->setTexCoordArray(0, texes);
	
	Vec3Array* verts = new Vec3Array();
	verts->push_back(Vec3(-width*0.5, 0, -height*0.5));
	verts->push_back(Vec3(width*0.5, 0, -height*0.5));
	verts->push_back(Vec3(width*0.5, 0, height*0.5));
	verts->push_back(Vec3(-width*0.5, 0, height*0.5));
	geom->setVertexArray(verts);
	
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));
	b->addDrawable(geom);
	
	Image* img = osgDB::readImageFile(texture);
	if(!img)
		Util::logError("Couldn't open texture %s!", texture.c_str());
	Texture2D* tex = new Texture2D(img);
	b->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex);
	b->getOrCreateStateSet()->setMode(GL_LIGHTING, false);
	b->setPosition(0, pos);
	mRoot->addChild(b);

	b->getOrCreateStateSet()->setAttribute(new BlendFunc(GL_SRC_ALPHA, GL_ONE));
}
