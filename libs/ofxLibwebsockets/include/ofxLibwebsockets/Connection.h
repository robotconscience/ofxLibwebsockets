//
//  Connection.h
//  ofxLibwebsockets
//
//  Created by Brett Renfer on 4/11/12.
//  Copyright (c) 2012 Robotconscience. All rights reserved.
//

#pragma once

#include <libwebsockets.h>

#include <vector>
#include <string>

namespace ofxLibwebsockets {
    
    class Reactor;
    class Protocol;
    
    class Session {
        public:
    };
    
    class Connection {
        friend class Reactor;
    public:
        Connection();
        
        Connection(Reactor* const _reactor=NULL, Protocol* const _protocol=NULL, const bool supportsBinary=false);
        
        ~Connection();
        void close();
        void send(const std::string& message);
        const std::string recv(const std::string& message);  
        
        void setupAddress( const long fd );
        
        std::string getClientIP();
        std::string getClientName();
        
        libwebsocket* ws;
        Reactor*  reactor;
        Protocol* protocol;
        
    protected:
        Session*  session;
        
        std::string client_ip;
        std::string client_name;
        
        bool binary;
        bool supportsBinary;
        int buffersize;
        unsigned char* buf;
        int bufsize;
        //std::vector<unsigned char> buf;
    };
    

}