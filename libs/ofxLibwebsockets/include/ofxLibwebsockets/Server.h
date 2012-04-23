//
//  Server.h
//  ofxLibwebsockets
//
//  Created by Brett Renfer on 4/11/12.
//  Copyright (c) 2012 Robotconscience. All rights reserved.
//

#pragma once

#include "ofThread.h"
#include "ofEvents.h"

#include <libwebsockets.h>

#include "ofxLibwebsockets/Reactor.h"

namespace ofxLibwebsockets {

    class Connection;
    class Protocol;

    class Server : public Reactor {
        friend class Protocol;
        
    public:
        Server();
        
        bool setup( const short _port=80 );
        bool setup( const short _port, string protocol, bool bAllowAllProtocols = true );
        
        template<class T>
        void addListener(T * app){
            ofAddListener( serverProtocol.onconnectEvent, app, &T::onConnect); 
            ofAddListener( serverProtocol.onopenEvent, app, &T::onOpen);
            ofAddListener( serverProtocol.oncloseEvent, app, &T::onClose);
            ofAddListener( serverProtocol.onidleEvent, app, &T::onIdle);
            ofAddListener( serverProtocol.onmessageEvent, app, &T::onMessage);
            ofAddListener( serverProtocol.onbroadcastEvent, app, &T::onBroadcast);
        }
        
    protected:
        std::string interface;
        
    private:
        Protocol serverProtocol;
        void threadedFunction();  
    };
};