#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    //http://localhost:9091/
    
    client.addListener(this);
    
    // connect to websocket.org's echo server on default port
    //bool connected = client.connect( "echo.websocket.org", true );
    bool connected = client.connect( "echo.websocket.org" );
}

//--------------------------------------------------------------
void testApp::update(){
}

//--------------------------------------------------------------
void testApp::draw(){
    client.send("Hello");
}

//--------------------------------------------------------------
void testApp::onConnect( ofxWebSocketEvent& args ){
    cout<<"on connected"<<endl;
}

//--------------------------------------------------------------
void testApp::onOpen( ofxWebSocketEvent& args ){
    cout<<"on open"<<endl;
}

//--------------------------------------------------------------
void testApp::onClose( ofxWebSocketEvent& args ){
    cout<<"on close"<<endl;
}

//--------------------------------------------------------------
void testApp::onIdle( ofxWebSocketEvent& args ){
    cout<<"on idle"<<endl;
}

//--------------------------------------------------------------
void testApp::onMessage( ofxWebSocketEvent& args ){
    cout<<"got message "<<args.message<<endl;
}

//--------------------------------------------------------------
void testApp::onBroadcast( ofxWebSocketEvent& args ){
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
void testApp::dragEvent(ofDragInfo dragInfo){ 

}