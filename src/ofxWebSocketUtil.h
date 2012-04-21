//
//  ofxWebSocketUtil.h
//  LibwebsocketClient
//
//  Created by Brett Renfer on 4/11/12.
//  Copyright (c) 2012 Robotconscience. All rights reserved.
//

#pragma once

#include <libwebsockets.h>
#include "ofxWebSocketConnection.h"
#include "ofxWebSocketReactor.h"
#include "ofxWebSocketClient.h"
#include "ofxWebSocketServer.h"

class ofxWebSocketClient;
class ofxWebSocketServer;

// CLIENT CALLBACK

static int lws_client_callback(struct libwebsocket_context* context, struct libwebsocket *ws, enum libwebsocket_callback_reasons reason, void *user, void *data, size_t len){
    const struct libwebsocket_protocols* lws_protocol = (ws == NULL ? NULL : libwebsockets_get_protocol(ws));
    int idx = lws_protocol? lws_protocol->protocol_index : 0;            
    
    ofxWebSocketConnection* conn;
    
    ofxWebSocketConnection** conn_ptr = (ofxWebSocketConnection**)user;
    ofxWebSocketReactor* reactor = NULL;
    ofxWebSocketProtocol* protocol;
        
    for (int i=0; i<reactors.size(); i++){
        if (reactors[i]->getContext() == context){
            reactor =  reactors[i];
            protocol = reactor->protocol(idx-1);
            conn = ((ofxWebSocketClient*) reactor)->getConnection();
            break;
        } else {
        }
    }
    
    if (reason == LWS_CALLBACK_CLIENT_ESTABLISHED){
    } else if (reason == LWS_CALLBACK_CLOSED){
    }
    
    switch (reason)
    {            
        case LWS_CALLBACK_CONFIRM_EXTENSION_OKAY:
            return 0;
            
        case LWS_CALLBACK_FILTER_NETWORK_CONNECTION:
            if (protocol != NULL){
                return reactor->_allow(protocol, (int)(long)user)? 0 : 1;
            } else {
                return 0;
            }
        case LWS_CALLBACK_HTTP:
            if ( reactor != NULL){
                return reactor->_http(ws, (char*)data);
            } else {
                return 0;
            }
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
        case LWS_CALLBACK_CLIENT_WRITEABLE:
        case LWS_CALLBACK_CLOSED:
        case LWS_CALLBACK_CLIENT_RECEIVE:
            if (reactor != NULL){
                //conn = *(ofxWebSocketConnection**)user;
                if (conn && conn->ws != ws) conn->ws = ws;
                return reactor->_notify(conn, reason, (char*)data, len);
            } else {
                return 0;
            }
        default:
            return 0;
    }

    return 1; // FAIL (e.g. unhandled case/break in switch)
}

// SERVER CALLBACK

static int lws_callback(struct libwebsocket_context* context, struct libwebsocket *ws, enum libwebsocket_callback_reasons reason, void *user, void *data, size_t len){
    const struct libwebsocket_protocols* lws_protocol = (ws == NULL ? NULL : libwebsockets_get_protocol(ws));
    int idx = lws_protocol? lws_protocol->protocol_index : 0;   
    
    bool bAllowAllProtocls = (ws != NULL ? lws_protocol == NULL : false);
        
    ofxWebSocketConnection* conn;    
    ofxWebSocketConnection** conn_ptr = (ofxWebSocketConnection**)user;
    ofxWebSocketServer* reactor = NULL;
    ofxWebSocketProtocol* protocol = NULL;
        
    for (int i=0; i<reactors.size(); i++){
        if (reactors[i]->getContext() == context){
            reactor = (ofxWebSocketServer*) reactors[i];
            protocol = reactor->protocol( (idx > 0 ? idx-1 : 0) );
            break;
        } else {
        }
    }
    
    if ( bAllowAllProtocls && reactor != NULL ){
        protocol = reactor->protocols[0].second;
    }
        
    if (reason == LWS_CALLBACK_ESTABLISHED){ 
        cout<<"yes"<<endl;
        
        if ( reactor != NULL ) *conn_ptr = new ofxWebSocketConnection(reactor, protocol);
    } else if (reason == LWS_CALLBACK_CLOSED){
        if (*conn_ptr != NULL)
        delete *conn_ptr;
    }
    
    switch (reason)
    {
        case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
            return 0;
            
        case LWS_CALLBACK_FILTER_NETWORK_CONNECTION:
            if (protocol != NULL ){
                if ( bAllowAllProtocls ){
                    protocol = reactor->protocols[0].second;
                    //conn = new ofxWebSocketConnection( reactor, protocol );
                    return reactor->_allow(protocol, (int)(long)user)? 0 : 1;
                }
                return reactor->_allow(protocol, (int)(long)user)? 0 : 1;
            } else {
                return 1;
            }
            
        case LWS_CALLBACK_HTTP:
            return reactor->_http(ws, (char*)data);
            
        case LWS_CALLBACK_ESTABLISHED:
        case LWS_CALLBACK_CLOSED:
        case LWS_CALLBACK_SERVER_WRITEABLE:
        case LWS_CALLBACK_RECEIVE:
        case LWS_CALLBACK_BROADCAST:
            conn = *(ofxWebSocketConnection**)user;
            if (conn && conn->ws != ws) conn->ws = ws;
            if (reactor){
                return reactor->_notify(conn, reason, (char*)data, len);                
            } else {
                return 0;
            }
            
        default:
            return 0;
    }
    
    return 1; // FAIL (e.g. unhandled case/break in switch)
}

static void dump_handshake_info(struct lws_tokens *lwst)
{
    int n;
    static const char *token_names[WSI_TOKEN_COUNT] = {
        "GET URI",
        "Host",
        "Connection",
        "key 1",
        "key 2",
        "Protocol",
        "Upgrade",
        "Origin",
        "Draft",
        "Challenge",
        
        /* new for 04 */
        "Key",
        "Version",
        "Sworigin",
        
        /* new for 05 */
        "Extensions",
        
        /* client receives these */
        "Accept",
        "Nonce",
        "Http",
        "MuxURL",
    };
    
    for (n = 0; n < WSI_TOKEN_COUNT; n++) {
        if (lwst[n].token == NULL)
            continue;
        
        fprintf(stderr, "    %s = %s\n", token_names[n], lwst[n].token);
    }
}