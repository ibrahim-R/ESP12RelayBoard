# ESP Relay Board

Buy at https://www.tindie.com/products/RelayBoards/miniature-home-automation-relay-board-esp8266/

A miniature board(85mm x 35mm x 21mm) for home automation. 
To power on the device connect the Live and Neutral terminals to the respective wires from the mains. Connect the loads to the ports named L1,L2,L3.

Your board should now appear as the wifi network- EspRelayBoard. 

Detailed wiring instructions at
http://www.instructables.com/id/Miniature-ESP-Relay-Board-for-Home-Automation/

To configure enter 192.168.4.1 in your browser, enter your home network name and password.
Your board should now be accessible at 192.168.1.140. 
The board also accepts commands like
http://192.168.1.140/gpio?state=2OFF 
which turns off switch number 2

## Credits
For several snippets used the credit goes to:
  -https://github.com/biohazardxxx/ESP_WiFiSwitch
 - https://github.com/esp8266
 - https://github.com/chriscook8/esp-arduino-apboot
 - https://github.com/knolleary/pubsubclient
 - https://github.com/vicatcu/pubsubclient <- Currently this needs to be used instead of the origin
 - https://gist.github.com/igrr/7f7e7973366fc01d6393
 - http://www.esp8266.com/viewforum.php?f=25
 - http://www.esp8266.com/viewtopic.php?f=29&t=2745
 - And the whole Arduino and ESP8266 comunity
