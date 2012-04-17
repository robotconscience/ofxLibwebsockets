#pragma once

#include "ofThread.h"
#include "ofEvents.h"

#include <libwebsockets.h>

#include "ofxWebSocketReactor.h"

class ofxWebSocketConnection;
class ofxWebSocketProtocol;

class ofxWebSocketServer;

class ofxWebSocketServer : public ofxWebSocketReactor {
    friend class ofxWebSocketProtocol;
    
public:
    ofxWebSocketServer();
    
    bool setup( const short _port=7681 );
    bool setup( const short _port, string protocol );
    
    template<class T>
    void addListener(T * app){
        ofAddListener( serverProtocol.onconnectEvent, app, &T::onConnect); 
        ofAddListener( serverProtocol.onopenEvent, app, &T::onOpen);
        ofAddListener( serverProtocol.oncloseEvent, app, &T::onClose);
        ofAddListener( serverProtocol.onidleEvent, app, &T::onIdle);
        ofAddListener( serverProtocol.onmessageEvent, app, &T::onMessage);
        ofAddListener( serverProtocol.onbroadcastEvent, app, &T::onBroadcast);
    }
    
    void exit();
    
    ofxWebSocketProtocol* const protocol(const unsigned int idx);
    
protected:
    std::string interface;
    
private:
    ofxWebSocketProtocol serverProtocol;
    void threadedFunction();  
};
