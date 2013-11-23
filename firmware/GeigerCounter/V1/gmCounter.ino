int pulse_counter = 0;
int pulse_counter2 = 0;
int pulse_counter_raw = 0;

int pulse_raw = 0;

void gmCounterSetup(void)
{
    // attach interrupt at pin 2 for the geiger counter ticks
  attachInterrupt(1, interrupt_handler, RISING);
  
}

int gmGetRaw(void)
{
 return  pulse_raw;
}

#define SAMP_COUNT_PER_MINUTE  12

struct
{
  unsigned long OneMinute[SAMP_COUNT_PER_MINUTE];
  unsigned char OneMinuteIndex;
  unsigned char oneMinuteValid;
} 
sum;


void sumSetOneMinute(unsigned long geigerTicks)
{
  sum.OneMinute[sum.OneMinuteIndex] = geigerTicks;

  //pre-load data into average array.
  if(sum.oneMinuteValid == 0)
  {
    sum.oneMinuteValid = 1;
    for (int i = 0; i <= (SAMP_COUNT_PER_MINUTE - 1); i++) {
      sum.OneMinute[i] = geigerTicks;
    }
  }

  if (sum.OneMinuteIndex >= SAMP_COUNT_PER_MINUTE - 1) {
    sum.OneMinuteIndex = 0;
  }
  else sum.OneMinuteIndex++;
}

unsigned long sumGetOneMinute(void)
{
  unsigned long tempSum = 0;
  for (int i = 0; i <= (SAMP_COUNT_PER_MINUTE - 1); i++) {
    tempSum += sum.OneMinute[i];
  }
  return tempSum;
}

void gmLoop()
{
  static unsigned long txCounter = 0;
  static unsigned long timerX = 0;
  
  if (millis() - timerX > (500)) {
    timerX = millis();
    txCounter++;

    Serial.print("$");
    Serial.print("RAW:");
    Serial.print(pulse_counter_raw);
    pulse_raw = pulse_counter_raw;
    pulse_counter_raw =0;
    Serial.print(",");

    Serial.print("Counter");
    Serial.print(txCounter);

    Serial.print(",\n");
  }

  static unsigned long timerX2 = 0;
  if (millis() - timerX2 > (5000)) {
    timerX2 = millis();

    sumSetOneMinute(pulse_counter);
    pulse_counter = 0;

    Serial.print("$");
    Serial.print("CPM:");
    Serial.print(sumGetOneMinute());
    Serial.print(",");

    Serial.print("CPM2:");
    Serial.print(pulse_counter2 * 12);
    Serial.print(",\n");

    pulse_counter2 = 0;
  }
}

void interrupt_handler()
{
  pulse_counter_raw++;
  pulse_counter++;
  pulse_counter2++;
}



























