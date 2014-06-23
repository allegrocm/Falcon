//
//  ScreenImage.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky on 4/16/14.
//
//

#include "ScreenImage.h"
#include <osg/Geometry>
#include <osg/Geode>
#include <osgDB/ReadFile>
#include <osg/Texture2D>
#include <osg/AlphaFunc>
#include <osg/BlendFunc>
#include <osg/Depth>
#include "Util.h"


using namespace osg;

ScreenImage::ScreenImage(bool noDepth)
{
	transform = new PositionAttitudeTransform();
	visible = true;
	scale = 1.0;
	aspect = 1.0;
	Geode* geode = new Geode();
	transform->addChild(geode);
	
	//make a quad
	
	osg::Vec3Array* verts = new osg::Vec3Array;
	
	float width = 1.0;
	float height = 1.0;
	float x0 = -width*0.5;
	float y0 = -height*0.5;
	verts->push_back(osg::Vec3(x0, y0, 0));
	verts->push_back(osg::Vec3(x0 + width, y0, 0));
	verts->push_back(osg::Vec3(x0 + width, y0 + height, 0));
	verts->push_back(osg::Vec3(x0, y0 + height, 0));

	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(1, 1, 1, 1));
		
	geometry = new Geometry();

	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
	geode->addDrawable(geometry);

	geometry->setVertexArray(verts);

	geometry->setColorArray(colors);
	geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	osg::Vec2Array* texes = new osg::Vec2Array;
	texes->push_back(osg::Vec2(0, 0));
	texes->push_back(osg::Vec2(1, 0));
	texes->push_back(osg::Vec2(1, 1));
	texes->push_back(osg::Vec2(0, 1));
	geometry->setTexCoordArray(0, texes);
	transform->getOrCreateStateSet()->setMode(GL_BLEND, true);
//	transform->getOrCreateStateSet()->setAttribute(new BlendFunc(GL_SRC_ALPHA, GL_ONE));
	transform->getOrCreateStateSet()->setAttribute(new AlphaFunc(AlphaFunc::NEVER, 0.05));
	if(noDepth)
		transform->getOrCreateStateSet()->setAttribute(new Depth(Depth::LESS, 0, 1, false));		//don't write to the depth buffer
//	geom->setTexCoordArray(1, texes);			//set texcoord array 1 also, for showing high-score entry

	
}

void ScreenImage::setAspect(float s)
{
	aspect = s;
	setHeight(scale);
}


void ScreenImage::setHeight(float s)
{
	transform->setScale(Vec3(s * aspect, s, 1) * visible);
	scale = s;
}

void ScreenImage::setPos(Vec3 v)
{
	transform->setPosition(v);
}

void ScreenImage::setImage(std::string name)
{
	Image* img = osgDB::readImageFile(name);
	if(!img)
	{
		Util::logError("Couldn't load image %s", name.c_str());
		return;
	}
	Texture2D* tex = new Texture2D(img);
	setAspect((float)img->s() / (float)img->t());

	transform->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex);
	
}

void ScreenImage::setColor(Vec4 c)
{
	osg::Vec4Array* colors = (Vec4Array*)geometry->getColorArray();
	colors->at(0) = c;
	geometry->dirtyDisplayList();


}

void ScreenImage::setTopLeft(Vec3 p)
{
	setPos(p + Vec3(scale * aspect, -scale, 0) * 0.5);
}

void ScreenImage::setVisible(bool b)
{
	visible = b;
	setHeight(scale);
}

void ScreenImage::setAngle(float degrees)
{
	transform->setAttitude(Quat(degrees/57.3, Vec3(0, 0, 1)));
}
