Sentinel is our project for IndiaHacks:Internet of Things 2016.  
Basically Sentinel consists of 3 parts. 1)I-Plus 2)W-Plus 3)Cloud Application  
1) For I-Plus use Sentinel.ino  
2) For W-Plus use ZNRF_receive.ino  
3) For Cloud Application use Cloud Application folder  
4) For I-Plus RFID use HACKER_NEW

Arduino libararies used for the prject. Thanks to all the contributors  
1)	RF24 library for NRF24L01. https://github.com/maniacbug/RF24  
2)	ESPduino library for  ESP8266. https://github.com/tuanpmt/espduino  
3)	DHT libarary for DHT11. https://github.com/adafruit/DHT-sensor-library  

##Instruction for Testing Cloud Application
W-Plus logs data to server through HTTP GET request.  
## To log data
Paste the following link in your browser to log data.  
http://128.199.166.20/c/request.php?i=3&ra=0&rac=0&oac=0&oa=0&gac=0&ga=0&h=30&t=30&eid=1&s=1458244246
Legend:  
	i – stands for machine id  
	rac – Red Alerts  
    ra - Red Counts  
    gac – Green Alerts  
    ga - Green Counts  
    oac – Orange Alerts  
    oa - Orange Counts  
	h – humidity  
	t – temperature  
    eid - Employee ID  
	s – unix timestamp  
    You can change these parameters to experiment but make sure you enter a valid unix time  
##To check data online
Paste to following link in your browser:  
http://128.199.166.20/c/  
Use the following credentials:  
ID:1  
Password: 1234  
Select Machine 3 from dropdown menu to see the relevant data  

##Hardware Files
We believe in open hardware. Here are our schematics and PCB design files.
