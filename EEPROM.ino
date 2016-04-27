void clearEEPROM(){
  EEPROM.begin(512);
  // write a 0 to all 512 bytes of the  EEPROM
  for (int i = 0; i < 512; i++){
    EEPROM.write(i, 0);    
  }
  ESP.wdtFeed();
  delay(200);
  ESP.wdtFeed();
  EEPROM.commit(); 
  EEPROM.end(); 
}

void loadConfig(){
  EEPROM.begin(512);
  ESP.wdtFeed();
  delay(10);
  ESP.wdtFeed();
  String temp="";
  
  //len: 32
  //addr += EEPROM.get(addr, esid);
  for (int i = 0; i < 32; ++i)
    {
      esid += char(EEPROM.read(i));
    }
  Serial.print("SSID: ");
  Serial.println(esid);
  
  //len: 64+32=96
  //addr += EEPROM.get(addr, epass);
  for (int i = 32; i < 96; ++i)
    {
      epass += char(EEPROM.read(i));
    }
  Serial.print("PASS: ");
  Serial.println(epass);  
  ESP.wdtFeed();
  FQDN="";
  for (int i = 96; i < 120; ++i)
    {
      FQDN += char(EEPROM.read(i));
      
    }
    
  Serial.print("deviceName: ");
  Serial.println(FQDN);  
  ESP.wdtFeed();
  prevSwitchState="";
  for (int i = 128; i < (128+numSwitches); ++i)
    {
      prevSwitchState+= char(EEPROM.read(i));
      
    } 

   deviceIP="";
  for (int i = 144; i < 144+32; ++i)
    {
      deviceIP += char(EEPROM.read(i));
      
    }
    
  Serial.print("deviceIP: ");
  Serial.println(deviceIP);  
  ESP.wdtFeed();
  /*
  //len: 1+96=97
  String eiot = "";
  //addr += EEPROM.get(addr, iotMode);
  for (int i = 96; i < 97; ++i)
    {
      eiot += char(EEPROM.read(i));
    }
  Serial.print("IOT Mode: ");
  if(eiot=="1"){
    iotMode=1;
  }else{
    iotMode=0;
  }
  Serial.println(iotMode); 
    
  //len: 64+97=161
  //addr += EEPROM.get(addr, subTopic);
  for (int i = 97; i < 161; ++i)
    {
      subTopic += char(EEPROM.read(i));
    }
  Serial.print("MQTT subscribe topic: ");
  Serial.println(subTopic); 
  
  //len: 64+161=225
  //addr += EEPROM.get(addr, pubTopic);
  for (int i = 161; i < 225; ++i)
    {
      pubTopic += char(EEPROM.read(i));
    }
  Serial.print("MQTT publish topic: ");
  Serial.println(pubTopic); 
  
  //len: 15+225=240
  //addr += EEPROM.get(addr, mqttServer);
  for (int i = 225; i < 240; ++i)
    {
      temp=EEPROM.read(i);
      if(temp!="0") mqttServer += char(EEPROM.read(i)); // Ignore spaces
    }
  Serial.print("MQTT Broker IP: ");
  Serial.println(mqttServer);*/     
  EEPROM.end();
}
