#define LATCH 9
#define CLK 8
#define DATA 7

//This is the hex value of each number stored in an array by index num
//  common    cathode      anode     
//            .gfedcba    .gfedcba
//0 = 0x3f = B00111111 = B11000000
//1 = 0x06 = B00000110 = B11111001
//2 = 0x5b = B01011011 = B10100100
//3 = 0x4f = B01001111 = B10110000
//4 = 0x66 = B01100110 = B10011001
//5 = 0x6d = B01101101 = B10010010
//6 = 0x7d = B01111101 = B10000010
//7 = 0x07 = B00000111 = B11111000
//8 = 0x7f = B01111111 = B10000000
//9 = 0x6f = B01101111 = B10010000
//A = 0x77 = B01110111 = B10001000
//b = 0x7c = B01111100 = B10000011
//C = 0x39 = B00111001 = B11000110
//d = 0x5e = B01011110 = B10100001
//E = 0x79 = B01111001 = B10000110
//F = 0x71 = B01110001 = B10001110
//. = 0x80 = B10000000 = B01111111

//common cathode 
byte digitOne[16]= {B00111111, B00000110, B01011011, B01001111, 
                    B01100110, B01101101, B01111101, B00000111, 
		    B01111111, B01101111, B01110111, B01111100, 
		    B00111001, B01011110, B01111001, B01110001};
int i;
int j;
int dot = B10000000;

void setup(){
  pinMode(LATCH, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(DATA, OUTPUT);
}

void loop(){
  for(int i=0; i<16; i++){
    for(int j=0; j<5; j++){
      digitalWrite(LATCH, LOW);
      shiftOut(DATA, CLK, MSBFIRST, ~digitOne[i] ^ 0xff ^ dot); // switch digitOne to common anode
      digitalWrite(LATCH, HIGH);
	  dot = dot ^ B10000000; // switch dot on/off
      delay(200);
	}
  }
}
