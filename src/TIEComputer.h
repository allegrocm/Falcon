//
//  TIERadar.h
//  Millennium Falcon
//
//  Created by Ken Kopecky II on 6/12/14.
//
//

#ifndef __Millennium_Falcon__TIERadar__
#define __Millennium_Falcon__TIERadar__

#include <osgText/Text>

#include "GameObject.h"
class PrerenderCamera;
class RadarScreen;
class ScreenImage;
class Spacecraft;

//display for the lower screen in the TIE fighter cockpit
//the radar part is composed of two instances of everything
//they fade in/out to give the impression of slow response

class TIEComputer : public GameObject
{
public:
	TIEComputer();
	
	bool update(float dt);
	PrerenderCamera* getCamera()		{return mCamera;}

protected:
	PrerenderCamera* mCamera;			//renders the display contents to a texture
	void makeScreenGeometry();
	ScreenImage* mArrowImage[2];			//shows where the Falcon is
	
	ScreenImage* mDamageImage[3];		//left, right, center
	//indicates that the Falcon is in our sites.  Two for a fade effect
	ScreenImage* mFalconImage[2];
	int mCurrentFalconImage;		//which one are we moving?
	float mFlashTime;				//how long till we update our falcon image position?
	void updateDamageDisplay();
	Spacecraft* getShip();
	void updateRadar(float dt);		//update the things that mvoe slowly (falcon image, arrow)
	float mArrowFlash;
	osg::ref_ptr<osgText::Text>			mAIText;
	float mAITextFlashTime;				//for flashing this text.  more like pulsing
	void updateStatusText(float dt);
};


#endif /* defined(__Millennium_Falcon__TIERadar__) */
