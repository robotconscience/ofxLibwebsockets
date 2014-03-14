//
//  Connection.cpp
//  ofxLibwebsockets
//
//  Created by Brett Renfer on 4/11/12.
//  Copyright (c) 2012 Robotconscience. All rights reserved.
//

#include "ofxLibwebsockets/Connection.h"
#include "ofxLibwebsockets/Reactor.h"
#include "ofxLibwebsockets/Protocol.h"

namespace ofxLibwebsockets {

    //--------------------------------------------------------------
    Connection::Connection(Reactor* const _reactor, Protocol* const _protocol, const bool _supportsBinary)
    : reactor(_reactor)
    , protocol(_protocol)
    , context(NULL)
    , ws(NULL)
    , session(NULL)
    , supportsBinary(_supportsBinary)
    //, buf(LWS_SEND_BUFFER_PRE_PADDING+1024+LWS_SEND_BUFFER_POST_PADDING)
    {
        if (_protocol != NULL){
            binary = _protocol->binary;
            bufsize = _protocol->rx_buffer_size;
            binaryBufsize = _protocol->rx_buffer_size;
            buf = (unsigned char*)calloc(LWS_SEND_BUFFER_PRE_PADDING+bufsize+LWS_SEND_BUFFER_POST_PADDING, sizeof(unsigned char));
            binaryBuf = (unsigned char*)calloc(LWS_SEND_BUFFER_PRE_PADDING+binaryBufsize+LWS_SEND_BUFFER_POST_PADDING, sizeof(unsigned char));
        }
    }

    //--------------------------------------------------------------
    Connection::~Connection(){
        free(buf);
        free(binaryBuf);
    }
    //--------------------------------------------------------------
    void Connection::close() {
        if (reactor != NULL){
            reactor->close(this);
        }
    }
    
    //--------------------------------------------------------------
    std::string Connection::getClientIP(){
        return client_ip;
    }
    
    //--------------------------------------------------------------
    std::string Connection::getClientName(){
        return client_name;
    }
    
    //--------------------------------------------------------------
    void Connection::setupAddress(){
        int fd = libwebsocket_get_socket_fd( ws );
        
        client_ip.resize(128);
        client_name.resize(128);
        
        libwebsockets_get_peer_addresses(context, ws, fd, &client_name[0], client_name.size(),
                                         &client_ip[0], client_ip.size());
    }

    //--------------------------------------------------------------
    void Connection::send(const std::string& message)
    {
        if ( message.size() == 0 ) return;
        int n = 0;
        
        // size packet based on either binaryBufSize (max) or passed 'size' (whichever is smaller)
        int dataSize = bufsize > message.size() ? message.size() : bufsize;
        memcpy(&buf[LWS_SEND_BUFFER_PRE_PADDING], message.c_str(), dataSize );
        
        // we have a big frame, so we need to send a few times
        if ( bufsize < message.size() ){
            // need to jump into thread
            TextPacket tp;
            tp.message = message;
            messages_text.push_back(tp);
            //if (!isThreadRunning()) startThread();
            
        // we have a nice small frame, just send it
        } else {
            n = libwebsocket_write(ws, &buf[LWS_SEND_BUFFER_PRE_PADDING], message.size(), LWS_WRITE_TEXT);
        }
        
        if (n < 0)
            ofLogError() << "ofxLibwebsockets: ERROR writing to socket" << std::endl;
    }
        
    //--------------------------------------------------------------
    void Connection::sendBinary( unsigned char * data, unsigned int size ){
        int n = -1;
        
        // TODO: when libwebsockets has an API supporting something this, we should use it
        // for now we are assuming that if you send binary your client supports it
        
        if ( supportsBinary ){
            if ( ws != NULL && binaryBuf ){
                // size binary packet based on either binaryBufSize (max) or passed 'size' (whichever is smaller)
                int dataSize = binaryBufsize > size ? size : binaryBufsize;
                memcpy(&binaryBuf[LWS_SEND_BUFFER_PRE_PADDING], data, dataSize );
                
                // we have a big frame, so we need to send a few times
                if ( binaryBufsize < size ){
                    
                    // need to jump into thread
                    BinaryPacket bp;
                    bp.data = data;
                    bp.size = size;
                    messages_binary.push_back(bp);
                    //if (!isThreadRunning()) startThread();
                    
                // we have a nice small frame, just send it
                } else {
                    n = libwebsocket_write(ws, &binaryBuf[LWS_SEND_BUFFER_PRE_PADDING], dataSize, LWS_WRITE_BINARY);
                }
            }
        } else {
            unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
            int encoded_len = 0;//lws_b64_encode_string((char *) data, size, (char*)p, bufsize-LWS_SEND_BUFFER_PRE_PADDING-LWS_SEND_BUFFER_POST_PADDING);
            
            if (encoded_len > 0){
                n = libwebsocket_write(ws, p, encoded_len, LWS_WRITE_TEXT);
            }
        }
        
        cout << "remains "<< n << endl;
        
        if (n < 0){
            ofLogError() << "ofxLibwebsockets: ERROR writing to socket" << std::endl;
        }
    }
    
    //--------------------------------------------------------------
    void Connection::update(){
        // process standard ws messages
        if ( messages_text.size() > 0 && protocol->idle ){
            TextPacket & packet = messages_text[0];
            
            if ( packet.index == 0 ){
                int dataSize = bufsize > packet.message.size() ? packet.message.size() : bufsize;
                memcpy(&buf[LWS_SEND_BUFFER_PRE_PADDING], packet.message.c_str(), dataSize );
                
                int writeMode = LWS_WRITE_TEXT;
                writeMode |= LWS_WRITE_NO_FIN; // add "we're not finished" flag
                
                protocol->idle = false;
                
                int n = libwebsocket_write(ws, &buf[LWS_SEND_BUFFER_PRE_PADDING], dataSize, (libwebsocket_write_protocol) writeMode );
                libwebsocket_callback_on_writable(context, ws);
                packet.index = dataSize;
                
            } else {
                // continue to send large message in chunks
                int dataSize = bufsize > packet.message.size() ? packet.message.size() : binaryBufsize;
                int writeMode = LWS_WRITE_CONTINUATION;
                writeMode |= LWS_WRITE_NO_FIN;
                
                bool bDone = false;
                if ( packet.index + dataSize >= packet.message.size() ){
                    dataSize = packet.message.size() - packet.index;
                    writeMode = LWS_WRITE_CONTINUATION;
                    bDone = true;
                }
                
                memcpy(&buf[LWS_SEND_BUFFER_PRE_PADDING], packet.message.c_str() + packet.index, dataSize );
                protocol->idle = false;
                int n = libwebsocket_write(ws, &buf[LWS_SEND_BUFFER_PRE_PADDING], dataSize, (libwebsocket_write_protocol) writeMode );
                libwebsocket_callback_on_writable(context, ws);
                packet.index += dataSize;
                
                if ( bDone ){
                    messages_text.erase(messages_text.begin());
                }
            }
        }  else if ( messages_text.size() > 0 && messages_text[0].index ){
            libwebsocket_callback_on_writable(context, ws);
        }
        
        // process binary messages
        if ( messages_binary.size() > 0 ){
            BinaryPacket tosend = messages_binary[0];
            messages_binary.erase(messages_binary.begin());
            
            int dataSize = binaryBufsize > tosend.size ? tosend.size : binaryBufsize;
            memcpy(&binaryBuf[LWS_SEND_BUFFER_PRE_PADDING], tosend.data, dataSize );
            
            int writeMode = LWS_WRITE_BINARY;
            writeMode |= LWS_WRITE_NO_FIN;
            
            protocol->idle = false;
            int n = libwebsocket_write(ws, &binaryBuf[LWS_SEND_BUFFER_PRE_PADDING], dataSize, (libwebsocket_write_protocol) writeMode );
            int index = dataSize;
            writeMode = LWS_WRITE_CONTINUATION;
            writeMode |= LWS_WRITE_NO_FIN;
            
            while ( true ){
                bool bNeedToReturn = false;
                if ( index + dataSize >= tosend.size ){
                    dataSize = tosend.size - index;
                    cout << dataSize << endl;
                    writeMode = LWS_WRITE_CONTINUATION;
                    bNeedToReturn = true;
                }
                // wait until we receive 'writable'
                while (!protocol->idle) {}
                
                memcpy(&binaryBuf[LWS_SEND_BUFFER_PRE_PADDING], tosend.data + index, dataSize );
                protocol->idle = false;
                n = libwebsocket_write(ws, &binaryBuf[LWS_SEND_BUFFER_PRE_PADDING], dataSize, (libwebsocket_write_protocol) writeMode );
                index += dataSize;
                if ( bNeedToReturn ) break;
            }

        }
    }
    
    //--------------------------------------------------------------
    const std::string Connection::recv(const std::string& message) {
        std::string decoded = message;
        
        //TODO: when libwebsockets has an API
        // to detect binary support, we should use it
        if (binary && !supportsBinary)
        {
            int decoded_len = 0;//lws_b64_decode_string(message.c_str(), &decoded[0], message.size());
            decoded.resize(decoded_len);
        }
        
        return decoded;
    }
    
    //--------------------------------------------------------------
    bool Connection::operator==( const Connection &other ){
        if ( other.ws == NULL ) return false;
        return other.ws == ws;
    }
    
    //--------------------------------------------------------------
    bool Connection::operator!=( const Connection &other ){
        return other.ws != ws;
    }
}


