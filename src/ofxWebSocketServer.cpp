#include "ofxWebSocket.h"

#include "ofEvents.h"
#include "ofUtils.h"

ofxWebSocketServer::ofxWebSocketServer(){
    context = NULL;
    waitMillis = 50;
    count_pollfds = 0;
    reactors.push_back(this);
}

//--------------------------------------------------------------
bool ofxWebSocketServer::setup(const short _port,
                          const std::string _sslCertFilename,
                          const std::string _sslKeyFilename)
{
    port = _port;  
    bool useSSL = (!_sslCertFilename.empty() && !_sslKeyFilename.empty());
    
    std::string sslCertPath, sslKeyPath;
    const char *_sslCertPath = NULL;
    const char *_sslKeyPath = NULL;
    if (useSSL)
    {
        if (_sslCertFilename.at(0) == '/')
            sslCertPath = _sslCertFilename;
        else
            sslCertPath = ofToDataPath(_sslCertFilename, true);
        _sslCertPath = sslCertPath.c_str();
        
        if (_sslKeyFilename.at(0) == '/')
            sslKeyPath = _sslKeyFilename;
        else
            sslKeyPath = ofToDataPath(_sslKeyFilename, true);
        _sslKeyPath = sslKeyPath.c_str();
    }  
    
    if (document_root.empty())
        document_root = "web";
    
    if (document_root.at(0) != '/')
        document_root = ofToDataPath(document_root, true);
    
    struct libwebsocket_protocols http_protocol = { "http", lws_callback, 0 };
    struct libwebsocket_protocols null_protocol = { NULL, NULL, 0 };
    
    lws_protocols.clear();
    lws_protocols.push_back(http_protocol);
    for (int i=0; i<protocols.size(); ++i)
    {
        struct libwebsocket_protocols lws_protocol = {
            protocols[i].first.c_str(),
            lws_callback,
            sizeof(ofxWebSocketConnection)
        };
        lws_protocols.push_back(lws_protocol);
    }
    lws_protocols.push_back(null_protocol);
    
    int opts = 0;
    context = libwebsocket_create_context(port, interface.c_str(),
                                          &lws_protocols[0],
                                          libwebsocket_internal_extensions,
                                          _sslCertPath, _sslKeyPath,
                                          -1, -1, opts);
    
	if (context == NULL){
        std::cerr << "libwebsocket init failed" << std::endl;
        return false;
    } else {
        startThread(true, false); // blocking, non-verbose        
        return true;
    }
}

//--------------------------------------------------------------
void ofxWebSocketServer::threadedFunction()
{
    while (isThreadRunning())
    {
        for (int i=0; i<protocols.size(); ++i)
            if (protocols[i].second != NULL){
                //lock();
                protocols[i].second->execute();
                //unlock();
            }
        libwebsocket_service(context, waitMillis);
    }
}
