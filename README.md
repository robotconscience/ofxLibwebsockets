IN PROGRESS
------------
* Server example currently works with client example and with Safari
* Client example currently tested with server example and with Autobahn's python websocket server
* Both examples don't really do anything right now....

TO-DO
------------

* Add functionality to examples
* Test JSON parsing with jsoncpp (seems to be working)
* Test SSL support more (working so far with echo.websocket.org!)
* Fix compression issue with newest versions of Chrome (either tell Chrome we're not using compression or implement libwebsockets' compression tools)
* Clean up names
* Compile for other platforms

openframeworks wrapper for ofxLibwebsockets
* implements both client and server functionality of libwebsockets
* based on Paul Reimer's work on ofxWebUI (https://github.com/paulreimer/ofxWebUI) and jason vancleave's ofxLibWebSockets (https://github.com/jvcleave/ofxLibWebSockets)