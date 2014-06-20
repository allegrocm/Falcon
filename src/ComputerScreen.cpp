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
#include "Defaults.h"
#include "ROM.h"

#ifdef KENS_MIDI_CONTROLLER
	#include "magicjoystick.h"
#endif


using namespace osg;
float gScreenAspect = 1.75;
ComputerScreen::ComputerScreen()
{
	//add some test events
	MenuPrompt ex;
	ex.buttonText[1] = "EM Escape";
	ex.buttonEvent[1] = "runAway";
	mMenus["Begin"] = ex;

	MenuPrompt ex1;
	ex1.text = "Hit CONFIRM to run away!";
	ex1.buttonText[0] = "CONFIRM";
	ex1.buttonEvent[0] = "confirmRunAway";
	ex1.buttonText[2] = "CANCEL";
	ex1.buttonEvent[2] = "cancelRunAway";

	mMenus["runAway"] = ex1;


	getDefault("screenAspect", gScreenAspect);
	mPendingEvent = "";
	//init our layouts with default values
	for(int i = 0; i < 2; i++)
	{
		mLayout[i].textPosX = -0.6;
		mLayout[i].textPosY = 0.35;
		mLayout[i].textSize = 0.1;
		mLayout[i].healthImageSize = 0.7;
		mLayout[i].healthImageX = 0.3 * gScreenAspect;
		mLayout[i].healthImageY = -0.1;
		mLayout[i].radarSize = 0.5;
		mLayout[i].radarX = 0;
		mLayout[i].radarY = -0.2;
		mLayout[i].buttonTextSize = 0.04;
		mLayout[i].buttonsX = -.55;
		mLayout[i].buttonsY = -.3;
		mLayout[i].buttonsDX = 0.1;
		mLayout[i].buttonsDY = 0.075;
		mLayout[i].buttonCircleSize = 0.1;
		getDefault("textPosX", mLayout[i].textPosX);
		getDefault("textPosY", mLayout[i].textPosY);
		getDefault("textSize", mLayout[i].textSize);
	
		
		getDefault("damageIndicatorSize", mLayout[i].healthImageSize);
		getDefault("damageIndicatorX", mLayout[i].healthImageX);
		getDefault("damageIndicatorY", mLayout[i].healthImageY);
	
		getDefault("radarSize", mLayout[i].radarSize);
		getDefault("radarX", mLayout[i].radarX);
		getDefault("radarY", mLayout[i].radarY);
		
		
		getDefault("buttonTextSize", mLayout[i].buttonTextSize);
		getDefault("buttonsX", mLayout[i].buttonsX);
		getDefault("buttonsY", mLayout[i].buttonsY);
		getDefault("buttonsDX", mLayout[i].buttonsDX);
		getDefault("buttonsDY", mLayout[i].buttonsDY);
		
		getDefault("buttonCircleSize", mLayout[i].buttonCircleSize);

	}
	
	//load the secondary (up) layout
	
	getDefault("radarSizeUp", mLayout[1].radarSize);
	getDefault("textPosXUp", mLayout[1].textPosX);
	getDefault("textPosYUp", mLayout[1].textPosY);
	getDefault("textSizeUp", mLayout[1].textSize);


	getDefault("damageIndicatorSizeUp", mLayout[1].healthImageSize);
	getDefault("damageIndicatorXUp", mLayout[1].healthImageX);
	getDefault("damageIndicatorYUp", mLayout[1].healthImageY);

	getDefault("radarSizeUp", mLayout[1].radarSize);
	getDefault("radarXUp", mLayout[1].radarX);
	getDefault("radarYUp", mLayout[1].radarY);

	float buttonScale = 1.2;
	getDefault("buttonsUpScale", buttonScale);
	mLayout[1].buttonCircleSize *= buttonScale;
	mLayout[1].buttonsDX *= buttonScale;
	mLayout[1].buttonsDY *= buttonScale;
	mLayout[1].buttonTextSize *= buttonScale;
	getDefault("buttonsXUp", mLayout[1].buttonsX);
	getDefault("buttonsYUp", mLayout[1].buttonsY);

	
	mUp = true;
	mUpness = 0.0;
	setGlows(false);


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

	mStatusText->setColor(Vec4(0.7, .7, 1, 1.0));

	mStatusText->setMaximumWidth(gScreenAspect);
	mStatusText->setAlignment(osgText::TextBase::LEFT_TOP);
	mStatusText->setText("Hi Ken");

	std::string where = osgDB::findDataFile("../Models/tarzeau_ocr_a.ttf");
//	std::string where = "../Models/arial.ttf";
	mStatusText->setFont(where.c_str());
	mScoreText = (osgText::Text*)mStatusText->clone(CopyOp::SHALLOW_COPY);		//copy the parameters for the status text

	
	Geode* textGeode = new Geode();

	//add our text displays to this geode
	textGeode->addDrawable(mStatusText);
	textGeode->addDrawable(mScoreText);
	mCamera->addChild(textGeode);
	
	//add a little ornament
	ScreenImage* image = new ScreenImage();
	mCamera->addChild(image->transform);
	image->setImage("data/textures/curve.png");
	image->setHeight(1);
	image->setAspect(gScreenAspect);
	image->setColor(Vec4(0.7, 0.7, 1.0, 1));
	image->setTopLeft(Vec3(-.5 * gScreenAspect + 0.01, 0.49, 0));

	//add the Falcon diagram
	mHealthImage = new ScreenImage();
	mCamera->addChild(mHealthImage->transform);
	mHealthImage->setImage("data/textures/falconOutlineInverted.png");

	mHealthImage->transform->getOrCreateStateSet()->setAttribute(new BlendFunc(GL_SRC_ALPHA, GL_ONE));
	mHealthImage->setColor(Vec4(0.7, 0.7, 1.0, 0.75));


	
	mRadar = new RadarScreen();
	mCamera->addChild(mRadar->transform);

	for(int i = 0; i < 4; i++)		//set up button indicators
	{
		mButtonText[i] = new osgText::Text();
		mButtonText[i]->setColor(Vec4(.7, .7, 1, 1));
		mButtonText[i]->setAlignment(osgText::TextBase::CENTER_CENTER);
		mButtonText[i]->setText(Util::stringWithFormat("Button%i", i+1));
		mButtonText[i]->setFont(where.c_str());
		textGeode->addDrawable(mButtonText[i]);
		mButtonTimer[i] = 10;
		

	}
	


	for(int i = 0; i < 4; i++)
	{
		//add circles behind the buttons
		ScreenImage* image = new ScreenImage();
		mButtonCircle[i] = image;
		mCamera->addChild(mButtonCircle[i]->transform);
		mButtonCircle[i]->setImage("data/textures/Circle.png");
		mButtonCircle[i]->setColor(Vec4(0.7, 0.7, .7, 1));

		mButtonCircle[i]->setPos(mButtonText[i]->getPosition());

	}
	
}

void ComputerScreen::makeScreenGeometry()
{
	osg::Vec3Array* verts = new osg::Vec3Array;
	float hite = 1.0;
	getDefault("screenSize", hite);
	float width = gScreenAspect * hite;
	float height = hite;
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

	//attach a texture so the screen displays what the camera sees
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
			
				if(mNextMenu.buttonText[i] != "")
				{
					mButtonText[i]->setText(mNextMenu.buttonText[i]);
					mCurrentMenu.buttonText[i] = mNextMenu.buttonText[i];
					mCurrentMenu.buttonEvent[i] = mNextMenu.buttonEvent[i];
					
					mNextMenu.buttonText[i] = "";						//clear the "next text" so we don't switch it next time it's pressed
				}
				else
				{
//					printf("Empty event on a button!\n");
				}
				
				if(mPendingEvent != "")
				{
					doEvent(mPendingEvent);
					mPendingEvent = "";
				}

				
			}
			
			t = 1.0;
		}
		
		//slowly flash buttons that have text on them
		mButtonText[i]->setColor(color2 * (1.0-t) + color1 * t);
		float pulseTime = 0.5 + 0.5 * sinf(mAge * 4.0);
		if(!mUp) pulseTime = 0;			//no flashing if the screen is down
		std::string buttonText = mButtonText[i]->getText().createUTF8EncodedString();
		if(buttonText == "")
			pulseTime = 0;
			
		float brite = 0.75 + 0.25 * pulseTime;
		mButtonCircle[i]->setColor(Vec4(1, 1, 1, brite));
		Vec4 c = mButtonText[i]->getColor();
		c.w() = brite;
		mButtonText[i]->setColor(c);
		
	}
	
	
	//update color for the health readout
	int hp = GameController::instance().getStats().health;
	int maxHP = GameController::instance().getStats().maxHealth;
	float percent = 1.0 * hp / maxHP;
	
	Vec4 healthColor(0, 1, 0, 1);

	//start off green, fade to yellow, then red
	if(percent > 0.5)
	{
		healthColor.x() = (1.0 - percent) * 2.0;
	}
	else
	{
		healthColor.x() = 1.0;
		healthColor.y() = 2.0 * percent;
	}
	
	if(percent < 0.25)		//getting bad.  start pulsing or something
	{
		mPulseTime += dt;
		float pulse = 0.75 + 0.25 * cosf(mPulseTime * 12.0);
		healthColor.x() *= pulse;
		healthColor.y() *= pulse;
	}
	else mPulseTime = 0;
//	printf("Health:  %i/%i (%.2f).  (%.2f, %.2f)\n", hp, maxHP, percent, healthColor.x(), healthColor.y());
	
	mHealthImage->setColor(healthColor);
	
	
	//finally, update position based on the want matrix
	
//	mUpness = MIDIControl::slider(1);
	float posUpness = mUpness;		//the upness we'll use when positioning the HUD
	if(mUp != mUpness)
	{

		mUpness += dt * (mUp ? 1.0 : -1.0);
		if(mUpness > 1) mUpness = 1;
		if(mUpness < 0) mUpness = 0;
		
		float hudUpness = mUpness;			//upness for arranging for the layout
//		if(mUp)		//layout changes, then position
		{
			hudUpness = 2.5 * mUpness;
			posUpness = mUpness * 2.5 - 1.5;

		}
//		else
//		{
//			hudUpness = 1.0 - 2.5 * mUpness;
//			posUpness = 1.5 - mUpness * 2.5;
//		}
//		printf("HUD upness now %.2f\n", mUpness);
		updateLayout(hudUpness);
		
		if(posUpness > 1) posUpness = 1;
		if(posUpness < 0) posUpness = 0;
//		printf("update position:  %.2f\n", posUpness);

	}
	
//	posUpness = MIDIControl::slider(1);
//	updateLayout(MIDIControl::slider(2));

	float scale = 1.0 +  posUpness;
	mPat->setScale(Vec3(scale, scale, scale));


	Matrix mat = FalconApp::instance().getWandMatrix();
	
	Vec3 wandX(mat.ptr()[0], mat.ptr()[1], mat.ptr()[2]);
	Vec3 wandY(mat.ptr()[4], mat.ptr()[5], mat.ptr()[6]);
	Vec3 wandZ(mat.ptr()[8], mat.ptr()[9], mat.ptr()[10]);
	wandX.y() = 0;
	wandX.normalize();
	wandZ.y() = 0;
	wandZ.normalize();
	
	wandY = Vec3(0, 1, 0);
	//create an un-pitched version of the wand matrix
	Matrix flatWand = mat;
	float pitchEffect = 1.0 - posUpness;

	//don't take wand pitch into account UNLESS we're pointing downward
	if(mat.ptr()[9] < 0)
		pitchEffect = 0;

	for(int i = 0; i < 3; i++)
	{
		flatWand.ptr()[i] += (wandX.ptr()[i] - flatWand.ptr()[i])*(1.0-pitchEffect);
		flatWand.ptr()[i+4] += (wandY.ptr()[i] - flatWand.ptr()[i+4])* (1.0 -pitchEffect);
		flatWand.ptr()[i+8] += (wandZ.ptr()[i] - flatWand.ptr()[i+8])* (1.0 -pitchEffect);
	}
	
	//the height of the HUD is constant if it's "up"
//	for(int i = 0; i < 3; i++)
	{
		flatWand.ptr()[13] = flatWand.ptr()[13]*(1.0-posUpness) + 2.25 * posUpness;
		float fwdAmount = 1.0 * posUpness;
		for(int i = 0; i < 3; i++)
			flatWand.ptr()[12+i] += fwdAmount * wandZ.ptr()[i] * -1;
	}
	
	Matrix newScreen = ROM::SCREEN_OFFSET * flatWand;
	
	
	setTransform(newScreen);
	


	return true;
}

void ComputerScreen::setStatusText(std::string t)
{
	mStatusText->setText(t.c_str());
}

void ComputerScreen::buttonPressed(int which)
{
	printf("Pressed button %i (%s)!\n", which, mCurrentMenu.buttonText[which].c_str());
	if(which >= 0 && which < 4)
		mButtonTimer[which] = 0;
	if(mPendingEvent == "")
		mPendingEvent = mCurrentMenu.buttonEvent[which];
}


void ComputerScreen::setButtonText(int which, std::string text, std::string event)
{
	if(event == "") event = text;
	mCurrentMenu.buttonText[which] = text;
	mCurrentMenu.buttonEvent[which] = event;
	mButtonText[which]->setText(text);
}

void ComputerScreen::setButtonChangeText(int which, std::string text, std::string event)
{
	if(event == "") event = text;
	if(text == "") text = " ";		//empty strings are ignored when the button changes, so put a space in
	mNextMenu.buttonText[which] = text;
	mNextMenu.buttonEvent[which] = event;
}
void ComputerScreen::updateLayout(float upness)
{
	if(upness > 1) upness = 1;
	if(upness < 0) upness = 0;
//	printf("layout upness:  %.2f\n", upness);
#define lurp(name) (t1*l1. name + t2*l2. name)		//shortcut macro to interpolate between the two states
	//interpolation values
	float t1 = 1.0 -upness;
	float t2 = upness;
	HUDLayout l1 = mLayout[0];
	HUDLayout l2 = mLayout[1];
	//HERE IT IS
	mStatusText->setCharacterSize(lurp(textSize));
	mStatusText->setPosition(Vec3(lurp(textPosX), lurp(textPosY), 0));
	mScoreText->setPosition(Vec3(lurp(textPosX), lurp(textPosY) - 1.25 * lurp(textSize), 0));
	mHealthImage->setHeight(lurp(healthImageSize));
	
	mHealthImage->setPos(Vec3(lurp(healthImageX), lurp(healthImageY), 0));
	mRadar->setHeight(lurp(radarSize));
	mRadar->setPos(Vec3(lurp(radarX), lurp(radarY), 0));

	
	//position the button texts in a plus layout
	float buttonX = lurp(buttonsX);
	float buttonY = lurp(buttonsY);
	float buttonDX = lurp(buttonsDX);
	float buttonDY = lurp(buttonsDY);
//	buttonX = 0;
//	buttonY = 0;
//	printf("BX:  %.2f, BY:  %.2f.  DX:  %.2f.  DY:  %.2f\n", buttonX, buttonY, buttonDX, buttonDY);
	mButtonText[0]->setPosition(Vec3(buttonX-buttonDX, buttonY, 0));
	mButtonText[1]->setPosition(Vec3(buttonX, buttonY-buttonDY, 0));
	mButtonText[2]->setPosition(Vec3(buttonX+buttonDX, buttonY, 0));
	mButtonText[3]->setPosition(Vec3(buttonX, buttonY+buttonDY, 0));
	
	for(int i = 0; i < 4; i++)
	{
		mButtonCircle[i]->setHeight(lurp(buttonCircleSize));
		mButtonText[i]->setCharacterSize(lurp(buttonTextSize));
		mButtonCircle[i]->setPos(mButtonText[i]->getPosition());
	}
	
			
#undef lurp
}

void ComputerScreen::doEvent(std::string event)
{
	printf("Computer Event:  %s\n", event.c_str());
	
	//does this event correspond to one of our built-in menus?
	if(mMenus.find(event) != mMenus.end())
	{
		MenuPrompt p = mMenus[event];
		if(p.text != "")
			setStatusText(p.text);
		for(int i = 0; i < 4; i++)
		{
			setButtonText(i, p.buttonText[i], p.buttonEvent[i]);
			
		}
	}
	else printf("No new menu for this event\n");
}


