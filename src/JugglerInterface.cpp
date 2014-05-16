#include "JugglerInterface.h"
#include "FalconApp.h"
#include "Util.h"


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
	
	mPadButtons[0].init("Button 6");
	mPadButtons[1].init("Button 1");
	 vrj::OsgApp::init();
	osg::setNotifyLevel(osg::FATAL);
}

void JugglerInterface::initScene()
{
	printf("______initing scene_____\n");
	 FalconApp::instance().init();
	 _rootNode = FalconApp::instance().getRoot();
}

void JugglerInterface::preFrame()
{	
	//trigger handling
	
	//if(mButton1->getData() == gadget::Digital::TOGGLE_ON)
		//ShaderManager::instance().reloadShaders();
	
	
	//printf("preframe end\n");
}

void JugglerInterface::latePreFrame()
{
	//std::cout << "begin preframe" << std::endl;
	static double tLast = mHead->getTimeStamp().secd();
	double tNow = mHead->getTimeStamp().secd();
	double dt = tNow - tLast;
	tLast = tNow;
	
	//pass changes in button state on to the app
	for(int i = 0; i < 6; i++)
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

	__FUNCTION_HEADER__
	vrj::OsgApp::configSceneView(newSceneViewer);


//	newSceneViewer->setClearColor(osg::Vec4(0.3f, 0.2f, 0.5f, 1.0f));
	printf("line %i\n", __LINE__);
	if(!newSceneViewer) return;
	osg::Camera* cam = newSceneViewer->getCamera();
	printf("line %i\n", __LINE__);
	//set the cull mask of our camera so that we don't draw invisible hitboxes
	newSceneViewer->setCullMask((1 << NON_GLOW_LAYER) | (1 << GLOW_LAYER) | (1 << BACKGROUND_LAYER));
	printf("line %i\n", __LINE__);
}

void JugglerInterface::draw()
{
	//std::cout << "begin draw" << std::endl;
	vrj::OsgApp::draw();
	//std::cout << "end draw" << std::endl;

}
