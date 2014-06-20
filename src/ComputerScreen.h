//
//  ComputerScreen.h
//  Millennium Falcon
//
//  Created by Ken Kopecky on 4/14/14.
//
//

#ifndef __Millennium_Falcon__ComputerScreen__
#define __Millennium_Falcon__ComputerScreen__

#include <osgText/Text>

#include "GameObject.h"
class PrerenderCamera;
class RadarScreen;
class ScreenImage;


//a helper struct that shows menu buttons and a prompt
struct MenuPrompt
{
	std::string text;			//what text is displayed here?
	std::string buttonText[4];	//what text goes on the buttons?
	std::string buttonEvent[4];	//what string event happens when the button is hit?
};

//A display that shows status and possibly radar or something for the Millennium Falcon App

class ComputerScreen : public GameObject
{
public:
	ComputerScreen();
	bool update(float dt);
	void setStatusText(std::string s);
	void buttonPressed(int which);
	
	void setButtonText(int which, std::string text, std::string event = "");
	void setButtonChangeText(int which, std::string text, std::string event = "");		//sets the next a button will change to after it's pressed
	void setIsUp(bool b)	{mUp = b;}
	PrerenderCamera* getCamera()		{return mCamera;}
	RadarScreen* getRadar()				{return mRadar;}
	virtual void doEvent(std::string s);		//does things when buttons are hit
protected:
	PrerenderCamera* mCamera;			//renders the display contents to a texture
	osg::ref_ptr<osgText::Text>			mStatusText;
	osg::ref_ptr<osgText::Text>			mScoreText;
	osg::ref_ptr<osgText::Text>			mButtonText[4];		//the text fields displaying what our gamepad buttons do right now
	ScreenImage* mButtonCircle[4];							//background for our buttons
	MenuPrompt mCurrentMenu;			//our current button stuff
	MenuPrompt mNextMenu;				//what's our button menu gonna be after things stop flashing?
	ScreenImage*						mHealthImage;		//shows the hull integrity of the Falcon
	float mButtonTimer[4];				//how long since each button has been pressed?  flashes if a button was pressed recently
	RadarScreen* mRadar;
	void makeScreenGeometry();
	float mPulseTime;					//for flashing our health indicator
	bool mUp;							//is the screen up and central for interaction, or down and out of the way for combat?
	float mUpness;
	std::string mPendingEvent;			//the event that will happen when the computer's button stops flashing
	//our layout.  we have two of them. battle mode and interaction mode
	struct HUDLayout
	{
		float textPosX;
		float textPosY;
		float textSize;
		float healthImageSize;
		float healthImageX;
		float healthImageY;
		float radarSize;
		float radarX;
		float radarY;
		float buttonTextSize;
		float buttonsX;
		float buttonsY;
		float buttonsDX;
		float buttonsDY;
		float buttonCircleSize;
	} mLayout[2];
	
	void updateLayout(float upness);		//interpolate between our two layouts
	std::map<std::string, MenuPrompt>	mMenus;		//menus that show up in response to strings
};

#endif /* defined(__Millennium_Falcon__ComputerScreen__) */
