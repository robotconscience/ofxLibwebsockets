ofxLibwebsockets
------------
* openframeworks wrapper for libwebsockets (http://git.warmcat.com/cgi-bin/cgit/libwebsockets/) 
* implements both client and server functionality of libwebsockets
* based on Paul Reimer's work on ofxWebUI (https://github.com/paulreimer/ofxWebUI) and jason vancleave's ofxLibWebSockets (https://github.com/jvcleave/ofxLibWebSockets)

ADDING TO PROJECTS
------------
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
			HEADER_SEARCH_PATHS = $(OF_CORE_HEADERS) $(OFX_LWS_INCLUDES)
			LIBRARY_SEARCH_PATHS = $(inherited) $(OFX_LWS_SEARCH)
			```

* Windows
	* Adding to new/existing project: 
		1. Add the ofxLibwebsockets source files to the C++ Linker
			* right click on project in the solution explorer, click "Properties", Go down to C++ > General
			* click the arrow at the right of "Additional include directories" and select "edit"
			* add the ofxLibwebsockets source files:
			
			```
			..\..\..\addons\ofxLibwebsockets\libs\jsonpp
			..\..\..\addons\ofxLibwebsockets\libs\jsonpp\json
			..\..\..\addons\ofxLibwebsockets\libs\libwebsockets\include
			..\..\..\addons\ofxLibwebsockets\libs\libwebsockets\include\win32port
			..\..\..\addons\ofxLibwebsockets\libs\ofxLibwebsockets\include
			..\..\..\addons\ofxLibwebsockets\libs\ofxLibwebsockets\src
			..\..\..\addons\ofxLibwebsockets\libs\openssl\openssl
			..\..\..\addons\ofxLibwebsockets\src
```


		2. Add the ofxLibwebsockets paths to the "Additional Library Directories":
			* right click on project in the solution explorer, click "Properties", Go down to Linker > General
			* click the arrow at the right of "Additional Library Directories" and select "edit"
			* add ```..\..\..\addons/ofxLibwebsockets\libs\libwebsockets\libs\libwebsockets\lib\win32``` and ```..\..\..\addons\ofxLibwebsockets\libs\libwebsockets\libs\openssl\lib\win32```
		3. Now go down to Linker > Input; open the editor "Additional dependencies" the same way
			* In your debug scheme, add libwebsocketswin32d.lib, libeay32.lib, and ssleay32.lib
			* In your release scheme, add libwebsocketswin32.lib, libeay32.lib, and ssleay32.lib
			
* Linux
	* Delete the entire directory at ofxLibwebsockets/libs/libwebsockets/include/win32port
	* 64-bit static version of libwebsockets is included; please let us know if you can contribute a 32 bit version!

STATUS
------------
* Server example currently works with Chrome, and Safari (need to test FF)
* Server and Client both support SSL, message sending/receiving, broadcasting, protocols and channels
* Client example currently tested with server example, echo.websockets.org (with and without SSL), local Autobahn ws server

TO-DO
------------
* Add functionality to examples
* More fun examples
* Compile for 32-bit linux
