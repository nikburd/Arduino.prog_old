#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Streaming.h>
#include <Metro.h>
 
const byte NokiaSCLKpin = 13; // compatible with hardware ISP
const byte NokiaDNpin = 11; // compatible with hardware ISP
const byte NokiaDCpin = 12; 
const byte NokiaRESETpin = 10; 
 
const char LCDcontrast = 57;
int i;
 
Adafruit_PCD8544 nokia(NokiaSCLKpin, NokiaDNpin, NokiaDCpin, 0, NokiaRESETpin); 
Metro LCDinitTimer(2000); 
 
void setup(void) 
{ Serial.begin(9600);
  nokia.init(LCDcontrast);
  nokia.clear();
  nokia << "Hello!\n"; // without using F
  nokia << F("Save RAM with the Flash library\n"); // use F to save RAM space
  nokia.display();
  findBestContrast(40, 60, 500);
  nokia.setContrast(LCDcontrast); 
}
 
void loop(void) 
{ nokia.clearDisplay();
  nokia << F("Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor ") << i++;
  nokia.display();
  if(LCDinitTimer.check()) nokia.init(); // allow disconnecting and connecting the LCD during execution
}
 
void findBestContrast(int contrastMin, int contrastMax, int _delay)
{ for(int contrast=contrastMin; contrast<=contrastMax; contrast++)
  { nokia.clearDisplay();
    nokia.setContrast(contrast);
    nokia << F("12345678901234");
    nokia << F("12345678901234\n"); // Bug: "\n" after 14 characters creates a blank line
    nokia << F("bla bla bla bla bla bla\n");
    nokia << contrast;    
    nokia.display();
    delay(_delay);
  }
}
