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
float gScreenAspect = 1.75;
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
	mStatusText->setCharacterSize(0.075);
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
	image->setImage("data/textures/curve.png");
	image->setHeight(.95);
	image->setAspect(gScreenAspect);
	image->setColor(Vec4(0.7, 0.7, 1.0, 1));
	image->setTopLeft(Vec3(-.5 * gScreenAspect + 0.05, 0.45, 0));
	
	mRadar = new RadarScreen();
	mCamera->addChild(mRadar->transform);
	mRadar->setHeight(0.5);
	mRadar->setPos(Vec3(-.2, -.2, 0));
	
	for(int i = 0; i < 4; i++)		//set up button indicators
	{
		mButtonText[i] = new osgText::Text();
		mButtonText[i]->setCharacterSize(0.05);
		mButtonText[i]->setColor(Vec4(.7, .7, 1, 1));
		mButtonText[i]->setAlignment(osgText::TextBase::CENTER_CENTER);
		mButtonText[i]->setText(Util::stringWithFormat("Button%i", i+1));
		mButtonText[i]->setFont(where.c_str());
		textGeode->addDrawable(mButtonText[i]);
		mButtonTimer[i] = 10;
	}
	

	
	//position the button texts in a plus layout
	float buttonX = 0.5;
	float buttonY = -0.3;
	float buttonDX = 0.2;
	float buttonDY = 0.1;
	mButtonText[0]->setPosition(Vec3(buttonX-buttonDX, buttonY, 0));
	mButtonText[1]->setPosition(Vec3(buttonX, buttonY-buttonDY, 0));
	mButtonText[2]->setPosition(Vec3(buttonX+buttonDX, buttonY, 0));
	mButtonText[3]->setPosition(Vec3(buttonX, buttonY+buttonDY, 0));

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
//	mStatusText->setText(Util::stringWithFormat("Time:  %2.2f", mAge).c_str());
	mScoreText->setText(Util::stringWithFormat("Score:  %i", GameController::instance().getStats().score).c_str());
	mRadar->update(dt);
	for(int i = 0; i < 4; i++)		//update buttons!
	{
		mButtonTimer[i] += dt;
		
		//if they've just been pressed, button flicker between two colors
		Vec4 color1 = Vec4(.7, .7, 1, 1);
		Vec4 color2 = Vec4(1, .2, .2, 1);
		float t = 0.5 + 0.5 * sinf(mButtonTimer[i] * 60.0);
		if(FalconApp::instance().getButton(i+1) == FalconApp::TOGGLE_ON)
			buttonPressed(i);
		float flashTime = 0.75;
		if(mButtonTimer[i] > flashTime)
		{
			if(mButtonTimer[i] - dt <= flashTime)		//did we just stop flashing?  might need to change the text
			{
			
				if(mNextText[i] != "")
				{
					mButtonText[i]->setText(mNextText[i]);
					mNextText[i] = "";						//clear the "next text" so we don't switch it next time it's pressed
				}
			}
			
			t = 1.0;
		}
		mButtonText[i]->setColor(color2 * (1.0-t) + color1 * t);
	}
	
	return true;
}

void ComputerScreen::setStatusText(std::string t)
{
	mStatusText->setText(t.c_str());
}

void ComputerScreen::buttonPressed(int which)
{
	printf("Pressed button %i!\n", which);
	if(which >= 0 && which < 4)
		mButtonTimer[which] = 0;
}

void ComputerScreen::setButtonText(int which, std::string text)
{
	mButtonText[which]->setText(text);
}

void ComputerScreen::setButtonChangeText(int which, std::string text)
{
	if(text == "") text = " ";		//empty strings are ignored when the button changes, so put a space in
	mNextText[which] = text;
}


