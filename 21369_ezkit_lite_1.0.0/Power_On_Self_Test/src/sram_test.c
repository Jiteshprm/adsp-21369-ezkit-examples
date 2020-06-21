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



//////////////////////////////////////////////////////////////////////////////
//
// common defines
//
//////////////////////////////////////////////////////////////////////////////
#define SRAM_START  0x200000	// start address of SRAM
#define SRAM_SIZE	0x1FFFF		// size of SRAM in 32-bit words.  (i.e. 128K x 32)




//////////////////////////////////////////////////////////////////////////////
//
// stand alone test jig
//
//////////////////////////////////////////////////////////////////////////////
#ifdef _STANDALONE_ // use this to run standalone tests
int main(void)
{
	int bPassed = 0;

	bPassed = TEST_SRAM();



	return 0;
}
#endif //#ifdef _STANDALONE_


//////////////////////////////////////////////////////////////////////////////
// int TEST_SRAM(void)
//
// PURPOSE:		Test the SRAM
//////////////////////////////////////////////////////////////////////////////
int TEST_SRAM(void)
{
	volatile unsigned int *pDst;
	int nIndex = 0;
	int bError = 1; 	// returning 1 indicates a pass, anything else is a fail

	// write incrementing values to each SRAM location
	for(nIndex = 0, pDst = (unsigned int *)SRAM_START; pDst < (unsigned int *)(SRAM_START + SRAM_SIZE); pDst++, nIndex++ )
	{
		*pDst = nIndex;
	}

	// verify incrementing values
	for(nIndex = 0, pDst = (unsigned int *)SRAM_START; pDst < (unsigned int *)(SRAM_START + SRAM_SIZE); pDst++, nIndex++ )
	{
		if( nIndex != *pDst )
		{
			bError = 0;
			break;
		}
	}

	// write all FFFF's
	for(nIndex = 0xFFFFFFFF, pDst = (unsigned int *)SRAM_START; pDst < (unsigned int *)(SRAM_START + SRAM_SIZE); pDst++ )
	{
		*pDst = nIndex;
	}

	// verify all FFFF's
	for(nIndex = 0xFFFFFFFF, pDst = (unsigned int *)SRAM_START; pDst < (unsigned int *)(SRAM_START + SRAM_SIZE); pDst++ )
	{
		if( nIndex != *pDst )
		{
			bError = 0;
			break;
		}
	}

	// write all AAAAAA's
	for(nIndex = 0xAAAAAAAA, pDst = (unsigned int *)SRAM_START; pDst < (unsigned int *)(SRAM_START + SRAM_SIZE); pDst++ )
	{
		*pDst = nIndex;
	}

	// verify all AAAAA's
	for(nIndex = 0xAAAAAAAA, pDst = (unsigned int *)SRAM_START; pDst < (unsigned int *)(SRAM_START + SRAM_SIZE); pDst++ )
	{
		if( nIndex != *pDst )
		{
			bError = 0;
			break;
		}
	}

	// write all 555555's
	for(nIndex = 0x55555555, pDst = (unsigned int *)SRAM_START; pDst < (unsigned int *)(SRAM_START + SRAM_SIZE); pDst++ )
	{
		*pDst = nIndex;
	}

	// verify all 55555's
	for(nIndex = 0x55555555, pDst = (unsigned int *)SRAM_START; pDst < (unsigned int *)(SRAM_START + SRAM_SIZE); pDst++ )
	{
		if( nIndex != *pDst )
		{
			bError = 0;
			break;
		}
	}

	return bError;
}

