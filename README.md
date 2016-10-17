#Sentinel  
Sentinel is our project for IndiaHacks:Internet of Things 2016 Hackathon which received 6th place at the competition.  

Sentinel is a system which can be used to remotely monitor a number of industrial machineries.
It connects the local machine with internet over HTTP to push the pertinent
parameters to cloud. We have also developed our own scalable cloud application
which is capable of visual representation of data.
  
###Building
Basically Sentinel consists of 3 parts. 1)I-Plus 2)W-Plus 3)Cloud Application  
1) For I-Plus use Sentinel_Trans.ino  
2) For W-Plus use Sentinel_Receive.ino  
3) For Cloud Application use Cloud Application folder  
4) For I-Plus RFID use HACKER_NEW

Arduino libararies used for the prject. Thanks to all the contributors  
1)	RF24 library for NRF24L01. https://github.com/maniacbug/RF24  
2)	ESPduino library for  ESP8266. https://github.com/tuanpmt/espduino  
3)	DHT libarary for DHT11. https://github.com/adafruit/DHT-sensor-library  


##Hardware Files  
Hardware schematics are available in Hardware Files.  
