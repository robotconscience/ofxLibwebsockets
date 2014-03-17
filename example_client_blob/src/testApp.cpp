#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    client.connect("localhost", 9093);
    
    client.addListener(this);
    ofSetFrameRate(60);
    
    needToLoad = false;
    locked = false;
}

//--------------------------------------------------------------
void testApp::update(){
    if ( needToLoad ){
        // you can write this directly to a file!
//        ofFile test;
//        test.open("data.jpg", ofFile::WriteOnly);
//        test.writeFromBuffer(buff);
//        test.close();
        
        turbo.load(buff, incoming);
        needToLoad = false;
        locked = false;

    }
}

//--------------------------------------------------------------
void testApp::draw(){
    if ( incoming.isAllocated() ){
        incoming.draw(0,0);
    }
}

//--------------------------------------------------------------
void testApp::onConnect( ofxLibwebsockets::Event& args ){
    cout<<"on connected"<<endl;
}

//--------------------------------------------------------------
void testApp::onOpen( ofxLibwebsockets::Event& args ){
    cout<<"on open"<<endl;
}

//--------------------------------------------------------------
void testApp::onClose( ofxLibwebsockets::Event& args ){
    cout<<"on close"<<endl;
}

//--------------------------------------------------------------
void testApp::onIdle( ofxLibwebsockets::Event& args ){
    cout<<"on idle"<<endl;
}

//--------------------------------------------------------------
void testApp::onMessage( ofxLibwebsockets::Event& args ){
    if ( locked ) return;
    // need to load this next frame!
    //cout << args.isBinary << ":" << args.size << endl;
    buff.clear();
    buff.set(args.data.getBinaryBuffer(), args.data.size());
    locked = true;
    needToLoad = true;
}

//--------------------------------------------------------------
void testApp::onBroadcast( ofxLibwebsockets::Event& args ){
    cout<<"got broadcast "<<args.message<<endl;    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

    client.send("Hello");
	cout << "sending hello" <<endl;
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
