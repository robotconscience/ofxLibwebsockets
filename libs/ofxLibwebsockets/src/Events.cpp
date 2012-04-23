#include "ofxLibwebsockets/Events.h"

namespace ofxLibwebsockets {
        
    //--------------------------------------------------------------
    Event::Event(Connection& _conn, std::string& _message)
    : conn(_conn)
    , message(_message)
    {}

    //_Events ofxWebSocketEvents;
}