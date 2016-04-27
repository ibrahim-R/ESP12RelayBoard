import urllib.request
from time import sleep
for x in range(0,1000):
    #print("Switching on relay")
    #r = urllib.request.urlopen("http://wifiswitch.local/gpio?state=0")
    #print (r.msg)
    #sleep(1);
    #print("Switching off relay")
    #r = urllib.request.urlopen("http://wifiswitch.local/gpio?state=1")
    r = urllib.request.urlopen("http://192.168.4.1")
    print (r.msg)
    sleep(1);
    
    
