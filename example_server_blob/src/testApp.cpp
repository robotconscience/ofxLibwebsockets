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
    
    ofxLibwebsockets::ServerOptions options = ofxLibwebsockets::defaultServerOptions();
    options.port = 9093;
    options.protocol = "of-protocol";
    options.bBinaryProtocol = true;
    
    bool connected = server.setup( options );
    
    // this adds your app as a listener for the server
    server.addListener(this);
    
    // setup message queue
    
    font.loadFont("myriad.ttf", 20);
    messages.push_back("WebSocket server setup at "+ofToString( server.getPort() ) + ( server.usingSSL() ? " with SSL" : " without SSL") );
    
    ofBackground(0);
    ofSetFrameRate(60);
    bSendImage = false;
    locked = needToLoad = false;
}

//--------------------------------------------------------------
void testApp::update(){
    if ( bSendImage && toLoad != "" ){
        turbo.load( toLoad, currentImage );
        unsigned long size;
        unsigned char * compressed = turbo.compress(&currentImage,100,&size);
        server.sendBinary(compressed, size);
        free(compressed);
        messages.push_back( "Sending image" );
        bSendImage = false;
        toLoad = "";
    }
    
    if ( needToLoad ){
        // you can write this directly to a file!
        //        ofFile test;
        //        test.open("data.jpg", ofFile::WriteOnly);
        //        test.writeFromBuffer(buff);
        //        test.close();
        
        cout << "load "<<buff.size()<<endl;
        
        turbo.load(buff, incoming);
        needToLoad = false;
        locked = false;
        
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
    if (currentImage.bAllocated()) currentImage.draw(0,0);
    if ( incoming.bAllocated() ){
        int y = 0;
        if ( currentImage.bAllocated() ){
            y += currentImage.height;
        }
        incoming.draw(0,y);
    }
}

//--------------------------------------------------------------
void testApp::onConnect( ofxLibwebsockets::Event& args ){
    cout<<"on connected"<<endl;
}

//--------------------------------------------------------------
void testApp::onOpen( ofxLibwebsockets::Event& args ){
    cout<<"new connection open"<<endl;
    messages.push_back("New connection from " + args.conn.getClientIP() );
    
    // send the latest image if there is one!
    if ( currentImage.bAllocated() ){
        unsigned long size;
        unsigned char * compressed = turbo.compress(&currentImage,100,&size);
        args.conn.sendBinary(compressed, size);
        free(compressed);
    }
}

//--------------------------------------------------------------
void testApp::onClose( ofxLibwebsockets::Event& args ){
    cout<<"on close"<<endl;
    messages.push_back("Connection closed");
}

//--------------------------------------------------------------
void testApp::onIdle( ofxLibwebsockets::Event& args ){
//    cout<<"on idle"<<endl;
}

//--------------------------------------------------------------
void testApp::onMessage( ofxLibwebsockets::Event& args ){
    if ( args.isBinary ){
        cout << "BINARY! "<<locked<<endl;
        if ( locked ) return;
        // need to load this next frame!
        buff.clear();
        buff.set(args.data.getBinaryBuffer(), args.data.size());
        locked = true;
        needToLoad = true;
    } else {
        cout<<"got message "<<args.message<<endl;
        
        // trace out string messages or JSON messages!
        // args.json is null if badly formed or just not JOSN
        if ( !args.json.isNull() ){
            messages.push_back("New message: " + args.json.toStyledString() + " from " + args.conn.getClientName() );
        } else {
            messages.push_back("New message: " + args.message + " from " + args.conn.getClientName() );
        }
        
        //if (messages.size() > NUM_MESSAGES) messages.erase( messages.begin() );
        
        // echo server = send message right back!
        args.conn.send( args.message );
    }
}

//--------------------------------------------------------------
void testApp::onBroadcast( ofxLibwebsockets::Event& args ){
    cout<<"got broadcast "<<args.message<<endl;    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){}

//--------------------------------------------------------------
void testApp::keyReleased(int key){}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){}

//-----------------------------------------------------------   ---
void testApp::mousePressed(int x, int y, int button){}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 
    for (int i=0; i<dragInfo.files.size(); i++){
        string file = dragInfo.files[i];
        ofFile f(file); 
        string ex = f.getExtension();
        std::transform(ex.begin(), ex.end(),ex.begin(), ::toupper);
        
        if ( ex == "JPG" || ex == "JPEG" || ex == "PNG" || ex == "GIF" ){
            toLoad = file;
            bSendImage = true;
        }
    }
}