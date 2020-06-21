///////////////////////////////////////////////////////////////////////////////////////
//NAME:     initSPORT.c 
//DATE:     7/15/04
//PURPOSE:  Talkthrough framework for sending and receiving samples to the AD1835.
//
//USAGE:    This file initializes the SPORTs for DMA Chaining
//
////////////////////////////////////////////////////////////////////////////////////////

#include <cdef21364.h>
#include <def21364.h>


//////////////////////////////////////////////////////////////////////////////
// void InitSPORT_SPDIF(void)
// 
// PURPOSE:		Initialise the SPORT 
//////////////////////////////////////////////////////////////////////////////
void InitSPORT_SPDIF(void)
{
	//============================================================
	//
	// Make sure that the multichannel mode registers are cleared
	//
	//============================================================
    *pSPMCTL01 = 0;
    *pSPMCTL23 = 0;

    *pSPCTL0 = 0;
    *pSPCTL1 = 0;
    *pSPCTL2 = 0;
	*pSPCTL2 = 0;
	

	//============================================================
	//
	// Configure SPORT 0 for input from SPDIF
	//
	//    OPMODE = I2S mode
	//    L_FIRST = I2S sends left channel first
	//    SLEN24 = 24 bit of data in each 32-bit word
	//    SPEN_A = Enable data channel A
	//
	//------------------------------------------------------------

	*pDIV0 = 0;
	*pSPCTL0 = (OPMODE | L_FIRST | SLEN24 | SPEN_A);


	//============================================================
	//
	// Configure SPORT 3 for output to SPDIF
	//
	//    SPTRAN = Transmit on serial port
	//    OPMODE = I2S mode
	//    L_FIRST = I2S sends left channel first
	//    SLEN24 = 24 bit of data in each 32-bit word
	//    SPEN_A = Enable data channel A
	//
	//------------------------------------------------------------

	*pDIV3 = 0;
	*pSPCTL3 = (SPTRAN | OPMODE | L_FIRST | SLEN32 | SPEN_A);
}

//////////////////////////////////////////////////////////////////////////////
// void InitSPDIF(void)
// 
// PURPOSE:		Initialise the SPDIF 
//////////////////////////////////////////////////////////////////////////////
void InitSPDIF(void)
{
	//Use default SPDIF Receiver mode
    //DIRCTL = 0
    *pDIRCTL = 0;
        
	//Initialize SPDIF transmitter in I2S mode
    *pDITCTL = (DIT_EN|DIT_IN_I2S);
}




