
/*****************************************************************************
**																			**
**	 Name: 	post_main.c															**
**																			**
******************************************************************************

(C) Copyright 2006 - Analog Devices, Inc.  All rights reserved.

Project Name:	21369/21375 POST ATE

Date Modified:	11 May 2006

Software:		VisualDSP++ 4.5

Hardware:		ADSP-21369/21375 EZ-KIT Lite

Connections:

Purpose:		Purpose:		Perform a POST ATE on the 21369/21375 EZ-Kit Lite


21369 EZ-KIT board test indicators 	(version 1.00.0 11 May 2006)


Enter 21369/21375 EZ-KIT POST test by pressing PB4.  All other LEDs not shown below do
not matter and may be on or off.

				LED1	LED2	LED3	LED4
VERSION			1   	0   	0   	0
SRAM			0		1		0		0
SDRAM			1		1		0		0
UART			0		0		1		0	SW6 All on
AM29LV081B		1		0		1		0
AT25F2048		0		1		1		0
ANALOG_AUDIO	1		1		1		0	SW14 All on
DIGITAL_AUDIO	0		0		0		1




The switch settings for the built in self test differ slightly from the default settings shipped.
See the users manual for default switch settings. Note that the boot mode switch (SW2.1 and SW2.2)
selects which flash device is loaded at power up and reset.
Switch settings:
	- SW1  		All OFF
	- SW2  		1 = OFF, 2 = ON,  3 = ON,  4 = ON	(serial flash)
	- SW2  		1 = ON,  2 = OFF, 3 = ON,  4 = ON	(parallel flash)
	- SW3 		1 = OFF, 2 = ON,  3 = ON,  4 = ON
	- SW4 		All OFF
	- SW5 		1 = OFF, 2 = ON,  3 = OFF, 4 = ON
	- SW6 		All ON
	- SW7 		All ON
	- SW13  	1 = ON,  2 = ON, 3 = OFF,  4 = OFF,  5 = OFF,  6 = OFF
	- SW14 		All ON

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Enter USB EZ-Extender POST test by pressing PB3 along with PB4

Test indicators are as follows:

			LED1	LED2	LED3	LED4
USB			1		0		0		1

The switch settings for the built in self test for the USB EZ-Extender differ slightly from the default
settings shipped.  The "*" indicates the only setting that is different from the normal
POST settings above.  See the users manual for default switch settings.
Switch settings:
	- SW1  		All OFF
	- SW2  		1 = OFF, 2 = ON,  3 = ON,  4 = ON	(serial flash)
	- SW2  		1 = ON,  2 = OFF, 3 = ON,  4 = ON	(parallel flash)
	- SW3 		1 = OFF, 2 = ON,  3 = ON,  4 = ON
	- SW4 		All OFF
	- SW5 		1 = OFF, 2 = ON,  3 = OFF, 4 = ON
	- SW6 		All ON
	- *SW7 		1 = OFF, 2 = OFF, 3 = ON,  4 = ON
	- SW13  	1 = ON,  2 = ON, 3 = OFF,  4 = OFF, 5 = OFF, 6 = OFF
	- SW14 		All ON

Settings for the USB EZ-Extender:
	- JP1:	installed



*****************************************************************************/

#ifdef __ADSP21371__
	#include <Cdef21371.h>
	#include <def21371.h>
#elif __ADSP21375__
	#include <Cdef21375.h>
	#include <def21375.h>
#elif __ADSP21369__
	#include <Cdef21369.h>
	#include <def21369.h>
#endif

#include <sysreg.h>
#include <string.h>

#include "post_common.h"
/* Managed drivers and/or services include */
#include "../system/adi_initialize.h"

//////////////////////////////////////////////////////////////////////////////
//
// variables
//
//////////////////////////////////////////////////////////////////////////////
#ifdef __ADSP21371__
	char g_szVersion[64] = "1.30.1";
#elif __ADSP21375__
	char g_szVersion[64] = "1.30.1";
#elif __ADSP21369__
	char g_szVersion[64] = "1.30.0";
#endif

char g_szBuildDate[64];
char g_szBuildTime[64];
int g_loops = 0;



//////////////////////////////////////////////////////////////////////////////
//
// function prototypes
//
//////////////////////////////////////////////////////////////////////////////
int TEST_VERSION(void);
int TEST_SRAM(void);
int TEST_PARALLEL_FLASH(void);
int TEST_SPI_FLASH(void);
int TEST_SDRAM(void);
int TEST_UART(void);
int TEST_ANALOG_AUDIO(void);
int TEST_DIGITAL_AUDIO(void);
int TEST_DUMMY(void) {return 1;} // place holder test




typedef enum TEST_INDEX_tag{
	VERSION,		//0001
	SRAM,			//0010
	SDRAM,			//0011
	UART,			//0100
	AM29LV081B,		//0101
	AT25F2048,		//0110
	ANALOG_AUDIO,	//0111
	DIGITAL_AUDIO,	//1000
	USB,			//1001
	DUMMY1,			//1010
	NUM_TESTS
}enTEST_INDEX;


typedef int (*pfnTests)(void);

typedef struct stTestParams_TAG
{
	enTESTS m_nTest;
	enTEST_STATES m_nTestState;
	pfnTests m_pTestFunctions;

}stTestParamaters;

#if defined (__ADSP21371__) || defined (__ADSP21375__)
stTestParamaters g_Tests[NUM_TESTS] = {
	{TEST_1, TEST_1_SET, TEST_VERSION},
 	{TEST_2, TEST_2_SET, TEST_DUMMY},
 	{TEST_3, TEST_3_SET, TEST_SDRAM},
 	{TEST_4, TEST_4_SET, TEST_UART},
 	{TEST_5, TEST_5_SET, TEST_PARALLEL_FLASH},
 	{TEST_6, TEST_6_SET, TEST_SPI_FLASH},
 	{TEST_7, TEST_7_SET, TEST_ANALOG_AUDIO},
 	{TEST_8, TEST_8_SET, TEST_DUMMY},
	{TEST_9, TEST_9_SET, TEST_DUMMY},
 	{TEST_10,TEST_10_SET,TEST_DUMMY}   };
#elif __ADSP21369__
stTestParamaters g_Tests[NUM_TESTS] = {
	{TEST_1, TEST_1_SET, TEST_VERSION},
 	{TEST_2, TEST_2_SET, TEST_SRAM},
 	{TEST_3, TEST_3_SET, TEST_SDRAM},
 	{TEST_4, TEST_4_SET, TEST_UART},
 	{TEST_5, TEST_5_SET, TEST_PARALLEL_FLASH},
 	{TEST_6, TEST_6_SET, TEST_SPI_FLASH},
 	{TEST_7, TEST_7_SET, TEST_ANALOG_AUDIO},
 	{TEST_8, TEST_8_SET, TEST_DIGITAL_AUDIO},
	{TEST_9, TEST_9_SET, TEST_DUMMY},
 	{TEST_10,TEST_10_SET,TEST_DUMMY}   };
#endif

//////////////////////////////////////////////////////////////////////////////
// void PerformTest( const stTestParamaters Test )
//
// Purpose:  Wrap up the test paramaters and perform the test
//////////////////////////////////////////////////////////////////////////////
void PerformTest( const stTestParamaters Test )
{
	Delay(BLINK_FAST * 10);
	ClearSet_LED_Bank( (-1), 0x0000);
	ClearSet_LED_Bank( Test.m_nTest, Test.m_nTestState); // change the state of the led
	if( 0 == Test.m_pTestFunctions() )
	{	// test failed
		Blink_LED( Test.m_nTest, BLINK_FAST );
	}
}

//////////////////////////////////////////////////////////////////////////////
//
// Main program
//
//////////////////////////////////////////////////////////////////////////////
int main(void)
{
	int iReturn = 0;

	int bPassed = 0;
	enLED enLEDs;
	int nDelay;

	/* Initialize managed drivers and/or services */
	adi_initComponents();

	strcpy(g_szBuildDate, __DATE__);
	strcpy(g_szBuildTime, __TIME__);

	InitPLL();

	Setup_leds();

	int nPB3_4 = 0;

	//Pin Assignments in SRU_PIN3 (Group D)
    SRU(LOW,DAI_PB19_I);	//assign pin buffer 19 low so it is an input
    SRU(LOW,DAI_PB20_I);    //assign pin buffer 20 low so it is an input

    nPB3_4 = ((*pDAI_PIN_STAT & 0x000C0000) >> 18);


     // if the test mode button SW9 was depressed at boot time,
	// then the test mode will be entered, otherwise the
	// led bar will display continuously.
	if( 0x02 == (nPB3_4 & 0x02) ) // At least PB4 was pressed.
	{
		//
		// MAKE SURE THIS TEST IS FOR THIS VERSION OF SILICON
		//
		PerformTest( g_Tests[VERSION] );



		// check to see if PB3 was also pressed.
		if( 0x01 == (nPB3_4 & 0x01) ) // PB3 was pressed, do the USB tests
		{
			// This thest the Analog Devices ADDS-SharcUSB-EZEXT extender board.
			// It uses the loopback example located in EZ-Kits\USB EZ-EXTENDER folder
			// by creating a library around the loopback functions
			//
			// USB (loopback) test
			//
			PerformTest( g_Tests[USB] );

		}
		else
		{
			ClearSet_LED_Bank( (-1), 0x0000);

			// do the push button & LED test first
			// if this fails, there is not much point
			// in going on as there is no way to get
			// feedback to the user about which test
			// failed.
			TEST_LED_and_PB();
		}


		// WAIT A BIT, THEN RESET THE LED's
		Delay(BLINK_FAST * 100);

		while(1)
		{
			InitPLL();

			int nTestIndex;
			for( nTestIndex = 0; nTestIndex < USB; nTestIndex++ )
			{
				PerformTest( g_Tests[nTestIndex] );
			}

			g_loops++;

			// WAIT A BIT, THEN RESET THE LED's
			Delay(BLINK_SLOW * 25);

			// indicate everything passed
			ClearSet_LED_Bank( (-1), 0x0000);
//			for( nDelay = 0; nDelay < g_loops; nDelay++)
//			{
				LED_BAR( BLINK_SLOW);
//			}
		}
	}


	// this is just normal blink mode.
	while(1)
	{
		LED_BAR(BLINK_SLOW);
	}
}



