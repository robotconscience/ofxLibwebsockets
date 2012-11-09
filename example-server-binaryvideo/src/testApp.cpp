#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    // setup a server with default options on port 9092
    // - pass in true after port to set up with SSL
    //bool connected = server.setup( 9092 );
    
    // Uncomment this to set up a server with a protocol
    // Right now, clients created via libwebsockets that are connecting to servers
    // made via libwebsockets seem to want a protocol. Hopefully this gets fixed, 
    // but until now you have to do something like this:
    
    //setup video grabber
    video.listDevices();
    bVideoSetup = video.initGrabber( 320, 240 );
    
    ofxLibwebsockets::ServerOptions options = ofxLibwebsockets::defaultServerOptions();
    options.port = 9093;
    options.protocol = "of-protocol";
    bool connected = server.setup( options );
    
    // this adds your app as a listener for the server
    server.addListener(this);
    
    // setup message queue
    font.loadFont("myriad.ttf", 20);
    messages.push_back("WebSocket server setup at "+ofToString( server.getPort() ) + ( server.usingSSL() ? " with SSL" : " without SSL") );
    
    ofBackground(0);
    ofSetFrameRate(30);
}

//--------------------------------------------------------------
void testApp::update(){
    video.update();
    if ( bVideoSetup && video.isFrameNew() ){
        server.send( ofToString(video.width) +":"+ ofToString( video.height ) +":"+ ofToString( 1 ) );
        server.sendBinary( video );
        //messages.push_back( "Sending image" );
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    int x = font.getSize();
    int y = font.getSize()*2;
    ofSetColor(255);
    for (int i = messages.size() -1; i >= 0; i-- ){
        //font.drawString( messages[i], x, y );
        y += font.stringHeight( messages[i] ) + font.getSize();
    }
    if ( bVideoSetup ) video.draw(0,0);
}

//--------------------------------------------------------------
void testApp::onConnect( ofxLibwebsockets::Event& args ){
    cout<<"on connected"<<endl;
}

//--------------------------------------------------------------
void testApp::onOpen( ofxLibwebsockets::Event& args ){
    cout<<"new connection open"<<endl;
    messages.push_back("New connection from " + args.conn.getClientIP() );
}

//--------------------------------------------------------------
void testApp::onClose( ofxLibwebsockets::Event& args ){
    cout<<"on close"<<endl;
    messages.push_back("Connection closed");
}

//--------------------------------------------------------------
void testApp::onIdle( ofxLibwebsockets::Event& args ){
    cout<<"on idle"<<endl;
}

//--------------------------------------------------------------
void testApp::onMessage( ofxLibwebsockets::Event& args ){
    cout<<"got message "<<args.message<<endl;
    
    // trace out string messages or JSON messages!
    if ( !args.json.isNull() ){
        messages.push_back("New message: " + args.json.toStyledString() + " from " + args.conn.getClientName() );
    } else {
        messages.push_back("New message: " + args.message + " from " + args.conn.getClientName() );
    }
    
    //if (messages.size() > NUM_MESSAGES) messages.erase( messages.begin() );
    
    // echo server = send message right back!
    args.conn.send( args.message );
}

//--------------------------------------------------------------
void testApp::onBroadcast( ofxLibwebsockets::Event& args ){
    cout<<"got broadcast "<<args.message<<endl;    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){}