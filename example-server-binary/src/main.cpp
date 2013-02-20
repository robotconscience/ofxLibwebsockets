#include "ofMain.h"
#include "testApp.h"
#ifndef TARGET_LINUX_ARM
#include "ofAppGlutWindow.h"
#endif
//========================================================================
int main( ){
#ifndef TARGET_LINUX_ARM
    ofAppGlutWindow window;
	ofSetupOpenGL(&window, 1024,768, OF_WINDOW);			// <-------- setup the GL context
#else
	ofSetupOpenGL(1024,768, OF_WINDOW);			// <-------- setup the GL context
#endif
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new testApp());

}
