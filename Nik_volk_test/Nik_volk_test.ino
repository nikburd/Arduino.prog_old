// HC-020K Double Speed Measuring Module with Photoelectric Encoders
/*
Module Working Voltage: 4.5-5.5V
Launch Tube Pressure Drop: Vf=1.6V
Launch Tube Current: If<20mA
Signal output: A, B two lines; TT power level;
Resolution: 0.01mm
Measurement frequency: 100KHz
Disc diameter: 24mm
Inner Disc Diameter: 4mm
Encoder resolution: 20 lines
*/
#define encoder0PinA  2
#define encoder0PinB  3

// Define physical constants of the wheel for wheel encoding
#define WHEEL_CIRCUMFERENCE 0.212 // In meters
#define WHEEL_TICKS 80            // The number of 'ticks' for a full wheel cycle
#define WHEEL_DIST 0.12           // The distance between wheels in meters

// Motor B Left Motor
#define speedPinB  6 // PWM speed control EnB
#define dir2PinB   7 // direction control In3
#define dir1PinB   8 // direction control In4
// Motor A Right Motor
#define dir1PinA   9 // direction control In1
#define dir2PinA  10 // direction control In2
#define speedPinA 11 // PWM speed control EnA
//---------------------------
// Set motor speed for straight forward moving
int ssA = 205;
int ssB = 205;

volatile unsigned long encoderAPos=0;
volatile unsigned long encoderBPos=0;

unsigned long newAposition;
unsigned long oldAposition = 0;
unsigned long newBposition;
unsigned long oldBposition = 0;
unsigned long newtime;
unsigned long oldtime = 0;
int velA;
int velB;

int sA=190;
int sB=190;

int ss = 0; // difference in A and B positions

double velocityA;
double velocityB;

void setup()
{
  Serial.begin(19200); // check the default speed on the BT module, it should match
  
  // initialize pins for motors
  pinMode(dir1PinA,  OUTPUT);
  pinMode(dir2PinA,  OUTPUT);
  pinMode(speedPinA, OUTPUT);
  pinMode(dir1PinB,  OUTPUT);
  pinMode(dir2PinB,  OUTPUT);
  pinMode(speedPinB, OUTPUT);
  
  motor_speed (0, 0);
  
  pinMode(encoder0PinA, INPUT_PULLUP); // turn on pullup resisto
  pinMode(encoder0PinB, INPUT_PULLUP); // turn on pullup resistor
  // CHANGE to trigger the interrupt whenever the pin changes value
  attachInterrupt(0, doEncoderA, CHANGE); // encoderA ON PIN 2  RISING or FALLING or CHANGE
  attachInterrupt(1, doEncoderB, CHANGE);  // encoderB ON PIN 3  RISING or FALLING
}

void loop() {
   //return;
   static byte dir='F';
   if( Serial.available() ) // if data is available to read
   {dir = Serial.read();    // read it and store it in 'val'
   }
   
   newAposition = encoderAPos;
   newBposition = encoderBPos;
   newtime = millis();
   velA = (newAposition-oldAposition)*1000 /(newtime-oldtime);
   velB = (newBposition-oldBposition)*1000 /(newtime-oldtime);
   
   // Convert between angular velocity to velocity
   double angVelA = velA / (double)WHEEL_TICKS;
   velocityA = (angVelA * (double)WHEEL_CIRCUMFERENCE);
   
   double angVelB = velB / (double)WHEEL_TICKS;
   velocityB = (angVelB * (double)WHEEL_CIRCUMFERENCE);

   Serial.print (newBposition); Serial.print ("  "); Serial.println (newAposition);
   Serial.print (newBposition-oldBposition); Serial.print ("  "); Serial.println (newAposition-oldAposition);
   PrintDouble((double)newBposition/(double)WHEEL_TICKS*(double)WHEEL_CIRCUMFERENCE, 4); Serial.print ("  "); 
   PrintDouble((double)newAposition/(double)WHEEL_TICKS*(double)WHEEL_CIRCUMFERENCE, 4); Serial.println ("  "); 
    
   Serial.print ("vB="); Serial.print (velB);
   Serial.print (" vA="); Serial.println (velA);
   Serial.print ("ss="); Serial.println (ss);
   
   // Print velocity (m/s) to serial line
   Serial.print("LVel(B): ");PrintDouble(velocityB, 4);
   Serial.print(" RVel(A): ");PrintDouble(velocityA, 4);
   Serial.println ("");
   
   oldAposition = newAposition;
   oldBposition = newBposition;
   oldtime = newtime;
   
   // adjust absolute wheel position
   ss = newBposition - newAposition;
   if (abs(ss) < 120/4) {ss = 4*ss;}
   else {ss = (ss/ss)*120;}
   // Correct motor speed to meet values set in sA an sB
   ssA += (sA-velA)/12; //if (velA > sA) ssA--; else ssA++;
   ssB += (sB-velB)/12; //if (velB > sB) ssB--; else ssB++;
   
   motor_speed(sB+ss,sA-ss);
   delay(200);
}

void doEncoderA(){encoderAPos++;}

void doEncoderB(){encoderBPos++;}

// Set motors speed and direction
void motor_speed (int vl, int vr) {

  byte SpeedLeft;
  byte SpeedRight;
  char dr;
 
  if ((vr== 0) && (vl== 0)) {dr = 'S';}
  if ((vr > 0) && (vl > 0)) {dr = 'F';}
  if ((vr < 0) && (vl < 0)) {dr = 'B';}
  if ((vr > 0) && (vl<= 0)) {dr = 'L';}
  if ((vr<= 0) && (vl > 0)) {dr = 'R';}
  
  SpeedLeft = constrain(abs(vl), 0,255);
  SpeedRight = constrain(abs(vr), 0,255);
  
  analogWrite(speedPinB, SpeedLeft);  // Left Motor
  analogWrite(speedPinA, SpeedRight); // Right Motor
  motor_move(dr);
  
  Serial.print("vl="); Serial.print(vl); Serial.print(" vr="); Serial.println(vr); 
  //Serial.print("SL="); Serial.print(SpeedLeft);  Serial.print(" SR="); Serial.println(SpeedRight); 
  //delay(500);
}

// set direction
void motor_move (char dir)
{  
  switch (dir) { 
        case 'B':
        {   digitalWrite(dir1PinA, LOW); 
            digitalWrite(dir2PinA, HIGH);
            digitalWrite(dir1PinB, HIGH);
            digitalWrite(dir2PinB, LOW);
            break;
        }
        case 'F':
        {   digitalWrite(dir1PinA, HIGH);
            digitalWrite(dir2PinA, LOW);
            digitalWrite(dir1PinB, LOW);
            digitalWrite(dir2PinB, HIGH);
            break;
        }
        case 'L':
        {   digitalWrite(dir1PinA, HIGH);
            digitalWrite(dir2PinA, LOW);
            digitalWrite(dir1PinB, HIGH);
            digitalWrite(dir2PinB, LOW);
            break;
        }  
        case 'R':
        {   digitalWrite(dir1PinA, LOW); 
            digitalWrite(dir2PinA, HIGH);
            digitalWrite(dir1PinB, LOW);
            digitalWrite(dir2PinB, HIGH);
            break;
        }
        case 'S':
        {  //Brake //Stop
            digitalWrite(dir1PinA, HIGH);
            digitalWrite(dir2PinA, HIGH);
            digitalWrite(dir1PinB, HIGH);
            digitalWrite(dir2PinB, HIGH);
            break;
        }
        default: 
        { // Coast // Stop
            digitalWrite(dir1PinA, LOW);
            digitalWrite(dir2PinA, LOW);
            digitalWrite(dir1PinB, LOW);
            digitalWrite(dir2PinB, LOW);
            break;
        }
    }
}

// Print a double value onto the serial stream
// This is from the Arduino.cc forum
void PrintDouble( double val, byte precision)
{
  Serial.print (int(val));  //prints the int part
  if( precision > 0) {
    Serial.print("."); // print the decimal point
    unsigned long frac;
    unsigned long mult = 1;
    byte padding = precision -1;
    while(precision--)
     mult *=10;
 
    if(val >= 0)
      frac = (val - int(val)) * mult;
    else
      frac = (int(val)- val ) * mult;
    unsigned long frac1 = frac;
    while( frac1 /= 10 )
      padding--;
    while(padding--)
      Serial.print("0");
    Serial.print(frac,DEC) ;
  }
}
