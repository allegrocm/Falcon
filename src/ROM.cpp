//
//  ROM.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky on 4/14/14.
//
//

#include "ROM.h"
#include "Defaults.h"

//instantiate static member variables
std::string ROM::FALCON_FIRE_SOUND;
float ROM::FALCON_FIRE_VOLUME;



void ROM::load()
{

	Defaults::instance().load("data/Settings.txt");
	//put in some defaults
	FALCON_FIRE_VOLUME = 0.25;
	FALCON_FIRE_SOUND =  "cheeseyBlaster.wav";
		
	getDefault("falconShootSound", FALCON_FIRE_SOUND);
	getDefault("falconShootVolume", FALCON_FIRE_VOLUME);
	printf("FFV:  %.2f\n", FALCON_FIRE_VOLUME);

}
