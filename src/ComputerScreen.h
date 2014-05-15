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
protected:
	PrerenderCamera* mCamera;			//renders the display contents to a texture
	osg::ref_ptr<osgText::Text>			mStatusText;
	osg::ref_ptr<osgText::Text>			mScoreText;
	osg::ref_ptr<osgText::Text>			mButtonText[4];		//the text fields displaying what our gamepad buttons do right now
	std::string							mNextText[4];		//a button can change its text after it stops flashing
	ScreenImage*						mHealthImage;		//shows the hull integrity of the Falcon
	float mButtonTimer[4];				//how long since each button has been pressed?  flashes if a button was pressed recently
	RadarScreen* mRadar;
	void makeScreenGeometry();
	float mPulseTime;					//for flashing our health indicator
	
};

#endif /* defined(__Millennium_Falcon__ComputerScreen__) */
