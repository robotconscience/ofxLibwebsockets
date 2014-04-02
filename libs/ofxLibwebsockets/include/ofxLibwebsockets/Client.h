//
//  Client.h
//  ofxLibwebsockets
//
//  Created by Brett Renfer on 4/11/12.
//  Copyright (c) 2012 Robotconscience. All rights reserved.
//

#pragma once

#include <libwebsockets.h>
#include "ofxLibwebsockets/Reactor.h"

namespace ofxLibwebsockets {

    struct ClientOptions {
        string  host;
        int     port;
        bool    bUseSSL;
        string  channel;
        string  protocol;
        int     version;
    };
    
    static ClientOptions defaultClientOptions(){
        ClientOptions opts;
        opts.host     = "localhost";
        opts.port     = 80;
        opts.bUseSSL  = false;
        opts.channel  = "/";
        opts.protocol = "NULL";
        opts.version  = -1;     //use latest version
        return opts;
    };
    
    class Client : public Reactor {
        friend class Protocol;
    public:  
        
        Client();
        ~Client();
        
        // Note: the boolean returned here == libwebsockets setup success
        // You will receive an "onOpen" event on successful connect
        // and "onClose" on unsuccessful
        bool connect ( string _address, bool bUseSSL=false );
        bool connect ( string _address, int _port, bool bUseSSL=false );
        bool connect ( ClientOptions options );
        
        void onClose( Event& args );
        void close();
        
        void send( string message );
        
        // send anything that has pixels
        template <class T>
        void sendBinary( T& image );
        
        // send any binary data
        void sendBinary( ofBuffer buffer );
        void sendBinary( unsigned char * data, int size );
        void sendBinary( char * data, int size );
        
        template<class T>
        void addListener(T * app){
            ofAddListener( clientProtocol.onconnectEvent, app, &T::onConnect); 
            ofAddListener( clientProtocol.onopenEvent, app, &T::onOpen);
            ofAddListener( clientProtocol.oncloseEvent, app, &T::onClose);
            ofAddListener( clientProtocol.onidleEvent, app, &T::onIdle);
            ofAddListener( clientProtocol.onmessageEvent, app, &T::onMessage);
            ofAddListener( clientProtocol.onbroadcastEvent, app, &T::onBroadcast);
        }
        
        Connection * getConnection(){
            return connection;
        }
        
    protected:
        ClientOptions defaultOptions;
        
    private:

        Connection * connection;
        
        struct libwebsocket *  lwsconnection;
        void threadedFunction();
        
        //wrap protocol
        Protocol clientProtocol;
        
    };
};