////////////////////////////////////////////////////////////////////////////
//
//  Program to check the functionality of the UART
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

#include <signal.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <filter.h>

#include <services\int\adi_int.h>
#include "post_common.h"


//////////////////////////////////////////////////////////////////////////////
//
// definitions
//
//////////////////////////////////////////////////////////////////////////////
#define AD1835_REQUIRED_SAMPLES			((MAX_SAMPLES) * 100)
#define AD1835_DESIRED_FREQ 			((float)3000.0)
#define AD1835_SAMPLE_RATE 				((float)48000.0)
#define AD1835_AMPLITUDE				((float)8388607.5)
#define AD1835_ACCEPTABLE_DEVIATION_PCT	((float)0.015)
#define AD1835_MAX_SIGNAL_STRENGTH		(float)5000000.0


//////////////////////////////////////////////////////////////////////////////
//
// constant values & data
//
//////////////////////////////////////////////////////////////////////////////
volatile unsigned int *g_pTXSPx_PTR;
volatile unsigned int * const g_pTXSPx[4] = {
	((volatile unsigned int *)pTXSP1A),
	((volatile unsigned int *)pTXSP1B),
	((volatile unsigned int *)pTXSP2A),
	((volatile unsigned int *)pTXSP2B)  };


volatile unsigned int * const g_pSPCTLx[4] = {
	((volatile unsigned int *)pSPCTL1),
	((volatile unsigned int *)pSPCTL1),
	((volatile unsigned int *)pSPCTL2),
	((volatile unsigned int *)pSPCTL2) };

const unsigned int g_pSPCTLx_values[4] = {
	(SPTRAN | BHD | OPMODE | SLEN24 | SPEN_A),
	(SPTRAN | BHD | OPMODE | SLEN24 | SPEN_B),
	(SPTRAN | BHD | OPMODE | SLEN24 | SPEN_A),
	(SPTRAN | BHD | OPMODE | SLEN24 | SPEN_B) };



//////////////////////////////////////////////////////////////////////////////
//
// function prototypes
//
//////////////////////////////////////////////////////////////////////////////
int TEST_ANALOG_AUDIO(void);
void Init1835viaSPI(void);
void TalkThroughISR(int sig, void *pCBParam);
void InitSPORT(void);



//////////////////////////////////////////////////////////////////////////////
//
// NOTES
//
//////////////////////////////////////////////////////////////////////////////
/*
   Here is the mapping between the SPORTS and the DACS
   ADC -> DSP  : SPORT0A : IIS
   DSP -> DAC1 : SPORT1A : IIS
   DSP -> DAC2 : SPORT1B : IIS
   DSP -> DAC3 : SPORT2A : IIS
   DSP -> DAC4 : SPORT2B : IIS
*/


//////////////////////////////////////////////////////////////////////////////
//
// stand alone test harness
//
//////////////////////////////////////////////////////////////////////////////
#ifdef _STANDALONE_ // use this to run standalone tests

int g_loops = 0;

int main(void)
{
	int bError = 1; 	// returning 1 indicates a pass, anything else is a fail

	InitPLL ();

    // Need to initialize DAI because the sport signals
    // need to be routed
    Config_SRU_DAI();

    // This function will configure the codec on the kit
    Init1835viaSPI();

    // Finally setup the sport to receive / transmit the data
    InitSPORT();

	while(1)
	{
		bError = TEST_ANALOG_AUDIO();
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



int TEST_ANALOG_AUDIO(void)
{
	long lTimeout = (AD1835_REQUIRED_SAMPLES * 4000);

	volatile int n, j;
	volatile int bError = 0; 	// returning 1 indicates a pass, anything else is a fail

 	//g_bLeftRight = 1;
	g_iSampleIndex = 1;
	g_iSampleCount = 0;


    // Need to initialize DAI because the sport signals
    // need to be routed
    Config_SRU_DAI();

    // This function will configure the codec on the kit
    Init1835viaSPI();

    // Finally setup the sport to receive / transmit the data
    InitSPORT();

    enable_interrupts();

	for(n = 0; n < 4; n++)
	{
		CreateSinTable(AD1835_AMPLITUDE, AD1835_DESIRED_FREQ, AD1835_SAMPLE_RATE);

		// reconfigure the sports for the
		// correct channel
	    *pSPCTL1 = 0 ;
	    *pSPCTL2 = 0 ;

		*g_pSPCTLx[n] = g_pSPCTLx_values[n];
	    g_pTXSPx_PTR = g_pTXSPx[n];

	    // reset the buffer indexs
	    g_iSampleIndex = 0;
		g_iSampleCount = 0;

		// setup interrupts
	    adi_int_InstallHandler(ADI_CID_SP0I,(ADI_INT_HANDLER_PTR)TalkThroughISR, NULL, true);

	    // Be in infinite loop and do nothing until done.
	    while( g_iSampleCount < AD1835_REQUIRED_SAMPLES )
	    {
	    	if( 0 == lTimeout-- )
	    	{
			    adi_int_EnableInt(ADI_CID_SP0I, false);
			    adi_int_UninstallHandler(ADI_CID_SP0I);
			    disable_interrupts();
			    return 0; // failed to get enough samples
	    	}
		    // once the required number of samples has been collected,
		    // process the signal.
	    };

	    // turn off interrupts so that the data is stable.
	    adi_int_EnableInt(ADI_CID_SP0I, false);
	    adi_int_UninstallHandler(ADI_CID_SP0I);


	    // test the left channel
	    bError = Test_Channel((float*)g_fSineWaveIn_Left, MAX_SAMPLES,
	    						AD1835_SAMPLE_RATE, AD1835_DESIRED_FREQ,
	    						AD1835_ACCEPTABLE_DEVIATION_PCT,
	    						AD1835_MAX_SIGNAL_STRENGTH,
	    						AD1835_MAX_SIGNAL_STRENGTH);
	    if( 1 == bError )
	    {
	    	// test the right channel
	    	bError = Test_Channel((float*)g_fSineWaveIn_Right, MAX_SAMPLES,
	    							AD1835_SAMPLE_RATE, AD1835_DESIRED_FREQ,
	    							AD1835_ACCEPTABLE_DEVIATION_PCT,
	    							AD1835_MAX_SIGNAL_STRENGTH,
	    							AD1835_MAX_SIGNAL_STRENGTH);
	    }

	    if( 0 == bError )
	    {
		    disable_interrupts();
	    	return bError;
	    }

	}

    disable_interrupts();

    return bError;
}


void TalkThroughISR(int sig, void *pCBParam)
{
	// receive a sample (could be left or right)
	int nInValue = (int)*pRXSP0A;

	if( nInValue & 0x00800000)
	{	// negative sign extend
		nInValue |= 0xFF000000;
	}

	//*g_pTXSPx_PTR = (int)(AD1835_AMPLITUDE * sin( (2.0 * PI * AD1835_DESIRED_FREQ * ( ((float)(g_iSampleIndex+1)) / AD1835_SAMPLE_RATE))) );

	*g_pTXSPx_PTR = (int)g_fSineWaveOut[g_iSampleIndex];


	if( g_bLeftRight )
	{	// right side
		g_fSineWaveIn_Right[g_iSampleIndex] = (float)nInValue;
	}
	else
	{	// left side
		g_fSineWaveIn_Left[g_iSampleIndex] = (float)nInValue;
		g_iSampleIndex++;	// only increment the index when both channels have been sent.
	}

	g_bLeftRight = !g_bLeftRight;

	if( g_iSampleIndex > MAX_SAMPLES-1 )
		g_iSampleIndex = 0;

	g_iSampleCount++;
}





