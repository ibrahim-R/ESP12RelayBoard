/*MQTT not supported currently*/
boolean connectMQTT(){
  /*if (mqttClient.connected()){
    return true;
  }  
  
  uint8_t mac[6];
  WiFi.macAddress(mac);
  clientName += "-";
  clientName += macToStr(mac);
  
  Serial.print("Connecting to MQTT server ");
  Serial.print(mqttServer);
  Serial.print(" as ");
  Serial.println(clientName);
  
  if (mqttClient.connect((char*) clientName.c_str())) {
    Serial.println("Connected to MQTT broker");
    if(mqttClient.subscribe((char*)subTopic.c_str())){
      Serial.println("Subsribed to topic.");
    } else {
      Serial.println("NOT subsribed to topic!");      
    }
    return true;
  }
  else {
    Serial.println("MQTT connect failed! ");
    return false;
  }*/
  return false;
}

void disconnectMQTT(){
  //mqttClient.disconnect();
}

void mqtt_handler(){
  /*if (toPub==1){
    if(pubState()){
     toPub=0; 
    }
  }
  mqttClient.loop();
  delay(100); //let things happen in background*/
}

void mqtt_arrived(char* subTopic, byte* payload, unsigned int length) { // handle messages arrived 
  /*int i = 0;
  Serial.print("MQTT message arrived:  topic: " + String(subTopic));
    // create character buffer with ending null terminator (string)
  for(i=0; i<length; i++) {    
    buf[i] = payload[i];
  }
  buf[i] = '\0';
  String msgString = String(buf);
  Serial.println(" message: " + msgString);
  if (msgString == "1"){
      Serial.print("Light is ");
      Serial.println(digitalRead(outPin[0]));      
      Serial.print("Switching light to "); 
      Serial.println("high");
      digitalWrite(outPin[0], 1); 
  } else if (msgString == "0"){
      Serial.print("Light is ");
      Serial.println(digitalRead(outPin[0]));    
      Serial.print("Switching light to "); 
      Serial.println("low");
      digitalWrite(outPin[0], 0); 
  }*/    
}

boolean pubState(){ //Publish the current state of the light    
  /*if (!connectMQTT()){
      delay(100);
      if (!connectMQTT){                            
        Serial.println("Could not connect MQTT.");
        Serial.println("Publish state NOK");
        return false;
      }
    }
    if (false){      
      String state = (digitalRead(outPin[0]))?"1":"0";
        Serial.println("To publish state " + state );  
      if (false) {
        Serial.println("Publish state OK");        
        return true;
      } else {
        Serial.println("Publish state NOK");        
        return false;
      }
     } else {
         Serial.println("Publish state NOK");
         Serial.println("No MQTT connection.");        
     }    */
     return false;
}
