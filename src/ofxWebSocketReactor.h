//
//  ofxWebSocketReactor.h
//  LibwebsocketClient
//
//  Created by Brett Renfer on 4/12/12.
//  Copyright (c) 2012 Robotconscience. All rights reserved.
//

#pragma once

#include "ofThread.h"
#include "ofEvents.h"
#include <libwebsockets.h>
#include "ofxWebSocketProtocol.h"
#include "ofxWebSocketConnection.h"

class ofxWebSocketConnection;
class ofxWebSocketProtocol;

class ofxWebSocketReactor : public ofThread {
    friend class ofxWebSocketProtocol;
    
public:
    ofxWebSocketReactor();
    ~ofxWebSocketReactor();
    
    void close(ofxWebSocketConnection* const conn);
    
    void exit();
    void registerProtocol(const std::string& name, ofxWebSocketProtocol& protocol);
    
    ofxWebSocketProtocol* const protocol(const unsigned int idx);
    
    //private:
    unsigned int _allow(ofxWebSocketProtocol* const protocol, const long fd);
    
    unsigned int _notify(ofxWebSocketConnection* const conn,
                         enum libwebsocket_callback_reasons const reason,
                         const char* const _message,
                         const unsigned int len);
    
    unsigned int _http(struct libwebsocket *ws,
                       const char* const url);
    
    std::string document_root;
    std::vector<std::pair<std::string, ofxWebSocketProtocol*> > protocols;
    
    struct libwebsocket_context * getContext();
    
    struct pollfd pollfds[100];
    int count_pollfds;
    
protected:
    unsigned int waitMillis;
    std::string interface;
    
    virtual void threadedFunction(){};  
    
    string address;
    string channel;
    int port;
    
	struct libwebsocket_context *context;
    
    std::vector<struct libwebsocket_protocols> lws_protocols;
};

static vector<ofxWebSocketReactor *> reactors;