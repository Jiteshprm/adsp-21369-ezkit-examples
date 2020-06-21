////////////////////////////////////////////////////////////////////////////
//
//  Program to check the functionality of core accesses to SDRAM
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



//////////////////////////////////////////////////////////////////////////////
//
// COMMON DEFINES
//
//////////////////////////////////////////////////////////////////////////////
#ifdef __ADSP21369__
	#define SDRAM_START  0x08000000	// start address of SDRAM
	#define SDRAM_SIZE	 0x00100000	// size of SDRAM in 32-bit words.  (i.e. 1M x 32)
#elif __ADSP21371__ || __ADSP21375__
	#define SDRAM_START  0x00200000	// start address of SDRAM
	#define SDRAM_SIZE	 0x00400000	// size of SDRAM in 32-bit words.  (i.e. 1M x 32)
#endif



//////////////////////////////////////////////////////////////////////////////
//
// function prototypes
//
//////////////////////////////////////////////////////////////////////////////
int TEST_SDRAM(void);


//////////////////////////////////////////////////////////////////////////////
// int TEST_SDRAM(void)
//
// PURPOSE:		Test the SDRAM
//////////////////////////////////////////////////////////////////////////////
int TEST_SDRAM(void)
{
	volatile unsigned int *pDst;
	int nIndex = 0;
	int bError = 1; 	// returning 1 indicates a pass, anything else is a fail
	int n;

    // Note that MS2 & MS3 pin multiplexed with flag2 & flag3.
    // MSEN bit must be enabled to access SDRAM, but LED8 cannot be driven with sdram
    // Note that MS2 & MS3 pin multiplexed with flag2 & flag3
    // Programming the mutliplexed pin as MS2
    *pSYSCTL |= MSEN;

    // Mapping Bank 2 to SDRAM
    // Make sure that jumper is set appropriately so that MS2 is connected to
    // chip select of 16-bit SDRAM device
#if defined  (__ADSP21369__)
    *pEPCTL |= B2SD;
#elif (__ADSP21371__) || (__ADSP21375__)
    // Mapping Bank 0 to SDRAM
    *pEPCTL |= B0SD;
#endif

	// write incrementing values to each SDRAM location
	for(nIndex = 0, pDst = (unsigned int *)SDRAM_START; pDst < (unsigned int *)(SDRAM_START + SDRAM_SIZE); pDst++, nIndex++ )
	{
		*pDst = nIndex;
	}

	// verify incrementing values
	for(nIndex = 0, pDst = (unsigned int *)SDRAM_START; pDst < (unsigned int *)(SDRAM_START + SDRAM_SIZE); pDst++, nIndex++ )
	{
		if( nIndex != *pDst )
		{
			bError = 0;
			break;
		}
	}

	// write all FFFF's
	for(nIndex = 0xFFFFFFFF, pDst = (unsigned int *)SDRAM_START; pDst < (unsigned int *)(SDRAM_START + SDRAM_SIZE); pDst++ )
	{
		*pDst = nIndex;
	}

	// verify all FFFF's
	for(nIndex = 0xFFFFFFFF, pDst = (unsigned int *)SDRAM_START; pDst < (unsigned int *)(SDRAM_START + SDRAM_SIZE); pDst++ )
	{
		if( nIndex != *pDst )
		{
			bError = 0;
			break;
		}
	}

	// write all AAAAAA's
	for(nIndex = 0xAAAAAAAA, pDst = (unsigned int *)SDRAM_START; pDst < (unsigned int *)(SDRAM_START + SDRAM_SIZE); pDst++ )
	{
		*pDst = nIndex;
	}

	// verify all AAAAA's
	for(nIndex = 0xAAAAAAAA, pDst = (unsigned int *)SDRAM_START; pDst < (unsigned int *)(SDRAM_START + SDRAM_SIZE); pDst++ )
	{
		if( nIndex != *pDst )
		{
			bError = 0;
			break;
		}
	}

	// write all 555555's
	for(nIndex = 0x55555555, pDst = (unsigned int *)SDRAM_START; pDst < (unsigned int *)(SDRAM_START + SDRAM_SIZE); pDst++ )
	{
		*pDst = nIndex;
	}

	// verify all 55555's
	for(nIndex = 0x55555555, pDst = (unsigned int *)SDRAM_START; pDst < (unsigned int *)(SDRAM_START + SDRAM_SIZE); pDst++ )
	{
		if( nIndex != *pDst )
		{
			bError = 0;
			break;
		}
	}


	// write all 0000000's
	for(nIndex = 0x00000000, pDst = (unsigned int *)SDRAM_START; pDst < (unsigned int *)(SDRAM_START + SDRAM_SIZE); pDst++ )
	{
		*pDst = nIndex;
	}

	// verify all 00000's
	for(nIndex = 0x00000000, pDst = (unsigned int *)SDRAM_START; pDst < (unsigned int *)(SDRAM_START + SDRAM_SIZE); pDst++ )
	{
		if( nIndex != *pDst )
		{
			bError = 0;
			break;
		}
	}
	
	// butterfly test.  Write incrementing values to beginging and end of SDRAM
	volatile unsigned int *pStart = (volatile unsigned int *)SDRAM_START;
	volatile unsigned int *pEnd = (volatile unsigned int *)(SDRAM_START + SDRAM_SIZE)-1;
	
	for(nIndex = 0; nIndex < (SDRAM_SIZE/2); nIndex++ )
	{
		*pStart = nIndex;
		*pEnd = (SDRAM_SIZE - nIndex);
		
		pStart++;
		pEnd--;
	}

	// verify butterfly test values
	pStart = (volatile unsigned int *)SDRAM_START;
	pEnd = (volatile unsigned int *)(SDRAM_START + SDRAM_SIZE)-1;

	for(nIndex = 0; nIndex < (SDRAM_SIZE/2); nIndex++ )
	{
		if( nIndex != *pStart )
		{
			bError = 0;
			break;
		}
		
		if( (SDRAM_SIZE - nIndex) != *pEnd )
		{
			bError = 0;
			break;
		}

		pStart++;
		pEnd--;
	}

	

	return bError;
}

