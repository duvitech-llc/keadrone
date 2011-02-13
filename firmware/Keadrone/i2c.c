/*****************************************************************************
 *   i2c.c:  I2C C file for NXP LPC17xx Family Microprocessors
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.05.26  ver 1.00    Prelimnary version, first Release
 *
 *****************************************************************************/
#include "LPC17xx.h"
//#include "type.h"
#include "i2c.h"
#include "lpc17xx_clkpwr.h"

typedef enum
{
	I2C_IDLE,
	I2C_ADDRESS_SEND,
	I2C_RESTARTED,
	I2C_REPEATED_START,
	I2C_REG_ADD_SEND,
	DATA_NACK,
	I2C_FAIL,
	I2C_WRITE_SEQ_STARTED,
	I2C_STOP_SEND,
	I2C_READ_SEQ_STARTED
} STATES_I2C;

/* I2STAT register meanings */
typedef enum
{
	//Common
	Mx_START = 0x08,
	Mx_REPEATED_START = 0x10,
	Mx_ARB_LOST = 0x38,
	//TRANSMIT
	MT_ACK_SLAVEADDR = 0x18,
	MT_NACK_SLAVEADDR = 0x20,
	MT_ACK_DATA = 0x28,
	MT_NACK_DATA = 0x30,
	// RECEIVE STATES
	MR_ACK_SLAVEADDR = 0x40,
	MR_NACK_SLAVEADDR = 0x48,
	MR_ACK_DATA = 0x50,
	MR_NACK_DATA = 0x58,
} STATES_MASTER;

volatile STATES_I2C I2CMasterState = I2C_IDLE;

static I2C_DATA *i2c =
{ 0 };

volatile uint32_t I2CCmd;

//volatile uint8_t I2CMasterBuffer[BUFSIZE];
volatile uint32_t I2CCount = 0;
volatile uint32_t I2CReadLength;
volatile uint32_t I2CWriteLength;

volatile uint32_t RdIndex = 0;
volatile uint32_t WrIndex = 0;
LPC_I2C_TypeDef *LPC_I2Cx;

/*****************************************************************************
 ** Function name:		I2C0_IRQHandler
 **
 ** Descriptions:		I2C0 interrupt handler, deal with master mode
 **						only.
 **
 ** parameters:			None
 ** Returned value:		None
 **
 *****************************************************************************/
void I2C1_IRQHandler(void)
{
	// static to reduce stack of the ISR
	static STATES_MASTER StatValue;

	/* this handler deals with master read and master write only */
	StatValue = LPC_I2Cx->I2STAT;

	switch (StatValue)
	{
	case Mx_START: /* A Start condition is issued. */
		LPC_I2Cx->I2DAT = i2c->address;
		LPC_I2Cx->I2CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
		WrIndex = 0; // reset the tx/rx counter
		I2CMasterState = I2C_ADDRESS_SEND;
		break;

	case Mx_REPEATED_START: /* A repeated started is issued */
		switch (I2CMasterState)
		{
		case I2C_REPEATED_START:
			LPC_I2Cx->I2DAT = i2c->address | 1;
			LPC_I2Cx->I2CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
			I2CMasterState = I2C_RESTARTED;
			break;
		default:
			I2CMasterState = I2C_FAIL;
			break;
		}

		break;

	case MT_ACK_SLAVEADDR:
		switch (I2CMasterState)
		{
		case I2C_ADDRESS_SEND:
			LPC_I2Cx->I2DAT = i2c->slaveRegister;
			LPC_I2Cx->I2CONCLR = I2CONCLR_SIC;
			I2CMasterState = I2C_REG_ADD_SEND;
			break;

		default:
			I2CMasterState = I2C_FAIL;
			break;
		}
		break;

	case MT_NACK_DATA:
		I2CMasterState = I2C_FAIL;
		break;
	case MT_ACK_DATA: /* Data byte (or address) has been transmitted, regardless ACK */

		if (i2c->readData == TRUE)
		// this is a read sequence
		{
			LPC_I2Cx->I2CONSET = I2CONSET_STA; /* Set Repeated-start flag */
			LPC_I2Cx->I2CONCLR = I2CONCLR_SIC;
			I2CMasterState = I2C_REPEATED_START;

		}
		else
		// this is a write sequence
		{
			if (WrIndex == i2c->bufLength)
			// buffer end reached
			{
				LPC_I2Cx->I2CONSET = I2CONSET_STO; /* Set Stop flag */
				LPC_I2Cx->I2CONCLR = I2CONCLR_SIC;
				I2CMasterState = I2C_STOP_SEND;
			}
			else
			{
				LPC_I2Cx->I2DAT = i2c->buffer[WrIndex];
				LPC_I2Cx->I2CONCLR = I2CONCLR_SIC;
				I2CMasterState = I2C_WRITE_SEQ_STARTED;
				WrIndex++;
			}

		}

		break;

	case MR_ACK_SLAVEADDR: /* Master Receive, SLA_R has been sent */
		switch (I2CMasterState)
		{
		case I2C_RESTARTED:
			// if we read multiple bytes, we need to assert ACK after each receive
			if ((i2c->bufLength - WrIndex) > 1)
				LPC_I2Cx->I2CONSET = I2CONSET_AA; /* assert ACK after data is received */
			else
				LPC_I2Cx->I2CONCLR = I2CONCLR_AAC;

			LPC_I2Cx->I2CONCLR = I2CONCLR_SIC;
			I2CMasterState = I2C_READ_SEQ_STARTED;
			break;
		default:
			I2CMasterState = I2C_FAIL;
			break;
		}
		break;

	case MR_ACK_DATA: /* Data byte has been received, regardless following ACK or NACK */
		i2c->buffer[WrIndex] = LPC_I2Cx->I2DAT;
		WrIndex++;
		// if we read multiple bytes, we need to assert ACK after each receive
		if ((i2c->bufLength - WrIndex) > 1)
			LPC_I2Cx->I2CONSET = I2CONSET_AA; /* assert ACK after data is received */
		else
			LPC_I2Cx->I2CONCLR = I2CONCLR_AAC;

		LPC_I2Cx->I2CONCLR = I2CONCLR_SIC;
		break;

	case MR_NACK_DATA:
		i2c->buffer[WrIndex] = LPC_I2Cx->I2DAT;
		WrIndex++;
		if ((i2c->bufLength - WrIndex) == 0)
		{
			I2CMasterState = I2C_STOP_SEND;
			LPC_I2Cx->I2CONSET = I2CONSET_STO; /* Set Stop flag */
			LPC_I2Cx->I2CONCLR = I2CONCLR_SIC;
		}
		else
			I2CMasterState = I2C_FAIL;

		break;

	case MT_NACK_SLAVEADDR: /* regardless, it's a NACK */
	case Mx_ARB_LOST:
	case MR_NACK_SLAVEADDR:
	default:
		I2CMasterState = I2C_FAIL;
		break;
	}

	if (I2CMasterState == I2C_FAIL)
	{
		LPC_I2Cx->I2CONSET = I2CONSET_STO; /* Set Stop flag */
		LPC_I2Cx->I2CONCLR = I2CONCLR_SIC;
	}
}

/*****************************************************************************
 ** Function name:		I2CStart
 **
 ** Descriptions:		Create I2C start condition, a timeout
 **				value is set if the I2C never gets started,
 **				and timed out. It's a fatal error.
 **
 ** parameters:			None
 ** Returned value:		true or false, return false if timed out
 **
 *****************************************************************************/
uint32_t I2CStart(void)
{
	uint32_t timeout = 0;
	uint32_t retVal = FALSE;

	/*--- Issue a start condition ---*/
	LPC_I2Cx->I2CONSET = I2CONSET_STA; /* Set Start flag */

	/*--- Wait until START transmitted ---*/
	while (1)
	{
		return TRUE;
	}
	return (retVal);
}

/*****************************************************************************
 ** Function name:		I2CStop
 **
 ** Descriptions:		Set the I2C stop condition, if the routine
 **				never exit, it's a fatal bus error.
 **
 ** parameters:			None
 ** Returned value:		true or never return
 **
 *****************************************************************************/
uint32_t I2CStop(void)
{
	LPC_I2Cx->I2CONSET = I2CONSET_STO; /* Set Stop flag */
	LPC_I2Cx->I2CONCLR = I2CONCLR_SIC; /* Clear SI flag */

	/*--- Wait for STOP detected ---*/
	while (LPC_I2Cx->I2CONSET & I2CONSET_STO)
		;
	return TRUE;
}

/*****************************************************************************
 ** Function name:		I2CInit
 **
 ** Descriptions:		Initialize I2C controller
 **
 ** parameters:			I2c mode is either MASTER or SLAVE
 ** Returned value:		true or false, return false if the I2C
 **				interrupt handler was not installed correctly
 **
 *****************************************************************************/
uint32_t I2CInit(void)
{
#ifdef I2C0
	LPC_SC->PCONP |= (1 << 19);

	/* set PIO0.27 and PIO0.28 to I2C0 SDA and SCK */
	/* function to 01 on both SDA and SCK. */
	LPC_PINCON->PINSEL1 &= ~0x03C00000;
	LPC_PINCON->PINSEL1 |= 0x01400000;

#endif

	LPC_PINCON->PINMODE0 |= 0x2 << 22; // pin has neither pull-up nor pull-down.
	// I2C1 setup (since 2 is not available??)
	CLKPWR_ConfigPPWR(CLKPWR_PCONP_PCI2C1, ENABLE);
	/* 00 PCLK_peripheral = CCLK/4
	 * 01 PCLK_peripheral = CCLK
	 * 10 PCLK_peripheral = CCLK/2
	 * 11 PCLK_peripheral = CCLK/8
	 */
	uint16_t value = 0b01;
	LPC_SC->PCLKSEL0 |= value << 20; // I2C1

	// ports used for I2C1 = p0.0 / SDA1 & p0.1 / SCL1
	LPC_PINCON->PINMODE_OD0 |= 1 << 0; // make them open drain
	LPC_PINCON->PINMODE_OD0 |= 1 << 1;// make them open drain

	LPC_PINCON->PINSEL0 |= 3 << 0; // select I2C function
	LPC_PINCON->PINSEL0 |= 3 << 2; // select I2C function
	LPC_PINCON->PINMODE0 |= 0x2 << 0; // pin has neither pull-up nor pull-down.
	LPC_PINCON->PINMODE0 |= 0x2 << 2; // pin has neither pull-up nor pull-down.

	LPC_I2Cx = LPC_I2C1;
	/*--- Clear flags ---*/
	LPC_I2Cx->I2CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC
			| I2CONCLR_I2ENC;

	/*--- Reset registers ---*/
	LPC_I2Cx->I2SCLL = I2SCLL_SCLL;
	LPC_I2Cx->I2SCLH = I2SCLH_SCLH;

	/* Install interrupt handler */
	//todo: choose I2Cx
	NVIC_EnableIRQ(I2C1_IRQn);

	LPC_I2Cx->I2CONSET = I2CONSET_I2EN;
	return (TRUE);
}

/*****************************************************************************
 ** Function name:		I2CEngine
 **
 ** Descriptions:		The routine to complete a I2C transaction
 **				from start to stop. All the intermitten
 **				steps are handled in the interrupt handler.
 **				Before this routine is called, the read
 **				length, write length, I2C master buffer,
 **				and I2C command fields need to be filled.
 **				see i2cmst.c for more details.
 **
 ** parameters:			None
 ** Returned value:		true or false, return false only if the
 **				start condition can never be generated and
 **				timed out.
 **
 *****************************************************************************/
uint32_t I2CEnginePolling(I2C_DATA *p)
{
	I2CMasterState = I2C_IDLE;
	i2c = p;

	if (I2CStart() != TRUE)
	{
		I2CStop();
		return (FALSE);
	}

	while (1)
	{
		if (I2CMasterState == I2C_STOP_SEND || I2CMasterState == I2C_FAIL)
		{
			if (I2CMasterState == I2C_FAIL)
				return FALSE;
			else
				return TRUE;
			//I2CStop();
			break;
		}
	}
	return (TRUE);
}
Bool I2CgetMasterState_FromISR(void)
{
	if (I2CMasterState == I2C_STOP_SEND || I2CMasterState == I2C_FAIL)
		return FALSE;
	else
		return TRUE;
}

uint32_t I2CEngine_FromISR(I2C_DATA *p)
{
	I2CMasterState = I2C_IDLE;
	i2c = p;
	/*--- Issue a start condition ---*/
	LPC_I2Cx->I2CONSET = I2CONSET_STA; /* Set Start flag */
}

/******************************************************************************
 **                            End Of File
 ******************************************************************************/

