const uint8_t PinLED = 13;
void setup( void )
{
 Serial.begin( 38400 );
 Serial.println( "\r\n\r\n" );
 pinMode( PinLED, OUTPUT );
 digitalWrite( PinLED, LOW );
 delay( 1000 );
 //analogReference( INTERNAL );
}
void loop( void )
{
 Serial.print(Thermistor()); Serial.print(" °C  Vref = ");
 Serial.print(readRefVcc()); Serial.print("  Raw = ");
 Serial.println(analogRead( A0 ));
 digitalWrite( PinLED, HIGH );
 delay( 1000 );
}

int Thermistor() {
  const double R = 8600.0;
  double Temp;
  double avg_temp = 0;
  for(byte i = 0; i < 3; i++) {
    Temp = log(R*(1024.0/analogRead(A0)-1)); // for pull-up configuration
    Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp )) * Temp );
    Temp = Temp - 273.15;              // Convert Kelvin to Celcius
    //Temp = (Temp * 9.0)/ 5.0 + 32.0; // Convert Celcius to Fahrenheit
    avg_temp += Temp;
    delay(3);
    }
    avg_temp /= 3;
  return (int)avg_temp;
}

int readRefVcc() {
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
  // make series of reading and compute average
  unsigned long LRefVcc = 0;
  for (int i=0; i<3; i++) {
    delay(3); // Wait for Vref to settle
    ADCSRA |= _BV(ADSC); // Start conversion
    while (bit_is_set(ADCSRA,ADSC)); // measuring
    uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
    uint8_t high = ADCH; // unlocks both
    long result = (high<<8) | low;
    // scale_constant = internal1.1Ref * 1023 * 1000
    // where internal1.1Ref = 1.1 * Vcc1 (per voltmeter) / Vcc2 (per readRefVcc() function)
    
    // kresult = 1125300L / result; // Calculate Vcc (in mV); 1125300L = 1.1*1023*1000
    result = 1107402L / result;
    LRefVcc += result;
  }
  LRefVcc /= 3;
  return (int)LRefVcc; // Vcc in mV
}

