//
//  Server.cpp
//  ofxLibwebsockets
//
//  Created by Brett Renfer on 4/11/12.
//  Copyright (c) 2012 Robotconscience. All rights reserved.
//

#include "ofxLibwebsockets/Server.h"
#include "ofxLibwebsockets/Util.h"

#include "ofEvents.h"
#include "ofUtils.h"

namespace ofxLibwebsockets {

    //--------------------------------------------------------------
    Server::Server(){
        context = NULL;
        waitMillis = 50;
        count_pollfds = 0;
        reactors.push_back(this);
    }

    //--------------------------------------------------------------
    bool Server::setup( const short _port )
    {
        // setup with default protocol (http) and allow ALL other protocols
        setup( _port, "/", true );
    }

    //--------------------------------------------------------------
    bool Server::setup( const short _port, string protocol, bool bAllowAllProtocols ){
        port = _port;  
        // libwebsockets isn't compiled with SSL support right now!
        bool useSSL = false;//(!_sslCertFilename.empty() && !_sslKeyFilename.empty());
        
        std::string sslCertPath, sslKeyPath;
        const char *_sslCertPath = NULL;
        const char *_sslKeyPath = NULL;
        
        /*if (useSSL)
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
         }*/
        
        // where our webserver is loading files from
        if (document_root.empty()){
            document_root = "web";        
        }
        if (document_root.at(0) != '/'){
            document_root = ofToDataPath(document_root, true);
        }
        
        // NULL protocol is required by LWS
        struct libwebsocket_protocols null_protocol = { NULL, NULL, 0 };
        
        // NULL name = any protocol
        struct libwebsocket_protocols null_name_protocol = { NULL, lws_callback, sizeof(Connection) };
        
        //setup protocols
        lws_protocols.clear();
        
        // register main protocol     
        registerProtocol( protocol, serverProtocol );            
        for (int i=0; i<protocols.size(); ++i)
        {
            struct libwebsocket_protocols lws_protocol = {
                protocols[i].first.c_str(),
                lws_callback,
                sizeof(Connection)
            };
            lws_protocols.push_back(lws_protocol);
        }
        
        if ( bAllowAllProtocols ){
            lws_protocols.push_back(null_name_protocol); 
        }
        lws_protocols.push_back(null_protocol);
        
        int opts = 0;
        context = libwebsocket_create_context( port, NULL, &lws_protocols[0],
                                              libwebsocket_internal_extensions, _sslCertPath, _sslKeyPath,
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
    void Server::threadedFunction()
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
}
