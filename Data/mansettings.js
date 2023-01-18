var lastGCF = '';
var chRL = false;
var chST = false;

domReady(function() {
document.getElementById("action").innerHTML = "Fetching Settings...";
document.getElementById("loader").classList.remove('hidden');
document.getElementById("action").classList.remove('hidden');
GetSettings();
});

function GetSettings(){
	
	var xmlhttp = new XMLHttpRequest();
	
	xmlhttp.onload = function() {
		var reads = JSON.parse(this.responseText);
		document.getElementById("tx_SSID").value = reads.settings.ssid;
		document.getElementById("tx_WFPW").value = reads.settings.password;
		document.getElementById("tx_DNAM").value = reads.settings.hostname;
		document.getElementById("tx_APW").value = reads.settings.apPassword;
		document.getElementById("tx_STI").value = reads.settings.StatusIntervalSec;
		document.getElementById("tx_HAUID").value = reads.settings.unique_Id;
		
		if(reads.settings.MQTTEN == "1"){
			document.getElementById("ch_MQTTEN").checked = true;
			//chST = document.getElementById("ch_MQTTEN").checked;
		}
		
		
		document.getElementById("tx_MQTTUSR").value = reads.settings.MQTTUser;
		document.getElementById("tx_MQTTSRV").value = reads.settings.MQTTSrvIp;
		document.getElementById("tx_MQTTPW").value = reads.settings.MQTTPW;
		
		document.getElementById("tx_MQTTPort").value = reads.settings.MQTTPort;
		document.getElementById("tx_WFCA").value = reads.settings.attempts;
		document.getElementById("tx_WFAD").value = reads.settings.attemptdelay;
		document.getElementById("cbo_WFMD").value = reads.settings.wifimode;
		
		
		document.getElementById("tx_OPURI").value = reads.settings.OPURI;
		document.getElementById("tx_OPPort").value = reads.settings.OPPort;
		document.getElementById("tx_OPAK").value = reads.settings.OPAK;
		document.getElementById("tx_OPLGCF").value = reads.settings.OPLGCF;
		lastGCF = document.getElementById("tx_OPLGCF").value;
		
		document.getElementById("tx_BVX").value = reads.settings.BVX;
		document.getElementById("tx_BVY").value = reads.settings.BVY;
		document.getElementById("tx_BVO").value = reads.settings.BVO;
		
		
		document.getElementById("tx_PPFL").value = reads.settings.PProfile;
		if(reads.settings.ACONP == "1"){document.getElementById("ch_ACONP").checked = true;}	
		
		if(reads.settings.EML == "1"){document.getElementById("ch_EML").checked = true;}		
		
		if(reads.settings.OPReload == "1"){
			document.getElementById("ch_OPReload").checked = true;
			chRL = document.getElementById("ch_OPReload").checked;
		}
		
		if(reads.settings.OPStart == "1"){
			document.getElementById("ch_OPStart").checked = true;
			chST = document.getElementById("ch_OPStart").checked;
		}
		
		if(reads.settings.BLAZ == "1"){document.getElementById("ch_BLAZ").checked = true;}
		
		
		document.getElementById("tx_TZOS").value = reads.settings.TimeZoneOffsetHours;
		document.getElementById("status").innerHTML = reads.settings.LastStatus;
		document.getElementById("deviceInfo").innerHTML = reads.settings.hostname;
		document.getElementById("firmwareV").innerHTML = reads.settings.firmwareV;
		document.getElementById("loader").classList.add('hidden');
		document.getElementById("gcontainer").classList.remove('hidden');
		document.getElementById("action").innerHTML = "";
		document.getElementById("action").classList.add('hidden');
		
		checkEML();
		
		
	};
	xmlhttp.open("GET", "/APIGetSettings", true);
	xmlhttp.send();
	
}



function checkEML(){
	if(document.getElementById("ch_EML").checked == true){
		document.getElementById("tx_OPLGCF").disabled = false;
		document.getElementById("tx_OPLGCF").value = lastGCF;
				
		document.getElementById("ch_OPReload").disabled = false;
		document.getElementById("ch_OPReload").checked = chRL;
		
		document.getElementById("ch_OPStart").disabled = false;
		document.getElementById("ch_OPStart").checked = chST;
		
	}else{
		lastGCF = document.getElementById("tx_OPLGCF").value;
		document.getElementById("tx_OPLGCF").value = '';
		document.getElementById("tx_OPLGCF").disabled = true;		
		
		chRL = document.getElementById("ch_OPReload").checked;
		document.getElementById("ch_OPReload").checked = false;
		document.getElementById("ch_OPReload").disabled = true;
		
		chST = document.getElementById("ch_OPStart").checked;
		document.getElementById("ch_OPStart").checked = false;
		document.getElementById("ch_OPStart").disabled = true;
	}
}

function GetNetworks(){
	document.getElementById("wfdts").open = true;
	document.getElementById("networks").innerHTML = "Scanning Networks...";
	RequestNetScan();
	setTimeout(() => { ReadNetworks();}, 3000);
}

function ReadNetworks(){
	var xmlhttp = new XMLHttpRequest();
	
	xmlhttp.onload = function() {
		var resp = this.responseText;
		var SSIDs = JSON.parse(resp);
		if (SSIDs.networks.length > 0){
			
			var nets = document.getElementById("networks");
			nets.innerHTML = "";
			var list = document.getElementById("SSIDs");
			for(i = 0; i < SSIDs.networks.length; i++) {
				var option = document.createElement('option');
				option.value = SSIDs.networks[i].split(' ')[0];
				list.appendChild(option);
				nets.innerHTML += SSIDs.networks[i] + "<br/>";
				console.log(SSIDs.networks[i]);
			}
			
		}
	}
	
	
	xmlhttp.open("GET", "/APIGetNetworks", true);
	xmlhttp.send();

	

	
}
