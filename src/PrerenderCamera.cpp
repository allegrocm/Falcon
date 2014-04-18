/*
 *  PrerenderCamera.cpp
 *  OSGEnhancer
 *
 *  Created by Kenneth Kopecky on 11/4/09.
 *  Copyright 2009 Orange Element Studios. All rights reserved.
 *
 */

#include "PrerenderCamera.h"
#include <osg/Image>
#include <osg/Geometry>

using namespace osg;

PrerenderCamera::PrerenderCamera(int texSize, int targets, int format, bool useDepth)
{
	setName("PrerenderCamera");
	if(targets > 16) targets = 16;
	mNumTargets = targets;
	mTexSize = texSize;
	setViewport(0, 0, texSize, texSize);
	int internalFormat = format;
	for(int i = 0; i < targets; i++)
	{
		osg::Texture2D* tex = new Texture2D;
		tex->setName("TargetTexture");
		tex->setInternalFormat(internalFormat);
		tex->setSourceFormat(GL_RGBA);
		tex->setTextureSize(texSize, texSize);
		tex->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
		tex->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
		mTargetTexes[i] = tex;
		if(useDepth && i == 0)
		{
			attach(Camera::BufferComponent(Camera::DEPTH_BUFFER), tex);
		}
		else
			attach(Camera::BufferComponent(Camera::COLOR_BUFFER0+i - useDepth), tex);
	}
	setRenderOrder(osg::Camera::PRE_RENDER);
	setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
	setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setClearColor(osg::Vec4(0, 0, 0, 0));
}

void PrerenderCamera::setShader(osg::Program* shader)
{
	getOrCreateStateSet()->setAttribute(shader, true);
}

osg::Texture2D* PrerenderCamera::getTargetTexture(int t)
{
	return mTargetTexes[t].get();
}

void PrerenderCamera::addQuadGeometry()
{
	setClearColor(osg::Vec4(0, 0, 0, 1));
	getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	setProjectionMatrix(osg::Matrix::ortho2D(0,1,0,1));
	// only clear the depth buffer
	setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//make a geode that shows the texture
	osg::Geode* geode = new osg::Geode;
	geode->setName("QuadPreRenderCameraGeode");
	osg::Vec3Array* verts = new osg::Vec3Array;
	float x0 = 0;
	float y0 = 0;
	float width = 1;
	float height = 1;
	verts->push_back(osg::Vec3(x0, y0, 0));
	verts->push_back(osg::Vec3(x0 + width, y0, 0));
	verts->push_back(osg::Vec3(x0 + width, y0 + height, 0));
	verts->push_back(osg::Vec3(x0, y0 + height, 0));

	osg::Vec3Array* colors = new osg::Vec3Array;
	colors->push_back(osg::Vec3(1, 1, 1));

	osg::Geometry* geom = new osg::Geometry;
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
	geode->addDrawable(geom);
	addChild(geode);

	geom->setVertexArray(verts);
	osg::Vec2Array* texes = new osg::Vec2Array;
	texes->push_back(osg::Vec2(0, 0));
	texes->push_back(osg::Vec2(1, 0));
	texes->push_back(osg::Vec2(1, 1));
	texes->push_back(osg::Vec2(0, 1));
	geom->setTexCoordArray(0, texes);

	geom->setColorArray(colors);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);
}

void PrerenderCamera::addInputTexture(osg::Texture2D* tex, std::string uniformName, int texUnit)
{
	getOrCreateStateSet()->setTextureAttributeAndModes(texUnit, tex, true);
	getOrCreateStateSet()->addUniform(new osg::Uniform(uniformName.c_str(), texUnit), true);

}
