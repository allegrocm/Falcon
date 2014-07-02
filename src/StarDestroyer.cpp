//
//  StarDestroyer.cpp
//  Millennium Falcon
//
//  Created by Ken Kopecky II on 7/2/14.
//
//

#include "StarDestroyer.h"
#include "FalconApp.h"
#include "Util.h"
#include "ShaderManager.h"

using namespace osg;

StarDestroyer::StarDestroyer()
:Spacecraft()
{
	MatrixTransform* ds = Util::loadModel("data/models/SD.ive", 4, 0, -90);
	ShaderManager::instance().applyShaderToNode("data/shaders/DeathStar", ds);
	mPat->addChild(ds);
	warp();
}


bool StarDestroyer::update(float dt)
{
	//rotate slowly for funsies for now
//	float theta = mAge * 0.1;
//	setForward(Vec3(cosf(theta), 0, -sinf(theta)));
	return Spacecraft::update(dt);

}

void StarDestroyer::warp()
{
	float theta = Util::random(0.0, 6.28);
	float phi = Util::random(0, 1.0);
	float dist = Util::random(2400, 4800);
	setPos(Vec3(cosf(theta)*cosf(phi), sinf(phi), sinf(theta)*cosf(phi))*dist);
	
	//random heading
//	theta = Util::random(0, 6.28);
//	setForward(Vec3(cosf(theta), 0, sinf(theta)));
	Vec3 up = Util::vectorInCone(getPos()*-1, 80, 20);
	setForward(up);
//	Vec3 pos = getPos() * -1;;
//	pos.normalize();
//	printf("pos:  %.2f, %.2f, %.2f\n", pos.x(), pos.y(), pos.z());
//	printf("up:  %.2f, %.2f, %.2f\n", up.x(), up.y(), up.z());
//	Quat q = getQuat();
//	Matrix xform = getTransform();
//	Matrix rot = Matrix::rotate(Util::yAxis(xform), up);
//	q = q * rot.getRotate();
//	setQuat(q);

}

