/*
 * dAccelerometer_BMA180.c
 *
 *  Created on: 13 feb 2011
 *      Author: Willem Pietersz
 */

#include "LPC17xx.h"
#include "lpc17xx_ssp.h"
#include "dAccelerometer_BMA180.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

#define SSP_CHANNEL LPC_SSP0
#define PORT_CS	LPC_GPIO0
#define PIN_MASK_CS (1<<16)

static volatile int ISR_FillsBuffer = 0;
static volatile sAcc_data acc[2];

#define delay_1ms			( 1 / portTICK_RATE_MS )
static void delay_ms(uint16_t value)
{
	portTickType xNextWakeTime;
	vTaskDelayUntil(&xNextWakeTime, (delay_1ms * value));

}

static char bmaRead(uint8_t address)
{
	//returns the contents of any 1 byte register from any address
	//sets the MSB for every address byte (READ mode)

	char rxBuff;
	SSP_DATA_SETUP_Type sspDataConfig;

	PORT_CS->FIOCLR |= PIN_MASK_CS; //CS low

	address |= 0x80;
	sspDataConfig.length = 1;
	sspDataConfig.tx_data = &address;
	sspDataConfig.rx_data = &rxBuff;

	SSP_ReadWrite(SSP_CHANNEL, &sspDataConfig, SSP_TRANSFER_POLLING);
	address = 0x55;//dummy write
	SSP_ReadWrite(SSP_CHANNEL, &sspDataConfig, SSP_TRANSFER_POLLING);

	PORT_CS->FIOSET |= PIN_MASK_CS; //CS High

	return rxBuff;
}

static void bmaReadMultiple_FromISR(uint8_t address, uint8_t *data, uint8_t len)
{
	//returns the contents of any 1 byte register from any address
	//sets the MSB for every address byte (READ mode)

	static char rxBuff[9];
	static char txBuff[9];

	SSP_DATA_SETUP_Type sspDataConfig;

	PORT_CS->FIOCLR |= PIN_MASK_CS; //CS low
	address |= 0x80;

	txBuff[0] = address;

	sspDataConfig.length = 8;
	sspDataConfig.tx_data = &address;
	sspDataConfig.rx_data = &rxBuff;
	/* todo: replace rx data for data directly, saves time */
	SSP_ReadWriteBMA180(SSP_CHANNEL, &sspDataConfig);//, SSP_TRANSFER_POLLING);
	static uint8_t counter;
	for (counter = 0; counter < len; counter++)
	{
		*(data + counter) = rxBuff[counter + 1];
	}

	PORT_CS->FIOSET |= PIN_MASK_CS; //CS High
}

static void bmaWrite(uint8_t address, char data)
{
	//write any data byte to any single address
	//adds a 0 to the MSB of the address byte (WRITE mode)

	address &= 0x7F;
	char rxBuff;
	SSP_DATA_SETUP_Type sspDataConfig;

	PORT_CS->FIOCLR |= PIN_MASK_CS; //CS low

	delay_ms(1);

	sspDataConfig.tx_data = &address;
	sspDataConfig.rx_data = &rxBuff;
	SSP_ReadWrite(SSP_CHANNEL, &sspDataConfig, SSP_TRANSFER_POLLING);

	delay_ms(1);

	sspDataConfig.tx_data = &data;
	sspDataConfig.rx_data = &rxBuff;
	SSP_ReadWrite(SSP_CHANNEL, &sspDataConfig, SSP_TRANSFER_POLLING);

	delay_ms(1);

	PORT_CS->FIOSET |= PIN_MASK_CS; //CS High
}

/* init_BMA180
 *  Input: range is a 3-bit value between 0x00 and 0x06 will set the
 *  range as described in the BMA180 datasheet (pg. 27)
 *  bw is a 4-bit value between 0x00 and 0x09.  Again described on pg. 27
 *  Output: -1 on error, 0 on success*/
static Bool bmaInit(uint8_t range, uint8_t bw)
{
	char temp, temp1;

	// if connected correctly, ID register should be 3
	if (bmaRead(ID) != 3)
		return FALSE;

	//-------------------------------------------------------------------------------------
	// Set ee_w bit
	temp = bmaRead(CTRLREG0);
	temp |= 0x10;
	bmaWrite(CTRLREG0, temp); // Have to set ee_w to write any other registers
	//-------------------------------------------------------------------------------------
	// Set mode
	temp = bmaRead(tco_z);
	temp &= ~3; // mask/clear mode setting
	temp |= 1; // enable high sens & ultra low noise mode
	bmaWrite(tco_z, temp); //Write new range data, keep other bits the same

	//-------------------------------------------------------------------------------------


	// Set BW
	temp = bmaRead(BWTCS);
	temp1 = bw;
	temp1 = temp1 << 4;
	temp &= (~BWMASK);
	temp |= temp1;
	bmaWrite(BWTCS, temp); // Keep tcs<3:0> in BWTCS, but write new BW
	//-------------------------------------------------------------------------------------
	// Set Range
	temp = bmaRead(OLSB1);
	temp1 = range;
	temp1 = (temp1 << RANGESHIFT);
	temp &= (~RANGEMASK);
	temp |= temp1;
	temp |= 1; // enable sample skipping (samp_skip)
	bmaWrite(OLSB1, temp); //Write new range data, keep other bits the same
	temp = bmaRead(OLSB1);

	//-------------------------------------------------------------------------------------

	// Set interrupt
	temp = bmaRead(CTRLREG3);
	temp |= (1 << NEW_DATA_INT_SHIFT);
	bmaWrite(CTRLREG3, temp); //Enable interrupt on new data only
	//-------------------------------------------------------------------------------------

	return TRUE;
}

static void gpioIntInit(void)
{
	//Enable rising edge interrupt for P0.8
	LPC_GPIOINT->IO0IntEnR |= 1 << 8;//| 1<<9;
	NVIC_SetPriority(EINT3_IRQn, 30);
	NVIC_EnableIRQ(EINT3_IRQn);
}
#define HISTORY_IIR 2

typedef struct
{
	int32_t IIR_Sum;
	int32_t currReading;
} IIR_DATA;
uint16_t IIR_Average(IIR_DATA *p)
{
	// on boot-up, our value is never 0xFFFFFFFF
	if (p->IIR_Sum == 0xFFFFFFFF)
		p->IIR_Sum = (int32_t) p->currReading * HISTORY_IIR;

	p->IIR_Sum -= (int32_t)(p->IIR_Sum / HISTORY_IIR);
	p->IIR_Sum += p->currReading;

	// note, this filter has an gain of HISTORY
	// therefore we must divide the average value
	// with HISTORY to get our current average
	return (uint16_t)(p->IIR_Sum / HISTORY_IIR);
}
static IIR_DATA x;
static IIR_DATA y;
static IIR_DATA z;

void spiReqNewData_FromISR(void)
{

	static uint8_t regData[7];

	// get 7 bytes starting from reg. ACCXLSB
	bmaReadMultiple_FromISR(ACCXLSB, &regData[0], 7);

	acc[ISR_FillsBuffer].X = regData[0];
	acc[ISR_FillsBuffer].X |= (uint16_t) regData[1] << 8;
	x.currReading = acc[ISR_FillsBuffer].X >> 2; // Get rid of two non-value bits in LSB
	acc[ISR_FillsBuffer].X = IIR_Average(&x);

	acc[ISR_FillsBuffer].Y = regData[2];
	acc[ISR_FillsBuffer].Y |= (uint16_t) regData[3] << 8;
	y.currReading = acc[ISR_FillsBuffer].Y >> 2; // Get rid of two non-value bits in LSB
	acc[ISR_FillsBuffer].Y = IIR_Average(&y);

	acc[ISR_FillsBuffer].Z = regData[4];
	acc[ISR_FillsBuffer].Z |= (uint16_t) regData[5] << 8;
	z.currReading = acc[ISR_FillsBuffer].Z >> 2; // Get rid of two non-value bits in LSB
	acc[ISR_FillsBuffer].Z = IIR_Average(&z) + 4096;

	acc[ISR_FillsBuffer].temp = (uint8_t) regData[6];

	/* Switch buffer to latest new data */
	if (ISR_FillsBuffer)
		ISR_FillsBuffer = 0;
	else
		ISR_FillsBuffer = 1;
}

void spiGetAccelero(sAcc_data *p)
{
	/* note that the acc. meter gives data at 2.66kHz rate, and gyro at 2kHz.
	 * Thats why we use dual buffer for mutual exclusion.
	 */

	int currBuffer;
	// Get the buffer that is currently filled by the interrupt
	currBuffer = ISR_FillsBuffer;

	/* Switch buffer to latest new data that is not being filled (toggle)*/
	if (currBuffer)
		currBuffer = 0;
	else
		currBuffer = 1;

	// copy current buffer to pointer.
	*p = acc[currBuffer];

}

void spiInit(void)
{
	SSP_CFG_Type sspChannelConfig;
	SSP_DATA_SETUP_Type sspDataConfig;

	x.IIR_Sum = y.IIR_Sum = z.IIR_Sum = 0xFFFFFFFF;

	LPC_PINCON->PINSEL0 |= 0x2 << 30; //SCK0 p0.15
	LPC_PINCON->PINSEL1 |= 0x2 << 4; //MOSI0 p0.18
	LPC_PINCON->PINSEL1 |= 0x2 << 2; //MISO0 p0.17

	LPC_PINCON->PINMODE1 |= 0x3 << 4;// pull down mosi pin.
	LPC_PINCON->PINMODE1 |= 0x2 << 2;// pull down mosi pin.

	PORT_CS->FIODIR |= 1 << 16; //P0.16 as CSn (acc. meter)

	// init BMA interrupt pin
	// POR VALUE IS ALREADY INPUT

	SSP_ConfigStructInit(&sspChannelConfig);
	SSP_Init(SSP_CHANNEL, &sspChannelConfig);
	SSP_Cmd(SSP_CHANNEL, ENABLE);
	while (bmaInit(0x02, 0) == FALSE)
	{
		printf("BMA180: Error communication\n");
		delay_ms(5);
	}
	printf("BMA180: Connected\n");

	gpioIntInit();
	spiReqNewData_FromISR();
}
