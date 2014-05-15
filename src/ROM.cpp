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
float ROM::FALCON_LASER_LENGTH;
float ROM::FALCON_LASER_SPEED;
float ROM::TIE_FIGHTER_LASER_LENGTH;
float ROM::TIE_FIGHTER_LASER_SPEED;

std::string ROM::PLACEHOLDER_EXPLOSION_SOUND;
float ROM::MUSIC_VOLUME;
std::string ROM::MAIN_MUSIC;
std::string ROM::COMBAT_MUSIC;


Gun ROM::TIE_FIGHTER_GUN;

float ROM::TIE_HITBOX_SIZE;

void ROM::load()
{

	Defaults::instance().load("data/Settings.txt");
	//put in some defaults
	FALCON_FIRE_VOLUME = 0.25;
	FALCON_FIRE_SOUND =  "cheeseyBlaster.wav";
	PLACEHOLDER_EXPLOSION_SOUND = "cheeseyBoom.wav";
	MAIN_MUSIC = "mainTheme.mp3";
	COMBAT_MUSIC = "mainTheme.mp3";
	FALCON_LASER_SPEED = 800;
	FALCON_LASER_LENGTH = 16;
	MUSIC_VOLUME = 0.2;
	getDefault("musicVolume", MUSIC_VOLUME);
	getDefault("falconShootSound", FALCON_FIRE_SOUND);
	getDefault("falconShootVolume", FALCON_FIRE_VOLUME);
	getDefault("placeholderExplosionSound", PLACEHOLDER_EXPLOSION_SOUND);
	getDefault("mainMusic", MAIN_MUSIC);
	getDefault("combatMusic", COMBAT_MUSIC);


	TIE_HITBOX_SIZE = 10;
	getDefault("tieHitboxSize", TIE_HITBOX_SIZE);


	TIE_FIGHTER_GUN.mAutoburst = true;
	TIE_FIGHTER_GUN.mBurstDelay = 3.0;
	TIE_FIGHTER_GUN.mShotDelay = 0.2;
	TIE_FIGHTER_GUN.mShotsPerBurst = 8;
	TIE_FIGHTER_GUN.mFireVolume = 0.5;
	TIE_FIGHTER_GUN.mAutoburst = false;
	TIE_FIGHTER_GUN.mFireSound = "TIE-Fire.wav";
	
	TIE_FIGHTER_LASER_SPEED = 200;
	TIE_FIGHTER_LASER_LENGTH = 4;
}
