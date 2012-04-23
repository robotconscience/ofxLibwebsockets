//
//  Reactor.h
//  ofxLibwebsockets
//
//  Created by Brett Renfer on 4/11/12.
//  Copyright (c) 2012 Robotconscience. All rights reserved.
//

#pragma once

#include "ofThread.h"
#include "ofEvents.h"
#include <libwebsockets.h>
#include "ofxLibwebsockets/Protocol.h"
#include "ofxLibwebsockets/Connection.h"

namespace ofxLibwebsockets {
        
    class Reactor : public ofThread {
        friend class Protocol;
        
    public:
        Reactor();
        ~Reactor();
        
        void close(Connection* const conn);
        
        void exit();
        void registerProtocol(const std::string& name, Protocol& protocol);
        
        Protocol* const protocol(const unsigned int idx);
        
        //private:
        unsigned int _allow(Protocol* const protocol, const long fd);
        
        unsigned int _notify(Connection* const conn,
                             enum libwebsocket_callback_reasons const reason,
                             const char* const _message,
                             const unsigned int len);
        
        unsigned int _http(struct libwebsocket *ws,
                           const char* const url);
        
        std::vector<std::pair<std::string, Protocol*> > protocols;
        
        struct libwebsocket_context * getContext();
        
        struct pollfd pollfds[100];
        int count_pollfds;
        
    protected:
        std::string     document_root;
        unsigned int    waitMillis;
        std::string     interface;
        
        virtual void threadedFunction(){};  
        
        string address;
        string channel;
        int port;
        
        struct libwebsocket_context *context;
        
        std::vector<struct libwebsocket_protocols> lws_protocols;
    };
    
    static vector<Reactor *> reactors;
};