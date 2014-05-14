//
//  ROM.h
//  Millennium Falcon
//
//  Created by Ken Kopecky on 4/14/14.
//
//

#ifndef __Millennium_Falcon__ROM__
#define __Millennium_Falcon__ROM__

#include <iostream>

//this is a little trick I learned from a buddy who does a lot of Flash/Flixel dev.
//One place to get at constants and stuff. Dunno if
//it's useful in C++, but we'll find out.


class ROM
{
public:
	static void load();		//loads the settings file

	//sound effects and music
	static std::string FALCON_FIRE_SOUND;		//sound file used
	static float FALCON_FIRE_VOLUME;			//default volume for falcon shooting
	static float FALCON_LASER_LENGTH;			//how long is a laser shot?
	static float FALCON_LASER_SPEED;			//how fast does it move?
	static float MUSIC_VOLUME;					//for all music
	static std::string PLACEHOLDER_EXPLOSION_SOUND;
	static std::string MAIN_MUSIC;
	static std::string COMBAT_MUSIC;
	
	
	//difficulty
	static float TIE_HITBOX_SIZE;		//hitbox size of a TIE fighter
};

#endif /* defined(__Millennium_Falcon__ROM__) */
