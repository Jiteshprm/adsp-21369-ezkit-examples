///////////////////////////////////////////////////////////////////////////////////////
//NAME:     initSPORT.c 
//DATE:     7/15/04
//PURPOSE:  Talkthrough framework for sending and receiving samples to the AD1835.
//
//
////////////////////////////////////////////////////////////////////////////////////////
#include <cdef21364.h>
#include <def21364.h>




//////////////////////////////////////////////////////////////////////////////
// void InitSPORT(void)
// 
// PURPOSE:		Initialise the SPORT 
//////////////////////////////////////////////////////////////////////////////
void InitSPORT(void)
{
	//============================================================
	//
	// Make sure that the multichannel mode registers are cleared
	//
	//============================================================
    *pSPMCTL01 = 0;
    *pSPMCTL23 = 0;

    *pSPCTL0 = 0 ;
    *pSPCTL1 = 0 ;
    *pSPCTL2 = 0 ;

	//============================================================
	//
	// Configure SPORT 0 for input from ADC
	//
	//    BHD = Buffer hang disable
	//    OPMODE = I2S mode
	//    SLEN24 = 24 bit of data in each 32-bit word
	//    SPEN_A = Enable data channel A
	//
	//------------------------------------------------------------
    *pSPCTL0 = (BHD | OPMODE | SLEN24 | SPEN_A);
}


