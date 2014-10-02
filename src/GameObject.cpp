//
//  GameObject.cpp
//  PaddleboatQuest
//
//  Created by Ken Kopecky II on 1/27/14.
//
//



#include "GameObject.h"
#include "Debris.h"
#include "Util.h"
#include "FalconApp.h"
#include "Layers.h"
#include <stdlib.h>
#include <osgUtil/IntersectVisitor>

using namespace osg;

GameObject::GameObject()
{
	mAge = 0;
	mRoot = new Group();
	mRoot->setName("GameObject Root");
	mPat = new PositionAttitudeTransform();
	mPat->setName("GameObject Pat");
	mRoot->addChild(mPat);
	setName("GameObject");
	setGlows(false);			//things don't glow by default
	mTransformUniform = new Uniform("transform", Matrixf());
	mInverseTransformUniform = new Uniform("inverseTransform", Matrixf());
	mRoot->getOrCreateStateSet()->addUniform(mTransformUniform);
	mRoot->getOrCreateStateSet()->addUniform(mInverseTransformUniform);
}

GameObject::~GameObject()
{
	//remove the root from everything that contains it
	if(mRoot)
		while(mRoot->getNumParents())
			mRoot->getParent(0)->removeChild(mRoot);

}

void GameObject::setGlows(bool b)
{
	setLayer(mRoot, GLOW_LAYER, b);
	setLayer(mRoot, NON_GLOW_LAYER, !b);
	setLayer(mRoot, GLOW_ONLY_LAYER, false);		//by default, nothing goes on the glow only layer
}

Matrixf GameObject::getTransform()
{
	//get our current transform as a matrix
	Matrixf currentXForm;
	Vec3 pos = getPos();
	mPat->getAttitude().get(currentXForm);
	currentXForm.ptr()[12] = pos.x();
	currentXForm.ptr()[13] = pos.y();
	currentXForm.ptr()[14] = pos.z();
	return currentXForm;

}

void GameObject::setTransform(Matrixf m)
{
	__FUNCTION_HEADER__
	mPat->setAttitude(m.getRotate());
	mPat->setPosition(m.getTrans());
	updateTransformUniforms();
	
}


//todo:  make this work better and not screw up "up"
void GameObject::setForward(osg::Vec3 f)
{
	if(f.length() < 0.001)
	{
		printf("attempting to use tiny vector for forward!  %.2f, %.2f, %.2f\n", f.x(), f.y(), f.z());
		return;
	}
	
	f.normalize();
	Vec3 posIn = getPos();
	Vec3 currentForward = getForward();
	
	Matrixf m = Matrixf::rotate(currentForward, f);
//	Util::printMatrix(m);
	Quat q; q.makeRotate(currentForward, f);
	
	setQuat(getQuat() * q);
//	setTransform(m*getTransform());
	
	Vec3 posOut = getPos();
	currentForward = f;
//	printf("set forward to %.2f, %.2f, %.2f\n", currentForward.x(), currentForward.y(), currentForward.z());
	currentForward = getForward();
//	printf("now forward is %.2f, %.2f, %.2f\n", currentForward.x(), currentForward.y(), currentForward.z());
//	printf("pos start:  %.2f, %.2f, %.2f.  end:  %.2f, %.2f, %.2f\n", posIn.x(), posIn.y(), posIn.z(), posOut.x(), posOut.y(), posOut.z());
}

osg::Vec3 GameObject::getForward()
{
	Matrixf x = getTransform();
	return (Vec3(x.ptr()[8], x.ptr()[9], x.ptr()[10])*-1.0);
}

void GameObject::setName(std::string s)
{
	mName = s;
	mRoot->setName(s);
}

std::vector<Debris*> GameObject::explodeSection(osg::Group* n, int depth, int maxDepth, float splitChance, float vanishChance, osg::Matrixf currentTransform, osg::Node* root, int maxPieces)
{
	std::vector<Debris*> debris;
	if(!n || !depth) return debris;
		//if this has a child transform, multiply it on
	Matrixf thisMat;
	MatrixTransform* mt = dynamic_cast<MatrixTransform*>(n);
	if(mt)
	{
		thisMat = mt->getMatrix();
//		printf("MatrixTransform:\n");
//		Util::printMatrix(thisMat);
	}
	
//	currentTransform = thisMat * currentTransform;
	if(mt)
	{
//		printf("Current transform:\n");
//		Util::printMatrix(currentTransform);
	}
	//make a line of dashes to show our hierarchy
	std::string line = "-----------------";
	line[depth+1] = 0;
	int numChildren = n->getNumChildren();
	for(int i = 0; i < numChildren; i++)
	{
		Node* node = n->getChild(i);
		//printf("%s: %s"   , line.c_str(), node->getName().c_str());

		//skip empty groups
		if(node->asGroup() && node->asGroup()->getNumChildren() == 0)
		{
		//	printf("Skip)\n");
			continue;
		}



		//top tier stuff can't vanish
		bool vanish = (1.0 * Util::loggedRandom("Explode Vanish") / RAND_MAX < vanishChance) && (depth > 2);
//		printf("Depth:  %i\n", depth);
		if(vanish)
		{
		//	printf("(vanish)\n");
			continue;
		}
		
		//only groups can split, and only if there's still depth to go
		float splitVal = 1.0 * Util::loggedRandom("Explode split") / RAND_MAX;
//		printf("Split val:  %.2f vs %.2f\n", splitVal, splitChance);
		float thisSplitChance = splitChance;
		if(depth == 1) thisSplitChance = 1.0;		//top level always splits
		bool split = ((splitVal < thisSplitChance) && node->asGroup() != NULL && depth <= maxDepth);


		if(split)
		{
			//add all the pieces onto this debris
		//	printf("(split)\n");
			
			std::vector<Debris*> more = explodeSection(node->asGroup(), depth+1, maxDepth, splitChance, vanishChance, currentTransform, root, maxPieces);
			for(size_t j = 0; j < more.size(); j++)
			{
				debris.push_back(more[j]);
				maxPieces--;
			}
		
		}
		else if(maxPieces > 0)
		{
		//	printf("(debris)\n");
			
			//set the proper offset and direction for this debris
			Vec4 cg = Util::getNodeCG(node, root);
			Vec3 cgDir = Vec3(cg.x()/cg.w(), cg.y()/cg.w(), cg.z()/cg.w());
			
			Debris* h = new Debris(node, cgDir, 20, 20.0 + 40.0 * Util::loggedRandom("Debris Speed") / RAND_MAX, 120 * depth);
			
			
//			printf("Cum matrix:  \n");
			Matrixf local = Util::getCumulativeTransform(n, root);		//our transform within the debris model
//			printf("Current:  \n");
//			Util::printMatrix(currentTransform);
//			h->setTransformAndOffset(local * currentTransform, cgDir);
			h->setTransformAndOffset(local * currentTransform, Vec3());
//			printf("CG:  %.2f, %.2f, %.2f, weight:  %.1f\n", cg.x()/cg.w(), cg.y()/cg.w(), cg.z()/cg.w(), cg.w());
			debris.push_back(h);
			maxPieces--;

		
		}
		
	
	}
	
	return debris;
}

void GameObject::drawDebug()
{
	glPushMatrix();
		glMultMatrixf(getTransform().ptr());
		glBegin(GL_LINES);
			float size = 2;
			for(int i = 0; i < 3; i++)
			{
				glColor3f(i==0, i==1, i==2);
				glVertex3f(0, 0, 0);
				glVertex3f(size*(i==0), size*(i==1), size*(i==2));
			}
		glEnd();
	glPopMatrix();
	

}

bool GameObject::checkRaycast(osg::Vec3 origin, osg::Vec3 vec, osg::Vec3& hitPos)
{
	__FUNCTION_HEADER__
	BoundingSphere bs = mRoot->getBound();
	//if(bs)
	{
	
		float dist = (origin-bs.center()).length();
		if(dist - vec.length() < bs.radius())
		{
		//	printf("early out!\n");
		//	return false;
		}
	}
	osgUtil::IntersectVisitor iv;
	//make a line segment representing the laser beam
	ref_ptr<LineSegment> seg = new LineSegment(origin, origin+vec);
	iv.setTraversalMask(1 << COLLISION_LAYER);		//DON'T check collisions with other lazer beams
	iv.addLineSegment(seg.get());
//	printf("Seg:  %.2f, %.2f, %.2f\n", pos.x(), pos.y(), pos.z());
	getRoot()->accept(iv);
	osgUtil::IntersectVisitor::HitList& hitList = iv.getHitList(seg.get());
	if(hitList.size())		//if there's any size in the hitlist, we HIT something!
	{
		hitPos = hitList.front().getWorldIntersectPoint();
		return true;
	}
		
	return false;
}

void GameObject::updateTransformUniforms()
{
	Matrix mat = getTransform();
	mTransformUniform->set(mat);
	mat.invert(mat);
	mInverseTransformUniform->set(mat);
}

