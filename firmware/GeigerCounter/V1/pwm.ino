int pwmPin = 9;
int pwmVal = 255;

void pwmSetup(void)
{

  pinMode(pwmPin, OUTPUT);

  // Set up Fast PWM on Pin 9
  TCCR1A = 0x23;     //0x23 COM2B1, WGM21, WGM20 
  TCCR1B = 0xA;//0x0B;   // Set prescaler  
  OCR1A = 199;    //
  OCR1B = 45;    // initial duty cycle for Pin 3 (0-199)

  // with 10Mohm at the converter, this will be exactly 400V at 5v inputvoltage
  pwmVal = 760;
  analogWrite(pwmPin, (pwmVal));

}

