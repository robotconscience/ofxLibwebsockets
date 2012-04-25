// GLOBAL VARS
var socket;
var canvas;
var ctx;

//----------------------------------------------------------------------------------------------------------------
// ON READY, SETUP SOCKET!
//----------------------------------------------------------------------------------------------------------------

$(document).ready( function() {
	canvas 		= document.getElementById('contourCanvas');
	ctx			= canvas.getContext('2d');
	document.getElementById("brow").textContent = " " + BrowserDetect.browser + " "
		+ BrowserDetect.version +" " + BrowserDetect.OS +" ";

	setupSocket();
});

//----------------------------------------------------------------------------------------------------------------
// WS:ON OPEN
//----------------------------------------------------------------------------------------------------------------
function onOpen(){
	document.getElementById("wslm_statustd").style.backgroundColor = "#40ff40";
	document.getElementById("wslm_statustd").textContent = " websocket connection opened ";
}

//----------------------------------------------------------------------------------------------------------------
// WS: ON CLOSE
//----------------------------------------------------------------------------------------------------------------
function onClose(){
	document.getElementById("wslm_statustd").style.backgroundColor = "#ff4040";
	document.getElementById("wslm_statustd").textContent = " websocket connection CLOSED ";
}

//----------------------------------------------------------------------------------------------------------------
// WS: ON MESSAGE
//----------------------------------------------------------------------------------------------------------------
function onMessage( messageEvent ){
	// check for binary
	if (messageEvent.data instanceof ArrayBuffer) {
		var image = new Image();
		
        var imgdata = ctx.getImageData(0,0,canvas.width, canvas.height);

		var bytearray = new Uint8Array( messageEvent.data );
		var index = 0;

		for (var i = 0; i < imgdata.data.length; i+=4) {
            imgdata.data[i] = bytearray[index]; index++;
            imgdata.data[i + 1] = bytearray[index]; index++;
            imgdata.data[i + 2] = bytearray[index]; index++;
            imgdata.data[i + 3] = 255;
		}
    
        ctx.putImageData(imgdata,0,0);


        /*var img = document.createElement('img');
            img.height = imageheight;
            img.width = imagewidth;
            img.src = tempcanvas.toDataURL();
        chatdiv.appendChild(img);*/

		/*
		if ( window.URL ){
			image.src = window.URL.createObjectURL(messageEvent.data);
		} else if ( window.webkitURL ){
			image.src = window.webkitURL.createObjectURL(messageEvent.data);
		} else {
			console.error( "your browser does not have a window.URL method :(")
		}*/
	//} else if ( messageEvent instanceof ArrayBuffer ){
	//	document.getElementById("textresponse").value = messageEvent.data;
	} else {
		var data;
		try {
			data =  jQuery.parseJSON( messageEvent.data );
		} catch( e ){
			data = messageEvent.data;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------
// setup web socket
//----------------------------------------------------------------------------------------------------------------

function setupSocket(){
	// setup!
	if (BrowserDetect.browser == "Firefox") {
		socket = new MozWebSocket( "ws://localhost:9093/", "of-protocol" );
		socket.binaryType = "arraybuffer";
	} else {
		socket = new WebSocket( "ws://localhost:9093/", "of-protocol");	
		socket.binaryType = "arraybuffer";
	}
	
	// open
	try {
		socket.onopen 		= onOpen;
		socket.onmessage 	= onMessage;
		socket.onclose 	 	= onClose;
	} catch(exception) {
		alert('Error' + exception);  
	}
}