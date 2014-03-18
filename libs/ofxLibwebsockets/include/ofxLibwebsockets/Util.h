//
//  Util.h
//  ofxLibwebsockets
//
//  Created by Brett Renfer on 4/11/12.
//  Copyright (c) 2012 Robotconscience. All rights reserved.
//

#pragma once

#include <libwebsockets.h>

#include "ofxLibwebsockets/Connection.h"
#include "ofxLibwebsockets/Reactor.h"
#include "ofxLibwebsockets/Client.h"
#include "ofxLibwebsockets/Server.h"

namespace ofxLibwebsockets {
        
    class Client;
    class Server;
    
    static string getCallbackReason( int reason );
    static void dump_handshake_info(struct lws_tokens *lwst);

    // CLIENT CALLBACK

    static int lws_client_callback(struct libwebsocket_context* context, struct libwebsocket *ws, enum libwebsocket_callback_reasons reason, void *user, void *data, size_t len){
        const struct libwebsocket_protocols* lws_protocol = (ws == NULL ? NULL : libwebsockets_get_protocol(ws));
        int idx = lws_protocol? lws_protocol->protocol_index : 0;          
        
        Connection* conn;
        
        Reactor* reactor = NULL;
        Protocol* protocol;
            
        for (int i=0; i<(int)reactors.size(); i++){
            if (reactors[i]->getContext() == context){
                reactor =  reactors[i];
                protocol = reactor->protocol(idx);
                conn = ((Client*) reactor)->getConnection();
                if (conn){
                    conn->context = context;
                }
                break;
            } else {
            }
        }
        
        ofLog( OF_LOG_VERBOSE, getCallbackReason(reason) );
        
        if (reason == LWS_CALLBACK_CLIENT_ESTABLISHED ){
            libwebsocket_callback_on_writable(context, ws);
        } else if (reason == LWS_CALLBACK_CLOSED){
        }
        
        switch (reason)
        {
            case LWS_CALLBACK_CONFIRM_EXTENSION_OKAY:
                return 0;
                
            case LWS_CALLBACK_FILTER_NETWORK_CONNECTION:
                if (protocol != NULL){
                    return reactor->_allow(ws, protocol, (int)(long)user)? 0 : 1;
                } else {
                    return 0;
                }
            case LWS_CALLBACK_HTTP:
                if ( reactor != NULL){
                    return reactor->_http(ws, (char*)data);
                } else {
                    return 0;
                }
                
            // we're not really worried about this at the moment
            case LWS_CALLBACK_ADD_POLL_FD:
			case LWS_CALLBACK_DEL_POLL_FD:
			case LWS_CALLBACK_SET_MODE_POLL_FD:
			case LWS_CALLBACK_CLEAR_MODE_POLL_FD:
                return;
                
            default:
                if ( reactor != NULL ){
                    //conn = *(Connection**)user;
                    if (conn && conn->ws != ws){
                        conn->ws = ws;
                        conn->context = context;
                    }
                    return reactor->_notify(conn, reason, (char*)data, len);
                } else {
                    return 0;
                }
        }
        
        return 1; // FAIL (e.g. unhandled case/break in switch)
    }
    
    // SERVER CALLBACK

    static int lws_callback(struct libwebsocket_context* context, struct libwebsocket *ws, enum libwebsocket_callback_reasons reason, void *user, void *data, size_t len){
        const struct libwebsocket_protocols* lws_protocol = (ws == NULL ? NULL : libwebsockets_get_protocol(ws));
        int idx = lws_protocol? lws_protocol->protocol_index : 0;   
        
        // valid connection w/o a protocol
        if ( ws != NULL && lws_protocol == NULL ){
            // OK for now, returning 0 above
        }
        
        //bool bAllowAllProtocls = (ws != NULL ? lws_protocol == NULL : false);
        
        Connection* conn;    
        Connection** conn_ptr = (Connection**)user;
        Server* reactor = NULL;
        Protocol* protocol = NULL;
        
        for (int i=0; i<(int)reactors.size(); i++){
            if (reactors[i]->getContext() == context){
                reactor = (Server*) reactors[i];
                protocol = reactor->protocol( (idx > 0 ? idx : 0) );
                break;
            } else {
            }
        }
        
        ofLog( OF_LOG_VERBOSE, getCallbackReason(reason) );
        
        if (reason == LWS_CALLBACK_ESTABLISHED){
            libwebsocket_callback_on_writable(context, ws);
            
            if ( reactor != NULL ){
                *conn_ptr = new Connection(reactor, protocol);
            }
        } else if (reason == LWS_CALLBACK_CLOSED){
            //if (*conn_ptr != NULL)
            //delete *conn_ptr;
        }

        switch (reason)
        {
            // we may use these in the future!
            case LWS_CALLBACK_PROTOCOL_INIT:
            case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
            case LWS_CALLBACK_HTTP_FILE_COMPLETION:
                return 0;
                
            case LWS_CALLBACK_FILTER_NETWORK_CONNECTION:
                if (protocol != NULL ){
                    return reactor->_allow(ws, protocol, (int)(long)user)? 0 : 1;
                } else {
                    return 1;
                }
                
            case LWS_CALLBACK_HTTP:
                return reactor->_http(ws, (char*)data);
                
                // we're not really worried about this at the moment
            case LWS_CALLBACK_ADD_POLL_FD:
			case LWS_CALLBACK_DEL_POLL_FD:
			case LWS_CALLBACK_SET_MODE_POLL_FD:
			case LWS_CALLBACK_CLEAR_MODE_POLL_FD:
                return;
                
            default:
                conn = *(Connection**)user;
                
                if (conn && (conn->ws != ws || conn->ws == NULL) ){
                    conn->context = context;
                    conn->ws = ws;
                    conn->setupAddress();
                }
                if (reactor){
                    return reactor->_notify(conn, reason, (char*)data, len);                
                } else {
                    return 0;
                }
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
    
    
    static string getCallbackReason( int reason ){
        switch (reason){
            case LWS_CALLBACK_ESTABLISHED : return "LWS_CALLBACK_ESTABLISHED";
			case LWS_CALLBACK_CLIENT_CONNECTION_ERROR : return "LWS_CALLBACK_CLIENT_CONNECTION_ERROR";
			case LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH : return "LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH";
			case LWS_CALLBACK_CLIENT_ESTABLISHED : return "LWS_CALLBACK_CLIENT_ESTABLISHED";
			case LWS_CALLBACK_CLOSED : return "LWS_CALLBACK_CLOSED";
			case LWS_CALLBACK_CLOSED_HTTP : return "LWS_CALLBACK_CLOSED_HTTP";
			case LWS_CALLBACK_RECEIVE : return "LWS_CALLBACK_RECEIVE";
			case LWS_CALLBACK_CLIENT_RECEIVE : return "LWS_CALLBACK_CLIENT_RECEIVE";
			case LWS_CALLBACK_CLIENT_RECEIVE_PONG : return "LWS_CALLBACK_CLIENT_RECEIVE_PONG";
			case LWS_CALLBACK_CLIENT_WRITEABLE : return "LWS_CALLBACK_CLIENT_WRITEABLE";
			case LWS_CALLBACK_SERVER_WRITEABLE : return "LWS_CALLBACK_SERVER_WRITEABLE";

			case LWS_CALLBACK_HTTP : return "LWS_CALLBACK_HTTP";
			case LWS_CALLBACK_HTTP_FILE_COMPLETION : return "LWS_CALLBACK_HTTP_FILE_COMPLETION";
			case LWS_CALLBACK_HTTP_WRITEABLE : return "LWS_CALLBACK_HTTP_WRITEABLE";
			case LWS_CALLBACK_FILTER_NETWORK_CONNECTION : return "LWS_CALLBACK_FILTER_NETWORK_CONNECTION";
			case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION : return "LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION";
			case LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS : return "LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS";
			case LWS_CALLBACK_OPENSSL_LOAD_EXTRA_SERVER_VERIFY_CERTS : return "LWS_CALLBACK_OPENSSL_LOAD_EXTRA_SERVER_VERIFY_CERTS";
			case LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION : return "LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION";
			case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER : return "LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER";
			case LWS_CALLBACK_CONFIRM_EXTENSION_OKAY : return "LWS_CALLBACK_CONFIRM_EXTENSION_OKAY";
	
			case LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED : return "LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED";
			case LWS_CALLBACK_PROTOCOL_INIT : return "LWS_CALLBACK_PROTOCOL_INIT";
			case LWS_CALLBACK_PROTOCOL_DESTROY : return "LWS_CALLBACK_PROTOCOL_DESTROY";
			case LWS_CALLBACK_ADD_POLL_FD : return "LWS_CALLBACK_ADD_POLL_FD";
			case LWS_CALLBACK_DEL_POLL_FD : return "LWS_CALLBACK_DEL_POLL_FD";
			case LWS_CALLBACK_SET_MODE_POLL_FD : return "LWS_CALLBACK_SET_MODE_POLL_FD";
			case LWS_CALLBACK_CLEAR_MODE_POLL_FD : return "LWS_CALLBACK_CLEAR_MODE_POLL_FD";

			default: 
				std::stringstream r;
				r << "Unknown callback reason id: " << reason;	
				return r.str();    
        }
    }
}
