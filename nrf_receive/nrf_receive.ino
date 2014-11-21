 /* YourDuinoStarter Example: nRF24L01 Receive Joystick values
 - CONNECTIONS: nRF24L01 Modules See:
 http://arduino-info.wikispaces.com/Nrf24L01-2.4GHz-HowTo
   1 - GND
   2 - VCC 3.3V !!! NOT 5V
   3 - CE to Arduino pin 9
   4 - CSN to Arduino pin 10
   5 - SCK to Arduino pin 13
   6 - MOSI to Arduino pin 11
   7 - MISO to Arduino pin 12
   8 - UNUSED
 */
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

#include <Servo.h>
Servo myservo1;  // create servo object to control servos
Servo myservo2;

/*-----( Declare Constants and Pin Numbers )-----*/
#define CE_PIN   9
#define CSN_PIN 10

// NOTE: the "LL" at the end of the constant is "LongLong" type
const uint64_t pipe = 0xE8E8F0F0E1LL; // Define the transmit pipe
                      
/*-----( Declare objects )-----*/
RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

/*-----( Declare Variables )-----*/
int joystick[2];  // 2 element array holding Joystick readings
int pos1=90;    // new position
int pos2=90;    // 2nd servo position

void setup()   /****** SETUP: RUNS ONCE ******/
{
  Serial.begin(38400);
  printf_begin();
  delay(1000);
  Serial.println("Nrf24L01 Receiver Starting");
  
  radio.begin();
  radio.setPALevel(RF24_PA_MAX); 
  radio.setDataRate(RF24_250KBPS);
  // 8 bits CRC
  radio.setCRCLength( RF24_CRC_8 );
 
  // Disable dynamic payloads
  //radio.write_register(DYNPD,0);
 
  // increase the delay between retries & # of retries
  radio.setRetries(10,10); -- 15,15
  
  radio.openReadingPipe(1,pipe);
  radio.startListening();
  radio.printDetails();
  
  myservo1.attach(6);  // attaches the servo on pin 6
  myservo2.attach(7);  // attaches the servo on pin 7
  myservo1.write(pos1);
  myservo2.write(pos2);
}


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{  
  if ( radio.available() )
  {
    // Read the data payload until we've received everything
    bool done = false;
    while (!done)
    {
      // Fetch the data payload
      done = radio.available();
      radio.read( joystick, sizeof(joystick) );
      //Serial.print("X = ");
      //Serial.print(joystick[0]);
      //Serial.print(" Y = ");      
      //Serial.println(joystick[1]);
      pos2 = map(joystick[0], 0,1023, 180, 0);
      pos1 = map(joystick[1], 0,1023, 0,   180);
      myservo1.write(pos1);
      myservo2.write(pos2);
    }
  }
  else
  {    
    //Serial.println("No radio available");
  }
delay(15);
}