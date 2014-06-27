//
//  SpaceBox.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky on 4/8/14.
//
//

#include "Util.h"
#include "SpaceBox.h"
#include "KenXML.h"

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
	Image* img = Util::loadImage("data/textures/Hyades.jpg");
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
	reload();
	mNearGroup = new MatrixTransform;
	mRoot->addChild(mNearGroup);
	mFarGroup = new MatrixTransform;
	mRoot->addChild(mFarGroup);
	
	loadSystem(0);
	//add a planet!
//	addPlanet("data/textures/gasgiant_map42.jpg", Vec3(0, -5000, 0), 3500);
//	addPlanet("data/textures/pluto.jpg", Vec3(1500, 700, 1200), 500);
//	addBillboard("data/textures/Ssc2003-06c.jpg", Vec3(-3500, 1000, -2400), 1000);
}

void SpaceBox::loadSystem(int which)
{
	__FUNCTION_HEADER__
	if(which < 0 || which >= mSystems.size())
	{
		Util::logError("Couldn't load system %i\n", which);
		return;
	}

	SpaceScene& scene = mSystems[which];
	mCurrentSystem = which;
	
	//clear out our stuffs
	mNearGroup->removeChild(0, mNearGroup->getNumChildren());
	mFarGroup->removeChild(0, mFarGroup->getNumChildren());
	
	//add in this scene's object groups
	mNearGroup->addChild(scene.mNearThings);
	mFarGroup->addChild(scene.mFarThings);

	if(!scene.loaded)		//load the scene if we haven't already
	{
		printf("Load system %s\n", scene.name.c_str());
		for(size_t i = 0; i < scene.objects.size(); i++)
		{
			SpaceObject o = scene.objects[i];
			if(o.type == SpaceObject::PLANET)
			{
				addPlanet(std::string("data/textures/")+o.texName, o.pos, o.size);
			}
			else
				addBillboard(std::string("data/textures/")+o.texName, o.pos, o.size);
		}
		scene.loaded = true;
		printf("Done loading\n");
	}
	

	
}

void SpaceBox::setViewerPos(Vec3 p)
{
	//move the everything to keep it centered around the viewer
	Matrix m;
	m.ptr()[12] = p.x();
	m.ptr()[13] = p.y();
	m.ptr()[14] = p.z();
	mRoot->setMatrix(m);
}


void SpaceBox::addPlanet(std::string texture, osg::Vec3 pos, float radius)
{
	osg::Sphere* sphere = new Sphere(Vec3(), radius);
	Geode* gee = new Geode();
	ShapeDrawable* sd = new ShapeDrawable(sphere);
	sd->setColor(Vec4(1, 1, 1, 1));
	sd->getOrCreateStateSet()->setRenderingHint(osg::StateSet::OPAQUE_BIN);
	gee->addDrawable(sd);
	
	//put a rotation in there
	MatrixTransform* mt = new MatrixTransform;
	mt->addChild(gee);
	Quat q;
	q.makeRotate(4.0/2, 1, 0, 0);
	Matrix mat(q);
	Util::setPos(mat, pos);
	mt->setMatrix(mat);

	mSystems[mCurrentSystem].mNearThings->addChild(mt);
	Image* img = Util::loadImage(texture);
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

	Image* img = Util::loadImage(texture);
	if(!img)
	{
		Util::logError("Couldn't open texture %s!", texture.c_str());
		return;
	}

	//kludgy OSG stuff
	Billboard* b = new Billboard;
	b->setMode(Billboard::POINT_ROT_WORLD	);
	Geometry* geom = new Geometry();
	Vec4Array* colors = new Vec4Array();
	colors->push_back(Vec4(1, 1, 1, 1));
	geom->setColorArray(colors);
	geom->setColorBinding(Geometry::BIND_OVERALL);
	float width = height * (float)img->s() / (float)img->t();
	Vec2Array* texes = new Vec2Array();
	texes->push_back(Vec2(1, 0));
	texes->push_back(Vec2(1, 1));

	texes->push_back(Vec2(0, 1));
	texes->push_back(Vec2(0, 0));

	geom->setTexCoordArray(0, texes);
	
	Vec3Array* verts = new Vec3Array();
	verts->push_back(Vec3(-height*0.5, 0, -width*0.5));
	verts->push_back(Vec3(height*0.5, 0, -width*0.5));
	verts->push_back(Vec3(height*0.5, 0, width*0.5));
	verts->push_back(Vec3(-height*0.5, 0, width*0.5));
	geom->setVertexArray(verts);
	
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));
	b->addDrawable(geom);
	
	Texture2D* tex = new Texture2D(img);
	b->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex);
	b->getOrCreateStateSet()->setMode(GL_LIGHTING, false);
	b->setPosition(0, pos);
	mSystems[mCurrentSystem].mFarThings->addChild(b);

	b->getOrCreateStateSet()->setAttribute(new BlendFunc(GL_SRC_ALPHA, GL_ONE));
}

bool SpaceObject::fromXML(TiXmlElement* element)
{
	std::string eventName = element->Attribute("type");
	printf("Got a %s\n", eventName.c_str());
	if(KenXML::CICompare(eventName, "Planet")) type = PLANET;
	else type = BILLBOARD;
	
	
	for(TiXmlElement* e = element->FirstChildElement(); e; e = e->NextSiblingElement())
	{
		std::string what = e->Value();
		if(KenXML::CICompare(what, "texture"))	KenXML::readValue(e, texName);
		else if(KenXML::CICompare(what, "x"))	KenXML::readValue(e, pos.x());
		else if(KenXML::CICompare(what, "y"))	KenXML::readValue(e, pos.y());
		else if(KenXML::CICompare(what, "z"))	KenXML::readValue(e, pos.z());
		else if(KenXML::CICompare(what, "radius"))	KenXML::readValue(e, size);
		else if(KenXML::CICompare(what, "height"))	KenXML::readValue(e, size);
	}
	
	return true;
	

}


bool SpaceBox::loadSystems(std::string file)
{

	TiXmlDocument* doc = KenXML::loadXmlFile(file);
	if(!doc)
	{
		Util::logError("Couldn't open XML file %s\n", file.c_str());
		return false;
	}
	
	mSystems.clear();
	for(TiXmlElement* eaNode = doc->FirstChildElement(); eaNode; eaNode = eaNode->NextSiblingElement())
	{
		if(!eaNode)
		{
			Util::logError("Error reading the eventSounds XML file\n");
			return false;
		}
		
		if(KenXML::CICompare(eaNode->Value(), "System") == false)
		{
			Util::logError("First node needs to be named Objects, not %s\n", eaNode->Value());
			continue;
		}
		SpaceScene s;
		for(TiXmlElement* eventNode = eaNode->FirstChildElement(); eventNode; eventNode = eventNode->NextSiblingElement())
		{
			if(eventNode->Type() == TiXmlNode::COMMENT) continue;		//skip commented nodes
			std::string what = eventNode->Value();
			if(KenXML::CICompare(what, "object") || KenXML::CICompare(what, "Billboard"))
			{
				SpaceObject object;
				if(object.fromXML(eventNode))
					s.objects.push_back(object);
			}
			else if(KenXML::CICompare(what, "name"))
				KenXML::readValue(eventNode, s.name);
			else Util::logError("SpaceScene doesn't know what a %s is\n", what.c_str());
			
		}
		
		std::string eventName = eaNode->Attribute("name");
		printf("Got name %s\n", eventName.c_str());
		s.name = eventName;

		printf("loaded scene called %s with %i objects\n", s.name.c_str(), (int)s.objects.size());
		mSystems.push_back(s);
		
	}
	printf("Loaded %i systems into the spacebox\n", (int)mSystems.size());
	
	
	return true;
	
}

void SpaceBox::reload()
{
	mSystems.clear();
	loadSystems("data/systems.xml");
	mCurrentSystem = 0;
	
	//make a default scene if we didn't get any
	if(!mSystems.size())
	{
		SpaceScene s;
		SpaceObject o;
		o.size = 3500;
		o.type = SpaceObject::PLANET;
		o.texName = "gasgiant_map42.jpg";
		o.pos = Vec3(0, -5000, 0);
		s.objects.push_back(o);

		o.size = 500;
		o.texName = "pluto.jpg";
		o.pos = Vec3(1500, 700, 1200);
		s.objects.push_back(o);

		o.size = 1000;
		o.texName = "Ssc2003-06c.jpg";
		o.pos = Vec3(-3500, 1000, -2400);
		s.objects.push_back(o);
		s.name = "NoLoadium";
		mSystems.push_back(s);
		
	}
}
