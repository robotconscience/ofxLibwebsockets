// GLOBAL VARS
var socket;
var canvas;
var ctx;

var canvasData;
var data;
var type;

//----------------------------------------------------------------------------------------------------------------
// ON READY, SETUP SOCKET!
//----------------------------------------------------------------------------------------------------------------

$(document).ready( function() {
	canvas 		= document.getElementById('contourCanvas');
	ctx			= canvas.getContext('2d');

    canvasData = ctx.getImageData(0,0,canvas.width, canvas.height);
    data = canvasData.data;

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
		data = canvasData.data;

		var bytearray = new Uint8Array( messageEvent.data );
		var index = 0;

		// b & w
		if ( type == 0 ){
			for (var i = 0; i < data.length; i+=4) {
            	data[i] = bytearray[index]; 
	            data[i + 1] = bytearray[index];
	            data[i + 2] = bytearray[index];
	            data[i + 3] = 255;
	            index++;
			}
		// rgb
		} else if ( type == 1 ){
			for (var i = 0; i < data.length; i+=4) {
	            data[i] = bytearray[index]; index++;
	            data[i + 1] = bytearray[index]; index++;
	            data[i + 2] = bytearray[index]; index++;
	            data[i + 3] = 255;
			}
		// rgba
		} else {
			console.log( bytearray.length );
			console.log( data.length );
			for (var i = 0; i < bytearray.length; i++) {
	            data[i] = bytearray[i]
			}
		}

        ctx.putImageData(canvasData,0,0);
	} else if (messageEvent.data instanceof Blob) {
		var image = new Image();
		image.onload = function () {
			ctx.clearRect(0, 0, destinationCanvas.width, destinationCanvas.height);
			ctx.drawImage(image, 0, 0);
			document.body.appendChild(image);
		}

		if ( window.URL ){
			image.src = window.URL.createObjectURL(messageEvent.data);
		} else if ( window.webkitURL ){
			image.src = window.webkitURL.createObjectURL(messageEvent.data);
		} else {
			console.error( "your browser does not have a window.URL method :(")
		}
	} else {
		// here's where we'll catch the image stuff!

		var imgData;
		try {
			imgData =  jQuery.parseJSON( messageEvent.data );
		} catch( e ){
			imgData = messageEvent.data;
			var vals = imgData.split(":");
			try {
				if ( canvas.width != vals[0] || canvas.height != vals[1] ){
					canvas.width = vals[0];
					canvas.height = vals[1];
				    canvasData = ctx.getImageData(0,0,canvas.width, canvas.height);
				    data = canvasData.data;
				}
				type = vals[2];
			} catch ( e ){

			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------
// setup web socket
//----------------------------------------------------------------------------------------------------------------

function setupSocket(){
	// setup!
	if (BrowserDetect.browser == "Firefox") {
		socket = new MozWebSocket( get_appropriate_ws_url());
		socket.binaryType = "arraybuffer";
	} else {
		socket = new WebSocket( get_appropriate_ws_url());	
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