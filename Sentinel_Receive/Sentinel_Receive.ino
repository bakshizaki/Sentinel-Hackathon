/*
 * ZNRF_receive.ino
 *
 *  
 *      Author: Indrajeet Patil and Zaki Bakshi
 */
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SoftwareSerial.h>
#include <espduino.h>
#include <rest.h>
#include "TimerOne.h"
SoftwareSerial debugPort(2, 3); // RX, TX

ESP esp(&Serial, &debugPort, 4);
uint16_t counter = 1, error_code;
//String get_req;
REST rest(&esp);
unsigned long http_time = 0;

boolean wifiConnected = false;

void wifiCb(void* response)
{
  uint32_t status;
  RESPONSE res(response);

  if (res.getArgc() == 1) {
    res.popArgs((uint8_t*)&status, 4);
    if (status == STATION_GOT_IP) {
      debugPort.println("WIFI CONNECTED");

      wifiConnected = true;
    } else {
      wifiConnected = false;
    }

  }
}

/*-----( Declare Constants and Pin Numbers )-----*/
#define CE_PIN   9
#define CSN_PIN 10

// NOTE: the "LL" at the end of the constant is "LongLong" type
const uint64_t pipe = 0xE8E8F0F0E1LL, pipe2 = 0xF0F0F0F0E1LL; // Define the transmit pipe


/*-----( Declare objects )-----*/
RF24 radio(CE_PIN, CSN_PIN); // Create a Radio
/*-----( Declare Variables )-----*/
uint8_t fsr[2];
int i, index;
String rec_data[5];
boolean is_msg_received = false;
uint8_t get_tries = 0;
unsigned long time_since_epoch = 0, time_from_ntp = 0;

void setup()   /****** SETUP: RUNS ONCE ******/
{
  Serial.begin(19200);
  debugPort.begin(19200);
  debugPort.println(F("\n------Starting------\n"));

  esp.enable();
  delay(500);
  esp.reset();
  delay(500);
  while (!esp.ready());

  debugPort.println(F("ARDUINO: setup rest client"));
  if (!rest.begin("128.199.166.20")) {
    debugPort.println(F("ARDUINO: failed to setup rest client"));
    while (1);
  }

  debugPort.println(F("ARDUINO: setup wifi"));
  esp.wifiCb.attach(&wifiCb);
//    esp.wifiConnect("Chitale Bandhu", "Chitale@123");
  esp.wifiConnect("Connectify-Z", "qwertyui");
//    esp.wifiConnect("who is back", "allianceisback");
  debugPort.println(F("ARDUINO: system started"));
  while (wifiConnected == false) {
    esp.process();
  }

  delay(1000);
  radio.begin();
  radio.openReadingPipe(1, pipe);
  radio.startListening();

  time_since_epoch = getTimeNTP();
  Timer1.initialize(1000000);
  Timer1.attachInterrupt(incrementEpoch);

  //  while (1) {
  //    esp.process();
  //    debugPort.print("Time: ");
  //    debugPort.println(time_since_epoch);
  //    delay(500);
  //  }

  /*  uint16_t counter = 0;
    while (1) {
      esp.process();
      if (wifiConnected) {
        char response[50];
        String get_req = "/request.php?i=3&c=55&h=66&t=77&s=";
        get_req.concat(String(counter++));
        debugPort.print("Paath: ");
        debugPort.println(get_req);
        do {
          rest.get(&get_req[0]);
          error_code = rest.getResponse(response, 50);
          delay(200);
        } while (response[0] != '1');
        debugPort.println("RESPONSE: ");
        debugPort.println(response);


      }
    }
  */
}//--(end setup )---



void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
//  debugPort.println(">");
  esp.process();
  //Serial.println("here");
  if (radio.available() > 0)
  {
    //esp.process();
    //Serial.println("here");
    // Read the data payload until we've received everything
    bool done = false;
    while (radio.available())
    {
      //esp.process();
      // Fetch the data payload
      radio.read(fsr, sizeof(fsr));
      for (i = 0; i < 2; i++) {
        //esp.process();

        if (fsr[i] != -1) {
          if (i == 0)
          {
            index = fsr[i];
          }
          if (i == 1)
          {

            if (fsr[i] == '\n')
            {
              debugPort.print(index);
              debugPort.print("=");
              debugPort.println(rec_data[index]);
              is_msg_received = true;
            }
            else
              rec_data[index] += (char)fsr[i];
          }
        }
      }
    }
  }
  else
  {
    //   Serial.println("No radio available");
  }

  if (is_msg_received == true) {
    char response[50];
    esp.process();
    if (wifiConnected) {
      String get_req = "/c/request.php?";
      get_req.concat(rec_data[index]);
      get_req.concat("&s=");
      get_req.concat(String(time_since_epoch));
      debugPort.print("Paath: ");
      debugPort.println(get_req);
      http_time = millis();
      do {
        rest.get(&get_req[0]);
        memset(response,0,sizeof(response));
        error_code = rest.getResponse(response, 50);
        get_tries++;
        debugPort.println("RESPONSE: ");
        debugPort.println(response);
      } while (response[0] != '1' && get_tries < 2);
      if (get_tries == 2)
      {
        debugPort.print(F("HTTP FAILED Error code: "));
        debugPort.println(error_code);
      }

      //      if (error_code == HTTP_STATUS_OK) {
      //        debugPort.println("RESPONSE: ");
      //        debugPort.println(response);
      //      }
      //      else {
      //        debugPort.print("HTTP FAILED Error code: ");
      //        debugPort.println(error_code);
      //      }
    }

    is_msg_received = false;
    rec_data[index] = "";
    debugPort.print(F("Time Duration"));
    debugPort.println(millis() - http_time);
    radio.flush_rx_buffer();
    //    radio.flush_rx();

  }

}//--(end main loop )---

/*-----( Declare User-written Functions )-----*/

void incrementEpoch() {
  time_since_epoch++;
}

unsigned long getTimeNTP() {
  unsigned long time_ntp = 0;
  if (wifiConnected == false)
    return 0;
  char response[50];
  esp.process();
  if (wifiConnected) {
    String get_req = "/c/time.php";
    debugPort.print("Paath: ");
    debugPort.println(get_req);
    rest.get(&get_req[0]);
    error_code = rest.getResponse(response, 50);


    if (error_code == HTTP_STATUS_OK) {
      debugPort.println("RESPONSE: ");
      debugPort.println(response);
      time_ntp = atol(response);
      return time_ntp;
    }
    else {
      debugPort.print(F("HTTP FAILED Error code: "));
      debugPort.println(error_code);
      return 0;
    }
    debugPort.println("$RESPONSE: ");
      debugPort.println(response);
      
  }


}

//NONE
//*********( THE END )***********
