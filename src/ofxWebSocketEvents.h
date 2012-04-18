#pragma once 

#include <string>
#include "json.h"

class ofxWebSocketConnection;

class ofxWebSocketEvent {
public:
    ofxWebSocketEvent(ofxWebSocketConnection& _conn, std::string& _message);
    
    ofxWebSocketConnection& conn;
    std::string& message;
    Json::Value json;
};

/*
 class _ofxWebSocketEvents {
 public:
 ofEvent<ofxWebSocketEvent> onConnect;
 ofEvent<ofxWebSocketEvent> onOpen;
 ofEvent<ofxWebSocketEvent> onClose;
 ofEvent<ofxWebSocketEvent> onIdle;
 ofEvent<ofxWebSocketEvent> onMessage;
 ofEvent<ofxWebSocketEvent> onBroadcast;
 };
 extern _ofxWebSocketEvents ofxWebSocketEvents;*/