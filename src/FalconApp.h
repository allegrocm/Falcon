/*
 *  FalconApp.h
 *  Hybrid OSG Template
 *
 *  Created by Kenneth Kopecky on 8/15/09.
 *  Copyright 2009 Orange Element Studios. All rights reserved.
 *
 */

#ifndef FalconAppCUZKENLOVESYOU
#define FalconAppCUZKENLOVESYOU


#include <osg/Group>
#include <osg/Geode>
#include <osg/LightSource>
#include <osg/MatrixTransform>
#include "Bloom.h"
#include "EnemyPlayer.h"

class TIEComputer;
class Spacecraft;
class Bullet;
class Falcon;
class GameObject;
class ParticleFX;
class ComputerScreen;
class EnemyController;
class GameController;
class EventAudio;
class SpaceBox;
class StarDestroyer;


class FalconApp
{
public:

	enum ButtonState {OFF, ON, TOGGLE_OFF, TOGGLE_ON};		//the four possible states of your standard button
	static FalconApp& instance()	{static FalconApp a;  return a;}				//singleton instance
	void init();
	void setHeadMatrix(osg::Matrixf m);
	void setWandMatrix(osg::Matrixf m);
	osg::Matrixf getHeadMatrix()		{return mHeadMatrix;}
	osg::Matrixf getWandMatrix()		{return mWandMatrix;}
	void update(float dt);										//timestep update
	osg::Group* getRoot() {return mRoot.get();}			//get our root scene node
	
	osg::Group* getModelGroup()	{return mModelGroup.get();}
	void buttonInput(unsigned int button, bool pressed);

	void setIsMaster(bool m)	{mIsMaster = m;}
	bool isMaster()				{return mIsMaster;}
	void drawStatus();									//draw run-time info, like frame rate, on the head node
	void handleArguments(int* argc, char** argv);		//process command line arguments
	void shutdown();									//clean up before exiting
	void drawDebug();
//	std::vector<Spacecraft*> getShips()				{return mShips;}
	EnemyController*		getEnemyController()	{return mEnemyController;}
	GameController*			getGameController()		{return mGameController;}
	ComputerScreen*			getScreen()				{return mScreen;}
//	std::vector<Bullet*>	getBullets()			{return mBullets;}

	bool addThis(GameObject* g);					//adds this to the proper group.  returns false if couldn't be added

	Falcon* getFalcon()								{return mFalcon;}
	ParticleFX*			getFX()						{return mParticleFX;}
	
	ButtonState getButton(int which)				{return (ButtonState)mButtons[which];}
	EventAudio* getEventAudio()						{return mEventAudioManager;}
	
	BloomController* getBloom()						{return mBloom;}	//for setting glow aspect ratio for proper rendering
	
	bool tieNode1()									{return mTieNode1;}		//for setting up the TIE fighter cockpit node
	void toggleTIEMode();							//for the GLUT version only I theenk
	osg::Group*		getTIEDisplayGroup()			{return mTIEDisplayGroup;}	//this is everything we need to show the TIE fighter radar
	float getTime()		{return mTotalTime;}
	static Spacecraft*	getEnemyPlayerShip();		//shortcut cuz we need this a lot
	
	bool lowerTurretAllowed()						{return mLowerTurretAllowed;}
	bool zeroPlayerMode()							{return mZeroPlayerMode;}
	SpaceBox* getSpaceBox()							{return mSpaceBox;}		//for scene loading
	void switchSystem();							//called when we warp to a new solar system
	void toggleShaders();
	StarDestroyer* getDestroyer()					{return mStarDestroyer;}

//potential spawn position/rotation for a TIE fighter.  nearship indicates if this position should be near a capital ship
	osg::Matrix getPotentialSpawnPosition(bool nearShip);


	void autoPlay(float dt);		//for zero-player mode
protected:

	//put very little in the constructor so we don't risk a recurisive call
	FalconApp()
	{
		mIsMaster = true; mTargetTime = 0; mTotalTime = 0; mTimeStep = 0.01; mTieNode1 = false;
		mEventAudioManager = NULL; mLowerTurretAllowed = true; mZeroPlayerMode = false;
	}
	
	//update our calculated frame rate
	void updateFrameRate(float dt);


	//game entities
//	std::vector<Spacecraft*>				mShips;				//all our spaceships that are doin stuff
	EnemyController*						mEnemyController;
	std::vector<Bullet*>					mBullets;			//pewpewpew
	std::vector<GameObject*>				mJunk;			//other stuff that will take care of itself
	Falcon*									mFalcon;			//the Millennium Falcon!
	ComputerScreen*							mScreen;			//in-game data display
	TIEComputer*							mTIEScreen;			//for the TIE figher cockpit
	GameController*							mGameController;
	SpaceBox*								mSpaceBox;			//holds distance scenery
	StarDestroyer*							mStarDestroyer;
	osg::ref_ptr<osg::Group> mRoot;								//root of our scenegraph
	osg::ref_ptr<osg::MatrixTransform> mNavigation;				//navigation matrix
	osg::Vec3 mListenerVelocity;								//for audio
	osg::ref_ptr<osg::Group> mModelGroup;						//this is where we put things into the app!
	
	osg::Matrixf mWandMatrix;
	osg::Matrixf mHeadMatrix;
	osg::ref_ptr<osg::LightSource> mLightSource;
	osg::ref_ptr<osg::Group>									mTIEDisplayGroup;		//shows the TIE fighter's radar, etc
	void deToggleButtons();									//remove toggle status from our buttons after each frame to prevent events from happening repeatedly
	static const int NUMBUTTONS = 8;
	
	int mButtons[NUMBUTTONS];

	osg::MatrixTransform* mWandXForm;

	//timekeeping
	float mTargetTime;
	float mTotalTime;
	float mTimeStep;
	float mAvgFrameRate;

	//needed for clusteredness
	bool mIsMaster;										//is this the master node of the cluster?
	bool mTieNode1;										//are we the TIE fighter cockpit?
	
	ParticleFX*						mParticleFX;
	EventAudio*						mEventAudioManager;
	
	BloomController* mBloom;					//for bloom!
	bool mLowerTurretAllowed;				//on by default
	float mSPOffset;						//z-offset for our spacebox when we fly to a new system
	bool mZeroPlayerMode;					//is the computer playing itself?  for testing
};



#endif

