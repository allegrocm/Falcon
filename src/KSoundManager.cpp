//    SkyPaint:  Virtual Stargazing With a Touch of Magic
//    Copyright (C) 2006-2008  Ken Kopecky (allegrocm@gmail.com)
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.



/*
 *  KSoundManager.cpp
 *  Boat.  uses fmodEX
 *
 *  written by Ken Kopecky
 */

#include "KSoundManager.h"
#include <iostream>
#include <stdio.h>
#define checkErrors()\
	if(!mSilent)\
{\
	if(result != FMOD_OK)\
        printf("FMOD error! (%d) %s from line %i\n", result, FMOD_ErrorString(result), __LINE__);\
}

//shows all the relevant parameters for a particular DSP
void printDSPParameterInfo(FMOD::DSP* dsp, char* description)
{
	char name[16];
	char label[16];
	char desc[256];
	int desclen = 256;
	float min;
	float max;
	printf("DSP Parameters: for %s\n", description);
	int imax;
	dsp->getNumParameters(&imax);
	for(int i = 0; i < imax; i++)
	{
		dsp->getParameterInfo(i, name, label, desc, desclen, &min, &max);
		printf("Param %i:	%s		%s		%s		%f		%f\n", i, name, label, desc, min, max);
	}
}

KSoundManager::KSoundManager()
{
//	doThis();
	printf("Initializing the KSoundManager\n");
	mEchoAmount = 1.5f;
	song = NULL;
	fading = false;
	musicFadeTime = 0.0;
	musicFadeTimeCounter = 0.0;
	mTrack = 0;
	mTrackTime = 0;
	mMusicEcho = false;
	mInitFailed = false;
	result = FMOD::System_Create(&mSystem);
	checkErrors();
	if(result != FMOD_OK)
	{
		mInitFailed = true;
		printf("FMOD Error!\n");
		checkErrors();
	}
	//printf("init system\n");
	result = mSystem->init(64, (FMOD_INITFLAGS)(FMOD_INIT_NORMAL), NULL);
	checkErrors();
	if(result != FMOD_OK)
		mInitFailed = true;
	//printf("make dsp\n");
//	result = mSystem->createDSPByType(FMOD_DSP_TYPE_SFREVERB, &mReverb);
//	result = mSystem->createDSPByType(FMOD_DSP_TYPE_ECHO, &mEcho);
//	mEcho->setParameter(0, 0.25);
	checkErrors();
	int imax;
	result = mEcho->getNumParameters(&imax);
	checkErrors();
	//printDSPParameterInfo(mEcho, "echo");
	//printDSPParameterInfo(mReverb, "reverb");
	 unsigned int      version;
    result = mSystem->getVersion(&version);
	printf("Using FMOD Version %08x\n", version);
    if (version < FMOD_VERSION)
    {
        printf("Warning!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);

    }
	mSilent = mInitFailed;;
	//loadPlaylist("music/StarList.m3u");
	std::cout<<"KSoundManager init complete" << std::endl;
}

KSoundManager::~KSoundManager()
{
	mSystem->close();
	mSystem->release();
}

bool KSoundManager::playSong(std::string name, float volume, float stereo, bool loop)
{
	if(mSilent) return true;
	//printf("play %s, with looping = %i\n", name.c_str(), (int) loop); 
	
	//stop any current song
	if(song)
		stopSong();
	// don't even bother if no sound file specified
	if ("" == name || "data/music/ " == name)
		return false;
				
		result = mSystem->createSound(name.c_str(), FMOD_CREATESTREAM | FMOD_SOFTWARE | FMOD_2D, 0, &song); 

	if (!song)
	{
		printf("FMOD Error loading %s!\n", name.c_str());
		checkErrors();
		return false;
	}
	//extract the important part out of the name
	int startPos = (int)name.find_last_of("/");
	mCurrentSongName = name.substr(startPos + 1, name.size() - 4 - startPos - 1);
	
	mMusicVolume = volume;
	song->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
	mSystem->playSound(FMOD_CHANNEL_FREE, song, false, &songChannel);

	songChannel->setPan(stereo);
	songChannel->setVolume(volume);
	songChannel->getFrequency(&songBaseFrequency);		//get the base frequency for modulation later
	songChannel->setPriority(0);		//the music is the top priority in SkyPaint
	// for now, just return true - of course this needs to be changed
	return true;
}

void KSoundManager::setMusicEcho(float on)
{
	if(mSilent) return;
	if(!mInitFailed) return;
	
	float amount = on;
//	printf("setting music echo to %s\n", on ? "on" : "off");
	if(on == mMusicEcho) return;
	if(!song) return;
	//printf("switching...\n");
	mMusicEcho = on;
	
	mReverb->setParameter(1, 0.8 * amount);
	mReverb->setParameter(2, 0.57 * amount);
	mReverb->setParameter(3, 0.5f + 0.2 * amount);
	
	mSystem->update();
}

void KSoundManager::setSongPosition(float px, float py, float pz, float vx, float vy, float vz)
{
	if(mSilent) return;
	FMOD_VECTOR pos = {px, py, pz};
	FMOD_VECTOR vel = {vx, vy, vz};
	songChannel->set3DAttributes(&pos, &vel);
}

void KSoundManager::setSongPitch(float p)
{
	if(mSilent) return;
	if(mInitFailed) return;
	
	result = songChannel->setFrequency((songBaseFrequency * p));
	checkErrors();
}

void KSoundManager::pauseSong()
{
	if(mSilent) return;
	if (!song) return;
	
	bool paused;
	songChannel->getPaused(&paused);
	songChannel->setPaused(!paused);
}

void KSoundManager::stopSong()
{
	if(mSilent) return;
	if (!song) return;
	
	songChannel->stop();
	song = NULL;
}

SkySound* KSoundManager::playSound(std::string name, float volume, float stereo, bool loop)
{
	if(mSilent) return NULL;
	if ("" == name) return NULL;
	
	FMOD::Sound* sound;
	result = mSystem->createSound(name.c_str(), FMOD_SOFTWARE | FMOD_2D, 0, &sound); 
	checkErrors();
	if(!sound) printf("Error playing %s\n", name.c_str());
	if (!sound) return NULL;

	FMOD::Channel* soundChannel;
	sound->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
	mSystem->playSound(FMOD_CHANNEL_FREE, sound, false, &soundChannel);
	soundChannel->setPan(stereo);
	soundChannel->setVolume(volume);
	mSystem->update();
	return soundChannel;
}


SkySound* KSoundManager::play3DSound(std::string name, float volume, float x, float y, float z, bool loop, float fallOffStart, float delay)
{
	if(mSilent) return NULL;
	if ("" == name) return NULL;
	if(mInitFailed) return NULL;
	
	FMOD::Sound* sound = 0;
	FMOD::Channel* soundChannel;
	result = mSystem->createSound(name.c_str(), FMOD_SOFTWARE | FMOD_3D, 0, &sound); 
	
	if (!sound)
	{
		printf("FMOD Error loading %s!\n", name.c_str());
		checkErrors();
		return NULL;
	}
	sound->set3DMinMaxDistance(fallOffStart, 2000);
	sound->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
	bool delayed = (delay > 0);
	mSystem->playSound(FMOD_CHANNEL_FREE, sound, delayed, &soundChannel);

	FMOD_VECTOR pos = {x, y, z};
	FMOD_VECTOR vel = {0, 0, 0};
	soundChannel->setVolume(volume);
	soundChannel->set3DAttributes(&pos, &vel);
	mSystem->update();
	
	//if this sound is delayed, queue it up
	if(delayed)
	{
		DelayedSound s;
		s.delay = delay;
		s.sound = soundChannel;
		mDelayedSounds.push_back(s);
	}
	return soundChannel;
}

void KSoundManager::setSound3DInfo(SkySound* soundChannel, float px, float py, float pz, float vx, float vy, float vz)
{
	if(mSilent) return;
	if(!soundChannel) return;
	if(mInitFailed) return;
	
	FMOD_VECTOR pos = {px, py, pz};
	FMOD_VECTOR vel = {vx, vy, vz};
	soundChannel->set3DAttributes(&pos, &vel);
	mSystem->update();
}

void KSoundManager::updateListener(float dt, float transform[16], float velx, float vely, float velz)
{
	if(mSilent) return;
	if(mInitFailed) return;
	
	update(dt);
	FMOD_VECTOR pos = {transform[12], transform[13], transform[14]};
	FMOD_VECTOR vel = {velx, vely, velz};
	FMOD_VECTOR forward = {transform[8], transform[9], transform[10]};
	FMOD_VECTOR up = {transform[4], transform[5], transform[6]};
	//takes position, velocity, forward vector, up vector
	mSystem->set3DListenerAttributes(0, &pos, &vel, &forward, &up);

	result = mSystem->update();
	checkErrors();
}
	
bool KSoundManager::preLoad(std::string name)
{
	if(mSilent) return true;
	// don't even bother if no sound file specified
	if ("" == name) return false;
	
	playSound(name, 0, 0);
	return true;
}

void KSoundManager::update(float dt)
{
	if(mSilent) return;
	mSystem->update();

	if (fading)
	{
		musicFadeTimeCounter -= dt;
		if (musicFadeTimeCounter < 0)
		{
			printf("fade song to %s\n", nextSong.c_str());
			stopSong();
			if(nextSong != "")
				playSong(nextSong, mMusicVolume, 0);
			nextSong = "";
			fading = false;
			musicFadeTimeCounter = 0.0;
			musicFadeTime = 0.0;
		}
		else
		{
			songChannel->setVolume(mMusicVolume * musicFadeTimeCounter/musicFadeTime);
		}
	}
	
	//if we have a play list...well, use it
	if(mUsePlayList && mPlayList.size())
	{
		unsigned int trackTime = 0;
		if(songChannel)
		{
			bool playing;
			songChannel->isPlaying(&playing);
			if(!playing) mTrackTime = 99999;
			else
			{
				songChannel->getPosition(&trackTime, FMOD_TIMEUNIT_MS);
				mTrackTime = 0.001 * trackTime;
			}
		}
		else
			mTrackTime = 99999;			//if the song stops, move on
		if(mTrackTime > mPlayList[mTrack].trackTime)
			skipSong();
	}
	
	for(int i = 0; i < (int)mDelayedSounds.size(); i++)
	{
		mDelayedSounds[i].delay-= dt;
		if(mDelayedSounds[i].delay <= 0)
		{
			mDelayedSounds[i].sound->setPaused(false);
			mDelayedSounds[i] = mDelayedSounds.back();
			mDelayedSounds.pop_back();
			i--;
		}
	}
}

void KSoundManager::setMusicVolume(float v)
{	
	if(mSilent) return;
	
	mMusicVolume = v;
	songChannel->setVolume(v);
}

void KSoundManager::fadeIntoSong(float fadeTime, const std::string& songName)
{
	if(mSilent) return;
	
	fading = true;
	musicFadeTime = fadeTime;
	musicFadeTimeCounter = fadeTime;
	nextSong = songName;
}



void KSoundManager::letSoundDie(SkySound* soundChannel)
{
	if(mSilent) return;
	if(!soundChannel) return;
	if(mInitFailed) return;
	
	soundChannel->setPriority(256);
}


void KSoundManager::setSoundVolume(SkySound* channel, float volume)
{
	if(mSilent) return;
	if(!channel) return;
	if(mInitFailed) return;
	if(channel)
		channel->setVolume(volume);
}

void KSoundManager::setSoundFrequency(SkySound* channel, float frequency)
{
	if(mSilent) return;
	if(!channel) return;
	if(mInitFailed) return;
	if(channel)
		channel->setFrequency(frequency);
}

void KSoundManager::stopSound(SkySound* channel)
{
	if(mSilent) return;
	if(!channel) return;
	if(mInitFailed) return;
	if(channel)
		channel->stop();
}

float KSoundManager::getSoundTimeRemaining(SkySound* channel)
{
	if(mSilent) return 0;
	if(!channel) return 0;
	if(mInitFailed) return 0;
	FMOD::Sound* sound;
	channel->getCurrentSound(&sound);
	if(!sound) return 0;
	unsigned int pos = 0;
	unsigned int length;
	result = channel->getPosition(&pos, FMOD_TIMEUNIT_MS);
	result = sound->getLength(&length, FMOD_TIMEUNIT_MS);
	return 0.001 * (length - pos);
}

void KSoundManager::loadPlaylist(std::string filename)
{
	FMOD::Sound* playlist;
	mSystem->createSound(filename.c_str(), FMOD_DEFAULT, 0, &playlist);
	FMOD_SOUND_TYPE type;
	playlist->getFormat(&type, 0, 0, 0);
	if(type != FMOD_SOUND_TYPE_PLAYLIST)
	{
		printf("%s, wasn't a playlist!  Make sure it exists and it's in .m3u format.\n", filename.c_str());
		return;
	}
	printf("loading playlist %s\n", filename.c_str());
	int count = 0;
	FMOD_TAG tag;
	result = FMOD_OK;
	//go through the m3u file and extract the playlist info
	result = playlist->getTag("FILE", count, &tag);
	while(result == FMOD_OK)
	{

		unsigned int trackTime = 0;
		FMOD::Sound* music;		//attempt to load the song
		result = mSystem->createSound((char*)tag.data, FMOD_CREATESTREAM | FMOD_SOFTWARE | FMOD_2D, 0, &music); 
		//if we successfully found the file, get its length and close it
		if(result == FMOD_OK)
		{
			music->getLength(&trackTime, FMOD_TIMEUNIT_MS);
			music->release();
			PlayListEntry p(std::string((char*)tag.data), 0.001 * trackTime + 2.f);
			mPlayList.push_back(p);
//			printf("our file is...%s at %f seconds\n", p.filename.c_str(), p.trackTime);
		}
		else printf("%s not found, but is listed in the playlist\n", (char*)tag.data);
		
				count++;
		//check to see if we have another file in the playlist
		result = playlist->getTag("FILE", count, &tag);
	}
	mUsePlayList = true;
}

void KSoundManager::skipSong()
{
	if(mSilent) return;
	
	mTrack++;
	if(mTrack < 0) mTrack += mPlayList.size();
	mTrack = (mTrack) % mPlayList.size();
	mTrackTime = 0;
	float vol =  1;
	if(songChannel)
	{
		bool playing;
		songChannel->isPlaying(&playing);
		songChannel->getVolume(&vol);
		songChannel->stop();
	}
	playSong(mPlayList[mTrack].filename, mMusicVolume, 0, false);
	printf("new song:  %i:  %s\n", mTrack, mPlayList[mTrack].filename.c_str() );
}

void KSoundManager::previousSong()
{
	if(mSilent) return;
	
	//if we didn't just start the song, just rewind it.  otherwise go to the previous song
	if(mTrackTime > 2 && songChannel)
	{
		printf("rewinding\n");
		mTrackTime = 0;
		songChannel->setPosition(5, FMOD_TIMEUNIT_RAWBYTES);
	}
	else
	{
		mTrack -= 2;				//go back two tracks
		skipSong();
	}
}
