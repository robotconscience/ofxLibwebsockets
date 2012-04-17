//
//  ofxWebSocketClient.h
//  LibwebsocketClient
//
//  Created by Brett Renfer on 4/11/12.
//  Copyright (c) 2012 Robotconscience. All rights reserved.
//

#pragma once

#include <libwebsockets.h>
#include "ofxWebSocketReactor.h"

class ofxWebSocketClient;
class ofxWebSocketClient : public ofxWebSocketReactor {
    friend class ofxWebSocketProtocol;
public:  
    
    ofxWebSocketClient();
    
    bool connect ( string _address = "localhost", int _port=7681 );
    bool connect ( string _address, int _port, string _channel );
    
    // this is ugly for now, should be a smarter way?
    bool connect ( string _address, int _port, string _channel, string protocol );
    
    void close();
    
    void send( string message );
    
    template<class T>
    void addListener(T * app){
        ofAddListener( clientProtocol.onconnectEvent, app, &T::onConnect); 
        ofAddListener( clientProtocol.onopenEvent, app, &T::onOpen);
        ofAddListener( clientProtocol.oncloseEvent, app, &T::onClose);
        ofAddListener( clientProtocol.onidleEvent, app, &T::onIdle);
        ofAddListener( clientProtocol.onmessageEvent, app, &T::onMessage);
        ofAddListener( clientProtocol.onbroadcastEvent, app, &T::onBroadcast);
    }
    
    ofxWebSocketConnection * getConnection(){
        return connection;
    }
    
private:

    ofxWebSocketConnection * connection;
    
    struct libwebsocket *  lwsconnection;
    void threadedFunction();
    
    //wrap protocol
    ofxWebSocketProtocol clientProtocol;
    
};