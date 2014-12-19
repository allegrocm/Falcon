#include "JugglerInterface.h"
#include "FalconApp.h"
#include "Util.h"
#include "EnemyController.h"
#include "CameraThatRendersAQuad.h"
#include "ComputerScreen.h"
#include "RadarScreen.h"

#include <vrj/Display/DisplayManager.h>
#include <vrj/Draw/OpenGL/DrawManager.h>
#include <vrj/Display/Display.h>
#include <vrj/Draw/OpenGL/Window.h>
#include "VRJSyncMonitor.h"

JugglerInterface::JugglerInterface(vrj::Kernel* kern, int& argc, char** argv) : vrj::OsgApp(kern)
{
	
}

JugglerInterface::~JugglerInterface()
{

}

void JugglerInterface::init()
{
	mHead.init("VJHead");		
	mWand.init("VJWand");

	mButton[0].init("VJButton0");
	mButton[1].init("VJButton1");
	mButton[2].init("VJButton2");
	mButton[3].init("VJButton3");
	mButton[4].init("VJButton4");
	mButton[5].init("VJButton5");
	//triggers map to buttons 0, 5, 6, 7
	mPadButtons[0].init("Button 6");
	mPadButtons[5].init("Button 7");
	mPadButtons[6].init("Button 8");
	mPadButtons[7].init("Button 5");
	mPadButtons[1].init("Button 3");		//gamepad is backwards for this one
	
	
	//init enemy controls
	for(int i = 0; i < 8; i++)
	{
		char name[1024];
		sprintf(name, "FlightStick1Button%i", i+1);
		mEnemyButtons[i].init(name);
	}
	
	mEnemyAxes[0].init("FlightStick1Axis1");
	mEnemyAxes[1].init("FlightStick1Axis2");
	mEnemyAxes[2].init("FlightStick1Axis3");
	
	 vrj::OsgApp::init();
	osg::setNotifyLevel(osg::FATAL);
	
	RNGSyncMonitor::init();
}

void JugglerInterface::initScene()
{
	printf("______initing scene_____\n");
	 FalconApp::instance().init();
	 _rootNode = FalconApp::instance().getRoot();
}

void JugglerInterface::preFrame()
{	
	
	RNGSyncMonitor::preFrame();
	//trigger handling
	
	//if(mButton1->getData() == gadget::Digital::TOGGLE_ON)
		//ShaderManager::instance().reloadShaders();
	
	
	//printf("preframe end\n");
}

void JugglerInterface::latePreFrame()
{
#ifdef RNG_LOGGING
	static int RNGFrame = 0;
	printf("LogRDM ===============================Preframe %06i=====================================================\n", RNGFrame++);
	RNGSyncMonitor::latePreFrame();
	if(RNGSyncMonitor::isSynced() == false)
	{
		FalconApp::instance().syncFaultHappened();
		fflush(stdout);
		::exit(0);		//quit so we kill the log now
	}
#endif
//	std::cout << "begin preframe" << std::endl;
	static double tLast = mHead->getTimeStamp().secd();
	double tNow = mHead->getTimeStamp().secd();
	double dt = tNow - tLast;
	tLast = tNow;
#ifdef RNG_LOGGING
	//dt = 0.05;
#endif
	//std::cout << "Timestamp:  " << tNow << std::endl;
	//pass changes in button state on to the app
	for(int i = 0; i < 8; i++)
	{
		//use gamepad buttons if they're available
		gadget::DigitalInterface* button = &mButton[i];
		if(mPadButtons[i]->isStupefied() == false)
			button = &mPadButtons[i];
		if((*button)->getData() == gadget::Digital::TOGGLE_ON)
			FalconApp::instance().buttonInput(i, true);
		else if((*button)->getData() == gadget::Digital::TOGGLE_OFF)
			FalconApp::instance().buttonInput(i, false);
		
	}
	
	//send enemy controls if possible
	EnemyControlInput ec;
	if(mEnemyAxes[0]->isStupefied() == false)
	{
		ec.xAxis = mEnemyAxes[0]->getData() * 2.0 - 1.0;
		ec.yAxis = mEnemyAxes[1]->getData() * 2.0 - 1.0;
		ec.thrustAxis = mEnemyAxes[2]->getData() * 2.0 - 1.0;
		ec.trigger = mEnemyButtons[0]->getData();
		ec.button1 = mEnemyButtons[1]->getData();
		ec.button2 = mEnemyButtons[2]->getData();
		//printf("Enemy input:  %.2f, %.2f, %.2f, %i\n", ec.xAxis, ec.yAxis, ec.thrustAxis, ec.trigger);
		EnemyController::instance().setEnemyInput(0, ec);
	}
	
	FalconApp::instance().update(dt);
	//attach the wand's matrix to the paint sprayer
	osg::Matrixf wandMatrix(mWand->getData().mData);
	FalconApp::instance().setWandMatrix(wandMatrix);
	osg::Matrixf headMatrix	(mHead->getData().mData);
	FalconApp::instance().setHeadMatrix(headMatrix);
	
		//KK:  on Mac, the first time this is called, the Kernel has zero users, and latePreFrame crashes. 
	//std::cout << "almost end preframe" << std::endl;
	if(mKernel->getUsers().size())
		vrj::osg::App::latePreFrame();
	//printf("end preframe\n");
	fflush(stdout);

}

void JugglerInterface::bufferPreDraw()
{
	glClearColor(0.5, 0.5, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	
	const osg::FrameStamp* frameStamp = (*sceneViewer)->getFrameStamp();
}

osg::Group* JugglerInterface::getScene()
{
	return _rootNode.get();
}


// Override OsgApp::configSceneView to add FrameStamp to SceneView.
// This helps animations and shaders work properly in OSG.
void JugglerInterface::configSceneView(osgUtil::SceneView* newSceneViewer)
{
	printf("----ConfigSceneView-----\n");
	static int times = 0;
	times++;
	
	//if this is the radar display, connect a different node to it
	__FUNCTION_HEADER__
	vrj::OsgApp::configSceneView(newSceneViewer);
	std::string windowName = "none yet";
	//if(vrj::opengl::DrawManager::instance()->currentUserData()->getViewport()->getDisplay())
		//windowName = vrj::opengl::DrawManager::instance()->currentUserData()->getViewport()->getDisplay()->getName();
	//printf("draw on window %s\n", windowName.c_str());

//	newSceneViewer->setClearColor(osg::Vec4(0.3f, 0.2f, 0.5f, 1.0f));
	printf("line %i\n", __LINE__);
	if(!newSceneViewer) return;
	osg::Camera* cam = newSceneViewer->getCamera();
	if(times == 2)
	{
		while(cam->getNumChildren())
		{
			printf("Remove a child from the camera\n");
			cam->removeChild(cam->getChild(0));
		}
	}
	printf("line %i:  camera is %p\n", __LINE__, cam);
	//set the cull mask of our camera so that we don't draw invisible hitboxes
	newSceneViewer->setCullMask((1 << NON_GLOW_LAYER) | (1 << GLOW_LAYER) | (1 << BACKGROUND_LAYER));
	printf("line %i\n", __LINE__);
}

void JugglerInterface::draw()
{
	//std::cout << "begin draw" << std::endl;
	vrj::OsgApp::draw();
	osg::ref_ptr<osgUtil::SceneView> sv = *sceneViewer;
	 std::string windowName = "none yet";
	if(vrj::opengl::DrawManager::instance()->currentUserData()->getViewport()->getDisplay())
		windowName = vrj::opengl::DrawManager::instance()->currentUserData()->getViewport()->getDisplay()->getName();
	//printf("draw on window %s\n", windowName.c_str());
	
	//if this is the radar view of the cockpit, we need to replace our scene with the radar scene
	if(windowName == "RadarView")
	{
		static int fixRadarOnce = 0;
		fixRadarOnce++;
		if(15 == fixRadarOnce)
		{
			printf("First draw!  Setting up radar view\n");
			osg::Camera* cam = sv->getCamera();
			while(cam->getNumChildren())
			{
				printf("Remove a child from the camera:  %s\n", cam->getChild(0)->getName().c_str());
				cam->removeChild(cam->getChild(0));
			}
			
			//add the TIE fighter radar and things necessary to render it
			cam->addChild(FalconApp::instance().getTIEDisplayGroup());
			
		}
	}

	FalconApp::instance().drawStatus();
	//std::cout << "end draw" << std::endl;

}
