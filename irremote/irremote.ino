#include <IRremote.h> // use the library

int receiver = 12; // IR receiver to Arduino digital pin 12
IRrecv irrecv(receiver); // create instance of 'irrecv'
decode_results results;

void setup()
{
  // initialize the digital pin as an output.
  DDRD = B11111111;
  DDRB = B11111111;
  DDRC = B11111111;
  // all set LOW
  PORTD = B00000000;  //pins 0 to 7 
  PORTB = B00000000;  //pins 8 to 13
  PORTC = B00000000;  //pins A0 to A5 
  Serial.begin(9600); // for serial monitor output
  irrecv.enableIRIn(); // Start the receiver
}

void loop()
{
  if (irrecv.decode(&results)) // have we received an IR signal?
  { //Serial.println(results.value, DEC); // display it on serial monitor in hexadecimal
    switch (results.value) { // KEYS Remote control
      case 1086292095: Serial.println("Source"); break;
      case 1086259455: Serial.println("Power"); break;
      case 1086283935: Serial.println("Up"); break; //Arrow Up  2
      case 1086288015: Serial.println("Down"); break; //Arrow Down  8
      case 1086312495: Serial.println("Right"); break; // Arrow Right  3
      case 1086296175: Serial.println("Left"); break; // Arrow Left   4 
      case 1086279855: Serial.println("Drag"); break; // 5 
      case 1086294135: Serial.println("Enter"); break; // OK button  0
      case 1086300255: Serial.println("Up Left"); break; // 1
      case 1086316575: Serial.println("Up Right"); break; // 3
      case 1086304335: Serial.println("Down Left"); break; // 7
      case 1086320655: Serial.println("Down Right"); break; // 9
      case 1086277815: Serial.println("Display L"); break;
      case 1086310455: Serial.println("CH RTN R"); break;
      case 1086318615: Serial.println("Snap Shot"); break;
      case 1086261495: Serial.println("16-CH Prev"); break;
      case 3225403455: Serial.println("Ch Up"); break;
      case 3225370815: Serial.println("Ch Down"); break;
      case 1086290055: Serial.println("Vol Down"); break;
      case 1086322695: Serial.println("Vol Up"); break;
      case 1086269655: Serial.println("Mute"); break;
      case 1086263535: Serial.println("Audio"); break;
      case 4294967295: /*Serial.println("Repeat");*/ break; // ? Repeat code ?
      default: {Serial.println(results.value, DEC); // display it on serial monitor in hexadecimal
                break;}
      };
      irrecv.resume(); // receive the next value
  }  // Your loop can do other things while waiting for an IR command
}
