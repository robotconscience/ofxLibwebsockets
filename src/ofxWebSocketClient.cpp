//
//  ofxWebSocketClient.cpp
//  LibwebsocketClient
//
//  Created by Brett Renfer on 4/12/12.
//  Copyright (c) 2012 Robotconscience. All rights reserved.
//

#include "ofxWebSocketClient.h"
#include "ofxWebSocketUtil.h"

ofxWebSocketClient::ofxWebSocketClient(){
    context = NULL;
    waitMillis = 1000;
    count_pollfds = 0;
    reactors.push_back(this);
    ofAddListener( clientProtocol.oncloseEvent, this, &ofxWebSocketClient::onClose);
}

//--------------------------------------------------------------
bool ofxWebSocketClient::connect ( string _address, bool bUseSSL ){
    connect( _address, (bUseSSL ? 443 : 80), bUseSSL, "/" );
}

//--------------------------------------------------------------
bool ofxWebSocketClient::connect ( string _address, int _port, bool bUseSSL ){
    connect( _address, _port, bUseSSL, "/" );
}

//--------------------------------------------------------------
bool ofxWebSocketClient::connect ( string _address, int _port, bool bUseSSL, string _channel ){
    cout<<"connect: "<<_address<<":"<<_port<<_channel<<":"<<bUseSSL<<endl;
    address = _address;
    port    = _port;  
    channel = _channel;
    
    // set up default protocols
    struct libwebsocket_protocols null_protocol = { NULL, NULL, 0 };
    
    // should this just be http? channel seems to just be path related?
    registerProtocol( _channel, clientProtocol );  
    
    lws_protocols.clear();
    for (int i=0; i<protocols.size(); ++i)
    {
        struct libwebsocket_protocols lws_protocol = {
            protocols[i].first.c_str(),
            lws_client_callback,
            sizeof(ofxWebSocketConnection)
        };
        lws_protocols.push_back(lws_protocol);
    }
    lws_protocols.push_back(null_protocol);
    
    context = libwebsocket_create_context(CONTEXT_PORT_NO_LISTEN, NULL,
                                          &lws_protocols[0], libwebsocket_internal_extensions,
                                          NULL, NULL, -1, -1, 0);
    if (context == NULL){
        std::cerr << "libwebsocket init failed" << std::endl;
        return false;
    } else {      
        std::cerr << "libwebsocket init success" << std::endl;  
        
        string host = address +":"+ ofToString( port );
        
        lwsconnection = libwebsocket_client_connect( context, address.c_str(), port, (bUseSSL ? 2 : 0 ), channel.c_str(), host.c_str(), host.c_str(), NULL, -1);
        
        if ( lwsconnection == NULL ){
            std::cerr << "client connection failed" << std::endl;
            return false;
        } else {
            
            connection = new ofxWebSocketConnection( (ofxWebSocketReactor*) &context, &clientProtocol );
            connection->ws = lwsconnection;
                                            
            std::cerr << "client connection success" << std::endl;
            startThread(true, false); // blocking, non-verbose   
            return true;
        }
    }
}

//--------------------------------------------------------------
bool ofxWebSocketClient::connect ( string _address, int _port, bool bUseSSL, string _channel, string protocol ){
    address = _address;
    port    = _port;  
    channel = _channel;
    
    // set up default protocols
    struct libwebsocket_protocols http_protocol = { "http", lws_client_callback, 0 };
    struct libwebsocket_protocols null_protocol = { NULL, NULL, 0 };
    
    registerProtocol( protocol, clientProtocol );  
    
    lws_protocols.clear();
    for (int i=0; i<protocols.size(); ++i)
    {
        struct libwebsocket_protocols lws_protocol = {
            protocols[i].first.c_str(),
            lws_client_callback,
            sizeof(ofxWebSocketConnection)
        };
        lws_protocols.push_back(lws_protocol);
    }
    lws_protocols.push_back(http_protocol);
    lws_protocols.push_back(null_protocol);
    
    if ( context == NULL ){
        context = libwebsocket_create_context(CONTEXT_PORT_NO_LISTEN, NULL,
                                              &lws_protocols[0], libwebsocket_internal_extensions,
                                              NULL, NULL, -1, -1, 0);
    }
    
    if (context == NULL){
        std::cerr << "libwebsocket init failed" << std::endl;
        return false;
    } else {      
        std::cerr << "libwebsocket init success" << std::endl;  
        
        string host = address +":"+ ofToString( port );
        
        lwsconnection = libwebsocket_client_connect( context, address.c_str(), port, (bUseSSL ? 2 : 0 ), channel.c_str(), host.c_str(), host.c_str(), lws_protocols[0].name, -1);
        
        if ( lwsconnection == NULL ){
            std::cerr << "client connection failed" << std::endl;
            return false;
        } else {
            
            connection = new ofxWebSocketConnection( (ofxWebSocketReactor*) &context, &clientProtocol );
            connection->ws = lwsconnection;
            
            std::cerr << "client connection success" << std::endl;
            startThread(true, false); // blocking, non-verbose   
            return true;
        }
    }
}

//--------------------------------------------------------------
void ofxWebSocketClient::close(){
    if (isThreadRunning()){
        stopThread();
    }
    if ( context != NULL){
        libwebsocket_close_and_free_session( context, lwsconnection, LWS_CLOSE_STATUS_NORMAL);
        libwebsocket_context_destroy( context );
        context = NULL;        
        lwsconnection = NULL;
        delete connection;
        connection = NULL;
    }
}


//--------------------------------------------------------------
void ofxWebSocketClient::onClose( ofxWebSocketEvent& args ){
    close();
}

//--------------------------------------------------------------
void ofxWebSocketClient::send( string message ){
    if ( connection != NULL){
        connection->send( message );
    }
}

//--------------------------------------------------------------
void ofxWebSocketClient::threadedFunction(){
    while ( isThreadRunning() ){
        for (int i=0; i<protocols.size(); ++i){
            if (protocols[i].second != NULL){
                //lock();
                protocols[i].second->execute();
                //unlock();
            }
        }
        libwebsocket_callback_on_writable(context, lwsconnection);
        libwebsocket_service(context, waitMillis);
    }
    
}