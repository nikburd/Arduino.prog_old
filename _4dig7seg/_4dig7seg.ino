/*
 Example code for driving a 4 digit common anode (cathode) 7-segment display with
 a 74HC595 8-bit shift register and 4 NPN transistors.

 The 74HC595 shift register attaches to pins 8, 11 and 12 of Arduino.
 See Arduino ShiftOut tutorial for more info: http://www.arduino.cc/en/Tutorial/ShiftOut
   
 Arduino digital pins 4 to 7 drive the cathode for digits 1 through 4.
 Digit 1 is the leftmost. Wire the emitter of the NPN transistor to ground.
 Connect the Arduino digital pin to the base of the transistor via a
 1 k-ohm resistor so as not to provide excessive base drive and damage
 the transistor. Connect the collector of the transistor to the cathode of
 the display.
 
 Be sure to use current limiting resistors from the shift register outputs
 Qa to Qf to the anode segments of the LED display. Use 220 ohm resistors.

 * Shift Register  Arduino
 * 14  SER           11       Serial Input
 * 11  SRCLK         12       Shift Clock
 * 12  RCLK           8       Latch Enable
 */

// Define the bit-patterns for the 7-segment displays 
const byte SevenSeg[16] = 
{// Hex digits
     B11111100, B01100000, B11011010, B11110010,   // 0123
     B01100110, B10110110, B10111110, B11100000,   // 4567
     B11111110, B11110110, B11101110, B00111110,   // 89AB
     B00011010, B01111010, B10011110, B10001110,   // CDEF
  
};
byte dot = B00000001; // show dot
boolean cc = true;    //show colons

// Pin connected to latch pin (RCLK,12) of 74HC595
const int latchPin = 8;
// Pin connected to clock pin (SRCLK,11) of 74HC595
const int clockPin = 12;
// Pin connected to Data in (SER,14) of 74HC595
const int dataPin  = 11;
// Pin connected to cathodes
const int disp1 = 7;  // Displays are numbered from left to right
const int disp2 = 6;  // as disp1 to disp4.
const int disp3 = 5;  // Scan displays quickly and use POV to display
const int disp4 = 4;  // 4 digits with only 1 shift register
const int col = 3;  // 4 digits with only 1 shift register

void setup() 
{
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin,  OUTPUT);
  pinMode(disp1, OUTPUT);
  pinMode(disp2, OUTPUT);
  pinMode(disp3, OUTPUT);
  pinMode(disp4, OUTPUT);
  pinMode(col, OUTPUT);
}  

void loop() {
for (int i=0; i < 10000; ++i)
  {
    // Display i as 4 digits on the display.
    // No leading zero blanking.
    for (int j=0; j < 20; j++){ // Draw the display several times to slow it down
      SevenSegDisplay(i);
      delay(0);
    }
    cc = !cc;  // switch colon on/off
    digitalWrite(col, cc);  // update colon pin state
  }
}

void DisplayADigit(int dispno, byte digit2disp)
{ 
// Turn off the shift register pins while you're shifting bits:
  digitalWrite(latchPin, LOW);
  
  AllDispOff();  // Turn off all cathode drivers.
  
// shift the bits out:
  shiftOut(dataPin, clockPin, LSBFIRST, digit2disp);
// Set latch high to set segments.
  digitalWrite(latchPin, HIGH);  
// Drive one cathode low to turn on display.
  digitalWrite(dispno, HIGH);  
  
  delay(5);  // Wait a bit for POV
}

void AllDispOff()
{// Turn all cathode drivers off
  digitalWrite(disp1, LOW);
  digitalWrite(disp2, LOW);
  digitalWrite(disp3, LOW);
  digitalWrite(disp4, LOW);
}

void SevenSegDisplay(int number)
{
  int d1,d2,d3,d4; // Temporary values for thousands, hundreds, tens and units
  
  if (number > 9999)
     number = 9999;  // Do some bounds checks to avoid strangeness
  if (number < 0)
     number = 0;
  
  d1 = (int) (number / 1000);    // Get thousands
  number = number - (d1 * 1000);
  d2 = (int) (number / 100);     // Get hundreds
  number = number - (d2 * 100);
  d3 = (int) (number / 10);      // Get tens
  d4 = number - (d3* 10);        // Get units
  
  DisplayADigit(disp1, byte(SevenSeg[d1]) ^ 0xff);  // Show thousands - invert for common anode display
  DisplayADigit(disp2, byte(SevenSeg[d2]) ^ 0xff);  // Show hundreds - invert for common anode display
  DisplayADigit(disp3, byte(SevenSeg[d3]) ^ 0xff);  // Show tens - invert for common anode display
  DisplayADigit(disp4, byte(SevenSeg[d4]) ^ dot ^ 0xff);  // Show units and dot - invert for common anode display
}
