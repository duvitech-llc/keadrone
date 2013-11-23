
#ifdef PCB_MOUNTED_BAROSENS_BMP085

#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

static float bmpTemp = 0;
static float bmpPressure = 0;

void bmp085_setup(void)
{
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {
    }
  }
  Serial.println("BMP initialized\n");
}

void bmp085_Loop(void){
  //Serial.print("BmpTemp = ");
  bmpTemp = bmp.readTemperature();

  //Serial.print(bmpTemp);
  //Serial.print(" *C  ");

  // multiply float to include 2 digits after the decimal point.
  //temp *= 100;
  // discard digits, by cast to integer, after decimal point  
  //sensor.temperature = (int16_t) temp;

  //Serial.print("Pressure = ");
  bmpPressure = (float)bmp.readPressure() / 100;
  //Serial.print(temp);
  //Serial.print(" hPa   ");

  // store hPa pressure as 16 bit
  sensor.pressure = (uint16_t)((float)bmpPressure*10.0);

  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
  //Serial.print("Altitude = ");
  //Serial.print(bmp.readAltitude());
  //Serial.print(" meters   ");

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
  //  Serial.print("Real altitude = ");
  //  Serial.print(bmp.readAltitude(101500));
  // Serial.print(" meters   ");

}

float bmpGetTemp(void)
{
  return  bmpTemp;
}

float bmpGetPressure(void)
{
  return bmpPressure;
}

#endif

