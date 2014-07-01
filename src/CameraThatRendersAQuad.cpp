/*
 *  CameraThatRendersAQuad.cpp
 *  DP3D
 *
 *  Created by Kenneth Kopecky on 6/10/08.
 *  Copyright 2008 Orange Element Studios. All rights reserved.
 *
 */

#include "CameraThatRendersAQuad.h"
#include <osg/Geometry>



CameraThatRendersAQuad::CameraThatRendersAQuad()
{
	setName("CameraThatRendersAQuad");
	getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	setProjectionMatrix(osg::Matrix::ortho2D(0,512,0,512));
	// only clear the depth buffer
	setClearMask(GL_DEPTH_BUFFER_BIT);

	//make a geode that shows the texture
	mGeode = new osg::Geode;
	mGeode->setName("QuadCameraGeode");
	osg::Vec3Array* verts = new osg::Vec3Array;
	float x0 = 0;
	float y0 = 0;
	float width = 512;
	float height = 512;
	float z = 0;
	verts->push_back(osg::Vec3(x0, y0, z));
	verts->push_back(osg::Vec3(x0 + width, y0, z));
	verts->push_back(osg::Vec3(x0 + width, y0 + height, z));
	verts->push_back(osg::Vec3(x0, y0 + height, z));

	osg::Vec3Array* colors = new osg::Vec3Array;
	colors->push_back(osg::Vec3(1, 1, 1));
		
	osg::Geometry* geom = new osg::Geometry;
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
	mGeode->addDrawable(geom);
	addChild(mGeode);

	geom->setVertexArray(verts);
	setTexCoordLimits(0, 512, 0, 512);
	geom->setColorArray(colors);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);
}

void CameraThatRendersAQuad::setTexCoordLimits(float x0, float x1, float y0, float y1)
{
	osg::Vec2Array* texes = new osg::Vec2Array;
	texes->push_back(osg::Vec2(x0, y0));
	texes->push_back(osg::Vec2(x1, y0));
	texes->push_back(osg::Vec2(x1, y1));
	texes->push_back(osg::Vec2(x0, y1));
	((osg::Geometry*)(mGeode->getDrawable(0)))->setTexCoordArray(0, texes);

}

void CameraThatRendersAQuad::setTexture(osg::Texture* tex)
{
	bool rect = (std::string(tex->className()) == std::string("TextureRectangle"));
	//if(rect) printf("rectangle texture!\n");
	if(!rect)
		setTexCoordLimits(0, 1, 0, 1);
	else
		setTexCoordLimits(0, tex->getTextureWidth(), 0, tex->getTextureHeight());
	//printf("setting txture size to %i, %i\n", tex->getTextureWidth(), tex->getTextureHeight());
	getOrCreateStateSet()->setTextureAttributeAndModes(0, tex, osg::StateAttribute::ON);
}
