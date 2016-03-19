/*
 * Sentinel.ino
 *
 *
 *      Author: Indrajeet Patil and Zaki Bakshi
 */

#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "DHT.h"
#define DHTPIN A5
#define DHTTYPE DHT11
#include <SPI.h>
#include <RF24.h>
#define CE_PIN   9
#define CSN_PIN 10
#define M_ID 3
#define GREEN_LED 0
#define RED_LED 1
#define ORANGE_LED 8
//#define BUZZER 8
boolean alert = 0, button_flag_A1 = 0, button_flag_A2 = 0, button_flag_A3 = 0, button_flag_A4 = 0, sensor_flag = 0;
const uint64_t pipe = 0xE8E8F0F0E1LL;
RF24 radio(CE_PIN, CSN_PIN);
unsigned int l_h, l_t, h, t;
uint8_t packet[2], option = 0, menu_flag = 0;
uint8_t identifier, id, c_read;
DHT dht(DHTPIN, DHTTYPE);
String t_s, h_s, to_nrf = "i=3&c=22&h=33&t=44";
unsigned long count = 0, difference_count = 0, red_count, orange_count, green_count;
unsigned int time_period, speed_pm = 0, threshold = 120, threshold_temp;
unsigned long lastUpdate = 0, speed_time = 600, nrf_prev = 0;
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
char lcd_buf[20] = "";
uint8_t alarm_setting = 1;
boolean send_only_once = true;
int page_number = 0;
boolean page_change = true, count_change = false;
uint8_t alert_type = 0;
int nrf_send_delay = 400;
unsigned int data_sending_interval = 10000;
int data_sending_stage = 0;
unsigned long data_sending_prev_time;
uint8_t low_threshold = 60, low_threshold_temp;
unsigned int red_alerts=0, orange_alerts=0,green_alerts=0;

void setup() {
  pinMode(GREEN_LED, OUTPUT); //Green LED
  pinMode(RED_LED, OUTPUT); //Red LED
  pinMode(ORANGE_LED, OUTPUT);
  //  digitalWrite(BUZZER, LOW);
  digitalWrite(A1, HIGH);
  digitalWrite(A2, HIGH);
  digitalWrite(A3, HIGH);
  digitalWrite(A4, HIGH);
  lcd.begin(20, 4);
  //Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(pipe);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("      DCAP      ");
  lcd.setCursor(0, 2);
  lcd.print(" Solutions LLP  ");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Threshold is set");
  lcd.setCursor(0, 2);
  lcd.print("    to ");
  EEPROM.put(0, threshold);
  
  EEPROM.get(0, threshold);
  lcd.print((String)threshold);
  lcd.print("c/m");
  EEPROM.put(sizeof(unsigned int), low_threshold);
  EEPROM.get(sizeof(unsigned int), low_threshold);
  
  lcd.setCursor(0, 3);
  lcd.print("Low Limit:");
  lcd.print(String(low_threshold));
  delay(3000);
  lcd.clear();
  dht.begin();
  digitalWrite(A0, HIGH);
  pciSetup(A0);
  pciSetup(A4);
  pciSetup(A3);
  pciSetup(A1);
  pciSetup(A2);
  h_s = String(dht.readHumidity(), 0);
  t_s = String(dht.readTemperature(), 0);
  printRTH();
  printSpeed();
  lcd.setCursor(0, 3);
  lcd.print("MENU       RESET");
  lcd.setCursor(0, 1);
  lcd.print("  ");
  lcd.print("C:");
  lcd.print(String(count));
  GreenAlert();
  data_sending_prev_time = millis();
}
void loop() {
  sendRGOCountAlert();

  if (page_change == true) {
    switch (page_number)
    {
      case 16: lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("Threshold is set");
        lcd.setCursor(0, 2);
        lcd.print("    to ");
        lcd.print((String)threshold);
        lcd.print("c/m");
        //delay(1000);
        page_change = false;
        break;

      case 1: lcd.setCursor(0, 0);
        lcd.print("Change Threshold");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print("      ");
        lcd.print((String)threshold);
        lcd.setCursor(0, 3);
        lcd.print("SET UP DOWN BACK");
        page_change = false;
        break;

      case 2: lcd.setCursor(0, 0);
        lcd.print("Change Low Limit");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print("      ");
        lcd.print((String)low_threshold);
        lcd.setCursor(0, 3);
        lcd.print("SET UP DOWN BACK");
        page_change = false;
        break;

      case 3: lcd.setCursor(0, 0);
        lcd.print("> Set High Limit");
        lcd.setCursor(0, 1);
        lcd.print("  Set Low Limit ");
        //        lcd.print("                ");
        lcd.setCursor(0, 2);
        lcd.print("                ");
        lcd.setCursor(0, 3);
        lcd.print("SELECT      BACK");
        page_change = false;
        break;

      case 4: //Low Threshold is set
        /*lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print("Alarm is set");
          lcd.setCursor(0, 2);
          lcd.print("    to ");
          lcd.print("ON");
          page_change = false;*/
        break;

      /*      case 5: lcd.clear();
              lcd.setCursor(0, 1);
              lcd.print("Alarm is set");
              lcd.setCursor(0, 2);
              lcd.print("    to ");
              lcd.print("OFF");
              page_change = false;
              break;
      */
      case 6: lcd.setCursor(0, 0);
        lcd.print(">");
        lcd.setCursor(0, 1);
        lcd.print(" ");
        page_change = false;
        break;

      case 7: lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print("      ");
        lcd.print((String)threshold_temp);
        page_change = false;
        break;

      case 8: lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print("      ");
        lcd.print((String)low_threshold_temp);
        page_change = false;
        break;

      case 9: lcd.setCursor(0, 0);
        lcd.print(" ");
        lcd.setCursor(0, 1);
        lcd.print(">");
        page_change = false;
        break;

      case 10:      lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print("      ");
        lcd.print((String)threshold_temp);
        page_change = false;
        break;

      case 11:       lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print("      ");
        lcd.print((String)low_threshold_temp);
        page_change = false;
        break;

      case 12:
        //        printRTH();
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print("  ");
        lcd.print("C:");
        lcd.print(String(count));
        printSpeed();
        page_change = false;
        break;

      case 13:
        lcd.clear();
        printRTH();
        printSpeed();

        lcd.setCursor(0, 3);
        lcd.print("MENU       RESET");
        lcd.setCursor(0, 1);
        lcd.print("  ");
        lcd.print("C:");
        lcd.print(String(count));
        page_change = false;
        break;

      case 14:      lcd.setCursor(0, 0);
        lcd.print("> Set High Limit");
        lcd.setCursor(0, 1);
        lcd.print("  Set Low Limit ");
        lcd.setCursor(0, 2);
        lcd.print("                ");
        lcd.setCursor(0, 3);
        lcd.print("SELECT      BACK");
        page_change = false;
        break;

      case 15:
        lcd.setCursor(0, 0);
        lcd.print("> Set High Limit");
        lcd.setCursor(0, 1);
        lcd.print("  Set Low Limit ");
        lcd.setCursor(0, 2);
        lcd.print("                ");
        lcd.setCursor(0, 3);
        lcd.print("SELECT      BACK");
        page_change = false;
        break;




    }
  }
  if (count_change == true) {
    lcd.setCursor(0, 1);
    lcd.print("  ");
    lcd.print("C:");
    lcd.print(String(count));
    count_change = false;
  }

  //boolean sensor=digitalRead(A0);
  unsigned long time = millis();
  if ((time  - lastUpdate) >= 1000 && (time - data_sending_prev_time))
  {
    h = (int)dht.readHumidity();
    t = (int)dht.readTemperature();
    if ((l_h != h && l_h != h + 1 && l_h != h - 1) || l_t != t) {
      formatAndSendData();
      t_s = (String)t;
      h_s = (String)h;
      if (menu_flag == 0) printRTH();
      l_h = h;
      l_t = t;
    }
    if (menu_flag == 0) printSpeed();
    lastUpdate = time;
    //sendNRFString(3,"i=3&c=22&h=33&t=44");
  }

}
void printRTH() {
  lcd.setCursor(0, 0);
  lcd.print("  ");
  lcd.print("T:");
  lcd.print(t_s);
  lcd.print("C ");
  lcd.print("RH:");
  lcd.print(h_s);
  lcd.print("%");
}
void printSpeed() {
  time_period = millis() - speed_time;
  if ((10000 / time_period) < 1 && send_only_once) {
    speed_pm = 0;
//    sendRGOCount();
    OrangeAlert();
    //    formatAndSendData();
    send_only_once = false;
  }
  lcd.setCursor(0, 2);
  lcd.print("                ");
  lcd.setCursor(0, 2);
  lcd.print("  ");
  lcd.print("S:");
  lcd.print(String(speed_pm));
  lcd.print("c/m");
}
void pciSetup(byte pin)
{
  *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));
  PCIFR  |= bit (digitalPinToPCICRbit(pin));
  PCICR  |= bit (digitalPinToPCICRbit(pin));
}
ISR (PCINT1_vect)
{
  boolean sensor = digitalRead(A0);
  if (sensor == 1) sensor_flag = 1;
  if (digitalRead(A1) == 1) {
    button_flag_A1 = 1;
    delay(50);
  }
  if (digitalRead(A2) == 1) {
    button_flag_A2 = 1;
    delay(50);
  }
  if (digitalRead(A3) == 1) {
    button_flag_A3 = 1;
    delay(50);
  }
  if (digitalRead(A4) == 1) {
    button_flag_A4 = 1;
    delay(50);
  }

  if (digitalRead(A1) == 0 && button_flag_A1 == 1) { //Pressing Select
reprint:
    button_flag_A1 = 0;
    if (menu_flag == 2) {
      page_number = 16;
      page_change = true;
      threshold = threshold_temp;
      EEPROM.put(0, threshold);
      /*      lcd.clear();
            lcd.setCursor(0, 1);
            lcd.print("Threshold is set");
            lcd.setCursor(0, 2);
            lcd.print("    to ");
            lcd.print((String)threshold);
            lcd.print("c/m");
            delay(3000);*/
      menu_flag = 0;
      goto reprint;
    }
    else if (menu_flag == 1 && option == 1) {
      threshold_temp = threshold;
      menu_flag = 2;
      page_number = 1;
      page_change = true;
      /*      lcd.setCursor(0, 0);
            lcd.print("Change Threshold");
            lcd.setCursor(0, 1);
            lcd.print("                ");
            lcd.setCursor(0, 1);
            lcd.print("      ");
            lcd.print((String)threshold);
            lcd.setCursor(0, 3);
            lcd.print("SET UP DOWN BACK");*/
    }
    else if (menu_flag == 1 && option == 2) { //Low Threshold
      low_threshold_temp = low_threshold;
      menu_flag = 3;
      page_number = 2;
      page_change = true;
      /*lcd.setCursor(0, 0);
      lcd.print("Change Setting  ");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("      >ON");
      lcd.setCursor(0, 2);
      lcd.print("                ");
      lcd.setCursor(0, 2);
      lcd.print("       OFF");
      lcd.setCursor(0, 3);
      lcd.print("SET UP DOWN BACK");*/

    }
    else if (menu_flag == 0) {
      menu_flag = 1;
      option = 1;
      page_number = 3;
      page_change = true;
      /*lcd.setCursor(0, 0);
      lcd.print("> Set Threshold ");
      lcd.setCursor(0, 1);
      lcd.print("  Alarm Settings");
      lcd.setCursor(0, 2);
      lcd.print("                ");
      lcd.setCursor(0, 3);
      lcd.print("SELECT      BACK");*/
    }
    else if (menu_flag == 3) { //Saving low_threshold in EEPROM
      page_number = 4;
      page_change = true;
      low_threshold = low_threshold_temp;
      EEPROM.put(sizeof(unsigned int), low_threshold);
      /* alarm_setting = 1;
       EEPROM.put(sizeof(unsigned int), alarm_setting);
       page_number = 4;
       page_change = true;
             lcd.clear();
             lcd.setCursor(0, 1);
             lcd.print("Alarm is set");
             lcd.setCursor(0, 2);
             lcd.print("    to ");
             lcd.print("ON");
       menu_flag = 0;
       goto reprint;*/
      menu_flag = 0;
      goto reprint;

    }
  }
  if (digitalRead(A2) == 0 && button_flag_A2 == 1) { //UP ARROW
    button_flag_A2 = 0;
    if (menu_flag == 1 && option == 2) {
      page_number = 6;
      page_change = true;
      /*      lcd.setCursor(0, 0);
            lcd.print(">");
            lcd.setCursor(0, 1);
            lcd.print(" ");*/
      option = 1;
    }
    if (menu_flag == 2) {
      threshold_temp = threshold_temp + 1;
      page_number = 7;
      page_change = true;
      /*      lcd.setCursor(0, 1);
            lcd.print("                ");
            lcd.setCursor(0, 1);
            lcd.print("      ");
            lcd.print((String)threshold_temp);*/
    }
    else if (menu_flag == 3) {
      low_threshold_temp = low_threshold_temp + 1;
      page_number = 8;
      page_change = true;
      /*      lcd.setCursor(0, 1);
            lcd.print("                ");
            lcd.setCursor(0, 1);
            lcd.print("      ");
            lcd.print((String)low_threshold_temp);*/

    }
  }
  if (digitalRead(A3) == 0 && button_flag_A3 == 1) {
    button_flag_A3 = 0;
    if (menu_flag == 1 && option == 1) {
      page_number = 9;
      page_change = true;
      /*      lcd.setCursor(0, 0);
            lcd.print(" ");
            lcd.setCursor(0, 1);
            lcd.print(">");*/
      option = 2;
    }
    if (menu_flag == 2) {
      threshold_temp = threshold_temp - 1;
      page_number = 10;
      page_change = true;
      /*      lcd.setCursor(0, 1);
            lcd.print("                ");
            lcd.setCursor(0, 1);
            lcd.print("      ");
            lcd.print((String)threshold_temp);*/
    }
    else if (menu_flag == 3) {
      low_threshold_temp = low_threshold_temp - 1;
      page_number = 11;
      page_change = true;
      /*      lcd.setCursor(0, 1);
            lcd.print("                ");
            lcd.setCursor(0, 1);
            lcd.print("      ");
            lcd.print((String)low_threshold_temp);*/

    }
  }
  if (digitalRead(A4) == 0 && button_flag_A4 == 1) {
    button_flag_A4 = 0;
    if (menu_flag == 0) { //Reset
      OrangeAlert();
      count = 0;
      speed_pm = 0;

      difference_count = 0;
      page_number = 12;
      page_change = true;

      /*       printSpeed();
              lcd.setCursor(0, 1);
            lcd.print("                ");
            lcd.setCursor(0, 1);
            lcd.print("  ");
            lcd.print("C:");
            lcd.print(String(count));*/
    }
    else if (menu_flag == 1) { //Back
      menu_flag = 0;
      page_number = 13;
      page_change = true;
      /*lcd.clear();
      printRTH();
      printSpeed();
      lcd.setCursor(0, 3);
      lcd.print("MENU       RESET");
      lcd.setCursor(0, 1);
      lcd.print("  ");
      lcd.print("C:");
      lcd.print(String(count));*/
    }
    else if (menu_flag == 2) { //Back
      menu_flag = 1;
      option = 1;
      page_number = 14;
      page_change = true;
      /*      lcd.setCursor(0, 0);
            lcd.print("> Set Threshold ");
            lcd.setCursor(0, 1);
            lcd.print("  Alarm Settings");
            lcd.setCursor(0, 2);
            lcd.print("                ");
            lcd.setCursor(0, 3);
            lcd.print("SELECT      BACK");*/
    }
    else if (menu_flag == 3) {
      menu_flag = 1;
      option = 1;
      page_number = 15;
      page_change = true;
      /*      lcd.setCursor(0, 0);
            lcd.print("> Set Threshold ");
            lcd.setCursor(0, 1);
            lcd.print("  Alarm Settings");
            lcd.setCursor(0, 2);
            lcd.print("                ");
            lcd.setCursor(0, 3);
            lcd.print("SELECT      BACK");*/
    }
  }
  if (sensor == 0 && sensor_flag == 1) {
    time_period = millis() - speed_time;
    speed_pm = 60000 / time_period;
    if (speed_pm > threshold) {
      if (alert_type != 1)
        RedAlert();
    }
    else if (speed_pm < threshold && speed_pm > (threshold * low_threshold / 100)) {
      if (alert_type != 2)
        GreenAlert();
    }
    else {
      if (alert_type != 3)
        OrangeAlert();
    }
    speed_time = millis();
    count++;
    if (alert_type == 1) {
      red_count++;
    }
    else if (alert_type == 2) {
      green_count++;
    }
    else if (alert_type == 3) {
      orange_count++;
    }

    send_only_once = true;
    sensor_flag = 0;
    if (menu_flag == 0) {
      //      page_number = 15;
      //      page_change = true;
      count_change = true;
      /*      lcd.setCursor(0, 1);
            lcd.print("  ");
            lcd.print("C:");
            lcd.print(String(count));*/
    }
    //sendNRFString(3,"i=3&c=22&h=33&t=44");
  }
  /* if (digitalRead(A2) == 0) {
     if (millis() - nrf_prev > 1000)
     {
       sendNRFString(3, "i=3&c=22&h=33&t=44");
       nrf_prev = millis();
     }

   }*/
}
void sendNRFString(uint8_t id, String msg) {
  uint16_t msg_length = msg.length();
  for (int i = 0; i < msg_length; i++) {
    radio.powerUp();
    packet[0] = id;
    packet[1] = msg[i];
    radio.write(packet, sizeof(packet));
     delay(10);
    radio.powerDown();
  }
  radio.powerUp();
  packet[0] = id;
  packet[1] = '\n';
  radio.write(packet, sizeof(packet));
  delay(10);
  radio.powerDown();
  //Serial.print("id: ");
  //Serial.print(id);
  //Serial.print(" msg: ");
  //Serial.println(msg);
}
void RedAlert() {
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(ORANGE_LED, LOW);
//  sendRGOCount();
  alert_type = 1;
  red_alerts++;
}
void GreenAlert() {
  
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(ORANGE_LED, LOW);
//  sendRGOCount();
  green_alerts++;
  alert_type = 2;
}

void OrangeAlert() {
  digitalWrite(ORANGE_LED, HIGH);
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
//  sendRGOCount();
  orange_alerts++;
  alert_type = 3;
}

void sendRGOCount() {
  if (millis() - nrf_prev > nrf_send_delay)
  {
    if (alert_type == 1 && red_count > 0)
    {
      to_nrf = "i=";
      to_nrf += M_ID;
      to_nrf += "&ra=";
      to_nrf += red_count;
      sendNRFString(M_ID, to_nrf);
      red_count = 0;
    }
    else if (alert_type == 3 && orange_count > 0)
    {
      to_nrf = "i=";
      to_nrf += M_ID;
      to_nrf += "&oa=";
      to_nrf += orange_count;
      sendNRFString(M_ID, to_nrf);
      orange_count = 0;
    }
    else if (alert_type == 2 && green_count > 0)
    {
      to_nrf = "i=";
      to_nrf += M_ID;
      to_nrf += "&ga=";
      to_nrf += green_count;
      sendNRFString(M_ID, to_nrf);
      green_count = 0;
    }
    nrf_prev = millis();
  }
}


void sendRGOCountAlert() {
    if (millis() - data_sending_prev_time > data_sending_interval) {
      if(red_count>0||orange_count>0||green_count>0) {
      to_nrf = "i=";
      to_nrf += M_ID;
      to_nrf += "&rac=";
      to_nrf += red_alerts;
      to_nrf += "&ra=";
      to_nrf += red_count;
      to_nrf += "&oac=";
      to_nrf += orange_alerts;
      to_nrf += "&oa=";
      to_nrf += orange_count;
      to_nrf += "&gac=";
      to_nrf += green_alerts;
      to_nrf += "&ga=";
      to_nrf += green_count;
      red_count = 0;
      red_alerts = 0;
      orange_count = 0;
      orange_alerts=0;
      green_count = 0;
      green_alerts = 0;

      sendNRFString(M_ID, to_nrf);
      }
/*    if (red_count > 0 && data_sending_stage == 0) {
      to_nrf = "i=";
      to_nrf += M_ID;
      to_nrf += "&rac=";
      to_nrf += red_alerts;
      to_nrf += "&ra=";
      to_nrf += red_count;
      
      sendNRFString(M_ID, to_nrf);
      red_count = 0;
      red_alerts = 0;
      data_sending_stage = 1;
      data_sending_prev_time += 10000;
    }
   else if (orange_count > 0 && data_sending_stage == 1) {
      to_nrf = "i=";
      to_nrf += M_ID;
      to_nrf += "&oac=";
      to_nrf += orange_alerts;
      to_nrf += "&oa=";
      to_nrf += orange_count;
      
      sendNRFString(M_ID, to_nrf);
      orange_count = 0;
      orange_alerts=0;
      data_sending_stage = 2;
      data_sending_prev_time += 10000;
    }
   else if (green_count > 0 && data_sending_stage == 2) {
      to_nrf = "i=";
      to_nrf += M_ID;
      to_nrf += "&gac=";
      to_nrf += green_alerts;
      to_nrf += "&ga=";
      to_nrf += green_count;
      sendNRFString(M_ID, to_nrf);
      green_count = 0;
      green_alerts = 0;
      data_sending_stage = 0;
      data_sending_prev_time = millis();
    }*/
    data_sending_prev_time = millis();
  }

  
}
void formatAndSendData() {
  if (millis() - nrf_prev > nrf_send_delay)
  {
    h = (int)dht.readHumidity();
    t = (int)dht.readTemperature();
    to_nrf = "i=";
    to_nrf += M_ID;
    //  to_nrf += "&c=";
    //  to_nrf += count - difference_count;
    to_nrf += "&h=";
    to_nrf += h;
    to_nrf += "&t=";
    to_nrf += t;
    // Serial.println(to_nrf);

    sendNRFString(M_ID, to_nrf);
    nrf_prev = millis();

    //  difference_count = count;
  }
}
