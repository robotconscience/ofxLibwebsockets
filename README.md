#ofxLibwebsockets
* Add the power of [WebSockets](http://www.websocket.org/) to your openFrameworks project!
* openframeworks wrapper for libwebsockets (http://git.warmcat.com/cgi-bin/cgit/libwebsockets/) 
* implements both client and server functionality of libwebsockets
* includes support for sending text (strings) and binary data
* based on Paul Reimer's work on ofxWebUI (https://github.com/paulreimer/ofxWebUI) and jason vancleave's ofxLibWebSockets (https://github.com/jvcleave/ofxLibWebSockets)

##Examples: Basic
* example_client_hello_world
	* The simplest example: open up a Websocket to echo.websocket.org and say "hello"!
* example_server_echo
	* A basic Websocket server you can connect to from your browser (http://localhost:9092) to send messages back and forth
	* Includes a simple javascript example that is hosted by the app
	* Can also connect to example_client_hello_world
* shared canvas
	* example_server_sharedcanvas + example_client_sharedcanvas
	* Clients in openFrameworks and javascript can collaboratively draw on a canvas

##Examples: Advanced
* basic binary
	* example_server_binary and example_server_binaryvideo
	* demonstrates a custom binary setup, sending the raw pixels from OF to javascript
	* requires a bit of processing on the javascript side, please see the code in data/web
* optimized binary
	* example_server_blob, example_client_blob, example_server_blobvideo
	* requires my fork of [ofxTurboJpeg](https://github.com/robotconscience/ofxTurboJpeg)
	* demonstrates how to send true binary data (i.e. a file) via ofxLibwebsockets
		* check out the javascript code in example_server_blob/bin/data/web
		* example_client_blob can connect to either server!
	* thanks to @zebradog for the start of these examples!

##Adding to Projects
* OS X
	* Via OF Project Generator:
		1. Delete ofxLibwebsockets/libs/libwebsockets/include/win32port
		2. Add ofxLibwebsockets to your addons via the projectGenerator
		3. That's it!
	* Adding to a new/existing project:
		* Include ofxLibwebsockets' xcconfig file via your project's Project.xcconfig file:
			1. Define where it lives: 
			```OFX_LWS_PATH = "$(OF_PATH)/addons/ofxLibwebsockets"```
			2. Include ofxLibwebsockets xcconfig
			```#include "../../../addons/ofxLibwebsockets/ofxLibwebsockets.xcconfig"```
			2. Add to existing vars in Project.xcconfig:
			
			```
			OTHER_LDFLAGS = $(OF_CORE_LIBS) $(OFX_LWS_LIBS)
			HEADER_SEARCH_PATHS = $(OF_CORE_HEADERS) 
			USER_HEADER_SEARCH_PATHS = $(OFX_LWS_INCLUDES)
			LIBRARY_SEARCH_PATHS = $(inherited) $(OFX_LWS_SEARCH)
			```

* Windows
	* Adding to new/existing project: 
		1. Add the ofxLibwebsockets source files to the C++ Linker
			* right click on project in the solution explorer, click "Properties", Go down to C++ > General
			* click the arrow at the right of "Additional include directories" and select "edit"
			* add the ofxLibwebsockets source files:
			
			```
			..\..\..\addons\ofxLibwebsockets\libs\jsoncpp
			..\..\..\addons\ofxLibwebsockets\libs\jsoncpp\json
			..\..\..\addons\ofxLibwebsockets\libs\libwebsockets\include\
			..\..\..\addons\ofxLibwebsockets\libs\openssl\openssl
			..\..\..\addons\ofxLibwebsockets\libs\ofxLibwebsockets\include
			..\..\..\addons\ofxLibwebsockets\libs\ofxLibwebsockets\include\ofxLibwebsockets
			..\..\..\addons\ofxLibwebsockets\libs\ofxLibwebsockets\src
			..\..\..\addons\ofxLibwebsockets\src
			..\..\..\addons\ofxLibwebsockets\libs\libwebsockets\include\win32port
			..\..\..\addons\ofxLibwebsockets\libs\libwebsockets\include\win32port\win32helpers
			```


		2. Add the ofxLibwebsockets paths to the "Additional Library Directories":
			* right click on project in the solution explorer, click "Properties", Go down to Linker > General
			* click the arrow at the right of "Additional Library Directories" and select "edit"
			* add 
			```
			..\..\..\addons\ofxLibwebsockets\libs\libwebsockets\lib\win32\Release
			```
			and
			```
			..\..\..\addons\ofxLibwebsockets\libs\libwebsockets\libs\openssl\lib\win32
			```
		3. Now go down to Linker > Input; open the editor "Additional dependencies" the same way
			* In your debug scheme, add libwebsocketswin32d.lib, libeay32.lib, and ssleay32.lib
			* In your release scheme, add libwebsocketswin32.lib, libeay32.lib, and ssleay32.lib
		4. Finally, go to C/C++ > Preprocessor
			* Make sure you're editing "All Configurations", not just Debug or Release (drop down at the top left)
			* Click the drop down next to "Preprocessor Definitions" and select <Edit>
			* In the text box that pops up, paste
			```
			_WIN32_WINNT=0x500
			```
			
* Linux
	* Linux support is offline until the library is recompiled. Please submit me a PR!!!

#STATUS
* Version 0.8.1 (current)
	* Overhauled all messaging, supporting senging/receiving string and binary messages of indefinite size via continuation frames
	* Added examples from @zebradog of sending Blob data over websockets: way faster, way cleaner!
	* Lots of cleanup of excessive logging, possible crashes on sending
	* Updated to latest libwebsockets on Mac and Windows
	* Cleaned up and fixed up Windows support
	* Tested on OF 0.8.0, 0.8.1 on Mac and Windows
	* Removed RPi and Linux64 until someone recompiles libwebsockets!!!
* Version 0.8.0
	* Basic support for binary data
	* Updated to latest libwebsockets
