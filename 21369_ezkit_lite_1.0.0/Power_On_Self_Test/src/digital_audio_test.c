////////////////////////////////////////////////////////////////////////////
//
//  Program to check the functionality of the UART
//    device
//

#ifdef __ADSP21375__
	#include <Cdef21375.h>
	#include <def21375.h>
#elif __ADSP21369__
	#include <Cdef21369.h>
	#include <def21369.h>
#endif

#include <signal.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include <services\int\adi_int.h>
#include "post_common.h"



//////////////////////////////////////////////////////////////////////////////
//
// COMMON DEFINES
//
//////////////////////////////////////////////////////////////////////////////
#define SPDIF_REQUIRED_SAMPLES			((MAX_SAMPLES) * 100)
#define SPDIF_DESIRED_FREQ 				((float)3000.0)
#define SPDIF_SAMPLE_RATE 				((float)48000.0)
#define SPDIF_AMPLITUDE					((float)8388607.5)
#define SPDIF_ACCEPTABLE_DEVIATION_PCT	((float)0.015)
#define SPDIF_MAX_SIGNAL_STRENGTH		(float)1000000.0


//////////////////////////////////////////////////////////////////////////////
//
// function prototypes
//
//////////////////////////////////////////////////////////////////////////////
int TEST_DIGITAL_AUDIO(void);
void InitSPORT_SPDIF(void);
void InitSPDIF(void);
void SPDIF_SPORT0_ISR(int sig, void *pCBParam);

//////////////////////////////////////////////////////////////////////////////
//
// stand alone test jig
//
//////////////////////////////////////////////////////////////////////////////
#ifdef _STANDALONE_ // use this to run standalone tests

int g_loops = 0;

int main(void)
{
	int bError = 1; 	// returning 1 indicates a pass, anything else is a fail

    InitPLL();     // Initializes PLL for the correct core clock (CCLK) frequency

	while(1)
	{
		bError = TEST_DIGITAL_AUDIO();
		if( 0 == bError )
		{
			asm("nop;");
			asm("nop;");
			asm("nop;");
			asm("emuidle;");
			asm("nop;");
			asm("nop;");
		}

		g_loops++;
	}

    return bError;
}

#endif //#ifdef _STANDALONE_


//////////////////////////////////////////////////////////////////////////////
// int TEST_DIGITAL_AUDIO(void)
//
// PURPOSE:  	test analog audio interface
//
//////////////////////////////////////////////////////////////////////////////
int TEST_DIGITAL_AUDIO(void)
{
	volatile int n, j;
	volatile int bError = 0; 	// returning 1 indicates a pass, anything else is a fail

    Config_SRU_SPDIF();      // Initializes the SRU & DAI/DPI pins
    InitSPORT_SPDIF();    // Initializes the transmit and receive serial ports (SPORTS)
    InitSPDIF();    // Initializes the SPDIF transmitter & receiver

    // reset the buffer indexs
    g_iSampleIndex = 0;
	g_iSampleCount = 0;

	enable_interrupts();

	// setup interrupts
    adi_int_InstallHandler(ADI_CID_P6I,(ADI_INT_HANDLER_PTR)SPDIF_SPORT0_ISR, NULL, true);

    // Be in infinite loop and do nothing until done.
    while( g_iSampleCount < SPDIF_REQUIRED_SAMPLES )
    {
	    // once the required number of samples has been collected,
	    // process the signal.
    };

    // turn off interrupts so that the data is stable.
    adi_int_EnableInt(ADI_CID_P6I, false);
    adi_int_UninstallHandler(ADI_CID_P6I);

    disable_interrupts();

    // test the left channel

    bError = Test_Channel((float*)g_fSineWaveIn_Left, MAX_SAMPLES, SPDIF_SAMPLE_RATE, SPDIF_DESIRED_FREQ, SPDIF_ACCEPTABLE_DEVIATION_PCT, SPDIF_MAX_SIGNAL_STRENGTH, SPDIF_MAX_SIGNAL_STRENGTH);

    return bError;
}



//////////////////////////////////////////////////////////////////////////////
// void SPDIF_SPORT0_ISR(int sig, void *pCBParam)
//
// PURPOSE:  	SPORT0 isr
//
// INPUT:		sig_int
// OUTPUT:		none
//////////////////////////////////////////////////////////////////////////////
void SPDIF_SPORT0_ISR(int sig, void *pCBParam)
{
	int nInValue = (int)*pRXSP0A;

	if( nInValue & 0x00800000)
	{	// negative sign extend
		nInValue |= 0xFF000000;
	}

	g_fSineWaveIn_Left[g_iSampleIndex] = (float)nInValue;

	*pTXSP3A = (int)(SPDIF_AMPLITUDE * sin( (2.0 * PI * SPDIF_DESIRED_FREQ * ( ((float)(g_iSampleIndex+1)) / SPDIF_SAMPLE_RATE))) );

	if( g_bLeftRight )
	{	// right side
		//fSineWaveIn_Right[g_iSampleIndex] = (float)nInValue;
	}
	else
	{	// left side
		//fSineWaveIn_Left[g_iSampleIndex] = (float)nInValue;
		g_iSampleIndex++;	// only increment the index when both channels have been sent.
	}

	g_bLeftRight = !g_bLeftRight;

	if( g_iSampleIndex > MAX_SAMPLES-1 )
		g_iSampleIndex = 0;

	g_iSampleCount++;
}





