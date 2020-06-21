///////////////////////////////////////////////////////////////////////////////////////
//NAME:     init1835viaSPI.c
//DATE:     9/03/04
//PURPOSE:  Talkthrough framework for sending and receiving samples to the AD1835.
//
//USAGE:    This file contains the subroutines for accessing the AD1835 control
//          registers via SPI.
//
////////////////////////////////////////////////////////////////////////////////////////
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

#include "post_common.h"
#include "ad1835.h"


//////////////////////////////////////////////////////////////////////////////
//
// common constants
//
//////////////////////////////////////////////////////////////////////////////

/* Setup the SPI pramaters here in a buffer first */
const unsigned int Config1835Param [] = {
			WR | DACCTRL1 | DACI2S | DAC24BIT | DACFS48,
			WR | DACCTRL2, // e.g.: | DACMUTE_R1 | DACMUTE_L2,
			WR | DACVOL_L1 | DACVOL_MAX,
			WR | DACVOL_R1 | DACVOL_MAX,
			WR | DACVOL_L2 | DACVOL_MAX,
			WR | DACVOL_R2 | DACVOL_MAX,
			WR | DACVOL_L3 | DACVOL_MAX,
			WR | DACVOL_R3 | DACVOL_MAX,
			WR | DACVOL_L4 | DACVOL_MAX,
			WR | DACVOL_R4 | DACVOL_MAX,
			WR | ADCCTRL1 | ADCFS48,
			WR | ADCCTRL2 | ADCI2S | ADC24BIT,
			WR | ADCCTRL3 | IMCLKx2 | PEAKRDEN
        } ;


//////////////////////////////////////////////////////////////////////////////
// void SetupSPI1835(void)
//
// PURPOSE:  	Set up the SPI port to access the AD1835
//
// INPUT:		none
// OUTPUT:		none
//////////////////////////////////////////////////////////////////////////////
void SetupSPI1835(void)
{
    /* First configure the SPI Control registers */
    /* First clear a few registers     */
    *pSPICTL = (TXFLSH | RXFLSH) ;
    *pSPIFLG = 0;

    /* Setup the baud rate to 1MHz */
    *pSPIBAUD = 100;

    /* Setup the SPI Flag register to FLAG3 : 0xF708*/
    *pSPIFLG = 0xF708;

    /* Now setup the SPI Control register : 0x5281*/
    *pSPICTL = (SPIEN | SPIMS | MSBF | WL16 | TIMOD1) ;

}

//////////////////////////////////////////////////////////////////////////////
// void DisableSPI1835(void)
//
// PURPOSE:  	Disable the SPI Port
//
// INPUT:		none
// OUTPUT:		none
//////////////////////////////////////////////////////////////////////////////
void DisableSPI1835(void)
{
    *pSPICTL = (TXFLSH | RXFLSH);
}


//////////////////////////////////////////////////////////////////////////////
// void Configure1835Register(int val)
//
// PURPOSE:  	Send a word to the AD1835 via SPI
//
// INPUT:		value to set
// OUTPUT:		none
//////////////////////////////////////////////////////////////////////////////
void Configure1835Register(int val)
{
    *pTXSPI = val ;
    Delay(100);

    //Wait for the SPI to indicate that it has finished.
    while (1)
    {
        if (*pSPISTAT & SPIF)
            break ;
    }
    Delay (100) ;
}


//////////////////////////////////////////////////////////////////////////////
// unsigned int Get1835Register(int val)
//
// PURPOSE:  	Receive a register setting from the AD1835
//
// INPUT:		register to get
// OUTPUT:		value from 1835
//////////////////////////////////////////////////////////////////////////////
unsigned int Get1835Register(int val)
{
    *pTXSPI = val ;
    Delay(100);

    //Wait for the SPI port to indicate that it has finished
    while (1)
    {
        if (SPIF & *pSPISTAT)
            break ;
    }
    Delay (100) ;
    return *pRXSPI ;
//  return i ;
}

//////////////////////////////////////////////////////////////////////////////
// void Init1835viaSPI(void)
//
// PURPOSE:  	Set up all AD1835 registers via SPI
//
// INPUT:		none
// OUTPUT:		none
//////////////////////////////////////////////////////////////////////////////
void Init1835viaSPI(void)
{
    int configSize = sizeof (Config1835Param) / sizeof (int) ;
    int i ;

    SetupSPI1835 () ;

    for (i = 0; i < configSize; ++i)
    {
        Configure1835Register (Config1835Param[i]) ;
    }

    DisableSPI1835 () ;

}



