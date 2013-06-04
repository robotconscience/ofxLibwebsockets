//
//  Client.cpp
//  ofxLibwebsockets
//
//  Created by Brett Renfer on 4/11/12.
//  Copyright (c) 2012 Robotconscience. All rights reserved.
//

#include "ofxLibwebsockets/Client.h"
#include "ofxLibwebsockets/Util.h"

namespace ofxLibwebsockets {

    Client::Client(){
        context = NULL;
        connection = NULL;
        waitMillis = 50;
        //count_pollfds = 0;
        reactors.push_back(this);
        
        defaultOptions = defaultClientOptions();
        
        ofAddListener( clientProtocol.oncloseEvent, this, &Client::onClose);
    }

    //--------------------------------------------------------------
    bool Client::connect ( string _address, bool bUseSSL ){
        defaultOptions.host     = _address;
        defaultOptions.bUseSSL  = bUseSSL;
        defaultOptions.port     = (bUseSSL ? 443 : 80);
        
        return connect( defaultOptions );
    }

    //--------------------------------------------------------------
    bool Client::connect ( string _address, int _port, bool bUseSSL ){
        defaultOptions.host     = _address;
        defaultOptions.port     = _port;
        defaultOptions.bUseSSL  = bUseSSL;
        
        return connect( defaultOptions );
    }

    //--------------------------------------------------------------
    bool Client::connect ( ClientOptions options ){
        ofLog( OF_LOG_VERBOSE, "connect: "+options.host+":"+ofToString(options.port)+options.channel+":"+ofToString(options.bUseSSL) );
        address = options.host;
        port    = options.port;  
        channel = options.channel;
        
		/*
			enum lws_log_levels {
			LLL_ERR = 1 << 0,
			LLL_WARN = 1 << 1,
			LLL_NOTICE = 1 << 2,
			LLL_INFO = 1 << 3,
			LLL_DEBUG = 1 << 4,
			LLL_PARSER = 1 << 5,
			LLL_HEADER = 1 << 6,
			LLL_EXT = 1 << 7,
			LLL_CLIENT = 1 << 8,
			LLL_LATENCY = 1 << 9,
			LLL_COUNT = 10 
		};
		*/
		lws_set_log_level(LLL_ERR, NULL);

        // set up default protocols
        struct libwebsocket_protocols null_protocol = { NULL, NULL, 0 };
        
        // setup the default protocol (the one that works when you do addListener())
        registerProtocol( options.protocol, clientProtocol );  
        
        lws_protocols.clear();
        for (int i=0; i<protocols.size(); ++i)
        {
            struct libwebsocket_protocols lws_protocol = {
                ( protocols[i].first == "NULL" ? NULL : protocols[i].first.c_str() ),
                lws_client_callback,
                sizeof(Connection)
            };
            lws_protocols.push_back(lws_protocol);
        }
        lws_protocols.push_back(null_protocol);

        struct lws_context_creation_info info;
        memset(&info, 0, sizeof info);
        info.port = CONTEXT_PORT_NO_LISTEN;
        info.protocols = &lws_protocols[0];
        info.extensions = libwebsocket_get_internal_extensions();
        info.gid = -1;
        info.uid = -1;

        context = libwebsocket_create_context(&info);

        
        //context = libwebsocket_create_context(CONTEXT_PORT_NO_LISTEN, NULL,
        //                                      &lws_protocols[0], libwebsocket_internal_extensions,
        //                                      NULL, NULL, /*NULL,*/ -1, -1, 0, NULL);
        if (context == NULL){
            std::cerr << "libwebsocket init failed" << std::endl;
            return false;
        } else {      
            std::cerr << "libwebsocket init success" << std::endl;  
            
            string host = options.host +":"+ ofToString( options.port );
            
            // register with or without a protocol
            if ( options.protocol == "NULL"){
                lwsconnection = libwebsocket_client_connect( context, 
                                                            options.host.c_str(), options.port, (options.bUseSSL ? 2 : 0 ), 
                                                            options.channel.c_str(), host.c_str(), host.c_str(), NULL, options.version);
            } else {
                lwsconnection = libwebsocket_client_connect( context, 
                                                            options.host.c_str(), options.port, (options.bUseSSL ? 2 : 0 ), 
                                                            options.channel.c_str(), host.c_str(), host.c_str(), options.protocol.c_str(), options.version);
            }
                        
            if ( lwsconnection == NULL ){
                std::cerr << "client connection failed" << std::endl;
                return false;
            } else {
                
                connection = new Connection( (Reactor*) &context, &clientProtocol );
                connection->ws = lwsconnection;
                                                
                std::cerr << "client connection success" << std::endl;
                startThread(true, false); // blocking, non-verbose   
                return true;
            }
        }
    }

    //--------------------------------------------------------------
    void Client::close(){
        if (isThreadRunning()){
            waitForThread(true);
        } else {
			return;
		}
        if ( context != NULL ){
            //libwebsocket_close_and_free_session( context, lwsconnection, LWS_CLOSE_STATUS_NORMAL);
            closeAndFree = true;
            libwebsocket_context_destroy( context );
            context = NULL;        
            lwsconnection = NULL;
        }
		if ( connection != NULL){
                //delete connection;
			connection = NULL;                
		}
    }


    //--------------------------------------------------------------
    void Client::onClose( Event& args ){
		// on windows an exit of the server let's the client crash
		// the event is called from the processing of the thread
		// thus all we can do wait for the thread to stop itself
		// by detecting that lwsconnection is NULL
		if ( context != NULL ){
			closeAndFree = true;
			lwsconnection = NULL;
		}     
    }

    //--------------------------------------------------------------
    void Client::send( string message ){
        if ( connection != NULL){
            connection->send( message );
        }
    }

    //--------------------------------------------------------------
    void Client::threadedFunction(){
        while ( isThreadRunning() ){
            for (int i=0; i<protocols.size(); ++i){
                if (protocols[i].second != NULL){
                    //lock();
                    protocols[i].second->execute();
                    //unlock();
                }
            }
            if (context != NULL && lwsconnection != NULL){
                //libwebsocket_callback_on_writable(context, lwsconnection);
                lock();
                int n = libwebsocket_service(context, waitMillis);
                unlock();
            } else {
				stopThread();
				if ( context != NULL ){
					closeAndFree = true;
					libwebsocket_context_destroy( context );
					context = NULL;        
					lwsconnection = NULL;
				}
				if (connection != NULL){
					connection = NULL;                
				}
            }
        }
    }
}
