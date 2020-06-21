//////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////////

#ifdef __ADSP21371__
	#include <Cdef21371.h>
	#include <def21371.h>

	#define SDNOBSTOP (BIT_31)	//  def21371.h
#elif __ADSP21375__
	#include <Cdef21375.h>
	#include <def21375.h>

	#define SDNOBSTOP (BIT_31)	//  def21375.h

#elif __ADSP21369__
	#include <Cdef21369.h>
	#include <def21369.h>
#endif

#include <sru.h>
#include <signal.h>
#include <math.h>
#include <stdlib.h>
#include <filter.h>

#include "post_common.h"



#define SDMODIFY  (BIT_17)
void InitPLL(void)
{
	int n;

#ifdef __ADSP21369__

	if( (0x1 == __SILICON_REVISION__) || (0 == __SILICON_REVISION__) || (0xFFFF == __SILICON_REVISION__) )
	{	// 333 Mhz
		//*pPMCTL = (PLLM27|PLLD2|DIVEN);

		// rev ezkit board REV is less than 1.2, run SDRAM at 133Mhz vs. 166Mhz.
		*pPMCTL = (PLLM27 | INDIV| DIVEN | SDCKR2_5);
	}
	else
	{
		// CLKIN= 24.576.5 MHz, Multiplier= 16, Divisor= 1, CCLK_SDCLK_RATIO 2.5.
		// Core clock = (24.576MHz * 16) /1 = 393.216 MHz
		// rev ezkit board REV is less than 1.2, run SDRAM at 133Mhz vs. 166Mhz.
    	*pPMCTL = (PLLM16 | PLLD1 | DIVEN | SDCKR2_5);
	}

#elif __ADSP21375__
	*pPMCTL = (PLLM32 | INDIV | DIVEN | SDCKR2);   // Input divider | PLL Multiplier 48;
#elif __ADSP21371__ 
//	//Core clock = (16.384MHz * 16) /1 = 262.144 MHz run SDRAM at 133
	*pPMCTL = (PLLM32 | PLLD2 | DIVEN | SDCKR2);   // Input divider | PLL Multiplier 48;
#endif

    n = *pPMCTL;
    n |= PLLBP;
    n  ^= DIVEN;
    *pPMCTL = n;

     // Wait at least 4096 cycles for the pll to lock
    for(n = 0; n < 5000; n++ )
    {
    	asm("nop;");
    }

    *pPMCTL ^= PLLBP;
    
	for(n=0;n<16;n++)
		asm("nop;");


#ifdef __ADSP21369__
    // Programming SDRAM control registers.
    // RDIV = ((f SDCLK X t REF )/NRA) - (tRAS + tRP )
    //===================================================================
    //
    // Configure SDRAM Control Register (SDCTL) for the Micron MT48LC4M32
    //
    //  SDCL3  : SDRAM CAS Latency= 3 cycles
    //  DSDCLK1: Disable SDRAM Clock 1
    //  SDPSS  : Start SDRAM Power up Sequence
    //  SDCAW8 : SDRAM Bank Column Address Width= 8 bits
    //  SDRAW12: SDRAM Row Address Width= 12 bits
    //  SDTRAS7: SDRAM tRAS Specification. Active Command delay = 7 cycles
    //  SDTRP3 : SDRAM tRP Specification. Precharge delay = 3 cycles.
    //  SDTWR2 : SDRAM tWR Specification. tWR = 2 cycles.
    //  SDTRCD3: SDRAM tRCD Specification. tRCD = 3 cycles.
    //
    //--------------------------------------------------------------------
	*pSDCTL = (SDCL3|DSDCLK1|SDPSS|SDCAW8|SDRAW12|SDTRAS7|SDTRP3|SDTWR2|SDTRCD3);


    // CCLK_SDCLK_RATIO==25
    // (160*(10^6)*64*(10^-3)/4096) - (7+3) = 2490
	*pSDRRC = (SDMODIFY|SDROPT | 0x9BA);// Enabling SDRAM read optimization

    // Note that MS2 & MS3 pin multiplexed with flag2 & flag3.
    // MSEN bit must be enabled to access SDRAM, but LED8 cannot be driven with sdram
    // Note that MS2 & MS3 pin multiplexed with flag2 & flag3
    // Programming the mutliplexed pin as MS2
    *pSYSCTL |= MSEN;

    // Mapping Bank 2 to SDRAM
    // Make sure that jumper is set appropriately so that MS2 is connected to
    // chip select of 16-bit SDRAM device
    *pEPCTL |= B2SD;

#elif __ADSP21375__
    // Programming SDRAM control registers.
    // RDIV = ((f SDCLK X t REF )/NRA) - (tRAS + tRP )
    //===================================================================
    //
    // Configure SDRAM Control Register (SDCTL) for the
    //
    //  SDNOBSTOP : Disable burst-mode on SDRAM
    //  SDCL3  : SDRAM CAS Latency= 3 cycles
    //  DSDCLK1: Disable SDRAM Clock 1
    //  SDPSS  : Start SDRAM Power up Sequence
    //  SDCAW9 : SDRAM Bank Column Address Width= 9 bits
    //  SDRAW12: SDRAM Row Address Width= 12 bits
    //  SDTRAS6: SDRAM tRAS Specification. Active Command delay = 6 cycles
    //  SDTRP3 : SDRAM tRP Specification. Precharge delay = 3 cycles.
    //  SDTWR2 : SDRAM tWR Specification. tWR = 2 cycles.
    //  SDTRCD3: SDRAM tRCD Specification. tRCD = 3 cycles.
    //  X16DE  : Enable external SDRAM bus width to be 16-bits wide
    //
    //--------------------------------------------------------------------

    *pSDCTL = SDNOBSTOP|SDCL3|DSDCLK1|SDPSS|SDCAW9|SDRAW12|SDTRAS6|SDTRP3|SDTWR2|SDTRCD3|X16DE;





    // CCLK_SDCLK_RATIO==2
	*pSDRRC = 0x307FC;

    // Programming the mutliplexed pin as MS0
    *pSYSCTL |= MSEN;

    // Mapping Bank 0 to SDRAM
    *pEPCTL |= B0SD;
    
#elif __ADSP21371__
    // Programming SDRAM control registers.
    // RDIV = ((f SDCLK X t REF )/NRA) - (tRAS + tRP )
    //===================================================================
    //
    // Configure SDRAM Control Register (SDCTL) for the
    //
    //  SDNOBSTOP : Disable burst-mode on SDRAM
    //  SDCL3  : SDRAM CAS Latency= 3 cycles
    //  DSDCLK1: Disable SDRAM Clock 1
    //  SDPSS  : Start SDRAM Power up Sequence
    //  SDCAW9 : SDRAM Bank Column Address Width= 9 bits
    //  SDRAW12: SDRAM Row Address Width= 12 bits
    //  SDTRAS6: SDRAM tRAS Specification. Active Command delay = 6 cycles
    //  SDTRP3 : SDRAM tRP Specification. Precharge delay = 3 cycles.
    //  SDTWR2 : SDRAM tWR Specification. tWR = 2 cycles.
    //  SDTRCD3: SDRAM tRCD Specification. tRCD = 3 cycles.
    //
    //--------------------------------------------------------------------

    *pSDCTL = SDCL3|DSDCLK1|SDPSS|SDCAW8|SDRAW12|SDTRAS7|SDTRP3|SDTWR2|SDTRCD3;


    // CCLK_SDCLK_RATIO==2
	*pSDRRC = 0x307FC;

    // Programming the mutliplexed pin as MS0
    *pSYSCTL |= MSEN;

    // Mapping Bank 0 to SDRAM
    *pEPCTL |= B0SD;    
#endif



    //===================================================================
    //
    // Configure AMI Control Register (AMICTL0) Bank 0 for the ISSI IS61LV5128
    //
    //  WS2 : Wait States= 2 cycles
    //  HC1  : Bus Hold Cycle (at end of write access)= 1 cycle.
    //  AMIEN: Enable AMI
    //  BW8  : External Data Bus Width= 8 bits.
    //
    //--------------------------------------------------------------------
#if ( defined(__ADSP21375__) || defined(__ADSP21369__) )
	*pEPCTL = (((*pEPCTL) & (~B0SD)) | EPBRCORE);
#endif

	*pAMICTL0 = ( (*pAMICTL0) & ( (~HC1) | (~HC2) | (~RHC1) | ~(RHC2) | (~PKDIS) | (~MSWF) | (~BW16) | (~BW32) | (~PREDIS) ) | (WS4 | IC7 | AMIEN | AMIFLSH) );

    //===================================================================
    //
    // Configure AMI Control Register (AMICTL) Bank 1 for the AMD AM29LV08
    //
    //  WS23 : Wait States= 23 cycles
    //  AMIEN: Enable AMI
    //  BW8  : External Data Bus Width= 8 bits.
    //
    //--------------------------------------------------------------------


	// setup EPCTL to use bank 2 (MS1) core accesses.
#if ( defined(__ADSP21375__) || defined(__ADSP21369__) )
	*pEPCTL = (((*pEPCTL) & (~B1SD)) | EPBRCORE);
#endif

	// NOTE:  The PKDIS bit is set which makes a 1 to 1 mapping, each 8 bit byte
	///       maps to an address.
	*pAMICTL1 = ( PKDIS | WS31 | HC1 | HC2 | RHC1 | RHC2 | IC7 | AMIEN | AMIFLSH);


/*	int n;

	//
	// init the PLL
	//
	*pPMCTL = (PMCTL_SETTING | PMCTL_SDRAM_SETTING);

     // Wait for around 4096 cycles for the pll to lock
    for(n = 0; n < 5000; n++ )
    {
    	asm("nop;");
    }

    *pPMCTL &= (~PLLBP);

    // init the SDRAM
	*pSDCTL = SDCTL_SETTING;
	*pSDRRC = (SDRRC_SETTING | SDMODIFY | SDROPT);
*/
}


void Delay(const int iVal)
{
	int nDelay;
	for( nDelay = 0; nDelay < iVal; nDelay++)
	{
		asm("nop;");
	}
}


void Config_SRU_DAI(void)
{

//-----------------------------------------------------------------------------
//
//  MCLK: The output of the 12.288 MHz xtal is either directly connected to the
//          codec, but also connected to DAI_P06, or just to DAI_P17. This is
//             determined by switches 7.1 and 7.2 For this example we route the
//             MCLK into DAI_P17 and supply the clock to the ADC via DAI_P06
//             by routing the signal through the SRU.

    SRU(LOW,DAI_PB17_I);	//  Tie the pin buffer input LOW.
    SRU(LOW,PBEN17_I);		//  Tie the pin buffer enable input LOW

///
/// external PLL stuff
///
    SRU(LOW,DAI_PB02_I);		//  Tie the pin buffer input LOW.
    SRU(LOW,PBEN02_I);			//  Tie the pin buffer enable input LOW
    SRU(HIGH,PBEN17_I);			//  Tie the pin buffer enable input LOW
    SRU(DAI_PB17_O,DAI_PB03_I);	//  Route the MCLK through the External Analog PLL
    SRU(DAI_PB02_O,DAI_PB06_I);	//  Route the MCLK from the External Analog PLL to the 1835


//-----------------------------------------------------------------------------
//
//  Connect the ADC: The codec drives a BCLK output to DAI pin 7, a LRCLK
//          (a.k.a. frame sync) to DAI pin 8 and data to DAI pin 5.
//
//          Connect the ADC to SPORT0, using data input A
//
//          All three lines are always inputs to the SHARC so tie the pin
//          buffer inputs and pin buffer enable inputs all low.


//------------------------------------------------------------------------
//  Connect the ADC to SPORT0, using data input A

    //  Clock in on pin 7
    SRU(DAI_PB07_O,SPORT0_CLK_I);

    //  Frame sync in on pin 8
    SRU(DAI_PB08_O,SPORT0_FS_I);

    //  Data in on pin 5
    SRU(DAI_PB05_O,SPORT0_DA_I);

//------------------------------------------------------------------------
//  Tie the pin buffer inputs LOW for DAI pins 5, 6 7 and 8.  Even though
//    these pins are inputs to the SHARC, tying unused pin buffer inputs
//    LOW is "good coding style" to eliminate the possibility of
//    termination artifacts internal to the IC.  Note that signal
//    integrity is degraded only with a few specific SRU combinations.
//    In practice, this occurs VERY rarely, and these connections are
//    typically unnecessary.


    SRU(LOW,DAI_PB05_I);
    SRU(LOW,DAI_PB07_I);
    SRU(LOW,DAI_PB08_I);

//------------------------------------------------------------------------
//  Tie the pin buffer enable inputs LOW for DAI pins 5, 6, 7 and 8 so
//    that they are always input pins.

    SRU(LOW,PBEN05_I);
    SRU(LOW,PBEN07_I);
    SRU(LOW,PBEN08_I);

//-----------------------------------------------------------------------------
//
//  Connect the DACs: The codec accepts a BCLK input from DAI pin 13 and
//          a LRCLK (a.k.a. frame sync) from DAI pin 14 and has four
//          serial data outputs to DAI pins 12, 11, 10 and 9
//
//          Connect DAC1 to SPORT1, using data output A
//          Connect DAC2 to SPORT1, using data output B
//          Connect DAC3 to SPORT2, using data output A
//          Connect DAC4 to SPORT2, using data output B
//
//          Connect the clock and frame sync inputs to SPORT1 and SPORT2
//          should come from the ADC on DAI pins 7 and 8, respectively
//
//          Connect the ADC BCLK and LRCLK back out to the DAC on DAI
//          pins 13 and 14, respectively.
//
//          All six DAC connections are always outputs from the SHARC
//          so tie the pin buffer enable inputs all high.
//

//------------------------------------------------------------------------
//  Connect the pin buffers to the SPORT data lines and ADC BCLK & LRCLK

    SRU(SPORT2_DB_O,DAI_PB09_I);
    SRU(SPORT2_DA_O,DAI_PB10_I);
    SRU(SPORT1_DB_O,DAI_PB11_I);
    SRU(SPORT1_DA_O,DAI_PB12_I);

//------------------------------------------------------------------------
//  Connect the clock and frame sync input from the ADC directly
//    to the output pins driving the DACs.

    SRU(DAI_PB07_O,DAI_PB13_I);
    SRU(DAI_PB08_O,DAI_PB14_I);
    //SRU(DAI_PB17_O,DAI_PB06_I); // comment out for external pll

//------------------------------------------------------------------------
//  Connect the SPORT clocks and frame syncs to the clock and
//    frame sync from the SPDIF receiver

    SRU(DAI_PB07_O,SPORT1_CLK_I);
    SRU(DAI_PB07_O,SPORT2_CLK_I);
    SRU(DAI_PB08_O,SPORT1_FS_I);
    SRU(DAI_PB08_O,SPORT2_FS_I);

//------------------------------------------------------------------------
//  Tie the pin buffer enable inputs HIGH to make DAI pins 9-14 outputs.
    SRU(HIGH,PBEN06_I);
    SRU(HIGH,PBEN09_I);
    SRU(HIGH,PBEN10_I);
    SRU(HIGH,PBEN11_I);
    SRU(HIGH,PBEN12_I);
    SRU(HIGH,PBEN13_I);
    SRU(HIGH,PBEN14_I);
}


void Config_SRU_UART(void)
{
	// connect the pin buffer output signal to the UART0 receive
	SRU2(DPI_PB10_O,UART0_RX_I);
	SRU2(LOW,DPI_PB10_I);
	SRU2(LOW,DPI_PBEN10_I)

	// UART transmit signal is connected to DPI pin 9
	SRU2(UART0_TX_O,DPI_PB09_I);
	SRU2(HIGH,DPI_PBEN09_I);
}


void Config_SRU_AT25F2048(void)
{
	SRU(SPI_CLK_O,DPI_PB03_I);
	SRU(HIGH,DPI_PBEN03_I);

	// for the flag pins to act as chip select
	SRU(FLAG4_O, DPI_PB05_I);
	SRU(HIGH, DPI_PBEN05_I);
}

void Config_SRU_INTS(void)
{
	//Pin Assignments in SRU_PIN3 (Group D)
    SRU(LOW,DAI_PB19_I);//assign pin buffer 19 low so it is an input
    SRU(LOW,DAI_PB20_I);    //assign pin buffer 20 low so it is an input

    //Route MISCA singnals in SRU_EXT_MISCA (Group E)
    SRU(DAI_PB19_O,MISCA1_I);//route so that DAI pin buffer 19 connects to MISCA1
    SRU(DAI_PB20_O,MISCA2_I);//route so that DAI pin buffer 20 connects to MISCA2

    //Pin Buffer Disable in SRU_PINEN0 (Group F)
    SRU(LOW,PBEN19_I);//assign pin 19 low so it is an input
    SRU(LOW,PBEN20_I);//assign pin 20 low so it is an input
}



void Config_SRU_LEDS(void)
{
//Setting the SRU and route so that Flag pins connects to DPI pin buffers.
//Use Flags 4 to 15 only. Flags 0 to 3 are reserved
	SRU(FLAG4_O,DPI_PB06_I);	//DPI => We can use flags.
	SRU(FLAG5_O,DPI_PB07_I);	//DPI => We can use flags.
	SRU(FLAG6_O,DPI_PB08_I);	//DPI => We can use flags.
	SRU(FLAG7_O,DPI_PB13_I);	//DPI => We can use flags.
	SRU(FLAG8_O,DPI_PB14_I);	//DPI => We can use flags.
	SRU(LOW,DAI_PB15_I);		//DAI => We can't use flags. Value has to be set low or high.
	SRU(LOW,DAI_PB16_I);		//DAI => We can't use flags. Value has to be set low or high.

//Enabling the Buffer using the following sequence: High -> Output, Low -> Input
	SRU(HIGH,DPI_PBEN06_I);
	SRU(HIGH,DPI_PBEN07_I);
	SRU(HIGH,DPI_PBEN08_I);
	SRU(HIGH,DPI_PBEN13_I);
	SRU(HIGH,DPI_PBEN14_I);
	SRU(HIGH,PBEN15_I);		//default format to enable the buffer using DAI
	SRU(HIGH,PBEN16_I);
}

void Config_SRU_SPDIF(void)
{
    // Disable the pull-up resistors on all 20 pins
    *pDAI_PIN_PULLUP = 0x000FFFFF;

//------------------------------------------------------------------------
//  Connect the SPDIF Receiver
    SRU(DAI_PB18_O, DIR_I);

//------------------------------------------------------------------------
//  Connect the SPDIF Receiver outputs.

    SRU(DIR_DAT_O, SPORT0_DA_I);
    SRU(DIR_FS_O, SPORT0_FS_I);
    SRU(DIR_CLK_O, SPORT0_CLK_I);

//------------------------------------------------------------------------
//  Connect the SPDIF Transmitter Inputs

    SRU(SPORT3_DA_O, DIT_DAT_I);
    SRU(DIR_FS_O, DIT_FS_I);
    SRU(DIR_CLK_O, DIT_CLK_I);
    SRU(DIR_CLK_O, SPORT3_CLK_I);


    SRU(DIR_FS_O, SPORT3_FS_I );


    SRU(DIR_TDMCLK_O, DIT_HFCLK_I); // also used in external pll code

    //SRU(DIR_TDMCLK_O,DAI_PB03_I);
    //SRU(DAI_PB02_O,DIT_HFCLK_I);
    //SRU(HIGH,PBEN03_I);
	//SRU(LOW,PBEN02_I);



//------------------------------------------------------------------------
//  Connect the SPDIF Transmitter Output.
    SRU(DIT_O, DAI_PB01_I);

//------------------------------------------------------------------------
//  Tie the pin buffer inputs LOW for DAI pin 18.  Even though
//    these pins are inputs to the SHARC, tying unused pin buffer inputs
//    LOW is "good coding style" to eliminate the possibility of
//    termination artifacts internal to the IC.  Note that signal
//    integrity is degraded only with a few specific SRU combinations.
//    In practice, this occurs VERY rarely, and these connections are
//    typically unnecessary.

    SRU(LOW, DAI_PB18_I);

//------------------------------------------------------------------------
//  Tie the pin buffer enable inputs LOW for DAI pin 18 so
//    that they are always input pins.

    SRU(LOW, PBEN18_I);

//-----------------------------------------------------------------------------

//------------------------------------------------------------------------
//  Tie the pin buffer enable inputs HIGH to make DAI pin 1 an output.
    SRU(HIGH,PBEN01_I);
}


volatile bool g_bLeftRight = 1;
volatile int  g_iSampleIndex = 1;
volatile int  g_iSampleCount = 0;

volatile float g_fSineWaveOut[MAX_SAMPLES];
volatile float g_fSineWaveIn_Left[MAX_SAMPLES];
volatile float g_fSineWaveIn_Right[MAX_SAMPLES];


int Test_Channel(float* pfRealIn, const int iMaxSamples, const int iSampleRate, const float fExpFreq, const float fFreqTol, const float fExpAmp, const float fAmpTol)
{
	const float fMaxFreq = (fExpFreq + (fExpFreq * fFreqTol));
	const float fMinFreq = (fExpFreq - (fExpFreq * fFreqTol));

	complex_float cfFFTOut[(MAX_SAMPLES/2)];
	float fTempFreq = 0;
	int nSampleNumber = 0;
	int nHighestFreqIndex = 0;
	float fSampledFrequency = 0;
	float fSlope = 0.0;
	int   iSlopeY1 = 0;


   	// Real input array fills from a converter or other source
	rfft256( pfRealIn, (complex_float*)cfFFTOut);

   	// Arrays are filled with FFT data
   	// scan the output array for the highest real value
   	fTempFreq = abs(cfFFTOut[0].re);
 	for (nSampleNumber=1; nSampleNumber < (iMaxSamples / 2); nSampleNumber++)
    {
		if( abs(cfFFTOut[nSampleNumber].re) > fTempFreq )
		{
			fTempFreq = abs(cfFFTOut[nSampleNumber].re);
			nHighestFreqIndex = nSampleNumber;
		}
    }

   // multiply the index of the array of the highest value with the sample rate value
   fSampledFrequency = nHighestFreqIndex * (iSampleRate / iMaxSamples);





   // make sure frequency is within acceptable ranges
   if( (fSampledFrequency < fMaxFreq) && (fSampledFrequency > fMinFreq) )
   {
   		// for now, take the point before, and after the max value
   		// average them, then find the distance between them

	    // the slope is given by b = (y2 - y1) / (x2 - x1) or b = y2-y1
	    if( 0 == nSampleNumber )
	    {	// roll around to the end of the array
	    	iSlopeY1 = iMaxSamples;
	    }
	    else if( iMaxSamples == nSampleNumber )
	    {
	    	iSlopeY1 = 0;
	    }
	    else
	    {
	    	iSlopeY1 = nSampleNumber - 1;
	    }

	    fSlope = (cfFFTOut[nHighestFreqIndex].re - cfFFTOut[iSlopeY1].re);

	    fSlope = abs(fSlope);


	    if( fSlope < fExpAmp )
	    {
	    	return 0;
	    }


	    //fDB = 10 * log10( fSNR );

   		return 1;
   }



	//asm("nop;"); // asm("emuidle;"); //
	return 0;	// test failed
}

void CreateSinTable(const float fAmp, const float fFreq, const float fSampleRate)
{
	int n;

	for(n = 0; n < MAX_SAMPLES; n++)
	{
		g_fSineWaveOut[n] = (float)(fAmp * sin( (2.0 * PI * fFreq * ( ((float)(n+1)) / fSampleRate))) );
	}
}










