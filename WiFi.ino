void initWiFi(){
  Serial.println();
  Serial.println();
  Serial.println("Startup");
  esid.trim();
  if ( esid.length() > 1 ) {
      // test esid 
      WiFi.disconnect();
      delay(100);
      WiFi.mode(WIFI_STA);
      Serial.print("Connecting to WiFi ");
      Serial.println(esid);
      WiFi.begin(esid.c_str(), epass.c_str());
      if ( testWifi() == 20 ) { 
        
        IPAddress gateway=WiFi.gatewayIP();
        IPAddress subnet=WiFi.subnetMask();
        if(deviceIP=="192.168.1.140") {
          IPAddress ip=WiFi.localIP();  //Node static IP
          ip[3]=140;        
          WiFi.config(ip,WiFi.gatewayIP(),WiFi.subnetMask());        
        }
        else {
          int dotIndex = deviceIP.indexOf('.');
          //  Search for the next comma just after the first
          int secondDotIndex = deviceIP.indexOf('.', dotIndex+1);
          int thirdDotIndex = deviceIP.indexOf('.', secondDotIndex+1);
          if((dotIndex>0)&&(secondDotIndex>0)&&(thirdDotIndex>0)) {
          int firstValue = deviceIP.substring(0, dotIndex).toInt();
          int secondValue = deviceIP.substring(dotIndex+1, secondDotIndex).toInt();
          int thirdValue = deviceIP.substring(secondDotIndex+1,thirdDotIndex ).toInt();
          int fourthValue = deviceIP.substring(thirdDotIndex).toInt();
          IPAddress ip=IPAddress(firstValue,secondValue,thirdValue,fourthValue);  //Node static IP         
          WiFi.config(ip,WiFi.gatewayIP(),WiFi.subnetMask());     
          }   
        }
          if ( testWifi() == 20 ) { 
            launchWeb(0);
            return;
          }
      }
  }
  Serial.println("Opening AP");
  setupAP();   
}

int testWifi(void) {
  int c = 0;
  Serial.println("Wifi test...");  
  while ( c < 30 ) {
    if (WiFi.status() == WL_CONNECTED) { return(20); } 
    ESP.wdtFeed();
    delay(500);
    ESP.wdtFeed();
    Serial.print(".");    
    c++;
  }
  Serial.println("WiFi Connect timed out");
  return(10);
} 

