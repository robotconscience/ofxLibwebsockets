//
//  Connection.h
//  ofxLibwebsockets
//
//  Created by Brett Renfer on 4/11/12.
//  Copyright (c) 2012 Robotconscience. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include <libwebsockets.h>

#include <iostream>
#include <vector>
#include <string>

namespace ofxLibwebsockets {
    
    class Reactor;
    class Protocol;
    
    class Session {
        public:
    };
    
    struct TextPacket {
        string message;
        int index = 0;
    };
    
    struct BinaryPacket {
        unsigned char * data;
        unsigned int size;
        int index = 0;
    };
    
    class Connection {
        friend class Reactor;
    public:
        Connection(Reactor* const _reactor=NULL, Protocol* const _protocol=NULL, const bool supportsBinary=true);
        
        ~Connection();
        void close();
        void send(const std::string& message);
        
        template <class T> 
        void sendBinary( T& image ){
            int size = image.width * image.height * image.getPixelsRef().getNumChannels();
            sendBinary( (unsigned char *) image.getPixels(), size );
        }
        
        void sendBinary( unsigned char * data, unsigned int size );
        
        const std::string recv(const std::string& message);  
        
        void setupAddress();
        
        std::string getClientIP();
        std::string getClientName();
        
        libwebsocket_context* context;
        libwebsocket* ws;
        Reactor*  reactor;
        Protocol* protocol;
        
        // operators
        bool operator==( const Connection &other );
        bool operator!=( const Connection &other );
        
        // MUST be called from main thread (e.g. client or server)
        void update();
        
    protected:
        Session*  session;
        
        std::string client_ip;
        std::string client_name;
        
        bool binary;            // is this connection sending / receiving binary?
        bool supportsBinary;    // does this connection support binary?
        int buffersize;
        unsigned char* buf;
        unsigned char* binaryBuf;
        int bufsize;
        int binaryBufsize;
        //std::vector<unsigned char> buf;
        
        // threading stuff
        vector<TextPacket> messages_text;
        vector<BinaryPacket> messages_binary;
    };
    

}
