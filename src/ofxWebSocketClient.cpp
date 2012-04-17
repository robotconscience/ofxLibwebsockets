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
}

bool ofxWebSocketClient::connect ( string _address, int _port){
    address = _address;
    port    = _port;  
    
    // set up default protocols
    //struct libwebsocket_protocols http_protocol = { "http", lws_client_callback, 0 };
    struct libwebsocket_protocols null_protocol = { NULL, NULL, 0 };
    
    registerProtocol( "http", clientProtocol );
    
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
    
    //libwebsocket_extension webkit_extension = { "x-webkit-deflate-frame", NULL, NULL, NULL };
    
    context = libwebsocket_create_context(CONTEXT_PORT_NO_LISTEN, NULL,
                                          &lws_protocols[0], libwebsocket_internal_extensions,
                                          NULL, NULL, -1, -1, 0);
    if (context == NULL){
        std::cerr << "libwebsocket init failed" << std::endl;
        return false;
    } else {      
        std::cerr << "libwebsocket init success" << std::endl;  
        
        string host = address +":"+ ofToString( port );
        
        lwsconnection = libwebsocket_client_connect( context, address.c_str(), port, 0, "/", host.c_str(), host.c_str(), NULL, -1);
        
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

bool ofxWebSocketClient::connect ( string _address, int _port, string _channel ){
    address = _address;
    port    = _port;  
    channel = _channel;
    
    // set up default protocols
    struct libwebsocket_protocols http_protocol = { "http", lws_client_callback, 0 };
    struct libwebsocket_protocols null_protocol = { NULL, NULL, 0 };
    
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
    lws_protocols.push_back(http_protocol);
    lws_protocols.push_back(null_protocol);
    
    //libwebsocket_extension webkit_extension = { "x-webkit-deflate-frame", NULL, NULL, NULL };
            
    context = libwebsocket_create_context(CONTEXT_PORT_NO_LISTEN, NULL,
                                          &lws_protocols[0], libwebsocket_internal_extensions,
                                          NULL, NULL, -1, -1, 0);
    if (context == NULL){
        std::cerr << "libwebsocket init failed" << std::endl;
        return false;
    } else {      
        std::cerr << "libwebsocket init success" << std::endl;  
        
        string host = address +":"+ ofToString( port );
        
        // where it says NULL should maybe be a channel?
        lwsconnection = libwebsocket_client_connect( context, address.c_str(), port, 0, channel.c_str(), host.c_str(), host.c_str(), lws_protocols[0].name, -1);
        
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
    stopThread();
    libwebsocket_close_and_free_session( context, lwsconnection, LWS_CLOSE_STATUS_NORMAL);
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
        /*int n = poll(pollfds, count_pollfds, 25);
        if (n < 0){
            cout<<"stop thread"<<endl;
            stopThread();
        }
        if (n){
            for (n = 0; n < count_pollfds; n++){
                if (pollfds[n].revents){
                    cout<<"service"<<endl;
                    libwebsocket_service_fd(context, &pollfds[n]);
                }
            }
        }*/
    }
    
}