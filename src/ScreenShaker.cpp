//
//  ScreenShaker.cpp
//  StarWars
//
//  Created by Ken Kopecky on 2/19/14.
//  Copyright (c) 2014 VRAC. All rights reserved.
//

#include "ScreenShaker.h"
#include "FalconApp.h"
#ifdef KENS_MIDI_CONTROLLER
	#include "magicjoystick.h"
#endif

using namespace osg;

ScreenShaker::ScreenShaker()
{
	_pat = new PositionAttitudeTransform();
	_damping = 12.0;
	_k = 73.0;
	_strength = 1.0;
	_tTillSettle = 0;
	_tTillShake = 99;
	_tMin = 0.05;
	_tMax = 0.1;
	_minDistance = 0.1;
	_maxDistance = 0.3;
	

}

void ScreenShaker::update(float dt)
{
	_tTillShake -= dt;
	_tTillSettle -= dt;
#ifdef KENS_MIDI_CONTROLLER
	if(MIDIControl::active())
	{
		_damping = 20.0 * MIDIControl::slider(1);
		_k = 100.0 * MIDIControl::slider(2);
		_tMin = 0.01 + 0.3 * MIDIControl::slider(3);
		_tMax = 0.01 +0.3 * MIDIControl::slider(4);
		printf("midi!\n");
	}
#endif

	//is it time to shake again?
	if(_tTillShake <= 0 && _tTillSettle > 0)
	{
		//shake shake shake!  shake shake shake!  shake your camera!
		Vec3 shake = osg::Vec3(-1.0+2.0 * rand() / RAND_MAX, -1.0 + 2.0 * rand()/RAND_MAX, 0);
		float distance = _minDistance + (_maxDistance-_minDistance) * rand() / RAND_MAX;
		_tTillShake = _tMin + (_tMax - _tMin) * rand() / RAND_MAX;		//shake again in a random amount of time
		if(_shakes++ == 0)
		{
			distance = _maxDistance;		//shake our max distance the first time.  it's the biggest shake
			_tTillShake = _tMin;
		}
		shake.normalize();
		shake *=distance;
		
		
		Matrix head = FalconApp::instance().getHeadMatrix();
		Vec3 xDir(head.ptr()[0], head.ptr()[1], head.ptr()[2]);
		Vec3 yDir(head.ptr()[4], head.ptr()[5], head.ptr()[6]);
		
		//aim for a position that's far away from our present position
		//start with the reflected position, and be near it
		_shakePos = _shakePos * -0.5 * _tTillSettle + (xDir * shake.x() + yDir * shake.y()) * _tTillSettle * 0.5;
//		_shakePos *= _tTillSettle;
		//only shake up, down, left, and right.  forward and backward is weird

//		printf("shake to %.2f, %.2f, %.2f, next in %.2f\n", _shakePos.x(), _shakePos.y(), _shakePos.z(), _tTillShake);
		_vel = (_shakePos - _pos) / _tTillShake;
	}
	
	
	//move the camera towards the current shakey position
	Vec3 dPos = _shakePos - _pos;
	Vec3 force = dPos * _k - _vel * _damping;
	
	if(_tTillSettle <= 0)		//if we're done shaking, move the camera towards the center
	{
		_shakePos = Vec3();
		_vel += force * dt * _strength;
	}
//	_pos += (_shakePos - _pos) * (_damping * dt);
	_pos += _vel * dt;
	
	//try a damped movement model.  might more closely model impulse and restoration
	_pat->setPosition(_pos);
	
	//let's try just using our position as a rotation as well
	float angle = _pos.length();
//	angle = sqrtf(angle);
	angle *= 0.3;
	Vec3 axis = _pos;
	axis.normalize();
	_pat->setAttitude(Quat(angle, axis));
//	printf("angle:  %.3f\n", angle);
//	printf("shake:  %.2f, %.2f, %.2f\n", _pos.x(), _pos.y(), _pos.z());
}

void ScreenShaker::activate(float amount)
{
	_shakes = 0;
	_tTillShake = 0;
	_tTillSettle = amount;
}