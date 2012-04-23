var socket;
var outputDiv;
var canvas;
var ctx;

$(document).ready( function() {
	outputDiv 	= document.getElementById('output');
	canvas 		= document.getElementById('contourCanvas');
	ctx			= canvas.getContext('2d');
	ctx.fillStyle = 'rgba(0,0,0,0)';
	setupSocket();
	
	document.getElementById("brow").textContent = " " + BrowserDetect.browser + " "
		+ BrowserDetect.version +" " + BrowserDetect.OS +" ";
	
});

function render(){
	/*canvas.width = canvas.width;
	// draw contours

	// draw rect
	ctx.strokeStyle = 'black';
	ctx.strokeRect(person.boundingrect.x*vid_width,person.boundingrect.y*vid_height, person.boundingrect.width*vid_width, person.boundingrect.height*vid_height);
	ctx.stroke();

	// draw contours
	ctx.fillStyle = '#111111';
	ctx.font	  = "11pt Helvetica";
	ctx.fillText("id: "+person.id, person.centroid.x*vid_width, person.centroid.y*vid_height);
	ctx.fillText("age: "+person.age, person.centroid.x*vid_width, person.centroid.y*vid_height+20);
	ctx.fillText("depth: "+person.depth, person.centroid.x*vid_width, person.centroid.y*vid_height+40);
	ctx.strokeStyle = '#ff0000';
	ctx.beginPath();
	ctx.moveTo(person.contours[0].x*vid_width,person.contours[0].y*vid_height);

	for (var j=1; j<person.contours.length; j++ ){
		ctx.lineTo( person.contours[j].x*vid_width,person.contours[j].y*vid_height );
	}				
	ctx.stroke();*/
}

// setup web socket
function setupSocket(){
	if (BrowserDetect.browser == "Firefox") {
		socket = new MozWebSocket(get_appropriate_ws_url());//,"tsps-protocol");
	} else {
		socket = new WebSocket(get_appropriate_ws_url());//,"tsps-protocol");
	}
	
	// open
	try {
		socket.onopen = function() {
			document.getElementById("wslm_statustd").style.backgroundColor = "#40ff40";
			document.getElementById("wslm_statustd").textContent = " websocket connection opened ";
		} 

		// received message
		socket.onmessage =function got_packet(msg) {
			try {
				var data =  jQuery.parseJSON( msg.data );
			} catch (e){
			}

			render();
		}

		socket.onclose = function(){
			document.getElementById("wslm_statustd").style.backgroundColor = "#ff4040";
			document.getElementById("wslm_statustd").textContent = " websocket connection CLOSED ";
		}
	} catch(exception) {
		alert('<p>Error' + exception);  
	}
}