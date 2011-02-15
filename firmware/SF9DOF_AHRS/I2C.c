/* ******************************************************* */
/* I2C code for ADXL345 accelerometer                      */
/* and HMC5843 magnetometer                                */
/* ******************************************************* */
//todo: put the init in a #define
static int SENSOR_SIGN[9] =
{ -1, 1, -1, 1, 1, 1, -1, -1, -1 }; //Correct directions x,y,z - gyros, accels, magnetormeter
static int AN[6]; //array that store the 3 ADC filtered data (gyros)
static int AN_OFFSET[6] =
{ 0, 0, 0, 0, 0, 0 }; //Array that stores the Offset of the sensors
static int ACC[3]; //array that store the accelerometers data

int accel_x;
int accel_y;
int accel_z;
int magnetom_x;
int magnetom_y;
int magnetom_z;

void initi2c1(void)
{
	int y;
	for (y = 0; y < 6; y++) // Cumulate values
		AN_OFFSET[y] += AN[y];
}
void initi2c2(void)
{
	int y;
	for (y = 0; y < 6; y++)
		AN_OFFSET[y] = AN_OFFSET[y] / 32;

	AN_OFFSET[5] -= GRAVITY * SENSOR_SIGN[5];

#ifdef NOT_USED_WP
	//Serial.println("Offset:");
	for (y = 0; y < 6; y++)
	Serial.println(AN_OFFSET[y]);
#endif
}

int AccelAddress = 0x53;
int CompassAddress = 0x1E; //0x3C //0x3D;  //(0x42>>1);

void I2C_Init()
{
	Wire.begin();
}

void Accel_Init()
{
	Wire.beginTransmission(AccelAddress);
	Wire.send(0x2D); // power register
	Wire.send(0x08); // measurement mode
	Wire.endTransmission();
	delay(5);
	Wire.beginTransmission(AccelAddress);
	Wire.send(0x31); // Data format register
	Wire.send(0x08); // set to full resolution
	Wire.endTransmission();
	delay(5);
	// Because our main loop runs at 50Hz we adjust the output data rate to 50Hz (25Hz bandwidth)
	Wire.beginTransmission(AccelAddress);
	Wire.send(0x2C); // Rate
	Wire.send(0x09); // set to 50Hz, normal operation
	Wire.endTransmission();
	delay(5);
}

// Reads x,y and z accelerometer registers
void Read_Accel()
{
	int i = 0;
	byte buff[6];

	Wire.beginTransmission(AccelAddress);
	Wire.send(0x32); //sends address to read from
	Wire.endTransmission(); //end transmission

	Wire.beginTransmission(AccelAddress); //start transmission to device
	Wire.requestFrom(AccelAddress, 6); // request 6 bytes from device

	while (Wire.available()) // ((Wire.available())&&(i<6))
	{
		buff[i] = Wire.receive(); // receive one byte
		i++;
	}
	Wire.endTransmission(); //end transmission

	if (i == 6) // All bytes received?
	{
		ACC[1] = (((int) buff[1]) << 8) | buff[0]; // Y axis (internal sensor x axis)
		ACC[0] = (((int) buff[3]) << 8) | buff[2]; // X axis (internal sensor y axis)
		ACC[2] = (((int) buff[5]) << 8) | buff[4]; // Z axis
		AN[3] = ACC[0];
		AN[4] = ACC[1];
		AN[5] = ACC[2];
		accel_x = SENSOR_SIGN[3] * (ACC[0] - AN_OFFSET[3]);
		accel_y = SENSOR_SIGN[4] * (ACC[1] - AN_OFFSET[4]);
		accel_z = SENSOR_SIGN[5] * (ACC[2] - AN_OFFSET[5]);
	}
	else
		Serial.println("!ERR: Acc data");
}

void Compass_Init()
{
	Wire.beginTransmission(CompassAddress);
	Wire.send(0x02);
	Wire.send(0x00); // Set continouos mode (default to 10Hz)
	Wire.endTransmission(); //end transmission
}

void Read_Compass()
{
	int i = 0;
	byte buff[6];

	Wire.beginTransmission(CompassAddress);
	Wire.send(0x03); //sends address to read from
	Wire.endTransmission(); //end transmission

	//Wire.beginTransmission(CompassAddress);
	Wire.requestFrom(CompassAddress, 6); // request 6 bytes from device
	while (Wire.available()) // ((Wire.available())&&(i<6))
	{
		buff[i] = Wire.receive(); // receive one byte
		i++;
	}
	Wire.endTransmission(); //end transmission

	if (i == 6) // All bytes received?
	{
		// MSB byte first, then LSB, X,Y,Z
		magnetom_x = SENSOR_SIGN[6] * ((((int) buff[2]) << 8) | buff[3]); // X axis (internal sensor y axis)
		magnetom_y = SENSOR_SIGN[7] * ((((int) buff[0]) << 8) | buff[1]); // Y axis (internal sensor x axis)
		magnetom_z = SENSOR_SIGN[8] * ((((int) buff[4]) << 8) | buff[5]); // Z axis
	}
	else
		Serial.println("!ERR: Mag data");
}

