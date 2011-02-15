// Axis definition: 
// X axis pointing forward (to the FTDI connector)
// Y axis pointing to the right
// and Z axis pointing down.
// Positive pitch : nose up
// Positive roll : right wing down
// Positive yaw : clockwise

/* Hardware version - v13

 ATMega328@3.3V w/ external 8MHz resonator
 High Fuse DA
 Low Fuse FF

 ADXL345: Accelerometer
 HMC5843: Magnetometer
 LY530:	Yaw Gyro
 LPR530:	Pitch and Roll Gyro

 Programmer : 3.3v FTDI
 Arduino IDE : Select board  "Arduino Duemilanove w/ATmega328"
 */
#include "stdint.h"
#include "calculations.h"

int8_t sensors[3] =
{ 1, 2, 0 }; // Map the ADC channels gyro_x, gyro_y, gyro_z

static long timer = 0; //general purpose timer
static long timer_old;
static unsigned int counter = 0;

//unused??(not using magnetometer)
float MAG_Heading;

void setup()
{
	/*
	Serial.begin(57600);
	pinMode(STATUS_LED, OUTPUT); // Status LED

	Analog_Init();
	I2C_Init();
	Accel_Init();
	Read_Accel();

	Serial.println("Sparkfun 9DOF Razor AHRS");

	digitalWrite(STATUS_LED, LOW);
	delay(1500);

	// Magnetometer initialization
	Compass_Init();

	// Initialze ADC readings and buffers
	Read_adc_raw();
	delay(20);

	for (int i = 0; i < 32; i++) // We take some readings...
	{
		Read_adc_raw();
		Read_Accel();
		initi2c1();
		delay(20);
	}
	initi2c2();

	delay(2000);
	digitalWrite(STATUS_LED, HIGH);

	Read_adc_raw(); // ADC initialization
	timer = millis();
	delay(20);
	counter = 0;
	*/
}

void loop() //Main Loop
{
#ifdef OLD
	if ((millis() - timer) >= 20) // Main loop runs at 50Hz
	{
		counter++;
		timer_old = timer;
		timer = millis();
		dcmElapsedTime((timer - timer_old);

		// *** DCM algorithm
		// Data adquisition
		// port gedeelte voor ARM

		// haalt alleen adc readings op van acc. meter en plaatst deze in AN[0..2] array
		Read_adc_raw(); // This read gyro data

		Read_Accel(); // Read I2C accelerometer

		/* File = removed
		 if (counter > 5) // Read compass data at 10Hz... (5 loop runs)

		 {
		 counter = 0;
		 Read_Compass(); // Read I2C magnetometer
		 Compass_Heading(); // Calculate magnetic heading
		 }
		 */
		// Calculations...
		Matrix_update();
		Normalize();
		Drift_correction();
		Euler_angles();
		// ***

		printdata();

		/* Cannot be used since variables are not declared extern (yet)
		 static byte gyro_sat = 0;
		 //Turn off the LED when you saturate any of the gyros.
		 if ((abs(Gyro_Vector[0]) >= ToRad(300)) || (abs(Gyro_Vector[1])
		 >=ToRad(300)) || (abs(Gyro_Vector[2]) >= ToRad(300)))
		 {
		 if (gyro_sat < 50)
		 gyro_sat += 10;
		 }
		 else
		 {
		 if (gyro_sat > 0)
		 gyro_sat--;
		 }

		 if (gyro_sat > 0)
		 digitalWrite(STATUS_LED, LOW);
		 else
		 digitalWrite(STATUS_LED, HIGH);
		 */
	}
#endif

}
