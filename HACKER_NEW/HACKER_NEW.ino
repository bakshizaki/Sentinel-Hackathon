#include <SPI.h>//include the SPI bus library
#include <MFRC522.h>//include the RFID reader library
#include <nRF24L01.h>
#include <RF24.h>

#define M_ID 3
#define CE_PIN   9
#define CSN_PIN 10
#define SS_PIN A2  //slave select pin
#define RST_PIN A3  //reset pin
String to_nrf="";
MFRC522 mfrc522(SS_PIN, RST_PIN);        // instatiate a MFRC522 reader object.
MFRC522::MIFARE_Key key;//create a MIFARE_Key struct named 'key', which will hold the card information
uint8_t packet[2];
const uint64_t pipe = 0xE8E8F0F0E1LL; //0xE8E8F0F0E1LL 0xF0F0F0F0E1LL
RF24 radio(CE_PIN, CSN_PIN);
float force, fsr[4];
#define Debug Serial
double load, min_load, max_load;
String emp_id = "";
unsigned long nrf_prev = 0;
int nrf_send_delay = 400;
void setup() {
  Serial.begin(9600);        // Initialize serial communications with the PC
  SPI.begin();               // Init SPI bus
  mfrc522.PCD_Init();        // Init MFRC522 card (in case you wonder what PCD means: proximity coupling device)
  Serial.println("Scan a MIFARE Classic card");
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;//keyByte is defined in the "MIFARE_Key" 'struct' definition in the .h file of the library
  }
  radio.begin();
  radio.openWritingPipe(pipe);

}
int block = 2; //this is the block number we will write into and then read. Do not write into 'sector trailer' block, since this can make the block unusable.

byte blockcontent[16] = {"101425"};//an array with 16 bytes to be written into one of the 64 card blocks is defined
//byte blockcontent[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//all zeros. This can be used to delete a block.
byte readbackblock[18];//This array is used for reading out a block. The MIFARE_Read method requires a buffer that is at least 18 bytes to hold the 16 bytes of a block.

void loop() {
  // Look for new cards
  //    if ( ! mfrc522.PICC_IsNewCardPresent())
  //        return;
  //
  //    // Select one of the cards
  //    if ( ! mfrc522.PICC_ReadCardSerial())
  //        return;


  //         writeBlock(block, blockcontent);

    if (readBlock(block, readbackblock) == 1) { //read the block back
    Serial.print("read block: ");
    emp_id = "";
    for (int j = 0 ; j < 2 ; j++) //print the block contents
    {
      emp_id += (char)readbackblock[j];
      //           Serial.write (readbackblock[j]);//Serial.write() transmits the ASCII numbers as human readable characters to serial monitor
    }
    sendEID();
  }

  }

void sendEID() {
  if (millis() - nrf_prev > nrf_send_delay)
  {
    to_nrf = "i=";
    to_nrf += M_ID;
    to_nrf += "&eid=";
    to_nrf += emp_id;
    sendNRFString(M_ID, to_nrf);
    nrf_prev = millis();
    
  }
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


int readBlock(int blockNumber, byte arrayAddress[])
{
  if (  mfrc522.PICC_IsNewCardPresent()) {

    if (  mfrc522.PICC_ReadCardSerial())
    {
      int largestModulo4Number = blockNumber / 4 * 4;
      int trailerBlock = largestModulo4Number + 3; //determine trailer block for the sector

      /*****************************************authentication of the desired block for access***********************************************************/
      MFRC522::StatusCode status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
      if (status != MFRC522::STATUS_OK) {
        Serial.print("PCD_Authenticate() failed (read): ");
        Serial.println(mfrc522.GetStatusCodeName(status));
        mfrc522.PICC_HaltA();
        // Stop encryption on PCD
        mfrc522.PCD_StopCrypto1();

        return 3;//return "3" as error message
      }


      /*****************************************reading a block***********************************************************/

      byte buffersize = 18;//we need to define a variable with the read buffer size, since the MIFARE_Read method below needs a pointer to the variable that contains the size...
      status = mfrc522.MIFARE_Read(blockNumber, arrayAddress, &buffersize);//&buffersize is a pointer to the buffersize variable; MIFARE_Read requires a pointer instead of just a number
      if (status != MFRC522::STATUS_OK) {
        Serial.print("MIFARE_read() failed: ");
        Serial.println(mfrc522.GetStatusCodeName(status));
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();

        return 4;//return "4" as error message
      }
      Serial.println(F("block was read"));
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
      return 1;
    }
  }
}


void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

