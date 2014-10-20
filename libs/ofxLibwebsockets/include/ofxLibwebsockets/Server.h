//
//  Server.h
//  ofxLibwebsockets
//
//  Created by Brett Renfer on 4/11/12. 
//
//  SETUP PROCESS:
//  1) initialize (automatic if not a pointer)
//  2) (optional) add protocols (e.g. server.addProtocol( "hey", myProtocol ); )
//      - see Reactor.h
//  3) (optional) add ofApp as a listener (defaults to "" protocol, can be set in options)
//  4) call setup( port ) or setup ( ServerOptions options )

#pragma once

#include "ofMain.h"

#include <libwebsockets.h>

#include "ofxLibwebsockets/Reactor.h"

namespace ofxLibwebsockets {

    class Connection;
    class Protocol;
    
    struct ServerOptions {
        int     port;               
        bool    bUseSSL;            // if you use ssl, you must connect clients to wss:// instead of ws://
        string  protocol;           // if you specify this, you must connect with a protocol on the client side
        string  sslCertPath;        // data path to ssl certificate
        string  sslKeyPath;         // data path to ssl key
        
        string  documentRoot;       // where your hosted files are (libwebsockets sets up a minimal webserver)
        
        // advanced: timeout options
        // names are from libwebsockets (ka == keep alive)
        int     ka_time;        // 0 == default, no timeout; nonzero == time to wait in seconds before testing conn
        int     ka_probes;      // # of times to test for connection; ignored if ka_time == 0
        int     ka_interval;    // how long to wait between probes, in seconds; ignored if ka_time == 0
    };
    
    static ServerOptions defaultServerOptions(){
        ServerOptions opts;
        opts.port           = 80;
        opts.protocol       = "NULL"; // NULL == no protocol. most websockets behave this way.
        opts.bUseSSL        = false;
        opts.sslCertPath    = ofToDataPath("ssl/libwebsockets-test-server.pem", true);
        opts.sslKeyPath     = ofToDataPath("ssl/libwebsockets-test-server.key.pem", true);
        opts.documentRoot   = ofToDataPath("web", true);
        opts.ka_time        = 0;
        opts.ka_probes      = 0;
        opts.ka_interval    = 0;
        return opts;
    }

    class Server : public Reactor {
        friend class Protocol;
        
    public:
        Server();
        ~Server();
        
        // setup with default options for websockets
        bool setup( int _port = 80, bool bUseSSL = false );
        
        // pass in ServerOptions object, which are easily instantiated
        // with ofxLibwebsockets::defaultServerOptions()
        bool setup( ServerOptions options );
        
        // close the server
        void close();
        
        // broadcast a message to all connections
        void broadcast( string message );
        
        // send to all connections 
        // (sends normal message instead of broadcast)
        void send( string message );
        
        // send anything that has pixels to all connections
        template <class T> 
        void sendBinary( T& image ){
            lock();
            for (int i=0; i<connections.size(); i++){
                if ( connections[i] != NULL ){
                    connections[i]->sendBinary( image );
                }
            }
            unlock();
        }
        
        // send any binary data to all connections
        void sendBinary( ofBuffer buffer );
        void sendBinary( unsigned char * data, int size );
        void sendBinary( char * data, int size );
        
        // send to a specific connection
        void send( string message, string ip );
        
        template<class T>
        void addListener(T * app){
            ofAddListener( serverProtocol.onconnectEvent, app, &T::onConnect); 
            ofAddListener( serverProtocol.onopenEvent, app, &T::onOpen);
            ofAddListener( serverProtocol.oncloseEvent, app, &T::onClose);
            ofAddListener( serverProtocol.onidleEvent, app, &T::onIdle);
            ofAddListener( serverProtocol.onmessageEvent, app, &T::onMessage);
            ofAddListener( serverProtocol.onbroadcastEvent, app, &T::onBroadcast);
        }
        
        template<class T>
        void removeListener(T * app){
            ofRemoveListener( serverProtocol.onconnectEvent, app, &T::onConnect);
            ofRemoveListener( serverProtocol.onopenEvent, app, &T::onOpen);
            ofRemoveListener( serverProtocol.oncloseEvent, app, &T::onClose);
            ofRemoveListener( serverProtocol.onidleEvent, app, &T::onIdle);
            ofRemoveListener( serverProtocol.onmessageEvent, app, &T::onMessage);
            ofRemoveListener( serverProtocol.onbroadcastEvent, app, &T::onBroadcast);
        }
        
        //getters
        int     getPort();
        string  getProtocol();
        bool    usingSSL();
        
    protected:
        std::string interfaceStr;
        ServerOptions defaultOptions;
        
    private:
        Protocol serverProtocol;
        void threadedFunction();  
    };
};