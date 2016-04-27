/*
 *  This sketch is running a web server for configuring WiFI if can't connect or for controlling of one GPIO to switch a light/LED
 *  Also it supports to change the state of the light via MQTT message and gives back the state after change.
 *  The push button has to switch to ground. It has following functions: Normal press less than 1 sec but more than 50ms-> Switch light. Restart press: 3 sec -> Restart the module. Reset press: 20 sec -> Clear the settings in EEPROM
 *  While a WiFi config is not set zzor can't connect:
 *    http://server_ip will give a config page with 
 *  While a WiFi config is set:
 *    http://server_ip/gpio -> Will display the GIPIO state and a switch form for it
 *    http://server_ip/gpio?state=0 -> Will change the GPIO directly and display the above aswell
 *    http://server_ip/cleareeprom -> Will reset the WiFi setting and rest to configure mode as AP
 *  server_ip is the IP address of the ESP8266 module, will be 
 *  printed to Serial when the module is connected. (most likly it will be 192.168.4.1)
 * To force AP config mode, press button 20 Secs!
 *  For several snippets used, the credit goes to:
 *  - https://github.com/esp8266
 *  - https://github.com/chriscook8/esp-arduino-apboot
 *  - https://github.com/knolleary/pubsubclient
 *  - https://github.com/vicatcu/pubsubclient <- Currently this needs to be used instead of the origin
 *  - https://gist.github.com/igrr/7f7e7973366fc01d6393
 *  - http://www.esp8266.com/viewforum.php?f=25
 *  - http://www.esp8266.com/viewtopic.php?f=29&t=2745
 *  - And the whole Arduino and ESP8266 comunity
 */

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <Ticker.h>
#include <PubSubClient.h>
#include <ESP8266HTTPUpdateServer.h>

extern "C" {
  #include "user_interface.h" //Needed for the reset command
}

//***** Settings declare ********************************************************************************************************* 
String ssid = "EspRelayBoard"; //The ssid when in AP mode
String clientName ="EspRelayBoard"; //The MQTT ID -> MAC adress will be added to make it kind of unique
String FQDN ="EspRelayBoard"; //The DNS hostname - Does not work yet?
//select GPIO's
const int numSwitches=3;
const int outPin[4] = {5,4,3}; //output pin
const int inPin = 13;  // input pin (push button)

const int restartDelay = 3; //minimal time for button press to reset in sec
const int humanpressDelay = 50; // the delay in ms untill the press should be handled as a normal push by human. Button debouce. !!! Needs to be less than restartDelay & resetDelay!!!
const int resetDelay = 20; //Minimal time for button press to reset all settings and boot to config mode in sec
const int setupTimeOutLimit=600; //Limit for setupTimeOut to reset within setup mode after some time. usefull after power break and WiFi not ready.
const int debug = 0; //Set to 1 to get more log to serial
//##### Object instances #####
ESP8266WebServer server(80);
WiFiClient wifiClient;
ESP8266HTTPUpdateServer httpUpdater;


//##### Flags ##### They are needed because the loop needs to continue and cant wait for long tasks!
int rstNeed=0;   // Restart needed to apply new settings
int toPub=0; // determine if state should be published.
int eepromToClear=0; // determine if EEPROM should be cleared.

//##### Global vars ##### 
int iotMode=0; //IOT mode: 0 = Web control, 1 = MQTT (No const since it can change during runtime)
int webtypeGlob; // Are we in normal(0) or AP web(1)
int current; //Current state of the button
unsigned long count = 0; //Button press time counter
String st; //WiFi Stations HTML list
String st_radiobutton; //WiFi Stations HTML list
String css;
char buf[40]; //For MQTT data recieve
int setupTimeOut=0; //Counter to reset within setup mode after some time. usefull after power break and WiFi not ready.

//To be read from EEPROM Config
String esid="";
String epass = "";
String pubTopic;
String subTopic;
String mqttServer = "";
String spVersion="v1";
String prevSwitchState="0000";
String deviceIP="192.168.1.140";
  

//-------------- void's -------------------------------------------------------------------------------------
void setup() {
  
  Serial.begin(115200);
  httpUpdater.setup(&server);
  loadConfig();
  if(debug==1) Serial.println("DEBUG: loadConfig() passed");
  
  uint8_t mac[6];
  WiFi.macAddress(mac);

  
  // Connect to WiFi network
  initWiFi();
  if(debug==1) Serial.println("DEBUG: initWiFi() passed");
  if(debug==1) Serial.println("DEBUG: Starting the main loop");
  for(int ii=0;ii<numSwitches;ii++) {
    pinMode(outPin[ii], OUTPUT);
    digitalWrite(outPin[ii], prevSwitchState[ii]-48);
  }
 
  css="";
  }



void launchWeb(int webtype) {
    Serial.println("");
    Serial.println("WiFi connected");    
    //Start the web server or MQTT
     if (webtype==1 || iotMode==0){ //in config mode or WebControle
        if (webtype==1) {           
          webtypeGlob == 1;
          Serial.println(WiFi.softAPIP());
          server.on("/", webHandleConfig);
          server.on("/a", webHandleConfigSave);          
          server.on("/info", webHandleConfigInfo);
          server.on("/gpio", webHandleGpio);
          server.on("/gpioL", webHandleGpio);
        } else {
          //setup DNS since we are a client in WiFi net
                    
          Serial.println(WiFi.localIP());
          server.on("/", webHandleRoot);  
          server.on("/cleareeprom", webHandleClearRom);
          server.on("/gpio", webHandleGpio);
          server.on("/info", webHandleConfigInfo);
          server.on("/gpioL", webHandleGpio);
        }
        //server.onNotFound(webHandleRoot);
        server.begin();
        Serial.println("Web server started");   
        webtypeGlob=webtype; //Store global to use in loop()
      } 
}



void setupAP(void) {
  
  WiFi.softAPdisconnect();
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  ESP.wdtFeed();
  delay(100);
  ESP.wdtFeed();
  populateSt();
  Serial.println(""); 
  WiFi.disconnect();
  ESP.wdtFeed();
  delay(100);
  ESP.wdtFeed();
  WiFi.mode(WIFI_AP);


  WiFi.softAP((char*) ssid.c_str());

  Serial.print("Access point started with name ");
  Serial.println(ssid);
  setupTimeOut=0;

  launchWeb(1);
}

void populateSt() {

  int n = WiFi.scanNetworks();
  ESP.wdtFeed();
  Serial.println("scan done");
  if (n == 0){
    Serial.println("no networks found");
    st ="<b>No networks found:</b>";
  } else {
    Serial.print(n);
    Serial.println(" Networks found");
    st = "<ul>";
    /* sample code for radio form
     <input type="radio" name="sex" value="male" checked>Male
    <br>
    <input type="radio" name="sex" value="female">Female*/
    st_radiobutton="";
    for (int i = 0; i < n; ++i)
     {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" (OPEN)":"*");
      
      // Print to web SSID and RSSI for each network found
      st += "<li>";
      st +=i + 1;
      st += ": ";
      st += WiFi.SSID(i);

      st_radiobutton+="<input type='radio' name='ssid' value='";
      st_radiobutton+= WiFi.SSID(i);
      st_radiobutton+="' ";
      if(i==0) {
        st_radiobutton+=" checked style='height: 3vh'><span >";
      } else {
        st_radiobutton+=" ><span style='height: 3vh'>";
      }
      st_radiobutton+= WiFi.SSID(i) ;
      
      st += " (";

      st += WiFi.RSSI(i);
      
      st += ")";
      
      st += (WiFi.encryptionType(i) == ENC_TYPE_NONE)?" (OPEN)":"*";
      st_radiobutton += (WiFi.encryptionType(i) == ENC_TYPE_NONE)?" (OPEN)":"";
      st += "</li>";
      st_radiobutton+= "</span><br>";
      ESP.wdtFeed();
      delay(10);
      ESP.wdtFeed();
     }
    st += "</ul>";
  }
}
//-------------------------------- Main loop ---------------------------
void loop() {
  
  if(debug==1) Serial.println("DEBUG: loop() begin");
  if(eepromToClear==1){
    if(debug==1) Serial.println("DEBUG: loop() clear EEPROM flag set!");
    clearEEPROM();
    ESP.wdtFeed();
    for(int ii=0;ii<100; ii++) {
      delay(10);
      ESP.wdtFeed();
    }
    
    system_restart();
  }
  if(debug==1) Serial.println("DEBUG: eeprom reset check passed");  
  if (WiFi.status() == WL_CONNECTED || webtypeGlob == 1){
    if(debug==1) Serial.println("DEBUG: loop() wifi connected & webServer ");
    if (iotMode==0 || webtypeGlob == 1){
      if(debug==1) Serial.println("DEBUG: loop() Web mode requesthandling ");
      server.handleClient();
      //mdns.update(); we get problems with this.
      delay(10);
    } 
  } else{
    setup();
    if(debug==1) Serial.println("DEBUG: loop - WiFi not connected");  
    ESP.wdtFeed();
    for(int ii=0;ii<100;ii++) {
        ESP.wdtFeed();
        delay(10);
        yield();        
    }
    ESP.wdtFeed();
    initWiFi(); //Try to connect again
  }
    if(debug==1) Serial.println("DEBUG: loop() end");
}
