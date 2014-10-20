//
//  Connection.cpp
//  ofxLibwebsockets
//
//  Created by Brett Renfer on 4/11/12. 
//

#include "ofxLibwebsockets/Connection.h"
#include "ofxLibwebsockets/Reactor.h"
#include "ofxLibwebsockets/Protocol.h"

namespace ofxLibwebsockets {

    //--------------------------------------------------------------
    Connection::Connection(Reactor* const _reactor, Protocol* const _protocol)
    : reactor(_reactor)
    , protocol(_protocol)
    , context(NULL)
    , ws(NULL)
    //, buf(LWS_SEND_BUFFER_PRE_PADDING+1024+LWS_SEND_BUFFER_POST_PADDING)
    {
        if (_protocol != NULL){
            bufferSize = _protocol->rx_buffer_size;
            buf = (unsigned char*)calloc(LWS_SEND_BUFFER_PRE_PADDING+bufferSize+LWS_SEND_BUFFER_POST_PADDING, sizeof(unsigned char));
            binaryBuf = (unsigned char*)calloc(LWS_SEND_BUFFER_PRE_PADDING+bufferSize+LWS_SEND_BUFFER_POST_PADDING, sizeof(unsigned char));
        }
    }

    //--------------------------------------------------------------
    Connection::~Connection(){
        free(buf);
        free(binaryBuf);
    }
    //--------------------------------------------------------------
    void Connection::close() {
        // delete all pending frames
        messages_binary.clear();
        messages_text.clear();
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
        
        // size packet based on either bufferSize (max) or passed 'size' (whichever is smaller)
        int dataSize = bufferSize > message.size() ? message.size() : bufferSize;
        memcpy(&buf[LWS_SEND_BUFFER_PRE_PADDING], message.c_str(), dataSize );
        
        // we have a big frame, so we need to send a few times
        if ( bufferSize < message.size() ){
            // need to jump into thread
            TextPacket tp;
			tp.index = 0;
            tp.message = message;
            messages_text.push_back(tp);
            
        // we have a nice small frame, just send it
        } else {
            n = libwebsocket_write(ws, &buf[LWS_SEND_BUFFER_PRE_PADDING], message.size(), LWS_WRITE_TEXT);
        }
        
        if (n < 0)
            ofLogError() << "[ofxLibwebsockets]  ERROR writing to socket" << std::endl;
    }
    
    //--------------------------------------------------------------
    void Connection::sendBinary( ofBuffer buffer ){
        sendBinary(buffer.getBinaryBuffer(), buffer.size());
    }
    
    //--------------------------------------------------------------
    void Connection::sendBinary( unsigned char * data, unsigned int size ){
        sendBinary(reinterpret_cast<char *>(data), size);
    }
    
    //--------------------------------------------------------------
    void Connection::sendBinary( char * data, unsigned int size ){
        int n = -1;
        // size binary packet based on either bufferSize (max) or passed 'size' (whichever is smaller)
        int dataSize = bufferSize > size ? size : bufferSize;
        memcpy(&binaryBuf[LWS_SEND_BUFFER_PRE_PADDING], data, dataSize );
        
        // we have a big frame, so we need to send a few times
        if ( bufferSize < size ){
            
            // need to split into packets
            BinaryPacket bp;
            bp.index = 0;
            bp.size = size;
            
            // copy data into array, in case user frees it
            bp.data = (unsigned char*)calloc(size, sizeof(unsigned char));
            memcpy(bp.data, data, size);
            
            messages_binary.push_back(bp);
            
            n = 0;
            
        // we have a nice small frame, just send it
        } else {
            n = libwebsocket_write(ws, &binaryBuf[LWS_SEND_BUFFER_PRE_PADDING], dataSize, LWS_WRITE_BINARY);
        }
        
        if (n < 0){
            ofLogError() << "[ofxLibwebsockets]  ERROR writing to socket" << std::endl;
        }
    }
    
    //--------------------------------------------------------------
    void Connection::update(){
        // process standard ws messages
        if ( messages_text.size() > 0 && protocol->idle ){
            TextPacket & packet = messages_text[0];
            
            if ( packet.index == 0 ){
                int dataSize = bufferSize > packet.message.size() ? packet.message.size() : bufferSize;
                memcpy(&buf[LWS_SEND_BUFFER_PRE_PADDING], packet.message.c_str(), dataSize );
                
                int writeMode = LWS_WRITE_TEXT;
                writeMode |= LWS_WRITE_NO_FIN; // add "we're not finished" flag
                
                protocol->idle = false;
                
                int n = libwebsocket_write(ws, &buf[LWS_SEND_BUFFER_PRE_PADDING], dataSize, (libwebsocket_write_protocol) writeMode );
                libwebsocket_callback_on_writable(context, ws);
                packet.index = dataSize;
                
            } else {
                // continue to send large message in chunks
                int dataSize = bufferSize > packet.message.size() ? packet.message.size() : bufferSize;
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
        } else if ( messages_text.size() > 0 && messages_text[0].index ){
            libwebsocket_callback_on_writable(context, ws);
        }
        
        // process binary messages
        if ( messages_binary.size() > 0 && protocol->idle ){
            if ( messages_binary.size() > 0 ){
                BinaryPacket & packet = messages_binary[0];
                
                if ( packet.index == 0 ){
                    // write beginning of packet
                    int dataSize = bufferSize;
                    memcpy(&binaryBuf[LWS_SEND_BUFFER_PRE_PADDING], packet.data, dataSize );
                    
                    int writeMode = LWS_WRITE_BINARY;
                    writeMode |= LWS_WRITE_NO_FIN;
                    
                    protocol->idle = false;
                    int n = libwebsocket_write(ws, &binaryBuf[LWS_SEND_BUFFER_PRE_PADDING], dataSize, (libwebsocket_write_protocol) writeMode );
                    libwebsocket_callback_on_writable(context, ws);
                    packet.index += dataSize;
                } else {
                    // continue to send large message in chunks
                    int dataSize = bufferSize > packet.size ? packet.size : bufferSize;
                    int writeMode = LWS_WRITE_CONTINUATION;
                    writeMode |= LWS_WRITE_NO_FIN;
                    
                    bool bDone = false;
                    if ( packet.index + dataSize >= packet.size ){
                        dataSize = packet.size - packet.index;
                        writeMode = LWS_WRITE_CONTINUATION;
                        bDone = true;
                    }
                    
                    memcpy(&binaryBuf[LWS_SEND_BUFFER_PRE_PADDING], packet.data + packet.index, dataSize );
                    
                    // this sets the protocol to wait until "idle"
                    protocol->idle = false; // todo: this should be automatic on write!
                    int n = libwebsocket_write(ws, &binaryBuf[LWS_SEND_BUFFER_PRE_PADDING], dataSize, (libwebsocket_write_protocol) writeMode );
                    libwebsocket_callback_on_writable(context, ws);
                    packet.index += dataSize;
                    
                    if ( bDone ){
                        free(packet.data);
                        messages_binary.erase(messages_binary.begin());
                    }
                }
            }
        } else if ( messages_text.size() > 0 && messages_text[0].index ){
            libwebsocket_callback_on_writable(context, ws);
        }
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


