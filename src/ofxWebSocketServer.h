#pragma once

#include "ofThread.h"
#include "ofEvents.h"

#include <libwebsockets.h>

#include "ofxWebSocketUtil.h"
#include "ofxWebSocketReactor.h"

class ofxWebSocketConnection;
class ofxWebSocketProtocol;

class ofxWebSocketServer;

class ofxWebSocketServer : public ofxWebSocketReactor {
    friend class ofxWebSocketProtocol;
    
public:
    ofxWebSocketServer();
    
    bool setup(const short _port=7681,
               const std::string sslCertFilename="libwebsockets-test-server.pem",
               const std::string sslKeyFilename="libwebsockets-test-server.key.pem");
    
    void exit();
    
    ofxWebSocketProtocol* const protocol(const unsigned int idx);
    
protected:
    std::string interface;
    
private:
    void threadedFunction();  
};
