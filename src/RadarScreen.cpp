//
//  RadarScreen.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky on 4/17/14.
//
//

#include <stdio.h>
#include "RadarScreen.h"
#include "FalconApp.h"
#include "EnemyController.h"
#include "Spacecraft.h"


using namespace osg;

void RSDrawable::drawImplementation(osg::RenderInfo& renderInfo) const
{

	if(radar)
		radar->draw();
}

osg::BoundingBox RSDrawable::computeBound() const
{
	//make a big ol bounding box that can't be culled
//	printf("*************HUDDrawable:  ComputeBound!\n");
	osg::BoundingBox bb;
	bb.expandBy(osg::Vec3(-10, -10, -1));
	bb.expandBy(osg::Vec3(10, 10, 1));
	
	return bb;
}

RadarScreen::RadarScreen()
{
	mCamera = new PrerenderCamera(1024, 1, GL_RGBA);
	
	//create a radar drawable and assign it to us
	mDrawable = new RSDrawable;
	mDrawable->radar = this;
	Geode* g = new Geode();
	mCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	mCamera->setProjectionMatrixAsOrtho(-.5, .5, -.5, .5, -1, 1);
	g->addDrawable(mDrawable);
	mCamera->addChild(g);
//	transform->addChild(g);
	FalconApp::instance().getRoot()->addChild(mCamera);
	transform->getOrCreateStateSet()->setTextureAttributeAndModes(0, mCamera->getTargetTexture(0));
	mDrawable->setUseDisplayList(false);
	mDrawable->setUseVertexBufferObjects(false);
	
	
}

RadarScreen::~RadarScreen()
{
	//release our drawable so it can die in the wild
	mDrawable->radar = NULL;
	mDrawable = NULL;
}

Vec3 RadarScreen::worldToRadar(osg::Vec3 worldPos)
{
	Vec3 local = worldPos * mRadarMatInverse;		//move into wandspace for the radar
	local.normalize();
	float theta = atan2f(local.x(), -local.z());
//		theta /= 3.14159 * 2.0;
	float phi = atan2f((local.y()), sqrtf(local.z()*local.z()+local.x()*local.x()));
//		phi /= 3.141590;
	local.x() = theta * 0.5;
	local.y() = phi * 0.5;
	local.z() = 0;

	//clamp to the edges of our circle
	if(local.length() > 0.45) local = local / local.length() * 0.45;

	return local;
}


void RadarScreen::draw()
{
	glClearColor(0, 0, 0, .25);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);

	//grab the line width so we can restore it later
	int lw;
	glGetIntegerv(GL_LINE_WIDTH, &lw);
	//draw some circles
	float radius[] = {.125, .25, .375};
	glLineWidth(14);
	//draw background lines
	for(int j = 0; j < sizeof(radius) / sizeof(float); j++)
	{
		glBegin(GL_LINE_LOOP);
			for(int i = 0; i < 36; i++)
			{
				float theta = 6.28 * i / 36;
				float s = sinf(theta);
				float c = cosf(theta);
				glVertex2f(s*radius[j], c * radius[j]);
				
			}
		glEnd();
		glColor3f(.7, .7, .7);
		
	}
	
	glColor3f(1, 1, 1);
	//and a lil frame
	glBegin(GL_LINE_LOOP);
	float w = 0.475;
	float h = 0.475;
		glVertex2f(-w, -h);
		glVertex2f(w, -h);
		glVertex2f(w, h);
		glVertex2f(-w, h);
	glEnd();
	
	
	//draw all the ships
	std::vector<Spacecraft*> ships = EnemyController::instance().getShips();
	
	for(size_t i = 0; i < ships.size(); i++)
	{
		Vec3 pos = worldToRadar(ships[i]->getPos());
//		printf("In:  %.2f, %.2f, %.2f\n", pos.x(), pos.y(), pos.z());

//		theta = osg::clampBetween(theta, -0.49f, 0.49f);
//		phi = osg::clampBetween(phi, -0.49f, 0.49f);
//		if(i==2)
//		printf("converted:  %.2f, %.2f, %.2f\n", pos.x(), pos.y(), pos.z());
//		if(i == 2) printf("angles:  %.2f, %.2f\n", theta, phi);
		bool isOtherPlayer = (ships[i]->getPlayer() != NULL);
		float size = 0.025;
		if(isOtherPlayer)		//is this the enemy player??
			size = 0.035;

		//flash between two colors of the enemy player
		Vec3 colormain(1, 1, 1);
		Vec3 colorFlash(1, .5, .5);

		float flash = 0 + FalconApp::instance().getTime() * 20.0 * isOtherPlayer;
		float t = 0.5 + 0.5 * sinf(flash);
		Vec3 color = colormain * (1.0-t) + colorFlash * (t);
		glColor3fv(color.ptr());
		glBegin(GL_LINE_LOOP);
			glVertex2f(pos.x()+size*0.5, pos.y()+size*0.5);
			glVertex2f(pos.x()+size*0.5, pos.y()-size*0.5);
			glVertex2f(pos.x()-size*0.5, pos.y()-size*0.5);
			glVertex2f(pos.x()-size*0.5, pos.y()+size*0.5);
		glEnd();
	}
	
	//restore the original line width
	glLineWidth(lw);
}

void RadarScreen::update(float dt)
{
	Matrix wand = FalconApp::instance().getWandMatrix();
	wand.invert(wand);
	mRadarMatInverse = wand;


}
