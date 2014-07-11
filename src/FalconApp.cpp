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
#include <stdarg.h>


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
#include "EventAudio.h"
#include "RadarScreen.h"
#include "TIEComputer.h"
#include "ShaderManager.h"
#include "Hyperspace.h"
#include "StarDestroyer.h"
#include <osgDB/WriteFile>
#include <osgUtil/IntersectVisitor>

using namespace osg;


void FalconApp::init()
{
	srand(65);
	PROFILER.init();		//init profiling
	__FUNCTION_HEADER__
	ROM::load();			//load our "ROM" right off the bat so we have access to its data
	mEventAudioManager = new EventAudio();
	mTimeStep = 0.01;
	mGameController = new GameController();
	
	//set up our OSGDB search paths
	osgDB::getDataFilePathList().push_back("data");
	osgDB::getDataFilePathList().push_back("data/models");
	osgDB::getDataFilePathList().push_back("data/shaders");
	
	mTotalTime = 0;
	mRoot = new osg::Group;
	
	//put our non-far-background stuff into a camera that only clears the depth buffa
	Camera* nearCam = new Camera();
	nearCam->setClearMask(GL_DEPTH_BUFFER_BIT);
	mNavigation = new osg::MatrixTransform;
	nearCam->addChild(mNavigation);
	mRoot->addChild(nearCam);
	mRoot->getOrCreateStateSet()->setMode(GL_NORMALIZE, true);
	mAvgFrameRate = 30;
	mModelGroup = new osg::Group;
	mNavigation->addChild(mModelGroup.get());
	mScreen = new ComputerScreen();
	mTIEScreen = new TIEComputer();
	
	
	mSPOffset = 0;
	//quickly add a lil spacebox
	mSpaceBox = new SpaceBox();
// 	mModelGroup->addChild(mSpaceBox->getRoot());
	mRoot->addChild(mSpaceBox->getRoot());
	mLightSource = new osg::LightSource;
	mNavigation->addChild(mLightSource.get());

	osg::Light* light = mLightSource->getLight();
	light->setDiffuse(osg::Vec4(0.7f, 0.7f, 0.7f, 1.0f));
	light->setSpecular(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	light->setAmbient(osg::Vec4(0.25f, 0.25f, .25, 1.0f));
	light->setPosition(osg::Vec4(100.0f, 100.0f, 100.0f, 0.0f));

	//have to add these to the spacebox too for the planets to light properly
	mSpaceBox->getRoot()->addChild(mLightSource);

	mLightSource = new osg::LightSource;
	mNavigation->addChild(mLightSource.get());
	mNavigation->getOrCreateStateSet()->setMode(GL_LIGHT1, true);
	light = mLightSource->getLight();
	light->setLightNum(1);
	light->setDiffuse(osg::Vec4(0.3f, 0.3f, 0.3f, 1.0f));
	light->setSpecular(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	light->setAmbient(osg::Vec4(0.1f, 0.1f, 0.1f, 1.0f));
	light->setPosition(osg::Vec4(-10.0f, -100.0f, 0.0f, 0.0f));

	//have to add these to the spacebox too for the planets to light properly
	mSpaceBox->getRoot()->addChild(mLightSource);
	
	mWandXForm = new osg::MatrixTransform;
	mModelGroup->addChild(mWandXForm);

	mFalcon = new Falcon();
	mEnemyController = new EnemyController(mTieNode1);


	mModelGroup->addChild(mFalcon->getRoot());

	mModelGroup->addChild(mScreen->getRoot());
//	mFalcon->getAimedPart()->addChild(mScreen->getRoot());
	mParticleFX = new ParticleFX();
	mModelGroup->addChild(mParticleFX->getRoot());

//todo: hack:  no bloom temporarily
	bool bloom = true;
	mBloom = new BloomController();
	if(bloom)
	{

		mRoot->addChild(mBloom->getRoot());
		mBloom->getScene()->addChild(mNavigation);
	}
//	else
//		mRoot->addChild(mNavigation);
	
	
	//audio only plays on the master and tie fighter nodes
	mEventAudioManager->setDontPlay(!(mIsMaster || mTieNode1));
	
	//now set up the TIE fighter's radar
	mTIEDisplayGroup = new Group();
	mTIEDisplayGroup->addChild(mTIEScreen->getCamera());

	CameraThatRendersAQuad* qrc = new CameraThatRendersAQuad();
	qrc->setTexture(mTIEScreen->getCamera()->getTargetTexture(0));
	mTIEDisplayGroup->addChild(qrc);
	
	//HACK:  for temps, put the TIE radar on the main screen
#if(0)
#ifndef USE_VRJ
	qrc = new CameraThatRendersAQuad();
	qrc->setTexture(mTIEScreen->getCamera()->getTargetTexture(0));
	qrc->setViewport(10, 10, 480 * 1.6, 480);
	mRoot->addChild(qrc);
	mRoot->addChild(mTIEScreen->getCamera());
#endif
#endif
	mEventAudioManager->setListener(mTieNode1 ? "Vader" : "Falcon");
	printf("eventaudio listener: %s\n", mTieNode1 ? "Vader" : "Falcon");
	//add some uniforms for use with per-pixel lighting shaders
	mModelGroup->getOrCreateStateSet()->addUniform(new Uniform("tex0", 0));
	mModelGroup->getOrCreateStateSet()->addUniform(new Uniform("tex1", 1));
	//add a default white texture for things that don't have a texture
	Texture2D* t = new Texture2D(Util::loadImage("data/textures/white.png"));
	t->setFilter(Texture::MAG_FILTER, Texture::NEAREST);
	t->setFilter(Texture::MIN_FILTER, Texture::NEAREST);
	mModelGroup->getOrCreateStateSet()->setTextureAttribute(0, t);
	toggleShaders();

	//MatrixTransform* sd = Util::loadModel("data/models/SD.ive", 2.0, 0, -90);
	//mRoot->addChild(sd);
	//osgDB::writeNodeFile(*sd, "SD.ive");

	//mStarDestroyer = new StarDestroyer();
//	mStarDestroyer->setPos(Vec3(0, -500, 0));
	//addThis(mStarDestroyer);
}


void FalconApp::buttonInput(unsigned int button, bool pressed)
{
	//printf("Button %i: %i\n", button, pressed);
	if(button < NUMBUTTONS)
	{
		if(pressed && (mButtons[button] == TOGGLE_OFF || mButtons[button] == OFF))
			mButtons[button] = TOGGLE_ON;
		if(!pressed && (mButtons[button] == TOGGLE_ON || mButtons[button] == ON))
			mButtons[button] = TOGGLE_OFF;

	
	}
	
	if(mButtons[3] == TOGGLE_ON)
		mSpaceBox->nextSystem();

}

void FalconApp::update(float fulldt)
{
	__FUNCTION_HEADER__
	updateFrameRate(fulldt);

	mTargetTime += fulldt;
	while(mTotalTime < mTargetTime)
	{
	
		mTotalTime += mTimeStep;
		
		
		//buttons 0, 5, 6, and 7 all shoot
		if(mButtons[0] == TOGGLE_ON || mButtons[5] == TOGGLE_ON || mButtons[6] == TOGGLE_ON || mButtons[7] == TOGGLE_ON)
		{
			//fire!
			mFalcon->shoot();
		//	printf("BAM!\n");
		}

		mScreen->update(mTimeStep);
		mTIEScreen->update(mTimeStep);
		mParticleFX->update(mTimeStep);
		mFalcon->update(mTimeStep);
		mEnemyController->update(mTimeStep);
		mGameController->update(mTimeStep);
		EnemyPlayer* player = mEnemyController->getPlayer();
		mListenerVelocity = Vec3();			//zero by default
		
		
		//the falcon moves with its hyperspace thingy
		mFalcon->setPos(Vec3(0, 0, -mFalcon->getHyperspace()->getZ()));
		
		//updating navigation for the TIE fighter

		//if the TIE fighter has won and the falcon is jumping out, do a back-view of it
		if(mFalcon->getHyperspace()->done() == false && mTieNode1)
		{
			Matrix nav;
			Vec3 viewPos(20, 40, 100);
			Vec3 z(viewPos -mFalcon->getPos());
			z.normalize();
			float HSTime = mFalcon->getHyperspace()->getHSTime();
			
			//wait a few seconds before we start spinning
			float spinDelay = 5.0;
			float spin = HSTime-spinDelay;
			if(spin < 0) spin = 0;
			
			Vec3 up(0, 1, 0);
			Vec3 x = up ^ z;
			x.normalize();
			viewPos += z * 10 * HSTime;
			Util::setZAxis(nav, z);
			Util::setXAxis(nav, x);
			Util::setYAxis(nav, up);
			nav = nav * Matrix::rotate(spin, z);
			Util::setPos(nav, viewPos);
		
			nav.invert(nav);
			
			mNavigation->setMatrix(nav);
		}

		else if(player && player->getShip() && mTieNode1)
		{
			Spacecraft* enemy = player->getShip();

			Matrix nav = enemy->getTransform();

			float ahead = -50.3;
			float lower = 0;
			float right = 0.00;
#ifndef USE_VRJ
			ahead = -3;
			lower = -3;
#else
#warning look into TIE camera position
#endif
			for(int i = 0; i < 3; i++)
			{
				nav.ptr()[12+i] += nav.ptr()[8+i] * ahead + nav.ptr()[4+i] * lower + nav.ptr()[i]*right;
			}
			
			
			
			
			//Util::printMatrix(nav);
			nav.invert(nav);
			
			mNavigation->setMatrix(nav);
			mListenerVelocity = enemy->getVel();
		}
		else if (!mTieNode1)
		{
			Matrix nav = mFalcon->getTransform();
			nav.invert(nav);
			mNavigation->setMatrix(nav);

		}
		
		


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
		

		//the spacebox's position always follows the navigation
		//so we can never approach the actual objects in it
		Matrix nav = mNavigation->getMatrix();

		mSpaceBox->setNavMatrix(nav, mSPOffset);
//		nav.invert(nav);

//		mSpaceBox->setViewerPos(Util::pos(nav));
		
		//spacebox root navigates with our navigation, minus the position
		Matrix navRot = mNavigation->getMatrix();
		
		//
//		Vec3 navPos = Util::pos(navRot);
//		Util::setPos(navRot, Vec3());
//		Matrix navUnrot = navRot;
//		navRot.invert navRot;		//for undoing the navrot before applying nav to the neargroup
//		mSpaceBox->getRoot()->setMatrix(nav);
//		mSpaceBox->getNearGroup()->setMatrix((navPos + Vec3(0, 0, -mSPOffset));

		//fly planets toward us after we jump
		float SPSpeed = 100000;
		if(mSPOffset < 10000)
			SPSpeed = (mSPOffset / 10000 * 0.9 + 0.1) * SPSpeed;
		mSPOffset -= SPSpeed * mTimeStep;
		if(mSPOffset < 0) mSPOffset = 0;

		
		//neither the TIE fighter nor the MF need to change their audio in response to head movement
		Matrixf navf = mNavigation->getMatrix();
		
		KSoundManager::instance()->updateListener(mTimeStep, navf.ptr(),
			mListenerVelocity.x(), mListenerVelocity.y(), mListenerVelocity.z());
		mEventAudioManager->update(mTimeStep);


		
		deToggleButtons();			//it's important that this be called every frame
	}
	
	PROFILER.endCycle();		//needed for time profiling
}

void FalconApp::deToggleButtons()
{
	//remove any toggleness from the buttons.
	//this is important if we want to attach events to the moment a button is pressed
	for(int i = 0; i < NUMBUTTONS; i++)
		if(mButtons[i] == TOGGLE_ON)	mButtons[i] = ON;
		else if(mButtons[i] == TOGGLE_OFF) mButtons[i] = OFF;
}

//transform the head and wand matrix into navigation space (optional, depends on how you're using them)
void FalconApp::setHeadMatrix(osg::Matrixf mat)
{
	mHeadMatrix = mat*mNavigation->getInverseMatrix();
	mHeadMatrix = mat;

}

void FalconApp::setWandMatrix(osg::Matrixf mat)
{
//	mWandMatrix = mat*mNavigation->getInverseMatrix();
	mWandMatrix = mat;
	mWandXForm->setMatrix(osg::Matrixf::scale(0.25, 0.125, 1.0)*mWandMatrix);
	
	//the computer screen will update on its own
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
	glColor3f(.6, .6, .9);
	int rowHeight = 20;
	int row = 2;
::Stats& stats = GameController::instance().getStats();	
	drawStringOnScreen(20, rowHeight*row++, "Frame Rate:  %.2f        Game Mode %i In the %s System (%i/%i)",
		mAvgFrameRate, mGameController->getMode(), mSpaceBox->getSystemName().c_str(), stats.health, stats.maxHealth);
	int active = mEnemyController->getShips().size();
	int left = mEnemyController->getShipsLeftToSpawn();
	drawStringOnScreen(20, rowHeight*row++, "Enemies:  %i/%i", active, left);
	drawStringOnScreen(20, rowHeight * row++, "Buttons:  %i, %i, %i, %i, %i, %i",
		mButtons[0], mButtons[1], mButtons[2], mButtons[3], mButtons[4], mButtons[5]);
	drawStringOnScreen(20, rowHeight * row++, "Head at:  %.2f, %.2f, %.2f",
		mHeadMatrix.ptr()[12], mHeadMatrix.ptr()[13], mHeadMatrix.ptr()[14]);
	drawStringOnScreen(20, rowHeight * row++, "Wand at:  %.2f, %.2f, %.2f",
		mWandMatrix.ptr()[12], mWandMatrix.ptr()[13], mWandMatrix.ptr()[14]);
	
	EnemyPlayer* player = mEnemyController->getPlayer();
	if(player)
	{
		EnemyControlInput input = player->getInput();
		drawStringOnScreen(20, rowHeight * row++, "Enemy Input:  %.2f, %.2f, %.2f (%i, %i, %i)", 
			input.xAxis, input.yAxis, input.thrustAxis, input.trigger, input.button1, input.button2);
	
	}
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
	printf("=====================Handling %i arguments===================\n", *argc);
	//like the 'i' below, but doesn't change when an arg is handled and is used for clear output in this function
	int num = 1;
	std::string tieNode = "";		//find which node is hosting the TIE fighter
	for(int i = 1; i < *argc; i++)
	{
	//	printf("Arg %i:  ", i);
		//printf("%s\n", argv[i]);
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
			printf("We are the master node!\n");
//			handled = true;
		}
		else if(KenXML::CICompare(arg, "--noTurret"))		//no lower turret allowed
		{
			mLowerTurretAllowed = false;
			handled = true;
			printf("Lower turret turned off\n");
		}
		else if(KenXML::CICompare(arg, "--tienode"))
		{
			if(i < *argc-1)
			{
				printf("TIE arg!\n");
				tieNode = argv[i+1];
				printf("TIE node:  %s\n", tieNode.c_str());
				//grab the argument, then pull it off the list
				i++;
				for(int j = i; j < *argc-1; j++)
				{
					argv[j] = argv[j+1];
				}
				i--;			//and go back an index so we'll look at the next argument
				(*argc)--;		//and decrement the number of args
				char* hn = getenv("HOSTNAME");
				if(!hn)
				{ 
					printf("Couldn't get hostname!\n");
					hn = getenv("HOST");
				}
				if(!hn)
				{ 
					printf("Couldn't get hostname again!\n");
					hn = getenv("REMOTEHOST");
				}
				if(!hn) printf("Couldn't get hostname at all!\n");
				
				if(hn)
				{
					std::string hostname = hn;
					printf("TIE node:  %s, we are %s\n", tieNode.c_str(), hostname.c_str());
					if(KenXML::CICompare(hostname, tieNode))
					{
						printf("We are the TIE fighter node!\n");
						mTieNode1 = true;
					}
				}
			}
			else
				Util::logError("TIE node arg found, but not hostname supplied\n");
			handled = true;
		
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
	StarDestroyer* sd = dynamic_cast<StarDestroyer*>(g);
	Bullet* bullet = dynamic_cast<Bullet*>(g);
	if(bullet)
	{
		mBullets.push_back(bullet);
		mModelGroup->addChild(bullet->getRoot());
		return true;
	}
	
	if(ship && !sd)
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
	glColor3f(0, 1, 0);
	glPushMatrix();
//		glMultMatrixf(mWandMatrix.ptr());
		glBegin(GL_LINES);

		glEnd();
	glPopMatrix();
	

	mEnemyController->drawDebug();
	
}

void FalconApp::toggleTIEMode()
{
	mTieNode1 = !mTieNode1;
	if(mEventAudioManager)
		mEventAudioManager->setListener(mTieNode1 ? "Vader" : "Falcon");
}

Spacecraft* FalconApp::getEnemyPlayerShip()
{
	EnemyPlayer* player = EnemyController::instance().getPlayer();
	if(!player) return NULL;		//no player?  we can't do anything
	Spacecraft* playerShip = player->getShip();
	if(!playerShip)
	{
		return NULL;
	}

	return playerShip;
	
}

void FalconApp::toggleShaders()
{
//	return;
	static bool shaders = false;
	shaders = !shaders;
	printf("shaders set to %i\n", shaders);
	osg::Program* shader = ShaderManager::loadShader("data/shaders/perVertexLighting");
	if(shaders)
		mFalcon->getRoot()->getOrCreateStateSet()->setAttributeAndModes( shader, osg::StateAttribute::ON);
	else
		mFalcon->getRoot()->getOrCreateStateSet()->removeAttribute(shader);

}

void FalconApp::switchSystem()
{
	mSPOffset = 100000;
	mSpaceBox->nextSystem();
	//mStarDestroyer->warp();
}

Matrix FalconApp::getPotentialSpawnPosition(bool nearShip)
{
	MatrixTransform* capitalShip = mSpaceBox->getCapitalShip();
	if(mFalcon->getHyperspace()->done() == false)
	{
		printf("Spawning ship during hyperspace jump!  Won't use the local star system for positioning\n");
		capitalShip = NULL;
	}
	
	//if there are no capital ships around, make a random spawn position
	if(!nearShip || !capitalShip)
	{
		Matrix mat;
		
		//generate a position
		float theta = Util::random(0, 6.28);
		float phi = Util::random(0.1, 1.0);
		float r = 900;
		Vec3 pos = Vec3(sinf(theta)*cosf(phi) * r, sinf(phi)*r, cosf(theta)*cosf(phi)*r);
		
		//and construct a coordinate system around it
		Vec3 forward = Util::vectorInCone(pos * -1.0, 20, 10);
		Vec3 up(0, 1, 0);
		Vec3 x = forward ^ up;
		up = x ^ forward;
		up.normalize();
		x.normalize();
		forward.normalize();
		Util::setPos(mat, pos);
		Util::setXAxis(mat, x);
		Util::setYAxis(mat, up);
		Util::setZAxis(mat, forward*-1);

		return mat;

	}
	
	//there's a capital ship, so let's spawn near it!
	Matrix mat = capitalShip->getMatrix();
	int tries = 5;		//let's try a few times to get a position near it but not in it
	Vec3 spawnPos = Util::pos(mat);
	printf("Using capital ship at %.2f, %.2f, %.2f for spawn origin\n", spawnPos.x(), spawnPos.y(), spawnPos.z());
	while(tries)
	{
		
		osgUtil::IntersectVisitor iv;
		//make a line segment representing the laser beam
		Vec3 dir = spawnPos * -1.0;
		dir.normalize();
		dir = Util::vectorInCone(dir, 90);		//make sure the TIE fighters are always on the falcon side of the ship
		printf("dir:  %.2f, %.2f, %.2f\n", dir.x(), dir.y(), dir.z());
		ref_ptr<LineSegment> seg = new LineSegment(spawnPos, spawnPos+dir * 1000);

		iv.addLineSegment(seg.get());
	//	printf("Seg:  %.2f, %.2f, %.2f\n", pos.x(), pos.y(), pos.z());
		capitalShip->accept(iv);
		osgUtil::IntersectVisitor::HitList& hitList = iv.getHitList(seg.get());
		if(hitList.size())		//if there's any size in the hitlist, we HIT something!
		{
			spawnPos = hitList.back().getWorldIntersectPoint() + dir * Util::random(100, 200);
			printf("%i hits!  Found a spawn pos at %.2f, %.2f, %.2f\n",(int)hitList.size(), spawnPos.x(), spawnPos.y(), spawnPos.z());
			tries = 0;
		}
//		spawnPos = spawnPos + dir * 50;
		
	}
	
	//good to go!
	Util::setPos(mat, spawnPos);

	return mat;
	
	

}

