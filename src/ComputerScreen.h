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

//A display that shows status and possibly radar or something for the Millennium Falcon App


class ComputerScreen : public GameObject
{
public:
	ComputerScreen();
	bool update(float dt);

protected:
	PrerenderCamera* mCamera;			//renders the display contents to a texture
	osg::ref_ptr<osgText::Text>			mStatusText;
	RadarScreen* mRadar;
	void makeScreenGeometry();
	
};

#endif /* defined(__Millennium_Falcon__ComputerScreen__) */
