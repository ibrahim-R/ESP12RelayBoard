void webHandleConfigInfo(){
  uint8_t mac[6];
  String st;
  String sep="esp_sep";

  st="EspRelayBoard"+sep;
  IPAddress ip = WiFi.localIP();
  String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
  st+="IP:,:" + ipStr +sep;
  st+="AP:,:" + String(esid.c_str())+sep;
  //st+="\nP:" + String(epass.c_str())+sep;
  st+="DN:,:" + String(FQDN.c_str())+sep;
  st+="V:,:" + spVersion+sep;
  st+="numSwitches:,:" + String(numSwitches) +sep + "switchState:,:";
  for(int ii=0;ii<numSwitches;ii++) {
      st+=String(digitalRead(outPin[ii]));
  }


  ESP.wdtFeed();
  server.send(200, "text/html", st);
  ESP.wdtFeed();
  delay(1000);
  ESP.wdtFeed();
}


void webHandleConfig(){
  IPAddress ip = WiFi.softAPIP();
  String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
  String s;
  
  
  if( st_radiobutton.length()<0) {//we dont want to prettify if we have a large packet to send
    s=css;
  } 
  s += "<form method='get' action='a' ><p><h1>EspRelayBoard</h1></p>";
  
  if(st_radiobutton.length()<1400) {
    s+="<label>Select your Wifi Network: </label><br>";
    s += st_radiobutton;
  } else {
    s+= "<br><br><label>Enter your Wifi Network Name: </label><input name='ssid' type='text' length=32 style='height: 3vh;'></br>";
  }
  
  s += "<br><br><label>Enter your Wifi Password: </label><input name='pass' type='password' length=64 style='height: 3vh;'></br>";
  s += "<br><br><label>Name your switch (optional): </label><input name='deviceName' type='text' value='EspRelayBoard' length=32 style='height: 3vh;'></br>";
  s += "<br><br><label>Static IP (optional): </label><input name='staticIP' type='text'  length=32 style='height: 3vh;'></br>";
  
  s += "<br><br><button name='connect' value='connect' type='submit' style='height: 6vh; font: 4vh Arial, Helvetica, sans-serif;'>Connect</button></form>";
  s += "\r\n\r\n";

  
  Serial.println("Sending 200");  
  ESP.wdtFeed();
  server.send(200, "text/html", s); 
  ESP.wdtFeed();
  for(int ii=0;ii<10;ii++) {
    delay(100);
  }
  ESP.wdtFeed();
}
void webHandleConfigSave(){
  // /a?ssid=blahhhh&pass=poooo
  String s;
  s =css;
  s += "<p >Settings Saved. If successfull, the device will be accessible at "+server.arg("deviceName")+".local and the configured IP(http://*.*.*.140 if left blank ).  </p>\r\n\r\n";
  ESP.wdtFeed();
  server.send(200, "text/html", s);
  ESP.wdtFeed();
  delay(1000); 
  ESP.wdtFeed();
  if(1) {
    Serial.println("clearing EEPROM.");
    clearEEPROM();
    String qsid; 
    qsid = server.arg("ssid");
    qsid.replace("%2F","/");
    qsid.replace("+"," ");
    Serial.println(qsid);
    Serial.println("");
  
    String qpass;
    qpass = server.arg("pass");
    qpass.replace("%2F","/");
    qpass.replace("+"," ");
    Serial.println(qpass);
    Serial.println("");
  
    String qdeviceName;
    qdeviceName = server.arg("deviceName");
    qdeviceName.replace("%2F","/");
    qdeviceName.replace("+"," ");
    Serial.println(qdeviceName);
    Serial.println("");
    
    String qdeviceIP;
    qdeviceIP = server.arg("staticIP");
    qdeviceName.replace("%2F","/");
    qdeviceName.replace("+"," ");
    Serial.println(qdeviceIP);
    Serial.println("");

    EEPROM.begin(512);
    delay(10);
    Serial.println("writing eeprom ssid.");
    //addr += EEPROM.put(addr, qsid);
    for (int i = 0; i < qsid.length(); ++i)
      {
        EEPROM.write(i, qsid[i]);
        Serial.print(qsid[i]); 
      }
    Serial.println("");
      
    Serial.println("writing eeprom pass."); 
    //addr += EEPROM.put(addr, qpass);
    for (int i = 0; i < qpass.length(); ++i)
      {
        EEPROM.write(32+i, qpass[i]);
        Serial.print(qpass[i]); 
      }  
    Serial.println("");
    Serial.println("writing deviceName."); 
    //addr += EEPROM.put(addr, qpass);
    if(qdeviceName.length()==0) {
      qdeviceName="EspRelayBoard";
    }
    for (int i = 0; i < qdeviceName.length(); ++i)
      {
        EEPROM.write(96+i, qdeviceName[i]);
        Serial.print(qdeviceName[i]); 
      }  
      
    Serial.println("");
    for (int i = 0; i < numSwitches; ++i)
      {
        EEPROM.write(128+i, '0');
        
      }  
    if(qdeviceIP.length()==0) {
      qdeviceIP="192.168.1.140";
    }
    for (int i = 0; i < qdeviceIP.length(); ++i)
      {
        EEPROM.write(144+i, qdeviceIP[i]);
        Serial.print(qdeviceIP[i]); 
      }  
      
    Serial.println("");
  
    for (int i = 176; i < 300; ++i)
      {
        EEPROM.write(i, 0);
        
      }  
     
    ESP.wdtFeed();
    EEPROM.commit();
    ESP.wdtFeed();
    for(int ii=0;ii<100;ii++) {
      delay(10);
      ESP.wdtFeed();
    }
    EEPROM.end();
    
    Serial.println("Settings written, restarting!"); 
    ESP.wdtFeed();
    system_restart();
    
  }
}

void webHandleRoot(){
  /*String s;
  s=css;
  s += "<div ><p ><h1>Hey! EspRelayBoard here!</h1>";
  s += "</p>";
  s += "<a href=\"/gpio\">Control Switches</a><br />";
  s += "<a href=\"/cleareeprom\">Reconfigure EspRelayBoard</a><br /></div>";
  s += "\r\n\r\n";
  Serial.println("Sending 200");  
  server.send(200, "text/html", s); */
  webHandleGpio();
}

void webHandleClearRom(){
  String s;
  s = css;
  s += "<p >Reset Complete. Connect to the wifi network 'EspRelayBoard' to setup your wireless switch.<p>";
  s += "</html>\r\n\r\n";
  ESP.wdtFeed();
  Serial.println("Sending 200"); 
  server.send(200, "text/html", s);
  ESP.wdtFeed();
  delay(1000);
  ESP.wdtFeed(); 
  Serial.println("clearing eeprom");
  clearEEPROM();
  ESP.wdtFeed();
  delay(1000);
  ESP.wdtFeed();
  Serial.println("Done, restarting!");
  system_restart();
}

void webHandleGpio(){
  String s;
  ESP.wdtFeed();
   // Set GPIO according to the request
   String stateArg=server.arg("state");
   if (server.arg("state")=="reset") {
      webHandleClearRom();
      return;     
    }
    for(int ii=0;ii<numSwitches;ii++) {
      if (stateArg== (String(ii)+"ON") || stateArg==(String(ii)+"OFF") ) {
        int state = (stateArg.substring(1)=="OFF")? 0 : 1;
        
        prevSwitchState[ii]=state+48;  
        digitalWrite(outPin[ii], state);
        EEPROM.begin(512);
        EEPROM.write(128+ii,prevSwitchState[ii]);
        EEPROM.commit();
        EEPROM.end();
        Serial.print("Light " + String(ii) +" switched via web request to  ");      
        Serial.println(state);      
      }
    }
    s=css;
    s += "<div ><p><h1>EspRelayBoard</h1><br></p>";
    /*s+="Switch is currently ";
    s += (digitalRead(outPin))?"OFF":"ON";
    s += "<br>Change to <form action='gpio'><input type='radio' name='state' value='1' ";
    s += (~digitalRead(outPin))?"checked":"";
    s += ">On<input type='radio' name='state' value='0' ";
    s += (~digitalRead(outPin))?"":"checked";*/
    s += " <form action='gpio'>";
    for(int ii=0;ii<numSwitches;ii++) {
      if(digitalRead(outPin[ii])) {      
        s+="<p ><button type='submit' name='state' value='"+ String(ii) +"ON' style='height: 7vh; font: 6vh Arial, Helvetica, sans-serif; '>ON</button>    <span><button type='submit' name='state' value='"+ String(ii) +"OFF' style='height: 8vh; font: 6vh Arial, Helvetica, sans-serif; background-color: #FF0000;'>OFF</button></span></p>";
      } else {
       s+="<p ><button type='submit' name='state' value='"+ String(ii) +"ON' style='height: 8vh; font: 6vh Arial, Helvetica, sans-serif; background-color: #00FF00;'>ON</button>    <span><button type='submit' name='state' value='"+ String(ii) +"OFF' style='height: 7vh; font: 6vh Arial, Helvetica, sans-serif;'>OFF</button></span></p>"; 
      }
      
    }
    s +=  "<br><br><br><p><button type='submit' name='state' value='reset' style='background-color:#FFF; color: #333;'>RECONFIGURE</button></p>";
    s +="</form></div>"; 
    ESP.wdtFeed();
    server.send(200, "text/html", s); 
    ESP.wdtFeed();
    delay(1000);   
    ESP.wdtFeed();
}
