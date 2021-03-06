//
//  ROM.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky on 4/14/14.
//
//

#include "ROM.h"
#include "Defaults.h"


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
	FALCON_HIT_DETECTION = false;

	getDefault("musicVolume", MUSIC_VOLUME);
	getDefault("falconShootSound", FALCON_FIRE_SOUND);
	getDefault("falconShootVolume", FALCON_FIRE_VOLUME);
	getDefault("placeholderExplosionSound", PLACEHOLDER_EXPLOSION_SOUND);
	getDefault("mainMusic", MAIN_MUSIC);
	getDefault("combatMusic", COMBAT_MUSIC);
	getDefault("falconHitDetection", FALCON_HIT_DETECTION);

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
	TIE_WAVE_OFF_DISTANCE = 150;
	getDefault("FalconHitPoints", FALCON_HITPOINTS);
	
	float screenX = 0, screenY = .9, screenZ = 0.5;
	float screenAngle = -28;
	
	getDefault("screenOffsetX", screenX);
	getDefault("screenOffsetY", screenY);
	getDefault("screenOffsetZ", screenZ);
	getDefault("screenOffsetAngle", screenAngle);
	
	osg::Quat q;
	q.makeRotate(screenAngle / 57.3, osg::Vec3(1, 0, 0));
	SCREEN_OFFSET.setTrans(osg::Vec3(screenX, screenY, screenZ));
	SCREEN_OFFSET.setRotate(q);
	
	HUD_MOVEMENT_SPEED = 1.0;
	getDefault("hudMovementSpeed", HUD_MOVEMENT_SPEED);
	
	ENEMY_CONTROL_YAW_MIX = 0.1;
	getDefault("enemyControlYawMix", ENEMY_CONTROL_YAW_MIX);
	
	VADER_TIE_HP = 12;
	getDefault("vaderTieHP", VADER_TIE_HP);
	
	VADER_HITBOX_SIZE = 10;
	getDefault("vaderHitboxSize", VADER_HITBOX_SIZE);
	
	TIE_RADAR_UPDATE_RATE = 5;
	TIE_RADAR_FOV = 45;
	VADER_UNROLL_RATE = 0.5;
	getDefault("vaderRadarUpdateRate", TIE_RADAR_UPDATE_RATE);
	getDefault("vaderRadarFOV", TIE_RADAR_FOV);
	getDefault("vaderUnrollRate", VADER_UNROLL_RATE);
	
	VADER_DISPLAY_COLOR = osg::Vec4(1, .25, .25, 1);
	
	VADER_TIE_MAX_SPEED = 70;
	LOWER_TURRET_GRACE_PERIOD = 8.0;
	LOWER_TURRET_CATCHUP_TIME = 12.0;
	LOWER_TURRET_FIRE_DELAY = .25;
	LOWER_TURRET_BURST_DELAY = 2;
	getDefault("vaderTIEMaxSpeed", VADER_TIE_MAX_SPEED);
	getDefault("lowerTurretGracePeriod", LOWER_TURRET_GRACE_PERIOD);
	getDefault("lowerTurretCatchupTime", LOWER_TURRET_CATCHUP_TIME);
	getDefault("lowerTurretFireDelay", LOWER_TURRET_FIRE_DELAY);
	getDefault("lowerTurretBurstDelay", LOWER_TURRET_BURST_DELAY);
	
	
	VADER_HIT_SOUND = "torret_1.mp3";
	getDefault("vaderHitSound", VADER_HIT_SOUND);
	
	VADER_DIE_SOUND = PLACEHOLDER_EXPLOSION_SOUND;
	getDefault("vaderDieSound", VADER_DIE_SOUND);
}


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

int ROM::FALCON_HITPOINTS;

Gun ROM::TIE_FIGHTER_GUN;

float ROM::TIE_HITBOX_SIZE;

bool ROM::FALCON_HIT_DETECTION;

osg::Matrixf ROM::SCREEN_OFFSET;
float ROM::HUD_MOVEMENT_SPEED;

float ROM::ENEMY_CONTROL_YAW_MIX;
int ROM::VADER_TIE_HP;
float ROM::VADER_HITBOX_SIZE;
float ROM::TIE_RADAR_FOV;
float ROM::TIE_RADAR_UPDATE_RATE;
float ROM::VADER_UNROLL_RATE;


float ROM::VADER_TIE_MAX_SPEED;

float ROM::TIE_WAVE_OFF_DISTANCE;

osg::Vec4 ROM::VADER_DISPLAY_COLOR;
float ROM::LOWER_TURRET_GRACE_PERIOD;
float ROM::LOWER_TURRET_FIRE_DELAY;
float ROM::LOWER_TURRET_BURST_DELAY;
float ROM::LOWER_TURRET_CATCHUP_TIME;

std::string ROM::VADER_HIT_SOUND;
std::string ROM::VADER_DIE_SOUND;