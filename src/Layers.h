//
//  Layers.h
//  StarWars
//
//  Created by Ken Kopecky II on 10/4/13.
//  Copyright (c) 2013 VRAC. All rights reserved.
//


//this file enumerates our various layers used for nodemasks and provides a helper function to set them faster
//this is based on bitmasks, so objects belong to every layer except those that are disabled
//the main camera draws objects from both the glow and non-glow layers and the background layer, so invisible things like hitboxes
//should have all three of those layers disabled
#ifndef StarWars_Layers_h
#define StarWars_Layers_h
#include <osg/Node>

#define NON_GLOW_LAYER 0				//objects that draw for the non-glow camera.  is used to depth-mask glowing stuff
#define GLOW_LAYER 1					//objects that draw for the glow camera
#define COLLISION_LAYER 2				//objects that don't have this in their node mask won't hit detect against laser beams
#define BACKGROUND_LAYER 3				//draws, but isn't used for depth masking of glow (the non-glow layer)

//use this to easily enable and disable
static void setLayer(osg::Node* n, int layer, bool enabled)
{
	unsigned int currentMask = n->getNodeMask();
	
	//use bitwise operators to add or remove that bit from the mask
	if(enabled)
		currentMask = (currentMask | (1 << layer));
	else
		currentMask = (currentMask & (~(1 << layer)));

	n->setNodeMask(currentMask);
}

#endif

