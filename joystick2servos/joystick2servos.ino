#include <Servo.h>

Servo myservo1;  // create servo object to control a servo
Servo myservo2;

#define pinx  A3  // analog pin used to connect X
#define piny  A4  // analog pin used to connect Y
#define pinz  A5  // analog pin used to connect Z


const byte averageFactor = 3;   // коэффициент сглаживания показаний (0 = не сглаживать)
                                // чем выше, тем больше "инерционность"
int valx;    // variable to read the value
int valy;    // variable to read the value
int valz;    // variable to read the value

int valx0;   // previous value
int valy0;   // previous value

int pos1;     // new position
int pos2;    // 2nd servo position

void setup()
{
  myservo2.attach(7);  // attaches the servo on pin 9
  myservo1.attach(6);  // attaches the servo on pin 10
  
  Serial.begin(38400);
  
  //set starting position according input
  readSensors(3);
  pos2 = map(valx, 10, 1000, 0, 180);     // scale it between 0 and 180
  pos1 = map(valy, 10, 1000, 0, 180);
  
  myservo1.write(pos1);   
  myservo2.write(pos2);
  
  delay(30);
}

void loop()
{
  valx0 = valx;
  valy0 = valy;
  readSensors(3);         // reads the value of the pot
  if (averageFactor > 0)        // усреднение показаний для устранения "скачков"
	  {     
	    valx = (valx0 * (averageFactor - 1) + valx) / averageFactor; 
            valy = (valy0 * (averageFactor - 1) + valy) / averageFactor; 
            valx = min(1023,max(0,valx));
            valy = min(1023,max(0,valy));
	    // <новое среднее> = (<старое среднее>*(averageFactor-1) + <текущее значение>) / averageFactor
 	  }
	  
  pos2 = map(valx, 10, 1000, 0, 180);     // scale it between 0 and 180
  pos1 = map(valy, 10, 1000, 0, 180);
  
  myservo1.write(pos1);   
  myservo2.write(pos2);
  
  Serial.print( valx ); Serial.print("  ");
  Serial.print( valy ); Serial.print("  ");
  Serial.print( pos1 ); Serial.print("  ");
  Serial.print( pos2 ); Serial.println("  ");
  
  //Move2Servos(0, 0);   delay(500); 
  //Move2Servos(180, 180);  delay(500); 
  //Move2Servos(0, 180);   delay(500); 
  //Move2Servos(180, 0);   delay(500); 
}

void readSensors(int samples){ // samples - сколько раз нужно прочитать сенсор
     unsigned int avg_sumx=0;
     unsigned int avg_sumy=0;
	   for(byte i=0;i<samples;i++){
	       avg_sumx+= constrain(analogRead(pinx), 10, 1000);
               avg_sumy+= constrain(analogRead(piny), 10, 1000);
	       delay(5); // небольшая пауза между замерами
	   }
	   valx = avg_sumx/samples;
           valy = avg_sumy/samples;
	}
	
void Move2Servos (int pos1, int pos2) {
  int pos01 = myservo1.read();
  int pos02 = myservo2.read(); 
  if ((abs(pos01-pos1) > 2 ) || (abs(pos02-pos2) > 2 )) { 
  while ((pos01 != pos1) && (pos02 != pos2)) {
     if (pos01 > pos1) { myservo1.write(--pos01); };
     if (pos01 < pos1) { myservo1.write(++pos01); };
     if (pos02 > pos2) { myservo2.write(--pos02); }; 
     if (pos02 < pos2) { myservo2.write(++pos02); };
     delay(1);
    } 
  }
}	
