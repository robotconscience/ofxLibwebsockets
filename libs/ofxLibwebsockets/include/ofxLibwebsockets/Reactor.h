//
//  Reactor.h
//  ofxLibwebsockets
//
//  Created by Brett Renfer on 4/11/12. 
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
        void exit();
        
        // methods
        virtual void close() = 0; // close main ws connection (or close server)
        void close(Connection* const conn); // close a specific connection
        void registerProtocol(const std::string& name, Protocol& protocol);
        
        // parse JSON automatically? (true by default)
        bool bParseJSON;
        
        // getters
        struct lws_context *   getContext();
        vector<Connection *>            getConnections();
        Connection *                    getConnection( int index );
        
        Protocol* const protocol(string name);
        std::map<std::string, Protocol* > protocols;
        
        //private:
        unsigned int _allow(struct lws *ws, Protocol* const protocol, const long fd);
        
        unsigned int _notify(Connection* conn, enum lws_callback_reasons const reason,
                             const char* const _message, const unsigned int len);
        
        unsigned int _http(struct lws *ws, const char* const url);
        
    protected:
        std::string     document_root;
        unsigned int    waitMillis;
        std::string     interfaceStr;
        
        bool            bReceivingLargeMessage;
        std::string     largeMessage;
        
        unsigned int    largeBinarySize;
        ofBuffer        largeBinaryMessage;

        bool closeAndFree;
        
        virtual void threadedFunction(){};  
        
        string address;
        string channel;
        int port;
        
        struct lws_context *context;
        
        std::vector<struct lws_protocols> internal_protocols;
        
        // maybe this should be a map?
        vector<Connection *> connections;
        
    };
    
    extern vector<Reactor *> reactors;
};
