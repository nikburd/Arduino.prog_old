// RoboCar3 on Arduino Leonardo

#define serial Serial  // Serial1 - BlueTooth interface, Serial - USB
#define _DEBUG_ON_

// Serial1 (BT)
#define bt_rx      D0 // -> BT tx
#define bt_tx      D1 // -> BT rx resistive divider here: 2p12k to rx and 12k to GND

#define _SDA       D2 // (Data)  accelerometer
#define _SDL       D3 // (Clock) accelerometer
#define LDL        D4 // Left led
#define LDR        D5 // Right led

// motor A  Left Motor
#define speedPinB  D6  // A8, PWM
#define dir1PinB   D7  // Ain0
#define dir2PinB   D8  // A9
// motor B  Right Motor
#define dir2PinA   D9  // A10
#define dir1PinA   D10 // A11
#define speedPinA  D11 // PWM

#define BTN1       D12 // Button 1
#define BR         D12 // Tilt or 'Break' sensor
#define u_tone     D13 // Tone speaker
#define LED        D13 // Blinking on board led and rear led

//Connect LCD 5110 
//           Vcc   3.3V
//           GND   GND
#define   SS_SCE   GND or cheep select via R10k
//           RST   or Reset via R10k
#define  MISO_DC   D14 //R10k
#define  MOSI_DN   D16 //R10k
#define SCK_SCLK   D15 //R10k
//           LED   R100 - 5v
//       free      D17

// Line Detectors  > 800 - black,   < 200 - white
#define la_left    A0
#define la_center  A1
#define la_right   A2
#define BatVcc     A3 // Battery direct Voltage from motors driver via /3 divider (24k/12k)
#define PHDL       A4 // Photo Detector Left
#define PHDR       A5 // Photo Detector Right

// Timers & PWM
// D3:  8-bit  timer0
// D5:  16-bit timer1&3
// D6:  10-bit timer4
// D9:  16-bit timer1&3
// D10: 16-bit timer1&3
// D11: 8-bit  timer0
// D13: 10-bit timer4
// A0 – D18
// A1 – D19
// A2 – D20
// A3 – D21
// A4 – D22
// A5 – D23
// A6 – D4
// A7 – D6
// A8 – D8
// A9 – D9
// A10 – D10
// A11 – D12
    
// Line sensor read   1 - Line,  0 - No Line
volatile unsigned int sr; // Right
volatile unsigned int sc; // Center
volatile unsigned int sl; // Left
volatile unsigned int sp; // position: 2000 center; < 2000 right; > 2000 left; 0 all white(0), 4000 all dark(1)

unsigned long sTime; // timestamp for movements and tests
unsigned long mTime; // timestamp for movements and tests
unsigned long LTime; // timestamp for high-level loops
unsigned int curLTime; // Current Loop time

#define Vmin 6200  // mV Minimal Battery voltage allowed for operation: 3v+3v=6v
#define VminInt 4700  // mV Minimal internal voltage allowed for operation: 4.7v
unsigned int Vcc=readVccInt(); //Core Voltage mV - should be > 4700Mv for normal operation
unsigned int VccBat=readVcc(); //Core Voltage mV - should be > 4700Mv for normal operation

volatile char start = 0; // Start flag; set to 1 for start main loop

void(* resetFunc) (void) = 0; //declare reset function at address 0

// Read free SRAM in bytes 
int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void setup() { 
     serial.begin(38400);    // Open serial monitor. BT can operate at 38400 by default
     serial.println("\n\n\n  START  \n\n\n");
     
     pinMode(LED, OUTPUT);  // initialize the digital pin as an output.
     pinMode(LDR, OUTPUT);  // initialize the digital pin as an output.
     pinMode(LDL, OUTPUT);  // initialize the digital pin as an output.     
     
// initialize pins for motors
     pinMode(dir1PinA,  OUTPUT);
     pinMode(dir2PinA,  OUTPUT);
     pinMode(speedPinA, OUTPUT);
     pinMode(dir1PinB,  OUTPUT);
     pinMode(dir2PinB,  OUTPUT);
     pinMode(speedPinB, OUTPUT);
     motor_speed (0, 0);

// initialize pins for line detectors
     pinMode(la_left,   INPUT);
     pinMode(la_center, INPUT);
     pinMode(la_right,  INPUT);
     
// Photo detector
     pinMode(PHDR, INPUT);     
     pinMode(PHDL, INPUT);

// Breake detector
     pinMode(BR, INPUT);
     //digitalWrite(BR, HIGH); // Pull Up, so wait LOW when pressed

// Buttons    
     pinMode(BTN1, INPUT_PULLUP); // Pull Up, so wait LOW when pressed
     
// save the start time  
     sTime = millis();
     LTime = sTime;
     
// Check internal voltage
     VccBat = readVcc(); // switch robot off when VCCBat < VMin 6000
     Vcc = readVccInt(); // switch robot off when VCC < VMin 4700
     
     serial.print(F("VccInt(mV): ")); 
     serial.print(Vcc);
     serial.print(F("  VccBat(mV): ")); 
     serial.println(VccBat);
     
// wait for button pressed to continue
     while (digitalRead(BTN1)) {
        tone(u_tone, 194, 100); delay(10); noTone(u_tone);
        blink(3);
        delay(500);
     }
     
// Play Start
     start_tone(); 
    } 

// main loop cycle
void loop() { 
     serial.println(String(freeRam())+F("bytes"));
     
     if (digitalRead(BR)) {delay(100);}
     else {light_follow();}
     
     Vcc = readVcc();
     if (Vcc < VminBat) {
         blink(2);  
         Vcc = readVcc(); 
         motor_speed (0,0); 
         delay(5000); 
         return;
         };
     return;

     // start only when a black line is under middle sensor
     sp = read_line_analog(); 
     if ((start == 1) or (sp == 2000)) { 
        start=1;
        line_follow();
        };
     return;
       
} // main loop


// follow light source
void light_follow() { 

  int proportional;
  int derivative;
  static int integral;
  static int last_proportional;

  int lightR;
  int lightL;
 

  lightR = analogRead(PHDR);
  lightL = analogRead(PHDL);
  if (lightR <= 5 && lightL <= 5) { // too dark, stop and exit
    motor_speed(0,0);
    digitalWrite(LDR, LOW); // indicate stop
    digitalWrite(LDL, LOW); // indicate stop
    return;
  }
    if (lightR > 1500 && lightL > 1500) { // too bright, stop and exit
    motor_speed(0,0);
    digitalWrite(LDR, LOW); // indicate stop light
    digitalWrite(LDL, LOW); // indicate stop light
    return;
  }
  // Calculate proportional position 
  // The "proportional" term should be 0 when we are on the direct line to the source of the light.
  proportional = (lightL - lightR);

  // Compute the derivative (change) and integral (sum) of the position.
  derivative = proportional - last_proportional;
  integral += proportional;

  // Remember the last position.
  last_proportional = proportional;

  int power_difference = proportional/3 + derivative*3 + integral/5000;

  // Compute the actual motor settings.  We never set either motor to a negative value.
  const int max = 255;
  int k = 1; // Full forward speed gain
  int q = 1; //preceision of PID
  
  power_difference = constrain(power_difference, -max, max);
  #if defined _DEBUG_ON_
     serial.print(lightR); serial.print("  "); serial.println(lightL);
     serial.print("proportional=");      serial.println(proportional); 
     serial.print("derivative=");        serial.println(derivative);
     serial.print("integral=");          serial.println(integral);
     serial.print("power_difference=");  serial.println(power_difference);
  #endif
 if (abs(power_difference) > q) {
    motor_speed(max-power_difference, max+power_difference);
    digitalWrite(LDR, power_difference < 0); // Go Right
    digitalWrite(LDL, power_difference > 0); // Go Left
  }
  else {
    motor_speed(k*max, k*max);
    digitalWrite(LDR, HIGH); // Full Forward
    digitalWrite(LDL, HIGH); // Full Forward
  } 
  return;
} // end light_follow    
    
//light position: 0-1000 on the left, 2000 center, 3000-4000 on the right
unsigned int read_light_analog() { 

     unsigned int _numSensors = 2;
     unsigned char on_line = 0;
     unsigned long avg; // this is for the weighted total, which is long before division
     unsigned int sum;  // this is for the denominator which is <= 64000
     static int last_value = 1000; // assume initially that the line is left.
     
     avg = 0;
     sum = 0;
     lightR = analogRead(PHDR);
     lightL = analogRead(PHDL);
     
     // keep track of whether we see the line at all
     if ((lightR > 200) || (lightL > 200)) on_line = 1;
     
     // only average in values that are above a noise threshold
     if(lightL > 10) {
         avg += (long)(lightL) * 1000;
         sum += lightL;
        }
     if(lightR > 10) {
         avg += (long)(lightR) * 3000;
         sum += lightR;
        }
/*        
     serial.print("lightL="); Serial.println(lightL); 
     serial.print("lightR="); Serial.println(lightR);  
     serial.println(avg); Serial.println(sum); 
     serial.println(on_line);
*/
     if(!on_line) {
     // If it last read to the left of center, return the min (0).  2*1000 = 2000.
     if (last_value < (_numSensors-1)*1000) return 0;
         // If it last read to the right of center, return the max. 1*4*2000 = 4000
         else return (_numSensors-1)*1000*4;
        }
         last_value = avg/sum;
         return last_value;
    } // end read_light_analog
    
//int sp line position: 1000 under left sensor, 2000 center, 3000 right
unsigned int read_line_analog() { 

     unsigned int _numSensors = 3;
     unsigned char on_line = 0;
     unsigned long avg; // this is for the weighted total, which is long before division
     unsigned int sum; // this is for the denominator which is <= 64000
     static int last_value=1000; // assume initially that the line is left.
     
     avg = 0;
     sum = 0;
     sl = analogRead(la_left);
     sc = analogRead(la_center);
     sr = analogRead(la_right);
     
     // keep track of whether we see the line at all
     if ((sl > 400) || (sc > 400) || (sr > 400)) on_line = 1;
     
     // only average in values that are above a noise threshold
     if(sl > 60) {
         avg += (long)(sl) * 1000;
         sum += sl;
        }
     if(sc > 60) {
         avg += (long)(sc) * 2000;
         sum += sc;
        }
     if(sr > 60) {
         avg += (long)(sr) * 3000;
         sum += sr;
        }
/*        
     serial.print("sl="); Serial.println(sl); 
     serial.print("sc="); Serial.println(sc);
     serial.print("sr="); Serial.println(sr);  
     serial.println(avg); Serial.println(sum); 
     serial.println(on_line);
*/
     if(!on_line) {
     // If it last read to the left of center, return the min (0).  2*1000 = 2000.
     if (last_value < (_numSensors-1)*1000) return 0;
         // If it last read to the right of center, return the max. 2*2*2000 = 4000
         else return (_numSensors-1)*1000*2;
        }
         last_value = avg/sum;
         return last_value;
    } // end read_line_analog
    

// follow black line
void line_follow() { 

  static int integral;
  static int last_proportional;
  int proportional;
  int derivative;
  //unsigned int curTime = millis()-sTime;

  sp = read_line_analog();

  // The "proportional" term should be 0 when we are on the line.
  proportional = ((int)sp) - 2000;

  // Compute the derivative (change) and integral (sum) of the position.
  derivative = proportional - last_proportional;
  integral += proportional;

  // Remember the last position.
  last_proportional = proportional;

  // Compute the difference between the two motor power settings,
  // m1 - m2.  If this is a positive number the robot will turn
  // to the right.  If it is a negative number, the robot will
  // turn to the left, and the magnitude of the number determines
  // the sharpness of the turn.
  int power_difference = proportional/6 + derivative*12 + integral/50000;

  // Compute the actual motor settings.  We never set either motor to a negative value.
  const int max = 125;
  int k = 2; // Full forward speed gain
  int q = 10; //preceision of PID
  
  power_difference = constrain(power_difference, -max, max);

  /*
   serial.print("sp=");                serial.println(sp); 
   serial.print("proportional=");      serial.println(proportional); 
   serial.print("derivative=");        serial.println(derivative);
   serial.print("integral=");          serial.println(integral);
   serial.print("last_proportional="); serial.println(last_proportional);
   serial.print("power_difference=");  serial.println(power_difference);
   */
  if (power_difference < -q) {
    motor_speed(max-power_difference, max+power_difference);
    digitalWrite(LDR, LOW); 
    digitalWrite(LDL, HIGH);  // Full Left
  }
  else if (power_difference > q) {
    motor_speed(max-power_difference, max+power_difference);
    digitalWrite(LDR, HIGH); // Full Right
    digitalWrite(LDL, LOW);  
  }
  else {
    motor_speed(k*max, k*max);
    digitalWrite(LDR, HIGH); // Full Forward
    digitalWrite(LDL, HIGH); // 
  } 
  return;
} // end line_follow


// set speed and move
void motor_speed (int vl, int vr) { 
    char dr;
    
    if ((vr == 0) && (vl == 0)) { dr = 'S'; }
    if ((vr >  0) && (vl >  0)) { dr = 'F'; }
    if ((vr <  0) && (vl <  0)) { dr = 'B'; }
    if ((vr >  0) && (vl <  0)) { dr = 'L'; }
    if ((vr <  0) && (vl >  0)) { dr = 'R'; }
     
    analogWrite(speedPinB, constrain(abs(vl), 0, 255)); // Left Motors 
    analogWrite(speedPinA, constrain(abs(vr), 0, 255)); // Right Motors
    
    motor_move(dr);
    
    }

// set direction
void motor_move (char dir) { 
     switch (dir) { 
         case 'F': {  //Forward
             //serial.println("Mv F");
             digitalWrite(dir1PinA, LOW);  // Left Motors 
             digitalWrite(dir2PinA, HIGH);
             digitalWrite(dir1PinB, HIGH); // Right Motors
             digitalWrite(dir2PinB, LOW);
             break;
            };
         case 'B': { //Back
             //serial.println("Mv B"); 
             digitalWrite(dir1PinA, HIGH);  // Left Motors 
             digitalWrite(dir2PinA, LOW);
             digitalWrite(dir1PinB, LOW);   // Right Motors
             digitalWrite(dir2PinB, HIGH);   
             break;
            };
         case 'R': { // Turn Right Fast
             //serial.println("Mv R");
             digitalWrite(dir1PinA, HIGH);  // Left Motors   -- HIGH
             digitalWrite(dir2PinA, LOW);  // Coast  //Backward
             digitalWrite(dir1PinB, HIGH); // Right Motors
             digitalWrite(dir2PinB, LOW);  // Forward      
             break;
            };
         case 'L': { // Turn Left Fast
             //serial.println("Mv L");
             digitalWrite(dir1PinA, LOW);  // Left Motors
             digitalWrite(dir2PinA, HIGH); // Forward
             digitalWrite(dir1PinB, LOW);  // Right Motors
             digitalWrite(dir2PinB, HIGH);  // Coast //Backward -- HIGH
             break;
             };
         case 'S': {  //Brake //Stop
             //serial.println("S");
             digitalWrite(dir1PinA, HIGH);
             digitalWrite(dir2PinA, HIGH);
             digitalWrite(dir1PinB, HIGH);
             digitalWrite(dir2PinB, HIGH);   
             break;
            };
         default: { // Coast // Stop
             //serial.println("U");
             digitalWrite(dir1PinA, LOW);
             digitalWrite(dir2PinA, LOW);
             digitalWrite(dir1PinB, LOW);
             digitalWrite(dir2PinB, LOW);
             break;
            };
        }
    } // end motor_move    
        
int readVccInt() {
     // Read 1.1V reference against AVcc
     // set the reference to Vcc and the measurement to the internal 1.1V reference
     #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
        ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
     #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
        ADMUX = _BV(MUX5) | _BV(MUX0);
     #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
        ADMUX = _BV(MUX3) | _BV(MUX2);
     #else
        ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
     #endif  
 
     delay(2); // Wait for Vref to settle
     ADCSRA |= _BV(ADSC); // Start conversion
     while (bit_is_set(ADCSRA,ADSC)); // measuring
 
     uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
     uint8_t high = ADCH; // unlocks both
 
     long result = (high<<8) | low;
 
     result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
     return result; // Vcc in millivolts
    } // end readVcc
    
int readVcc() {
  analogReference(DEFAULT); // the default analog reference of 5 volts (on 5V Arduino boards)
  const byte samples = 3;
  unsigned int avg_sum = 0;
  static int val0;
  static int val = analogRead(BatVcc);
  const byte averageFactor = 2;   // коэффициент сглаживания показаний (0 = не сглаживать)
                                  // чем выше, тем больше "инерционность"
  for(byte i = 0; i < samples; i++){
      avg_sum += analogRead(BatVcc);
      delay(2); // небольшая пауза между замерами
    }
  val0 = val;
  val = avg_sum/samples;        // reads the value of the pot
  if (averageFactor > 0) {      // усреднение показаний для устранения "скачков"  
        val = (val0 * (averageFactor - 1) + val) / averageFactor; 
      } // <новое среднее> = (<старое среднее>*(averageFactor-1) + <текущее значение>) / averageFactor  
  return map(val, 0,1023, 0,5000*2); // R2/(R1+R2) 12/(12+12) = 2 
}

// blink N times with 60ms interval (adds s120ms delay per 1 blink)
void blink(byte n) {   
     byte i;
     for (i = 1; i <= n; i++) { 
         digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
         delay(60);
         digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
         delay(60);
        }
    }
    
// play start tone melody
/*************************************************
 * Public Constants
 *************************************************/

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

void start_tone() {
int melody[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {4, 8, 8, 4, 4, 4, 4, 4};
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations[thisNote];
    tone(u_tone, melody[thisNote], noteDuration);
    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(u_tone);
    }
}