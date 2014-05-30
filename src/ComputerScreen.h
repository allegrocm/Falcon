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

//A display that shows status and possibly radar or something for the Millennium Falcon App


class ComputerScreen : public GameObject
{
public:
	ComputerScreen();
	bool update(float dt);
	void setStatusText(std::string s);
	void buttonPressed(int which);
	void setButtonText(int which, std::string text);
	void setButtonChangeText(int which, std::string text);		//sets the next a button will change to after it's pressed
	void setIsUp(bool b)	{mUp = b;}
protected:
	PrerenderCamera* mCamera;			//renders the display contents to a texture
	osg::ref_ptr<osgText::Text>			mStatusText;
	osg::ref_ptr<osgText::Text>			mScoreText;
	osg::ref_ptr<osgText::Text>			mButtonText[4];		//the text fields displaying what our gamepad buttons do right now
	ScreenImage* mButtonCircle[4];							//background for our buttons
	std::string							mNextText[4];		//a button can change its text after it stops flashing

	ScreenImage*						mHealthImage;		//shows the hull integrity of the Falcon
	float mButtonTimer[4];				//how long since each button has been pressed?  flashes if a button was pressed recently
	RadarScreen* mRadar;
	void makeScreenGeometry();
	float mPulseTime;					//for flashing our health indicator
	bool mUp;							//is the screen up and central for interaction, or down and out of the way for combat?
	float mUpness;
	
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
};

#endif /* defined(__Millennium_Falcon__ComputerScreen__) */
