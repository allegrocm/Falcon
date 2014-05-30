#include <iostream>
#ifdef WIN32
#include <windows.h>
#endif
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#ifdef KENS_MIDI_CONTROLLER
	#include "magicjoystick.h"
#endif


#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <stdarg.h>
#include "FalconApp.h"
#include "quickprof.h"
#include "Layers.h"
#include "EnemyController.h"


int screenWidth = 1024;
int screenHeight = 768;
float gYaw = 45.0;
float gPitch = 45.0;
float gZoom = 1.0;
bool gRightClick = false;
bool gOrbitCam;		//are we using the orbit cam or the C6 cam?
float aspect;
bool gPaused = false;
bool gFullScreen;
int gMouseX, gMouseY;
float gTime = 0.0;
bool gShowC6 = false;
bool gSloMo = false;

#include "CameraController.h"

CameraController gCamera;


osg::ref_ptr<osgViewer::Viewer> viewer;
osg::observer_ptr<osgViewer::GraphicsWindow> window;

void drawStringOnScreen(int x, int y, const char* format, ...);
void drawStatus()
{
	glColor3f(1,1,1);
	static float t=0;
	static float fps=2;
	static float deltat=.2;
	static int reps=0;
	float tnow=glutGet(GLUT_ELAPSED_TIME);
	reps++;
//printf("%f\n", tnow - t);
	if(tnow - t > 500)		//update every 500 ms
	{
	//	printf("upd\n");
		//float tnow=glutGet(GLUT_ELAPSED_TIME);
		deltat=tnow-t;
		t=tnow;
		fps=1.f * reps/deltat*1000;
		reps = 0;
	}
	
		drawStringOnScreen(20, 20, "Frame Rate:             %.2f", fps);
	
	glColor3f(1, 0, 0);
	

}
void drawC6(bool filled)
{
	glCullFace(GL_BACK);
	if(filled)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);
	glLineWidth(5);
	glColor3f(0.7, 0.7, 0.7);
	if(filled)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
		glNormal3f(0, 1, -5);
		glVertex3f(-5, 0, -5);
		glVertex3f(-5, 0, 5);
		glVertex3f(5, 0, 5);
		glVertex3f(5, 0, -5);

		glNormal3f(0, 0, 1);
		glVertex3f(-5, 10, -5);
		glVertex3f(-5, 0, -5);
		glVertex3f(5, 0, -5);
		glVertex3f(5, 10, -5);

		glNormal3f(1, 0, 0);
		glVertex3f(-5, 10, -5);
		glVertex3f(-5, 10, 5);
		glVertex3f(-5, 0, 5);
		glVertex3f(-5, 0, -5);

		glNormal3f(-1, 0, 0);
		glVertex3f(5, 0, -5);
		glVertex3f(5, 0, 5);
		glVertex3f(5, 10, 5);
		glVertex3f(5, 10, -5);

	glEnd();
	
	//if we're using lined mode, draw a grid over the C6
	if(!filled)
	{
		glLineWidth(1);
		int numLines = 20;
		glColor3f(0.4, 0.4, 0.4);
		glBegin(GL_LINES);
			for(int i = 0; i < numLines; i++)
			{
				float coord = -5.0 + 10.0 * i / numLines;
				
				//floor
				glVertex3f(-5,0, coord);
				glVertex3f(5, 0, coord);
				glVertex3f(coord, 0, -5);
				glVertex3f(coord, 0, 5);
				
				//front
				glVertex3f(-5, coord+5, -5);
				glVertex3f(5, coord+5, -5);
				glVertex3f(coord, 0, -5);
				glVertex3f(coord, 10, -5);
				
				//left
				glVertex3f(-5, coord+5, -5);
				glVertex3f(-5, coord+5, 5);
				glVertex3f(-5, 0, coord);
				glVertex3f(-5, 10, coord);

				//right
				glVertex3f(5, coord+5, -5);
				glVertex3f(5, coord+5, 5);
				glVertex3f(5, 0, coord);
				glVertex3f(5, 10, coord);

			} 
		glEnd();
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	//square to indicate the C6 floor
	glBegin(GL_QUADS);
		glColor3f(0.9, 0.9, 0.9);
		glVertex3f(-5, -0.01, -5);
		glVertex3f(-5, -0.01, 5);
		glVertex3f(5, -0.01, 5);
		glVertex3f(5, -0.01, -5);
	glEnd();
}


void drawTheFPSGuy()
{
	float red[] = {1, 0, 0, 1};
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, red);
	glPushMatrix();
		glTranslatef(gCamera.mFPPos.x, gCamera.mFPPos.y, gCamera.mFPPos.z);
		glRotatef(gCamera.mFPYaw * 180.0 / 3.142, 0, 1, 0);
		glRotatef(gCamera.mFPPitch * 180.0 / 3.1412, 1, 0, 0);
		
		glEnable(GL_LIGHTING);
		
		//head!
		glPushMatrix();
			glTranslatef(-0, -0.2, 0);
			glutSolidSphere(0.4, 15, 15);
		glPopMatrix();

		//body!
		glPushMatrix();
			glTranslatef(0, -1.5, 0);
			glScalef(1.0, 2.0, 0.5);
			glutSolidCube(1.0);
		glPopMatrix();
		
		//legs!
		glPushMatrix();
			glTranslatef(-0.3, 1.5 - 5.5, 0);
			glScalef(0.3, 3.0, 0.3);
			glutSolidCube(1.0);
			glTranslatef(0.6/0.3, 0, 0);
			glutSolidCube(1.0);
		glPopMatrix();

		//axes!
		glDisable(GL_LIGHTING);
		glBegin(GL_LINES);
			float length = 2.0;
			glColor3f(1, 0, 0);
			glVertex3f(0, 0, 0);
			glVertex3f(length, 0, 0);
			glColor3f(0, 1, 0);
			glVertex3f(0, 0, 0);
			glVertex3f(0, length, 0);
			glColor3f(0, 0, 1);
			glVertex3f(0, 0, 0);
			glVertex3f(0, 0, -length);
		glEnd();
	glPopMatrix();
	
}

void setLight(float x, float y, float z)
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	float lightPosition[] = {x, y, z, 1};
	glLightfv (GL_LIGHT0, GL_POSITION, lightPosition);
}



void display(void)
{
    // update and render the scene graph
	osg::Camera* currentCam = viewer->getCamera();


//	printf("camera name:  %s\n", currentCam->getName().c_str());
	currentCam->setCullMask((1 << NON_GLOW_LAYER) | (1 << GLOW_LAYER) | (1 << BACKGROUND_LAYER));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	currentCam->setProjectionMatrixAsPerspective(60, aspect, 0.2, 8500.0);
	FalconApp::instance().getBloom()->setAspectRatio(aspect);

	currentCam->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
	gluPerspective(60, aspect, 0.2, 200.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//apply our chosen matrix!	
	KMatrix view = gCamera.getViewMatrix();

	glLoadMatrixf(view.m);

	//use this time to get the head and wand matrices and pass them to our app.  
	//this only works because GLUT uses a single OpenGL context.  if you send matrices to a C6 
	//app while you're drawing, bad things will happen	
	
	//which one should we send?  If we're view-aligning particles, it's useful to send whichever one we're actually using for our camera
	//but if we're using it to track the user's head and interact with it, we should send the one representing the user...the first person view matrix
	

	//here's a cool trick!
	//the head matrix we get from Juggler is actually the INVERSE of the modelview matrix that would be needed
	//for a camera at that same position
	//Right now, we can grab GL's modelview matrix and use that, but matrix inversions are hard
	//Fortunately, since we're not doing any scaling, our modelview matrix is an orthogonal matrix
	//this means that its inverse is the same as its transpose!  so....
	FalconApp::instance().setHeadMatrix(osg::Matrixf(gCamera.getViewMatrix(CameraController::FPS_VIEW).getInverse().m));
	

	//next we'll send the app a wand matrix based on the mouse position

	KMatrix wandMat = gCamera.getWandMatrix(KVec3(-1.0 + 2.0 * gMouseX / screenWidth, -2.0 + 2.0 * gMouseY / screenHeight, -2));

	//get a directional wand by projecting our mouse position
	double px, py, pz;
	int vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	osg::Matrixd vd(gCamera.getViewMatrix(CameraController::FPS_VIEW).m);
	gluUnProject(gMouseX, gMouseY, 0.99, vd.ptr(), currentCam->getProjectionMatrix().ptr(), vp, &px, &py, &pz);
	KVec3 headPos = gCamera.mFPPos;
	KVec3 z(headPos.x - px, headPos.y - py, headPos.z - pz);
	z.normalize();
	KVec3 x = KVec3(0, 1, 0).cross(z);
	x.normalize();
	KVec3 y = z.cross(x);

	x.normalize();
	y.normalize();
//	printf("y:  %.2f, %.2f, %.2f\n");
	//stick new directions into our wand matrix
	for(int i = 0; i < 3; i++)
	{
		wandMat.m[i] = x.ptr()[i];
		wandMat.m[4+i] = y.ptr()[i];
		wandMat.m[8+i] = z.ptr()[i];
	}
	
	//the wand rotates about a point a foot ahead of and a foot below the head position
	//...
	
//	wandMat.print();
//	printf("Unproject to %.2f, %.2f, %.2f\n", px, py, pz);
	FalconApp::instance().setWandMatrix(osg::Matrixf(wandMat.m));

	currentCam->setViewMatrix(osg::Matrixf(view.m));
	PROFILER.beginBlock("OSG Frame");
    if (viewer.valid()) viewer->frame(gTime);
	PROFILER.endBlock("OSG Frame");
	//go back to our view so we can draw some extras
	glViewport(0, 0, screenWidth, screenHeight);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
//	double fov, aspect, near, far;
//	currentCam->getProjectionMatrixAsPerspective(fov, aspect, near, far);
//	printf("Cam nearfar:  %.2f, %.2f\n", near, far);
	osg::Matrixf proj = 	currentCam->getProjectionMatrix();
	glMultMatrixf(proj.ptr());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(view.m);
	setLight(50, 50, 50);
	glEnable(GL_DEPTH_TEST);
	if(gCamera.getViewMode() != CameraController::FPS_VIEW)
		drawTheFPSGuy();
	glDisable(GL_LIGHTING);
	FalconApp::instance().drawDebug();
	if(gShowC6)
		drawC6(false);
	drawStatus();
    // Swap Buffers
    glutSwapBuffers();
}




void reshape( int w, int h )
{
	screenWidth = w;
	screenHeight = h;
	aspect = 1.0 * screenWidth / screenHeight;
	printf("reshape to %i, %i\n", screenWidth, screenHeight);
	glViewport(0, 0, screenWidth, screenHeight);
    // update the window dimensions, in case the window has been resized.
    if (window.valid()) 
    {
        window->resized(window->getTraits()->x, window->getTraits()->y, w, h);
        window->getEventQueue()->windowResize(window->getTraits()->x, window->getTraits()->y, w, h );
    }
}

void mousebutton( int button, int state, int x, int y )
{
  	gCamera.mouseClick(button, state, x, y);
	y = screenHeight - y;
	gMouseX = x;
	gMouseY = y;
	if(button == GLUT_RIGHT_BUTTON)
		gRightClick = true;
	else
		gRightClick = false;


}


void mouseMotion(int x, int y)
{
	gCamera.mouseMotion(x, y);
	y = screenHeight - y;
	gMouseX = x;
	gMouseY = y;
}

void mouser(int button, int state, int x, int y)
{
	y = screenHeight - y;
	gMouseX = x;
	gMouseY = y;
	if(button == GLUT_RIGHT_BUTTON)
		gRightClick = true;
	else
		gRightClick = false;

	glutPostRedisplay();
}

void passiveMotion(int x, int y)
{
	gCamera.passiveMouse(x, y);
	y = screenHeight - y;
	gMouseX = x;
	gMouseY = y;
}


void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
		//handle key input
		case 'q': gCamera.setStrafeLeft(true);	break;
		case 'w': gCamera.setUp(true);	break;
		case 'e': gCamera.setStrafeRight(true);	break;
		case 'a': gCamera.setLeft(true);	break;
		case 's': gCamera.setDown(true);	break;
		case 'S': gSloMo = !gSloMo; break;
		case 'd': gCamera.setRight(true);	break;
		case 'z': gCamera.setLower(true);		break;
		case 'x': gCamera.setRaise(true);		break;
		case 'K': EnemyController::instance().killAll(); break;
		case 'c': gShowC6 = !gShowC6; break;
		case ' ':	FalconApp::instance().buttonInput(0, true);	break;		//space bar controls the main wand button
	
		case 'p':	gPaused = !gPaused;	break;		//pause/unpause
		//switch input modes with tab
		case '	':	gCamera.cycleViewMode();	break;
		case 'f':
			if(!gFullScreen)	glutFullScreen();
			else glutReshapeWindow(1024, 768);
			gFullScreen = !gFullScreen;
			break;
		case 27:	viewer = NULL;  exit(1);	break;
		//case 13: FalconApp::instance().buttonInput(1, true);break;		//return key is button 1
		case 'F':
		{
			printf("fstats!\n");
			window->getEventQueue()->keyPress( (osgGA::GUIEventAdapter::KeySymbol) 's' );
			window->getEventQueue()->keyRelease( (osgGA::GUIEventAdapter::KeySymbol) 's' );
			break;
		}
		default:
//            if (window.valid())
//            {
//                window->getEventQueue()->keyPress( (osgGA::GUIEventAdapter::KeySymbol) key );
//                window->getEventQueue()->keyRelease( (osgGA::GUIEventAdapter::KeySymbol) key );
//            }
            break;
    }
	glutPostRedisplay();
}

void keyboardUp(unsigned char key, int x, int y)
{
	switch(key)
	{
	
	}


}

void keySpecial(int key, int x, int y)
{
	switch(key)
	{
	
		case GLUT_KEY_LEFT: FalconApp::instance().buttonInput(1, true); break;
		case GLUT_KEY_DOWN: FalconApp::instance().buttonInput(2, true); break;
		case GLUT_KEY_RIGHT: FalconApp::instance().buttonInput(3, true); break;
		case GLUT_KEY_UP: FalconApp::instance().buttonInput(4, true); break;
		default: break;

	}
	

}

void keySpecialUp(int key, int x, int y)
{
	switch(key)
	{
		case GLUT_KEY_LEFT: FalconApp::instance().buttonInput(1, false); break;
		case GLUT_KEY_DOWN: FalconApp::instance().buttonInput(2, false); break;
		case GLUT_KEY_RIGHT: FalconApp::instance().buttonInput(3, false); break;
		case GLUT_KEY_UP: FalconApp::instance().buttonInput(4, false); break;

		default: break;

	}
	

}


void timer(int bl)
{
	static int lastTime = glutGet(GLUT_ELAPSED_TIME);
	int thisTime = glutGet(GLUT_ELAPSED_TIME);
	float dt = 0.001 * ((float) thisTime - lastTime);

	lastTime = thisTime;
	gCamera.update(dt);
	if(gSloMo)
		dt *= 0.1;
#ifdef KENS_MIDI_CONTROLLER


	MagicJoystick::update();
	if(MagicJoystick::sticks())
	{
		//send gamepad button presses to the app
		MagicJoystick& gamepad = MagicJoystick::stick(0);
		for(int i = 0; i < 4; i++)
			FalconApp::instance().buttonInput(1+i, gamepad.button[i]);
	}
#endif

	if(!gPaused)
	{
		FalconApp::instance().update(dt);		//send the timestep to the app class
		gTime += dt;
	}
	glutTimerFunc(0, timer, 0);
	glutPostRedisplay();

}

void keyUpBoard(unsigned char key, int x, int y)
{

	switch(key)
	{
		//handle key input
		case 'q': gCamera.setStrafeLeft(false);	break;
		case 'w': gCamera.setUp(false);	break;
		case 'e': gCamera.setStrafeRight(false);	break;
		case 'a': gCamera.setLeft(false);	break;
		case 's': gCamera.setDown(false);	break;
		case 'd': gCamera.setRight(false);	break;
		case 'z': gCamera.setLower(false);		break;
		case 'x': gCamera.setRaise(false);		break;
		
		case ' ':	FalconApp::instance().buttonInput(0, false);	break;		//space bar controls the main wand button
	//	case 13: FalconApp::instance().buttonInput(1, false);		break;		//return key is button 1
	}

	glutPostRedisplay();
}

void quitski()
{
	FalconApp::instance().shutdown();
	if(viewer) viewer = NULL;
}

int main( int argc, char **argv )
{
    glutInit(&argc, argv);
	//osg::setNotifyLevel(osg::DEBUG_FP);
	osg::setNotifyLevel(osg::FATAL);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ALPHA );
    glutInitWindowPosition( 100, 100 );
    glutInitWindowSize( 800, 600 );
    glutCreateWindow( argv[0] );
    glutDisplayFunc(display);
    glutReshapeFunc( reshape );
    glutMouseFunc( mousebutton );
    glutMotionFunc( mouseMotion);
    glutPassiveMotionFunc(passiveMotion);
	glutKeyboardFunc( keyboard );
	glutKeyboardUpFunc(keyboardUp);
	glutSpecialFunc(keySpecial);
	glutSpecialUpFunc(keySpecialUp);
	glutKeyboardUpFunc(keyUpBoard);

    // create the view of the scene.
    viewer = new osgViewer::Viewer;
    window = viewer->setUpViewerAsEmbeddedInWindow(100,100,800,600);
	FalconApp::instance().init();
    viewer->setSceneData(FalconApp::instance().getRoot());

    viewer->addEventHandler(new osgViewer::StatsHandler);
    viewer->realize();
	glutTimerFunc(100, timer, 0);
	atexit(quitski);

    glutMainLoop();
    
    return 0;
}

/*EOF*/
