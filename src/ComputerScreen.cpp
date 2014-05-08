//
//  ComputerScreen.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky on 4/14/14.
//
//

#include "Util.h"
#include "ComputerScreen.h"
#include "PrerenderCamera.h"
#include "FalconApp.h"
#include <osgDB/FileUtils>
#include <osg/BlendFunc>
#include "ScreenImage.h"
#include "RadarScreen.h"
#include "GameController.h"

using namespace osg;
float gScreenAspect = 1.25;
ComputerScreen::ComputerScreen()
{
	mCamera = new PrerenderCamera(1024, 1, GL_RGBA);
	mCamera->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	mPat->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
//	mPat->getOrCreateStateSet()->setAttribute(new BlendFunc(GL_SRC_ALPHA, GL_ONE));
	mCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	mCamera->setProjectionMatrix(osg::Matrix::ortho2D(-.5 * gScreenAspect,.5 * gScreenAspect,-.5,.5));
	mCamera->setClearColor(Vec4(.5, .0, .0, .5));
	mPat->getOrCreateStateSet()->setMode(GL_BLEND, true);
	mPat->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	FalconApp::instance().getRoot()->addChild(mCamera);
	makeScreenGeometry();
	
	//add some text
	mStatusText = new osgText::Text();
	mStatusText->setCharacterSize(0.1);
	mStatusText->setColor(Vec4(0.7, .7, 1, 1.0));

	
	mStatusText->setMaximumWidth(5.0);
	mStatusText->setAlignment(osgText::TextBase::LEFT_TOP);
	mStatusText->setText("Hi Ken");
	mStatusText->setPosition(Vec3(-.4, .3, 0));
	std::string where = osgDB::findDataFile("../Models/tarzeau_ocr_a.ttf");
//	std::string where = "../Models/arial.ttf";
	mStatusText->setFont(where.c_str());
	mScoreText = (osgText::Text*)mStatusText->clone(CopyOp::SHALLOW_COPY);		//copy the parameters for the status text
	mScoreText->setPosition(Vec3(-.4, .15, 0));
	
	Geode* textGeode = new Geode();

	//add our text displays to this geode
	textGeode->addDrawable(mStatusText);
	textGeode->addDrawable(mScoreText);
	mCamera->addChild(textGeode);
	ScreenImage* image = new ScreenImage();
	mCamera->addChild(image->transform);
	image->setImage("data/textures/RingSS.png");
	image->setHeight(0.25);
	image->setPos(Vec3(.7, 0.0, 0));
	image = new ScreenImage();
	mCamera->addChild(image->transform);
	image->setImage("data/textures/curve.png");
	image->setHeight(.95);
	image->setAspect(gScreenAspect);
	image->setColor(Vec4(0.7, 0.7, 1.0, 1));
	image->setTopLeft(Vec3(-.5 * gScreenAspect + 0.05, 0.45, 0));
	
	mRadar = new RadarScreen();
	mCamera->addChild(mRadar->transform);
	mRadar->setHeight(0.5);
	mRadar->setPos(Vec3(-.2, -.2, 0));
}

void ComputerScreen::makeScreenGeometry()
{
	osg::Vec3Array* verts = new osg::Vec3Array;
	
	float width = gScreenAspect;
	float height = 1.0;
	float x0 = -width*0.5;
	float y0 = -height*0.5;
	verts->push_back(osg::Vec3(x0, y0, 0));
	verts->push_back(osg::Vec3(x0 + width, y0, 0));
	verts->push_back(osg::Vec3(x0 + width, y0 + height, 0));
	verts->push_back(osg::Vec3(x0, y0 + height, 0));

	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(1, 1, 1, 1));
		
	osg::Geometry* geom = new osg::Geometry;
	Geode* screenGeode = new Geode();
	mPat->addChild(screenGeode);
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
	screenGeode->addDrawable(geom);

	geom->setVertexArray(verts);

	geom->setColorArray(colors);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);

	osg::Vec2Array* texes = new osg::Vec2Array;
	texes->push_back(osg::Vec2(0, 0));
	texes->push_back(osg::Vec2(1, 0));
	texes->push_back(osg::Vec2(1, 1));
	texes->push_back(osg::Vec2(0, 1));
	geom->setTexCoordArray(0, texes);
//	geom->setTexCoordArray(1, texes);			//set texcoord array 1 also, for showing high-score entry
	geom->getOrCreateStateSet()->setTextureAttributeAndModes(0, mCamera->getTargetTexture(0));


}

bool ComputerScreen::update(float dt)
{
	GameObject::update(dt);
	mStatusText->setText(Util::stringWithFormat("Time:  %2.2f", mAge).c_str());
	mScoreText->setText(Util::stringWithFormat("Score:  %i", GameController::instance().getStats().score).c_str());
	mRadar->update(dt);
	return true;
}

