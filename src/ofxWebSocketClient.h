//
//  ofxWebSocketClient.h
//  LibwebsocketClient
//
//  Created by Brett Renfer on 4/11/12.
//  Copyright (c) 2012 Robotconscience. All rights reserved.
//

#pragma once

#include <libwebsockets.h>
#include "ofxWebSocketUtil.h"
#include "ofxWebSocketReactor.h"

class ofxWebSocketClient : public ofxWebSocketReactor {
    friend class ofxWebSocketProtocol;
public:  
    
    ofxWebSocketClient();
    
    bool connect ( string _address = "localhost", int _port=7681, string _channel = "/",  bool bUseSSL = false);
    
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
    
private:

    ofxWebSocketConnection * connection;
    
    struct libwebsocket *  lwsconnection;
    void threadedFunction();
    
    //wrap protocol
    ofxWebSocketProtocol clientProtocol;
    
};