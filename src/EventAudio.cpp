//
//  EventAudio.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky II on 5/16/14.
//
//

#include "EventAudio.h"
#include "KSoundManager.h"
#include "FalconApp.h"
#include "Util.h"
#include <stdlib.h>
#include "KenXML.h"


EventAudio& EventAudio::instance()	{return *FalconApp::instance().getEventAudio();}

EventAudio::EventAudio()
{
	//set up our arrays
	mSoundTimer = 0;

	load("data/eventSounds.xml");

}

bool EventAudio::load(std::string file)
{
	TiXmlDocument* doc = KenXML::loadXmlFile(file);
	if(!doc)
	{
		Util::logError("Couldn't open XML file %s\n", file.c_str());
		return false;
	}
	
	TiXmlElement* eaNode = doc->FirstChildElement();
	if(!eaNode)
	{
		Util::logError("Error reading the eventSounds XML file\n");
		return false;
	}
	
	if(KenXML::CICompare(eaNode->Value(), "EventSounds") == false)
	{
		Util::logError("First node needs to be named EventSounds, not %s\n", eaNode->Value());
		return false;
	}
	
	mEvents.clear();
	for(TiXmlElement* eventNode = eaNode->FirstChildElement(); eventNode; eventNode = eventNode->NextSiblingElement())
	{
		if(eventNode->Type() == TiXmlNode::COMMENT) continue;		//skip commented nodes
		std::string what = eventNode->Value();
		if(KenXML::CICompare(what, "Event"))
		{
			//this is an event!
			std::string eventName = eventNode->Attribute("name");
			printf("Got event called %s\n", eventName.c_str());
			Event event;
			event.name = eventName;
			//dive in
			for(TiXmlElement* soundNode = eventNode->FirstChildElement(); soundNode; soundNode = soundNode->NextSiblingElement())
			{
				if(soundNode->Type() == TiXmlNode::COMMENT) continue;		//skip commented nodes
				std::string whatwhat = soundNode->Value();
				if(KenXML::CICompare(whatwhat, "soundChance"))	//probability for this to play a sound
				{
					KenXML::readValue(soundNode, event.probability);
				}
				else if(KenXML::CICompare(whatwhat, "soundChance"))	//probability for this to play a sound
				{
					KenXML::readValue(soundNode, event.minTime);
				}

				else if(KenXML::CICompare(whatwhat, "sound"))		//otherwise this should be a sound entry
				{
					EventSound s;
					if(s.fromXML(soundNode))
					{
//						printf("Added sound %s\n", s.name.c_str());
						event.sounds.push_back(s);
					}
				}
			}
			mEvents.push_back(event);
		}
	}
	
	
	return true;
	
}

void EventAudio::update(float dt)
{
	mSoundTimer -= dt;
	for(size_t i = 0; i < mEvents.size(); i++)
	{
		mEvents[i].playAgainTimer -= dt;
		for(size_t j = 0; j < mEvents[i].sounds.size(); j++)
			mEvents[i].sounds[j].playAgainTimer-= dt;
	}
}


void EventAudio::reset()
{

	for(size_t i = 0; i < mEvents.size(); i++)
	{
		mEvents[i].playAgainTimer = 0;
		for(size_t j = 0; j < mEvents[i].sounds.size(); j++)
			mEvents[i].sounds[j].playAgainTimer = -1;;
	}
}



void EventAudio::eventHappened(std::string name)
{
	//grab the random numbers now before we exit out due to sounds already playing
	int r1 = rand();
	int r2 = rand();
	//do we have sounds for this event?
	Event* e = NULL;
	for(size_t i = 0; i < mEvents.size(); i++)
	{
//		printf("Checking %s vs %s\n", name.c_str(), mEvents[i].name.c_str());
		if(KenXML::CICompare(name, mEvents[i].name))
		{
			e = &mEvents[i];
		}
		
	}
	if(!e)
	{
		Util::logError("EventAudio has no sounds for the event %s\n", name.c_str());

		//add it so we don't get this error again
		Event v;
		v.name = name;
		mEvents.push_back(v);
		return;
	}
	
	//can we play a new sound yet?
	if(mSoundTimer > 0) return;
	if(e->playAgainTimer > 0) return;	//too soon for this event to trigger again
	//only play a sound a certain percent of the time
	if(1.0 * r1 / RAND_MAX > e->probability) return;
	

	EventSound* sound = NULL;		//find a sound that can play
	std::vector<EventSound*> possibles;
	for(size_t i = 0; i < e->sounds.size(); i++)
		if(e->sounds[i].playAgainTimer <= 0)
			possibles.push_back(&e->sounds[i]);
	
	//if we didn't find any possible candid8s, give up
	if(!possibles.size()) return;
	
	int which = r2 % possibles.size();
	sound = possibles[which];
	if(sound->playAgainTimer > 0) return;			//is it too soon to play this sound?  don't play anything
	
	sound->playAgainTimer = sound->howOften;
	printf("Playing %s for event %s\n", sound->name.c_str(), name.c_str());
	SkySound* s = KSoundManager::instance()->playSound(std::string("data/sounds/") + sound->name, sound->volume, sound->pan);
	if(!s)
	{
		Util::logError("Error playing sound file %s\n", sound->name.c_str());
	}
	mSoundTimer = KSoundManager::instance()->getSoundTimeRemaining(s);
//	printf("Playing %s!  %i of %i, Sound timer:  %f\n", sound->name.c_str(), which, e->sounds.size(), mSoundTimer);
		
}

bool EventAudio::EventSound::fromXML(TiXmlElement* element)
{
	for(TiXmlElement* e = element->FirstChildElement(); e; e = e->NextSiblingElement())
	{
		std::string what = e->Value();
		if(KenXML::CICompare(what, "filename"))	KenXML::readValue(e, name);
		else if(KenXML::CICompare(what, "volume"))	KenXML::readValue(e, volume);
		else if(KenXML::CICompare(what, "pan"))	KenXML::readValue(e, pan);
		else if(KenXML::CICompare(what, "minWait")) KenXML::readValue(e, howOften);
	}
	
	return true;
	
}


