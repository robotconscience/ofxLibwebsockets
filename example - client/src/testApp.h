#pragma once

#include "ofMain.h"

#include "ofxLibwebsockets.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
        ofxWebSocketClient client;
    
        // websocket methods
        void onConnect( ofxWebSocketEvent& args );
        void onOpen( ofxWebSocketEvent& args );
        void onClose( ofxWebSocketEvent& args );
        void onIdle( ofxWebSocketEvent& args );
        void onMessage( ofxWebSocketEvent& args );
        void onBroadcast( ofxWebSocketEvent& args );
};
