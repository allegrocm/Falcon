//
//  EventAudio.h
//  Millennium Falcon
//
//  Created by Ken Kopecky II on 5/16/14.
//
//

#ifndef __Millennium_Falcon__EventAudio__
#define __Millennium_Falcon__EventAudio__


//manages playing of random sound clips in response to events that occur in the game
#include <iostream>

#include <vector>
#include <string>

class TiXmlElement;		//for XML parsing
class EventAudio
{
public:

	//a struct to let us store the name, volume, and pan position of a sound we'll be playing
	struct EventSound
	{
		std::string name;
		float volume;
		float pan;
		float howOften;		//how many seconds between plays of this sound?  if it's too soon, we won't play anything
		float playAgainTimer;
		EventSound()	{volume = 0.5; pan = 0; howOften = -1; playAgainTimer = 0;}
		EventSound(std::string file, float vol = 0.5, float panning = 0)
		{name = file; volume = vol; pan = panning; playAgainTimer = 0; howOften = -1;}
		bool fromXML(TiXmlElement* e);
	};
	
	
					
	struct Event
	{
		std::string name;			//name of this event
		float probability;			//how likely are we to play a sound for a particular event?
		float minTime;				//this event can play every this many seconds
		float playAgainTimer;		//how long till this event can play a sound again?
		Event()	{probability = 0.5; minTime = -1; playAgainTimer = 0.0;}
		std::vector<EventSound> sounds;		//associated sounds
	};
	
	bool load(std::string file);
	static EventAudio& instance();

	void eventHappened(std::string name);		//something happened.  maybe play audio
	void update(float dt);
	void reset();		//reset individual sound timers
	EventAudio();			//we don't ever need to construct this.  it's handled by the instance
protected:
	std::vector<Event> mEvents;		//the good stuff

	float mSoundTimer;				//how long till we can play another sound?
};

#endif /* defined(__Millennium_Falcon__EventAudio__) */
