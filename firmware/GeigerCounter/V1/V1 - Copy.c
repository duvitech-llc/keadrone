#include <RF12.h>
#include <Wire.h>
#include <avr/sleep.h>
#include <avr/power.h> //Needed for powering down perihperals such as the ADC/TWI and Timers

#define PCB_MOUNTED_BAROSENS_BMP085
#define PCB_MOUNTED_HUMSENS_DHT22

//#define PCB_MOUNTED_LIGHTSENS_TSL2561
#define PCB_MOUNTED_AMBSENS_LDR
//#define PCB_MOUNTED_TEMPSENS_MCP9700
//#define PCB_MOUNTED_WIRELESS_RFM12B
#define PCB_MOUNTED_BATT_SENSOR
//#define PCB_MOUNTED_ANALOG_OUTPUTS
//#define PCB_OUTPUT_PWM_ENABLED

/*********************************************************************/

/* Global data used for wireless transmission */
struct
{
    uint8_t messageIndex;
    uint8_t msgSize;

    int16_t clear;
    int16_t soilTemperature;
    // temp will be stored as value that has to be divided by 100 to get real temp.
    int16_t temperature;
    // pressure is in hPa value. unsigned!
    uint16_t pressure;
    // humidity scale = 0.0 to 100.0%
    uint16_t humidity;
    // relative measured capacistance
    uint16_t relativeCapacistance;
    //
    uint16_t lightIntensityLUX;
    uint16_t battVoltage;

    //
    uint16_t solarVoltage;
    uint16_t solarCurrent;

}
sensor;

struct
{
    unsigned int ldr;
    float battVoltage;
    float bmpPressure;
    float bmpTemp;
    float dhtTemp;
    float dhtHum;
    float gmTubeVoltage;
} data;

void fillData(void)
{
    data.bmpPressure = bmpGetPressure();
    data.bmpTemp = bmpGetTemp();
    data.dhtTemp = dhtGetTemp();
    data.dhtHum = dhtGetHum();

}

void dataHandle(void)
{
    // make a string for assembling the data to log:
    String dataString = "";

    char convert[16];
    dataString += String(millis());
    dataString += ',';
    dataString += "CPM:,";
    dataString += String(sumGetOneMinute());
    dataString += ',';
    dataString += "Ldr:,";
    dataString += String(data.ldr);
    dataString += ',';
    dataString += "battV:,";
    dtostrf(data.battVoltage, 5, 2, convert);
    dataString += convert;
    dataString += ',';
    dataString += "hPa:,";
    dtostrf(data.bmpPressure, 5, 2, convert);
    dataString += convert;
    dataString += ',';
    dataString += "BmpTemp:,";
    dtostrf(data.bmpTemp, 5, 2, convert);
    dataString += convert;
    dataString += ',';
    dataString += "dhtTemp:,";
    dtostrf(data.dhtTemp, 5, 2, convert);
    dataString += convert;
    dataString += ',';
    dataString += "Hum:,";
    dtostrf(data.dhtHum, 5, 2, convert);
    dataString += convert;
    dataString += ',';
    dataString += "gmVolt:,";
    dataString += String((uint16_t) data.gmTubeVoltage);

    sdPrintData(&dataString);

    Serial.println();
}

void setup()
{

    Serial.begin(9600);
    Serial.println("StartUp\n");

    // chip select rfm12
    pinMode(8, OUTPUT);
    digitalWrite(8, 1);
    delay(1000);

    pwmSetup();
    sdInit();

    gmCounterSetup();

    //enable power to ext. units
    pinMode(4, OUTPUT);
    digitalWrite(4, 1);

    //enable power to DHT11
    pinMode(7, OUTPUT);
    digitalWrite(7, 1);

    delay(2000);

#ifdef PCB_MOUNTED_LIGHTSENS_TSL2561
    TSL2561_setup();
#endif
#ifdef PCB_MOUNTED_BAROSENS_BMP085
    bmp085_setup();
#endif

#ifdef PCB_MOUNTED_WIRELESS_RFM12B
    RFM12B_setup();
#endif

#ifdef PCB_OUTPUT_PWM_ENABLED
    pwmOutputSetup();
#endif

    // enable led output
    pinMode(5, OUTPUT);

    Serial.println("Init OK \n");
}

void loop()
{

#ifdef PCB_MOUNTED_LIGHTSENS_TSL2561
    TSL2561_getvalues();
#endif

#ifdef PCB_MOUNTED_BAROSENS_BMP085
    bmp085_Loop();
#endif

#ifdef PCB_MOUNTED_HUMSENS_DHT22
    dht22_Loop();
#endif

    // to settle external units
    delay(10);

    gmLoop();
    unsigned int val;

#ifdef PCB_MOUNTED_AMBSENS_LDR
    // ldr
    val = 1023 - analogRead(6); // read the input pin
    val += 1023 - analogRead(6); // read the input pin
    val += 1023 - analogRead(6); // read the input pin
    val += 1023 - analogRead(6); // read the input pin
    val = val / 4;
    data.ldr = (val + data.ldr) / 2;

#endif

#ifdef PCB_MOUNTED_TEMPSENS_MCP9700
    Serial.print("SoilTemp ");
    //temp sensor
    val = analogRead(2); // read the input pin
    val += analogRead(2); // read the input pin
    val += analogRead(2); // read the input pin
    val += analogRead(2); // read the input pin
    val = val / 4;
    float voltage = (float) val * (1100.0 / 1024.0);
    float tempC = (voltage - 500.0) / 10.0;
    Serial.print(tempC);
    Serial.print(", ");
    // multiply to send comma trhough wireless
    tempC = tempC * 100;
    sensor.soilTemperature = (tempC); // + sensor.soilTemperature)/2;
#endif

#ifdef PCB_MOUNTED_BATT_SENSOR
    analogReference(INTERNAL);
    for (int count = 0; count < 10; count++) {
        val = analogRead(7); // read the input pin
        delay(10);
    }

    val = analogRead(7); // read the input pin
    val += analogRead(7); // read the input pin
    val += analogRead(7); // read the input pin
    val += analogRead(7); // read the input pin

    analogReference(DEFAULT);
    val = val / 4;
    float batteryVoltage = (val * (1100.0 / 1024.0)) * 11.0 / 1000.0;

    data.battVoltage = batteryVoltage;
    sensor.battVoltage = ((batteryVoltage * 100)); //+ sensor.battVoltage)/2;
#endif

    val = analogRead(1); // read the input pin
    val += analogRead(1); // read the input pin
    val += analogRead(1); // read the input pin
    val += analogRead(1); // read the input pin
    val = val / 4;
    float gmTubeVoltage = val * (254.0 / 320.0);

    data.gmTubeVoltage = gmTubeVoltage;

#ifdef PCB_MOUNTED_WIRELESS_RFM12B
    RFM12B_loop();
#endif
    fillData();
    dataHandle();
#ifdef SLEEP_ENABLED

    //disable power to ext. units
    digitalWrite(4, 0);

    set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here

    sleep_enable(); // enables the sleep bit in the mcucr register
    // so sleep is possible. just a safety pin

    // extra delay to empty the uart before shutting it down.
    Serial.flush();
    //Shut off ADC, TWI, SPI, Timer0, Timer1
    ADCSRA &= ~(1 << ADEN); //Disable ADC
    ACSR = (1 << ACD); //Disable the analog comparator
    DIDR0 = 0x3F; //Disable digital input buffers on all ADC0-ADC5 pins
    DIDR1 = (1 << AIN1D) | (1 << AIN0D); //Disable digital input buffer on AIN1/0

    power_twi_disable();
    power_spi_disable();
    power_usart0_disable();
    power_timer0_disable(); //Needed for delay_ms
    power_timer1_disable();
    power_timer2_disable();

    // wakeUp when pin 2 gets LOW
    attachInterrupt(0, wakeUpNow, LOW); // use interrupt 0 (pin 2) and run function
    sleep_mode(); // here the device is actually put to sleep!!

    // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
    sleep_disable(); // first thing after waking from sleep:
    detachInterrupt(0); // disables interrupt 0 on pin 2

#ifdef PCB_MOUNTED_WIRELESS_RFM12B
    rf12_initialize(1, RF12_433MHZ, 33);
#endif

#else
    delay(2000);
#endif

    // power to ext. units
    digitalWrite(4, 1);
}

void wakeUpNow() // here the interrupt is handled after wakeup
{
    // execute code here after wake-up before returning to the loop() function
    // timers and code using timers (serial.print and more...) will not work here.
    // we don't really need to execute any special functions here, since we
    // just want the thing to wake up

    ADCSRA |= (1 << ADEN); //Disable ADC
    //ACSR = (1<<ACD); //Disable the analog comparator
    //DIDR0 = 0x3F; //Disable digital input buffers on all ADC0-ADC5 pins
    //DIDR1 = (1<<AIN1D)|(1<<AIN0D); //Disable digital input buffer on AIN1/0

    //power_twi_enable();
    power_spi_enable();
    power_usart0_enable();
    power_timer0_enable(); //Needed for delay_ms
    power_timer1_enable();
    power_timer2_enable(); //Needed for asynchronous 32kHz operation
}






























