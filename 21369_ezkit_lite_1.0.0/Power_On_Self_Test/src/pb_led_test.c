////////////////////////////////////////////////////////////////////////////
//
//  Program to check the functionality of core accesses to 32-bit SRAM
//    device
//

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

#include <sru.h>
#include <signal.h>
#include <stdio.h>
#include <sysreg.h>
#include <services\int\adi_int.h>

#include "post_common.h"
#include "pb_led_test.h"


//////////////////////////////////////////////////////////////////////////////
//
// globals
//
//////////////////////////////////////////////////////////////////////////////
int g_iEnterTestMode = 0;



//////////////////////////////////////////////////////////////////////////////
//
// Function Prototypes
//
//////////////////////////////////////////////////////////////////////////////
int TEST_LED_and_PB(void);
void ClearSet_LED(const enLED led, const int bState);
void IRQ0_ISR(int sig, void *pCBParam);
void IRQ1_ISR(int sig, void *pCBParam);
void DAI_ISR(int sig, void *pCBParam);

void Cleanup_SRU_INTS(void);


//////////////////////////////////////////////////////////////////////////////
//
// Stand alone test jig
//
//////////////////////////////////////////////////////////////////////////////
#ifdef _STANDALONE_ // use this to run standalone tests
int main(void)
{
	int bPassed = 0;
	int n;

	Setup_leds();

	while(1)
	{
		bPassed = TEST_LED_and_PB();
	}

	return 0;
}
#endif //#ifdef _STANDALONE_


//////////////////////////////////////////////////////////////////////////////
// int TEST_LED_and_PB(void)
//
// PURPOSE:		Test the push buttons and LED's
//
//////////////////////////////////////////////////////////////////////////////
int TEST_LED_and_PB(void)
{
	int iReturn = 0;

    Setup_Ints();

    *pSYSCTL &= (~MSEN);


	// for each push button, illuminate two LED's
	// when all PB's have been pressed, test will return
	// with a pass or fail, or it will get stuck here.
	do{
	    asm("nop;");
	}while( 0x0F != g_iEnterTestMode );

	disable_interrupts();
	iReturn = 1;

	Cleanup_SRU_INTS();

 	*pSYSCTL |= MSEN;


	return iReturn;
}


//////////////////////////////////////////////////////////////////////////////
// void LED_BAR(const int iSpeed)
//
// PURPOSE:		Display a blinking LED bar
//
// INPUT:		iSpeed - the speed of the blink
//////////////////////////////////////////////////////////////////////////////
void LED_BAR(const int iSpeed)
{
	enLED n;
	int j;

    // Note that MS2 & MS3 pin multiplexed with flag2 & flag3.
    // MSEN bit must be enabled to access SDRAM, but LED8 cannot be driven with sdram
    // Note that MS2 & MS3 pin multiplexed with flag2 & flag3
    // Programming the mutliplexed pin as MS2
    *pSYSCTL &= (~MSEN);

	for( n = LED1; n < LAST_LED; (n <<= 1) )
	{
		ClearSet_LED(n, 3);
		Delay(iSpeed);
	}


    // Note that MS2 & MS3 pin multiplexed with flag2 & flag3.
    // MSEN bit must be enabled to access SDRAM, but LED8 cannot be driven with sdram
    // Note that MS2 & MS3 pin multiplexed with flag2 & flag3
    // Programming the mutliplexed pin as MS2
    *pSYSCTL |= MSEN;

}


//////////////////////////////////////////////////////////////////////////////
// void Blink_LED(const int enleds, const int iSpeed)
//
// PURPOSE:		Blink various LED's
//
// INPUT:		iSpeed - the speed of the blink
//				enleds - the LED to actually blink
//////////////////////////////////////////////////////////////////////////////
void Blink_LED(const int enleds, const int iSpeed)
{
	int j;
	enLED n;

	while( 1 )
	{
		for( n = LED1; n < LAST_LED; (n <<= 1) )
		{
			if( n & enleds )
			{
				ClearSet_LED(n, 3);
			}
		}

		Delay(iSpeed);
	}
}


//////////////////////////////////////////////////////////////////////////////
// void ClearSet_LED_Bank(const int enleds, const int iState)
//
// PURPOSE:		Clear or set a particular LED or group of LED's
//
// INPUT:		iState - the state ON, OFF, TGL
//				enleds - the LED(s) to actually blink
//////////////////////////////////////////////////////////////////////////////
void ClearSet_LED_Bank(const int enleds, const int iState)
{
	enLED n;
	int nTempState = iState;


	for( n = LED1; n < LAST_LED; (n <<= 1) )
	{
		if( n & enleds )
		{
			ClearSet_LED(n, (nTempState & 0x3) );
		}

		nTempState >>= 2;
	}
}


//////////////////////////////////////////////////////////////////////////////
// void ClearSet_LED(const enLED led, const int bState)
//
// PURPOSE:		Clear or set a particular LED (NOT A GROUP)
//
// INPUT:		iState - the state ON, OFF, TGL
//				enleds - the LED(s) to actually blink
//////////////////////////////////////////////////////////////////////////////
void ClearSet_LED(const enLED led, const int bState)
{
	switch( led )
	{
		case LED6:
		{
			if( 0 == bState )
			{
				SRU(LOW,DAI_PB15_I);
			}
			else if( 1 == bState )
			{
				SRU(HIGH,DAI_PB15_I);
			}
			else // toggle the bits
			{
				(*pSRU_PIN3) ^= 0x00004000;
			}
		}
		break;

		case LED7:
		{
			if( 0 == bState )
			{
				SRU(LOW,DAI_PB16_I);
			}
			else if( 1 == bState )
			{
				SRU(HIGH,DAI_PB16_I);
			}
			else // toggle the bits
			{
				(*pSRU_PIN3) ^= 0x00200000;
			}
		}
		break;

		case LED1:
		{
			if( 0 == bState )
			{
				sysreg_bit_clr(sysreg_FLAGS, FLG4);
			}
			else if( 1 == bState )
			{
				sysreg_bit_set(sysreg_FLAGS, FLG4);
			}
			else // toggle the bits
			{
				sysreg_bit_tgl(sysreg_FLAGS, FLG4);
			}
		}
		break;

		case LED2:
		{
			if( 0 == bState )
			{
				sysreg_bit_clr(sysreg_FLAGS, FLG5);
			}
			else if( 1 == bState )
			{
				sysreg_bit_set(sysreg_FLAGS, FLG5);
			}
			else // toggle the bits
			{
				sysreg_bit_tgl(sysreg_FLAGS, FLG5);
			}
		}
		break;

		case LED3:
		{
			if( 0 == bState )
			{
				sysreg_bit_clr(sysreg_FLAGS, FLG6);
			}
			else if( 1 == bState )
			{
				sysreg_bit_set(sysreg_FLAGS, FLG6);
			}
			else // toggle the bits
			{
				sysreg_bit_tgl(sysreg_FLAGS, FLG6);
			}
		}
		break;

		case LED4:
		{
			if( 0 == bState )
			{
				sysreg_bit_clr(sysreg_FLAGS, FLG7);
			}
			else if( 1 == bState )
			{
				sysreg_bit_set(sysreg_FLAGS, FLG7);
			}
			else // toggle the bits
			{
				sysreg_bit_tgl(sysreg_FLAGS, FLG7);
			}
		}
		break;

		case LED5:
		{
			if( 0 == bState )
			{
				sysreg_bit_clr(sysreg_FLAGS, FLG8);
			}
			else if( 1 == bState )
			{
				sysreg_bit_set(sysreg_FLAGS, FLG8);
			}
			else // toggle the bits
			{
				sysreg_bit_tgl(sysreg_FLAGS, FLG8);
			}
		}
		break;

		case LED8:
		{
			if( 0 == bState )
			{
				sysreg_bit_clr(sysreg_FLAGS, FLG3);
			}
			else if( 1 == bState )
			{
				sysreg_bit_set(sysreg_FLAGS, FLG3);
			}
			else // toggle the bits
			{
				sysreg_bit_tgl(sysreg_FLAGS, FLG3);
			}
		}
		break;
	}// end switch

}


//////////////////////////////////////////////////////////////////////////////
// void Setup_Ints(void)
//
// PURPOSE:		Configure the system to accept the push buttons as inputs
//
//////////////////////////////////////////////////////////////////////////////
void Setup_Ints(void)
{
	Config_SRU_INTS();


    (*pDAI_IRPTL_PRI) = (SRU_EXTMISCA1_INT  | SRU_EXTMISCA2_INT);    //unmask individual interrupts
    (*pDAI_IRPTL_RE) = (SRU_EXTMISCA1_INT  | SRU_EXTMISCA2_INT);    //make sure interrupts latch on the rising edge

	//Set up interrupt priorities
    sysreg_bit_set(sysreg_IMASK, DAIHI); //make DAI interrupts high priority

    (*pSYSCTL) |= (IRQ0EN|IRQ1EN);

    Delay(100);

    sysreg_bit_set(sysreg_MODE2, (IRQ0E|IRQ1E) );
    sysreg_bit_clr(sysreg_IRPTL, (IRQ1I|IRQ0I) );
    sysreg_bit_set(sysreg_IMASK, (IRQ1I|IRQ0I) ); //enable IRQ interrupts
    enable_interrupts();

    if(adi_int_InstallHandler(ADI_CID_P0I,(ADI_INT_HANDLER_PTR)DAI_ISR, NULL, true) != ADI_INT_SUCCESS)
       	return;
    if(adi_int_InstallHandler(ADI_CID_IRQ0I,(ADI_INT_HANDLER_PTR)IRQ0_ISR, NULL, true) != ADI_INT_SUCCESS)
       	return;
    if(adi_int_InstallHandler(ADI_CID_IRQ1I,(ADI_INT_HANDLER_PTR)IRQ1_ISR, NULL, true) != ADI_INT_SUCCESS)
       	return;

}

//////////////////////////////////////////////////////////////////////////////
// void Cleanup_SRU_INTS(void)
//
// PURPOSE: 	Re Configure the interrupts after LED test
//
// INPUT:	none
// RETURNS: none
//
//////////////////////////////////////////////////////////////////////////////
void Cleanup_SRU_INTS(void)
{
    adi_int_EnableInt(ADI_CID_P0I, false);
    adi_int_UninstallHandler(ADI_CID_P0I);
    adi_int_EnableInt(ADI_CID_IRQ0I, false);
    adi_int_UninstallHandler(ADI_CID_IRQ0I);
    adi_int_EnableInt(ADI_CID_IRQ1I, false);
    adi_int_UninstallHandler(ADI_CID_IRQ1I);
}


//////////////////////////////////////////////////////////////////////////////
// void Setup_leds(void)
//
// PURPOSE:		Setup LED's to be outputs
//
//////////////////////////////////////////////////////////////////////////////
void Setup_leds(void)
{
	Config_SRU_LEDS();

    sysreg_bit_set(sysreg_FLAGS, (FLG3O|FLG4O|FLG5O|FLG6O|FLG7O|FLG8O) ); 	//Setting flag pins as outputs
	sysreg_bit_clr(sysreg_FLAGS, (FLG3|FLG4|FLG5|FLG6|FLG7|FLG8) );			//Clearing flag pins
}


//////////////////////////////////////////////////////////////////////////////
// void IRQ0_ISR(int sig, void *pCBParam)
//
// PURPOSE:		ISR for pushbutton 2
//
//////////////////////////////////////////////////////////////////////////////
void IRQ0_ISR(int sig, void *pCBParam)
{
	g_iEnterTestMode |= 0x02;

	ClearSet_LED(LED3, 3);
   	ClearSet_LED(LED4, 3);
}

//////////////////////////////////////////////////////////////////////////////
// void IRQ1_ISR(int sig, void *pCBParam)
//
// PURPOSE:		ISR for pushbutton 1
//
//////////////////////////////////////////////////////////////////////////////
void IRQ1_ISR(int sig, void *pCBParam)
{
	g_iEnterTestMode |= 0x01;

	ClearSet_LED(LED1, 3);
   	ClearSet_LED(LED2, 3);
}


//////////////////////////////////////////////////////////////////////////////
// void DAI_ISR(int sig, void *pCBParam)
//
// PURPOSE:		ISR for pushbutton 3 and 4
//
//////////////////////////////////////////////////////////////////////////////
void DAI_ISR(int sig, void *pCBParam)
{
	int iTest;
    iTest = (*pDAI_IRPTL_H); // reading pDAI_IRPTL_H clears the latched interrupt.
    if( SRU_EXTMISCA1_INT == iTest )
    {
		g_iEnterTestMode |= 0x04;
    	ClearSet_LED(LED5, 3);
    	ClearSet_LED(LED6, 3);
    }
    else if( SRU_EXTMISCA2_INT == iTest )
    {
		g_iEnterTestMode |= 0x08;
    	ClearSet_LED(LED7, 3);
    	ClearSet_LED(LED8, 3);
    }
}
