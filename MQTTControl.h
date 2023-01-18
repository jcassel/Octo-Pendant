//#define _debugopraMQTT  //uncomment to get Serial debug messages related to MQTT

void MQTTTele(String subject, String message){
  if(mqttClient.connected()){
    mqttClient.publish("tele/"+ String(wificonfig.hostname) +"/"+ subject,message);
    #ifdef _debugopraMQTT
    Serial.println("Sending MQTTInfo");
    #endif
  }
}


void MQTTLWT(bool setWill){
  
    String topic = "tele/"+ String(wificonfig.hostname) +"/LWT";
    if(setWill){
      mqttClient.setWill(topic.c_str(), "Offline",true,2);
      #ifdef _debugopraMQTT
      Serial.println("Set Will: Offline");
      #endif
    }else{
      if(mqttClient.connected()){
        mqttClient.publish(topic.c_str(), "Online",true,0);
    }
  }
}

char MQTTSrvIp[128];
char MQTTUser[32];
char MQTTPW[64];
  
  
boolean connectMqttOptions()
{
  boolean result;
  MQTTLWT(true); //set last will and testament
  if (mqttConfig.MQTTPW != "")
  {
    #ifdef _debugopraMQTT
    Serial.print("Connecting to MQTT Server with UserName and Password: ");
    #endif
    result = mqttClient.connect(wificonfig.hostname, mqttConfig.MQTTUser, mqttConfig.MQTTPW);
    
  }
  else if (mqttConfig.MQTTUser != "")
  {
    #ifdef _debugopraMQTT
    Serial.print("Connecting to MQTT Server with UserName no password: ");
    #endif
    result = mqttClient.connect(wificonfig.hostname, mqttConfig.MQTTUser);
    
  }
  else
  {
    #ifdef _debugopraMQTT
    Serial.print("Connecting to MQTT Server with no credentials: ");
    #endif
    result = mqttClient.connect(wificonfig.hostname);
    
  }
  #ifdef _debugopraMQTT
  Serial.println(result);
  #endif
  return result;
}

void MQTTStatus(){
  if(mqttClient.connected()){
    mqttClient.publish("stat/"+ String(wificonfig.hostname) + "/","Ready");
    MQTTTele("IP",IpAddress2String(WiFi.localIP()));
    MQTTTele("HAUniqueId",settingsConfig.unique_Id);
    #ifdef _debugopraMQTT
    Serial.println("Sending MQTTStatus");
    #endif
  }
}

boolean connectMqtt() {
  #ifdef _debugopraMQTT
  Serial.println("Attempting Connection to MQTT server...");
  #endif
  if (!connectMqttOptions()) {
    #ifdef _debugopraMQTT
    Serial.println("Attempt Failed");
    #endif
    return false;
  }
  #ifdef _debugopraMQTT
  Serial.println("Connected!");
  #endif
    
  MQTTLWT(false); //send current Testiment Of Online
  MQTTStatus();
  
  //Subscribe to command channel
  mqttClient.subscribe("cmnd/"+ String(wificonfig.hostname) + "/#");
  
  return true;
  
}

void mqttMessageReceived(String &topic, String &payload)
{
  #ifdef _debugopraMQTT
  Serial.println("Incoming: " + topic + " - " + payload);
  #endif
  
  if(topic.endsWith("Info")){
    if(payload == "IP"){
       MQTTTele("IP",IpAddress2String(WiFi.localIP()));
    }
  }
  
}


bool CheckMQTTConnection(){
  if(isWiFiConnected(true) && !mqttClient.connected()){
    #ifdef _debugopraMQTT
    Serial.println("WiFi is connected and MQTTClient is not connected");
    #endif
    if(ReconnectMQTT.check()){ //has a connection attempt been made reciently
      #ifdef _debugopraMQTT
      Serial.println("Attempting MQTT (re)connect");
      #endif
      connectMqtt();  
      ReconnectMQTT.set(30000UL); //30Sec
      
    }else{
      #ifdef _debugopraMQTT
      Serial.print("MQTT Reconnect timer running: ");
      Serial.println(ReconnectMQTT.timeLeft());
      #endif
    }
  }else{
    if(ReconnectMQTT.check()){ //has a connection attempt been made reciently
      #ifdef _debugopraMQTT
      Serial.println("Wifi and MQTT are connected");
      #endif
      ReconnectMQTT.set(30000UL); //30Sec 
    }
  }
  return mqttClient.connected();
}
