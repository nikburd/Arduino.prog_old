/* YourDuinoStarter Example: nRF24L01 Transmit Joystick values
 - WHAT IT DOES: Reads Analog values on A0, A1 and transmits
   them over a nRF24L01 Radio Link to another transceiver.
 - SEE the comments after "//" on each line below
 - CONNECTIONS: nRF24L01 Modules See:
 http://arduino-info.wikispaces.com/Nrf24L01-2.4GHz-HowTo
   1 - GND
   2 - VCC 3.3V !!! NOT 5V
   3 - CE to Arduino pin 9
   4 - CSN to Arduino pin 10
   5 - SCK to Arduino pin 13
   6 - MOSI to Arduino pin 11
   7 - MISO to Arduino pin 12
   8 - UNUSED - INT
   - 
   Analog Joystick or two 10K potentiometers:
   GND to Arduino GND
   VCC to Arduino +5V
   X Pot to Arduino A0
   Y Pot to Arduino A1
   BZ button to Arduino D8 
   
 - V1.00 11/26/13
   Based on examples at http://www.bajdi.com/
   Questions: terry@yourduino.com */

/*-----( Import needed libraries )-----*/
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

/*-----( Declare Constants and Pin Numbers )-----*/
#define CE_PIN   9
#define CSN_PIN 10
#define JOYSTICK_X A0
#define JOYSTICK_Y A1
#define BZ         8

// NOTE: the "LL" at the end of the constant is "LongLong" type
const uint64_t pipe = 0xE8E8F0F0E1LL; // Define the transmit pipe


/*-----( Declare objects )-----*/
RF24 radio(CE_PIN, CSN_PIN); // Create a Radio
/*-----( Declare Variables )-----*/
int joystick[2];  // 2 element array holding Joystick readings

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation is used in I2Cdev.h
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"
// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;

int16_t ax, ay, az;
int16_t gx, gy, gz;

// timestamp for movements and tests
unsigned long previousTime = 0;
unsigned long currentTime = 0;
unsigned long deltaTime = 0; // loop time in microseconds   

unsigned long frameCounter = 0; // main loop executive frame counter

unsigned long dTime; // timestamp for movements and tests
int dt;      // loop time in milliseconds 

int pos0=0;
int pos1=0;    // new position
int pos2=0;    // 2nd servo position
  
void setup()
{
  pinMode(BZ, INPUT_PULLUP); // pressed: LOW, released: HIGH
  Serial.begin(38400);
  printf_begin();
  radio.begin();
  radio.setPALevel(RF24_PA_HIGH); 
  radio.setDataRate(RF24_250KBPS);
  
  // 8 bits CRC
  radio.setCRCLength( RF24_CRC_8 );
 
  // Disable dynamic payloads
  //radio.write_register(DYNPD,0);
 
  // increase the delay between retries & # of retries
  radio.setRetries(15,15);
  
  radio.openWritingPipe(pipe);
  radio.printDetails();
  
  // initialize device
  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();

// verify connection
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
}


void loop() {
  currentTime = micros();
  deltaTime = currentTime - previousTime;
  
  dt = millis() - dTime; // time since previous measurement
  dTime = millis();
  
  // Main scheduler loop set for 100hz
  if (deltaTime >= 10000) {
  
    frameCounter++;
    
    // ================================================================
    // 100hz task loop
    // ================================================================
    if (frameCounter %   1 == 0) {  //  100 Hz tasks

     //  accelgyro.getAcceleration(&ax, &ay, &az);
     //  accelgyro.getRotation(&gx, &gy, &gz);
     //  default set +/- 250 degree per secong 
     //  setFullScaleGyroRange(MPU6050_GYRO_FS_250);
     
     //these methods (and a few others) are available  
     accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      
     //  pos1 = pos1 - dt*map( gz, -32768, 32768, -250, 250)/1000;
     //  pos2 = pos2 + dt*map( gy, -32768, 32768, -250, 250)/1000;
     //pos0 = pos0 + (deltaTime*gx / 131)/1000000;
     //pos1 = pos1 + (deltaTime*gz / 131)/1000000;
     //pos2 = pos2 + (deltaTime*gy / 131)/1000000;
     pos0 = pos0 - dt * (gx / 131)/1000;
     pos1 = pos1 - dt * (gz / 131)/1000;
     pos2 = pos2 - dt * (gy / 131)/1000;
     
     // Convert accelerometer raw values to degrees
     //double accZangle = (atan2(ay, ax) + PI) * RAD_TO_DEG;
     //double accYangle = (atan2(ax, az) + PI) * RAD_TO_DEG;
     //double accXangle = (atan2(ay, az) + PI) * RAD_TO_DEG;
  
     // Convert raw gyro values to degrees per second
     //double gyroZrate = ((double)gz / 131.0);
     //double gyroYrate = ((double)gy / 131.0);
     //double gyroXrate = ((double)gx / 131.0);

     // Calculate angles using complementary filter
    //static int  pos01 = (0.93 * (pos01 + (gyroZrate * dt / 1000))) + (0.07 * accZangle);
    //static int  pos02 = (0.93 * (pos02 + (gyroYrate * dt / 1000))) + (0.07 * accYangle);

     joystick[0] = constrain(7*pos0+analogRead(JOYSTICK_X), 0, 1023);
     joystick[1] = constrain(7*pos1+analogRead(JOYSTICK_Y), 0, 1023);
     /*
     Serial.print(frameCounter); Serial.print(" ");
     Serial.print(dt); Serial.print(" ");
     Serial.print(dt * (gx / 131)/1000); Serial.print(" ");
     Serial.print(dt * (gz / 131)/1000); Serial.print(" ");
     Serial.print(joystick[0]);Serial.print(" ");
     Serial.print(joystick[1]);Serial.print(" ");
     Serial.println(digitalRead(BZ));
     */
   }//  100 Hz tasks
   
   // ================================================================
    // 50hz task loop
    // ================================================================
    if (frameCounter %   2 == 0) {  //  50 Hz tasks
    }//  50 Hz tasks
   
   
    // ================================================================
    // 25hz task loop
    // ================================================================
    if (frameCounter %   4 == 0) {  //  25 Hz tasks
     radio.write( joystick, sizeof(joystick) );
     // reset position to central on BZ button pressed
     if (!digitalRead(BZ)) {pos0=0; pos1=0; pos2=0;}
    }//  25 Hz tasks
    
    // ================================================================
    // 10hz task loop
    // ================================================================
    if (frameCounter %  10 == 0) {  //   10 Hz tasks
    /*
     Serial.print(frameCounter); Serial.print(" ");
     Serial.print(dt); Serial.print(" ");
     Serial.print(pos0); Serial.print(" ");
     Serial.print(pos1); Serial.print(" ");
     Serial.print(joystick[0]);Serial.print(" ");
     Serial.print(joystick[1]);Serial.print(" ");
     Serial.println(digitalRead(BZ));
     */
    }//   10 Hz tasks
    
    // ================================================================
    // 1hz task loop
    // ================================================================
    if (frameCounter %  100 == 0) {  //   1 Hz tasks
    }//   1 Hz tasks
    previousTime = currentTime;
  
  } // Main scheduler loop
  if (frameCounter >= 100) {
      frameCounter = 0;
  } 
} // loop
