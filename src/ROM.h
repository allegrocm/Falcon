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

#include "Gun.h"
#include <osg/MatrixTransform>

class ROM
{
public:
	static void load();		//loads the settings file

	//sound effects and music
	static std::string FALCON_FIRE_SOUND;		//sound file used
	static float FALCON_FIRE_VOLUME;			//default volume for falcon shooting
	static float FALCON_LASER_LENGTH;			//how long is a laser shot?
	static float FALCON_LASER_SPEED;			//how fast does it move?
	static float TIE_FIGHTER_LASER_SPEED;
	static float TIE_FIGHTER_LASER_LENGTH;
	static float MUSIC_VOLUME;					//for all music
	static std::string PLACEHOLDER_EXPLOSION_SOUND;
	static std::string MAIN_MUSIC;
	static std::string COMBAT_MUSIC;

	//difficulty
	static Gun TIE_FIGHTER_GUN;					//the basic gun used by TIE fighters
	static int FALCON_HITPOINTS;
	static float TIE_HITBOX_SIZE;		//hitbox size of a TIE fighter
	static float TIE_WAVE_OFF_DISTANCE;		//when does a TIE fighter stop its attack and pull up?
	//performace/difficulty
	static bool FALCON_HIT_DETECTION;
	
	//setting up the HUD/targetting computer
	static osg::Matrixf SCREEN_OFFSET;		//from the wand
	static float HUD_MOVEMENT_SPEED;		//when it moves up and down for combat/interaction
	
	static float ENEMY_CONTROL_YAW_MIX;		//how much does the enemy yaw instead of roll in response to x-axis input?
	static int VADER_TIE_HP;				//hitpoints of human-controlled enemies
	static float VADER_HITBOX_SIZE;
	static float TIE_RADAR_UPDATE_RATE;
	static float TIE_RADAR_FOV;
	static float VADER_UNROLL_RATE;		//how quickly does the player TIE fighter return to y-up when it rolls?
	static osg::Vec4 VADER_DISPLAY_COLOR;
	static float VADER_TIE_MAX_SPEED;
};

#endif /* defined(__Millennium_Falcon__ROM__) */
