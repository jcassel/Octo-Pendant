function RequestNetScan(){
	var res = fetch("/APIScanWifi");
}

function GetInfo(){
	
	var xmlhttp = new XMLHttpRequest();
	document.getElementById("loader").classList.remove('hidden');
	xmlhttp.onload = function() {
		var res = this.responseText;
		document.getElementById("deviceInfo").innerHTML = res.split(':')[0];
		document.getElementById("firmwareV").innerHTML = res.split(':')[1];
		document.getElementById("loader").classList.add('hidden');
		document.getElementById("gcontainer").classList.remove('hidden');
	};
	xmlhttp.open("GET", "/getDeviceName", true);
	xmlhttp.send();
	
}
