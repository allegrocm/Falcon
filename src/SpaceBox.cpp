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
#include "ShaderManager.h"

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
	mCurrentSystem = 0;



	osg::Box* cube = new Box(Vec3(), 20000);

	mRoot->getOrCreateStateSet()->setMode(GL_LIGHTING, false);

	ShapeDrawable* sd = new ShapeDrawable(cube);
	mBox = new Geode();
	mBox->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex);
	mBox->addDrawable(sd);
	mBox->getOrCreateStateSet()->setAttribute(new Depth(Depth::LESS, 0, 1, false));		//don't write to the depth buffer
	//we render the spacebox to a separate camera so we can have it very, very far away without it
	//interfering with close-range depth testing
	Camera* c = new Camera;
	Matrix projShift;		//matrix to shift the culling distances farther
	projShift.ptr()[15] = 100;
	c->setProjectionMatrix(projShift);
	c->setName("Spacebox Camera");
	mRoot->addChild(c);

	c->setRenderOrder(Camera::NESTED_RENDER, -1);
	mRoot->setNodeMask(1 << BACKGROUND_LAYER);
	mRoot->getOrCreateStateSet()->setMode(GL_BLEND, true);

	mNearGroup = new MatrixTransform;
	//mRoot->addChild(mNearGroup);
	mFarGroup = new MatrixTransform;
	//mRoot->addChild(mFarGroup);
	mFarGroup->addChild(mBox);
	mNearSystemStuff = new Group();
	mNearGroup->addChild(mNearSystemStuff);
	mNearGroup->getOrCreateStateSet()->setMode(GL_LIGHTING, true);
	mNearGroup->setNodeMask(1 << BACKGROUND_LAYER);
	c->addChild(mFarGroup);
	c->addChild(mNearGroup);
	reload();

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
	mNearSystemStuff->removeChild(0, mNearSystemStuff->getNumChildren());
	mFarGroup->removeChild(0, mFarGroup->getNumChildren());
	
	//add in this scene's object groups
	mNearSystemStuff->addChild(scene.mNearThings);
	mFarGroup->addChild(mBox);
	mFarGroup->addChild(scene.mFarThings);


	if(!scene.loaded)		//load the scene if we haven't already
	{
		printf("Load system %s\n", scene.name.c_str());
		for(size_t i = 0; i < scene.objects.size(); i++)
		{
			SpaceObject o = scene.objects[i];
			//printf("This object type is %i\n", o.type);
			if(o.type == SpaceObject::PLANET)
			{
				addPlanet(std::string("data/textures/planets/")+o.texName, o.pos, o.size);
			}
			else if(o.type == SpaceObject::BILLBOARD_PLUS)
				addBillboard(std::string("data/textures/planets/")+o.texName, o.pos, o.size, true);
			else if(o.type == SpaceObject::MODEL)
				addModel(o);
			else
				addBillboard(std::string("data/textures/planets/")+o.texName, o.pos, o.size, false);
		}
		scene.loaded = true;
		printf("Done loading\n");
	}
	else printf("Switch to system %s\n", scene.name.c_str());
	

	
}

void SpaceBox::setViewerPos(Vec3 p)
{
	//move the everything to keep it centered around the viewer
	Matrix m;
	m.ptr()[12] = p.x();
	m.ptr()[13] = p.y();
	m.ptr()[14] = p.z();
	mFarGroup->setMatrix(m);
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


void SpaceBox::addModel(SpaceObject o)
{
	//load the model.  pass in the rotation here cuz it's easy
	printf("add model.  size = %.2f, angles:  %.2f, %.2f, %.2f\n", o.size, o.heading.x(), o.heading.y(), o.heading.z());
	MatrixTransform* matx = Util::loadModel(o.texName, 1.0);
	Matrix mat = Matrix::scale(o.size, o.size, o.size);
	mat = mat * Matrix::rotate(o.heading.x() / 57.296, Vec3(1, 0, 0));
	mat = mat * Matrix::rotate(o.heading.z() / 57.296, Vec3(0, 0, 1));
	mat = mat * Matrix::rotate(o.heading.y() / 57.296, Vec3(0, 1, 0));


	mat = mat * Matrix::translate(o.pos);

	//position the model

	matx->setMatrix(mat);
	
	//apply shader
	ShaderManager::instance().applyShaderToNode("data/shaders/DeathStar", matx);
	
	//and add it!
	mSystems[mCurrentSystem].mNearThings->addChild(matx);
	mSystems[mCurrentSystem].mCapitalShips.push_back(matx);
	
}


void SpaceBox::addBillboard(std::string texture, Vec3 pos, float height, bool additiveBlend)
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

	if(additiveBlend)
		b->getOrCreateStateSet()->setAttribute(new BlendFunc(GL_SRC_ALPHA, GL_ONE));
}

bool SpaceObject::fromXML(TiXmlElement* element)
{
	__FUNCTION_HEADER__
	std::string eventName = element->Attribute("type");
	printf("Got a %s\n", eventName.c_str());
	if(KenXML::CICompare(eventName, "Planet")) type = PLANET;
	else if(KenXML::CICompare(eventName, "BillBoard")) type = BILLBOARD;
	else if(KenXML::CICompare(eventName, "Model")) type = MODEL;
	else type = BILLBOARD_PLUS;
	
	
	for(TiXmlElement* e = element->FirstChildElement(); e; e = e->NextSiblingElement())
	{
		std::string what = e->Value();
		if(KenXML::CICompare(what, "texture"))	KenXML::readValue(e, texName);
		else if(KenXML::CICompare(what, "file"))	KenXML::readValue(e, texName);
		else if(KenXML::CICompare(what, "x"))	KenXML::readValue(e, pos.x());
		else if(KenXML::CICompare(what, "y"))	KenXML::readValue(e, pos.y());
		else if(KenXML::CICompare(what, "z"))	KenXML::readValue(e, pos.z());
		else if(KenXML::CICompare(what, "radius"))	KenXML::readValue(e, size);
		else if(KenXML::CICompare(what, "height"))	KenXML::readValue(e, size);
		else if(KenXML::CICompare(what, "scale"))	KenXML::readValue(e, size);
		else if(KenXML::CICompare(what, "yaw")) KenXML::readValue(e, heading.y());
		else if(KenXML::CICompare(what, "roll")) KenXML::readValue(e, heading.z());
		else if(KenXML::CICompare(what, "pitch")) KenXML::readValue(e, heading.x());
		else Util::logError("SpaceObject doesn't know what to do with %s\n", what.c_str());
	}
	
	//preload all the textures
	if(type != MODEL)
		Util::loadImage(std::string("data/textures/planets/") + texName);
	return true;
	

}


bool SpaceBox::loadSystems(std::string file)
{
	__FUNCTION_HEADER__
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
//		printf("Got name %s\n", eventName.c_str());
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
	loadSystem(mCurrentSystem);
}

MatrixTransform* SpaceBox::getCapitalShip()
{
	if(!mSystems[mCurrentSystem].mCapitalShips.size()) return NULL;
	int which = rand()%mSystems[mCurrentSystem].mCapitalShips.size();
	return mSystems[mCurrentSystem].mCapitalShips[which];
}

void SpaceBox::setNavMatrix(Matrix nav, float z)
{
	Matrix poslessNav = nav;
	Util::setPos(poslessNav, Vec3());		//remove the position
	Matrix navInverse = poslessNav;
	navInverse.invert(navInverse);
	//everything but the near group just uses the regular nav matrix
	mRoot->setMatrix(poslessNav);
	
	//but we DO move the near group around us
	//so undo the previous operation first
	nav.ptr()[14] -= z;
	mNearGroup->setMatrix(nav * navInverse);
}
