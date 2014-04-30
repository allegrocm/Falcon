/*
 *  FalconApp.cpp
 *  Hybrid OSG Template
 *
 *  Created by Kenneth Kopecky on 8/15/09.
 *  Copyright 2009 Orange Element Studios. All rights reserved.
 *
 */

#ifdef _WIN32
#include <windows.h>
#include <gl/glut.h>
#endif

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif
#include <osg/Geometry>
#include <osg/Geode>
#include <osgDB/FileUtils>



#include "FalconApp.h"
#include "ParticleFX.h"
#include "StupidPlaceholderShip.h"
#include "Bullet.h"
#include "Falcon.h"

#include "XML/KenXML.h"
#include "Util.h"
#include "SpaceBox.h"
#include "ROM.h"
#include "ComputerScreen.h"
#include "EnemyController.h"
#include "GameController.h"

using namespace osg;


void FalconApp::init()
{
	PROFILER.init();		//init profiling
	__FUNCTION_HEADER__
	ROM::load();			//load our "ROM" right off the bat so we have access to its data
	mTimeStep = 0.01;
	mGameController = new GameController();
	
	//set up our OSGDB search paths
	osgDB::getDataFilePathList().push_back("data");
	osgDB::getDataFilePathList().push_back("data/models");
	osgDB::getDataFilePathList().push_back("data/shaders");
	
	mTotalTime = 0;
	mRoot = new osg::Group;
	mNavigation = new osg::MatrixTransform;
	mRoot->addChild(mNavigation.get());
	mRoot->getOrCreateStateSet()->setMode(GL_NORMALIZE, true);
	mLightSource = new osg::LightSource;
	mNavigation->addChild(mLightSource.get());
	mAvgFrameRate = 30;
	mModelGroup = new osg::Group;
	mNavigation->addChild(mModelGroup.get());
	mScreen = new ComputerScreen();

	mScreen->setPos(Vec3(0, 0, 0));
	//quickly add a lil spacebox
 	mModelGroup->addChild((new SpaceBox())->getRoot());
	
	
	osg::Light* light = mLightSource->getLight();
	light->setDiffuse(osg::Vec4(0.7f, 0.7f, 0.7f, 1.0f));
	light->setSpecular(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	light->setAmbient(osg::Vec4(0.05f, 0.05f, 0.05f, 1.0f));
	light->setPosition(osg::Vec4(100.0f, 100.0f, 100.0f, 0.0f));

	mWandXForm = new osg::MatrixTransform;
	mModelGroup->addChild(mWandXForm);

	mEnemyController = new EnemyController;

	mFalcon = new Falcon();
	mModelGroup->addChild(mFalcon->getRoot());
	mFalcon->getAimedPart()->addChild(mScreen->getRoot());
	mParticleFX = new ParticleFX();
	mModelGroup->addChild(mParticleFX->getRoot());
}


void FalconApp::buttonInput(unsigned int button, bool pressed)
{
	if(button < NUMBUTTONS)
	{
		mButtons[button] = pressed ? TOGGLE_ON : TOGGLE_OFF;
	}

}

void FalconApp::update(float fulldt)
{
	__FUNCTION_HEADER__

	mTargetTime += fulldt;
	while(mTotalTime < mTargetTime)
	{
		KSoundManager::instance()->updateListener(mTimeStep, mHeadMatrix.ptr(), 0, 0, 0);
		mTotalTime += mTimeStep;
		//process navigation, etc
		if(mButtons[0] == TOGGLE_ON)
		{
			//fire!
			mFalcon->fire();
		//	printf("BAM!\n");
		}

		mScreen->update(mTimeStep);
		mParticleFX->update(mTimeStep);
		mFalcon->update(mTimeStep);
		mEnemyController->update(mTimeStep);
		mGameController->update(mTimeStep);
		//update bullets. when one is "finished", delete it
		for(size_t i = 0; i < mBullets.size(); i++)
		{
			if(!mBullets[i]->update(mTimeStep))
			{
				mModelGroup->removeChild(mBullets[i]->getRoot());
				delete mBullets[i];
				mBullets[i] = mBullets.back();
				mBullets.pop_back();
			}
		}
		
		
		//update all the other stuff
		for(size_t i = 0; i < mJunk.size(); i++)
		{
			if(!mJunk[i]->update(mTimeStep))
			{
				mModelGroup->removeChild(mJunk[i]->getRoot());
				delete mJunk[i];
				mJunk[i] = mJunk.back();
				mJunk.pop_back();
			}
		}
		
	
		
		deToggleButtons();			//it's important that this be called every frame
	}
	
	PROFILER.endCycle();		//needed for time profiling
}

void FalconApp::deToggleButtons()
{
	//remove any toggleness from the buttons.  this is important if we want to attach events to the moment a button is pressed
	for(int i = 0; i < NUMBUTTONS; i++)
		if(mButtons[i] == TOGGLE_ON)	mButtons[i] = ON;
		else if(mButtons[i] == TOGGLE_OFF) mButtons[i] = OFF;
}

//transform the head and wand matrix into navigation space (optional, depends on how you're using them)
void FalconApp::setHeadMatrix(osg::Matrixf mat)
{
	mHeadMatrix = mat*mNavigation->getInverseMatrix();

}

void FalconApp::setWandMatrix(osg::Matrixf mat)
{
	mWandMatrix = mat*mNavigation->getInverseMatrix();
	mWandXForm->setMatrix(osg::Matrixf::scale(0.25, 0.125, 1.0)*mWandMatrix);
}

void drawStringOnScreen(int x, int y, const char* format, ...)
{
	char stringData[2024];
	va_list args;
	va_start(args, format);
	vsnprintf(stringData, 2024, format, args);
	
	//find the viewport size so we can properly size everything
	int vport[4];
	glGetIntegerv(GL_VIEWPORT, vport);
	int screenWidth = vport[2];
	int screenHeight = vport[3];
	glPushMatrix();
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
			glLoadIdentity();
			glOrtho(0, screenWidth, 0, screenHeight, -1, 1);

			glDisable(GL_LIGHTING);
			glDisable(GL_TEXTURE_2D);
			glRasterPos3i(x, screenHeight - y, 0);
			
			for(int i=0; i<2024 && stringData[i] != 0; i++ )
			{
				glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, stringData[i] );
			}
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	glPopMatrix();


}


void FalconApp::drawStatus()
{	
	if(!mIsMaster) return;		//only the master node draws stats, or cluster nodes could break
	__FUNCTION_HEADER__
#ifdef USE_VRJ
	int argc = 3;
	//pass fake args to GLUT so it uses the right display
	char* argv[] = {(char*)"SampleApp", (char*)"-display", (char*)":0.0"};
	static bool once = true;
	if(once)
	{
		std::cout << "initing glut..." << std::endl;
		glutInit(&argc, argv);
		once = false;
	}

#endif
	glColor3f(1, 1, 1);
	int rowHeight = 20;
	int row = 2;

	drawStringOnScreen(20, rowHeight*row++, "Frame Rate:  %.2f", mAvgFrameRate);
	drawStringOnScreen(20, rowHeight * row++, "Buttons:  %i, %i, %i, %i, %i, %i", mButtons[0], mButtons[1], mButtons[2], mButtons[3], mButtons[4], mButtons[5]);
	drawStringOnScreen(20, rowHeight * row++, "Head at:  %.2f, %.2f, %.2f", mHeadMatrix.ptr()[12], mHeadMatrix.ptr()[13], mHeadMatrix.ptr()[14]);
	drawStringOnScreen(20, rowHeight * row++, "Wand at:  %.2f, %.2f, %.2f", mWandMatrix.ptr()[12], mWandMatrix.ptr()[13], mWandMatrix.ptr()[14]);

}

void FalconApp::updateFrameRate(float dt)
{
	if(dt > 0 && dt < 0.05)		//don't update our framerate for crazy-ass timesteps
	{
		static float tSinceUpdate = 0.0;
		static int framesSinceUpdate = 0;
		tSinceUpdate += dt;
		framesSinceUpdate++;
		if(tSinceUpdate > 0.5 && framesSinceUpdate > 5)		//update every half second or so
		{
			float newAvg = (float)framesSinceUpdate / tSinceUpdate;
			mAvgFrameRate += (newAvg - mAvgFrameRate) * 0.5;
			tSinceUpdate = 0;
			framesSinceUpdate = 0;
		}
	}
}

void FalconApp::handleArguments(int* argc, char **argv)
{
	int num = 1;		//like the 'i' below, but doesn't change when an arg is handled and is used for clear output in this function
	for(int i = 1; i < *argc; i++)
	{
		std::string arg(argv[i]);

		bool handled = false;
		if(KenXML::CICompare(arg, "--vrjslave"))
		{
			mIsMaster = false;
//			handled = true;
		}
		else if(KenXML::CICompare(arg, "--vrjmaster"))
		{
			mIsMaster = true;
//			handled = true;
		}

		printf("Arg %i:  %s (%s)\n", num++, arg.c_str(), handled ? "Handled" : "Not Handled");
		
		//pull it out of the list if we handled the argument
		if(handled)
		{
			for(int j = i; j < *argc-1; j++)
			{
				argv[j] = argv[j+1];
			}
			i--;			//and go back an index so we'll look at the next argument
			(*argc)--;		//and decrement the number of args
		}
	}

}

void FalconApp::shutdown()
{
	//print out our profiler summary and important errors
	std::string prof = PROFILER.getSummary();
	printf("__________________Final Profiler Summary:____________________  \n%s\n", prof.c_str());
	Util::printErrors();
}

bool FalconApp::addThis(GameObject* g)
{
	//we can find out what kind of object this is through dynamic casts
	Spacecraft* ship = dynamic_cast<Spacecraft*>(g);
	Bullet* bullet = dynamic_cast<Bullet*>(g);
	if(bullet)
	{
		mBullets.push_back(bullet);
		mModelGroup->addChild(bullet->getRoot());
		return true;
	}
	
	if(ship)
		mEnemyController->addShip(ship);
	
	if(g)
	{
//		printf("add some other object\n");
		mJunk.push_back(g);
		mModelGroup->addChild(g->getRoot());

	}
	return false;
}

void FalconApp::drawDebug()
{
	for(size_t i = 0; i < mJunk.size(); i++)
	{
		mJunk[i]->drawDebug();
	}
	
}

