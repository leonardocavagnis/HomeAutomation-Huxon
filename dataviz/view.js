// initialize web Socket and event handlers
let socket = new WebSocket("ws://localhost:8889");

var last_etime_1 = 0;
var last_etime_2 = 0;
var last_etime_3 = 0;
var last_etime_4 = 0;

var tamper_detected = false;

// websocket connection and message handling from server
socket.onopen = function(e) {
    console.log("[open] Connection established")
};

socket.onmessage = function(event) {
    console.log("[message] Data received from server: " + event.data);
    handleMessage(JSON.parse(event.data));
};

socket.onclose = function(event) {
    if (event.wasClean) {
        console.log("[close] Connection closed cleanly");
    } else {
        console.log("[close] Connection died");
    }
};

socket.onerror = function(error) {
    console.log("[error] " + error.message);
};

function handleMessage(message)
{
	let current = Date.parse(new Date());
	
	if (message.deviceId == "climaticmonitoring-vsens") {		
		const date = new Date(message.telemetry.temperaturehumidity.timestamp);
	
		document.getElementById('temp_id').innerHTML = message.telemetry.temperaturehumidity.temperature.toFixed(1);
		document.getElementById('hum_id').innerHTML = message.telemetry.temperaturehumidity.humidity.toFixed(0);
		document.getElementById('time_id').innerHTML = date.toLocaleString('en-GB');
		
		last_etime_1 = Date.parse(message.enqueuedTime);
	}
	
	if (message.deviceId == "personroomdetection-vsens") {
		const date_1 = new Date(message.telemetry.microphone.timestamp_1);
		const date_2 = new Date(message.telemetry.microphone.timestamp_2);
		
		if (message.telemetry.microphone.noise_1 == true) {
			document.getElementById("lr_occupacy_id").className = "w3-tag w3-small w3-padding w3-round-large w3-red w3-center";
			document.getElementById("lr_occupacy_id").innerHTML = "Busy";
		} else {
			document.getElementById("lr_occupacy_id").className = "w3-tag w3-small w3-padding w3-round-large w3-green w3-center";
			document.getElementById("lr_occupacy_id").innerHTML = "Free";
		}
		
		if (message.telemetry.microphone.noise_2 == true) {
			document.getElementById("br_occupacy_id").className = "w3-tag w3-small w3-padding w3-round-large w3-red w3-center";
			document.getElementById("br_occupacy_id").innerHTML = "Busy";
		} else {
			document.getElementById("br_occupacy_id").className = "w3-tag w3-small w3-padding w3-round-large w3-green w3-center";
			document.getElementById("br_occupacy_id").innerHTML = "Free";
		}
		
		document.getElementById('lr_time_id').innerHTML 	= date_1.toLocaleString('en-GB');
		document.getElementById('br_time_id').innerHTML 	= date_2.toLocaleString('en-GB');
		
		last_etime_2 = Date.parse(message.enqueuedTime);
	}
	
	if (message.deviceId == "tamperingdetection-vsens") {
		if (message.telemetry.accelerometer.tamper_1 == true) {
			const date_1 = new Date(message.telemetry.accelerometer.timestamp_1);
			document.getElementById('lr_tamper_id').innerHTML 	= date_1.toLocaleString('en-GB');
		}
		if (message.telemetry.accelerometer.tamper_2 == true) {
			const date_2 = new Date(message.telemetry.accelerometer.timestamp_2);
			document.getElementById('br_tamper_id').innerHTML 	= date_2.toLocaleString('en-GB');			
		}
		if (message.telemetry.accelerometer.tamper_3 == true) {
			const date_3 = new Date(message.telemetry.accelerometer.timestamp_3);
			document.getElementById('ed_tamper_id').innerHTML 	= date_3.toLocaleString('en-GB');	
		}
		
		if ( (message.telemetry.accelerometer.tamper_1 == true) || (message.telemetry.accelerometer.tamper_2 == true) || (message.telemetry.accelerometer.tamper_3 == true)) {
			tamper_detected = true;
		} 
		
		if (tamper_detected == true) {
			document.getElementById("alarm_status_id").className = "w3-tag w3-small w3-padding w3-round-large w3-red w3-center";
			document.getElementById("alarm_status_id").innerHTML = "Tamper";
		} else {
			document.getElementById("alarm_status_id").className = "w3-tag w3-small w3-padding w3-round-large w3-green w3-center";
			document.getElementById("alarm_status_id").innerHTML = "Ok";
		}
	
		last_etime_3 = Date.parse(message.enqueuedTime);
	}
	
	if (message.deviceId == "openwindowdetection-vsens") {
		const date_1 = new Date(message.telemetry.gyroscope.timestamp_1);
		const date_2 = new Date(message.telemetry.gyroscope.timestamp_2);
	
		if (message.telemetry.gyroscope.window_opened_1 == true) {
			document.getElementById("wlr_status_id").className = "w3-tag w3-small w3-padding w3-round-large w3-red w3-center";
			document.getElementById("wlr_status_id").innerHTML = "Open";
		} else {
			document.getElementById("wlr_status_id").className = "w3-tag w3-small w3-padding w3-round-large w3-green w3-center";
			document.getElementById("wlr_status_id").innerHTML = "Close";
		}
		
		if (message.telemetry.gyroscope.window_opened_2 == true) {
			document.getElementById("wbr_status_id").className = "w3-tag w3-small w3-padding w3-round-large w3-red w3-center";
			document.getElementById("wbr_status_id").innerHTML = "Open";
		} else {
			document.getElementById("wbr_status_id").className = "w3-tag w3-small w3-padding w3-round-large w3-green w3-center";
			document.getElementById("wbr_status_id").innerHTML = "Close";
		}
	
		document.getElementById('lr_wtime_id').innerHTML 	= date_1.toLocaleString('en-GB');
		document.getElementById('br_wtime_id').innerHTML 	= date_2.toLocaleString('en-GB');
		
		last_etime_4 = Date.parse(message.enqueuedTime);
	}
	
	if (((current - last_etime_1) <= 70000) && ((current - last_etime_2) <= 10000) && ((current - last_etime_4) <= 10000)) {
		document.getElementById("heatman_status_id").className = "w3-tag w3-small w3-padding w3-round-large w3-green w3-center";
		document.getElementById("heatman_status_id").innerHTML = "ON";
	} else {
		document.getElementById("heatman_status_id").className = "w3-tag w3-small w3-padding w3-round-large w3-red w3-center";
		document.getElementById("heatman_status_id").innerHTML = "OFF";
		
		/* Set N/A */
		document.getElementById('temp_id').innerHTML = "N/A";
		document.getElementById('press_id').innerHTML = "N/A";
		document.getElementById('time_id').innerHTML = "N/A";
		
		document.getElementById("lr_occupacy_id").className = "w3-tag w3-small w3-padding w3-round-large w3-gray w3-center";
		document.getElementById("lr_occupacy_id").innerHTML = "N/A";
		document.getElementById("br_occupacy_id").className = "w3-tag w3-small w3-padding w3-round-large w3-gray w3-center";
		document.getElementById("br_occupacy_id").innerHTML = "N/A";
		document.getElementById('lr_time_id').innerHTML 	= "N/A";
		document.getElementById('br_time_id').innerHTML 	= "N/A";
		
		document.getElementById("wlr_status_id").className = "w3-tag w3-small w3-padding w3-round-large w3-gray w3-center";
		document.getElementById("wlr_status_id").innerHTML = "N/A";
		document.getElementById("wbr_status_id").className = "w3-tag w3-small w3-padding w3-round-large w3-gray w3-center";
		document.getElementById("wbr_status_id").innerHTML = "N/A";
	}
	
	if ((current - last_etime_3) <= 10000) {
		document.getElementById("alarmman_status_id").className = "w3-tag w3-small w3-padding w3-round-large w3-green w3-center";
		document.getElementById("alarmman_status_id").innerHTML = "ON";
	} else {
		document.getElementById("alarmman_status_id").className = "w3-tag w3-small w3-padding w3-round-large w3-red w3-center";
		document.getElementById("alarmman_status_id").innerHTML = "OFF";
		
		/* Set N/A */
		document.getElementById("alarm_status_id").className = "w3-tag w3-small w3-padding w3-round-large w3-gray w3-center";
		document.getElementById("alarm_status_id").innerHTML = "N/A";
		document.getElementById("lr_tamper_id").innerHTML 	= "N/A";
		document.getElementById("br_tamper_id").innerHTML 	= "N/A";
		document.getElementById("ed_tamper_id").innerHTML 	= "N/A";
	}
}