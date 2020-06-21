////////////////////////////////////////////////////////////////////////////
//
//  Program to check the functionality of core accesses to Flash
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

#include <stdlib.h>						// malloc includes
#include "common\flash.h"
#include "am29lv081b\am29lv081b.h"



//////////////////////////////////////////////////////////////////////////////
//
// DEFINES
//
//////////////////////////////////////////////////////////////////////////////
#define BUFFER_SIZE 		2
#define FLASH_START_ADDR	0x4000000 	// start address of FLASH (MS1)
#define FLASH_SECTOR_SIZE	0x10000
#define NUM_SECTORS	 		16
#define FLASH_SIZE	 		(FLASH_SECTOR_SIZE * NUM_SECTORS)	// size of FLASH in 8-bit words.  (i.e. 1M x 8)

#ifdef 	_FULL_FLASH_TEST_
	#define FLASH_TEST_SIZE		FLASH_SIZE
	#define FLASH_TEST_START	0
#else
	#define FLASH_TEST_SIZE		FLASH_SECTOR_SIZE
	#define FLASH_TEST_START	(15 * 0x10000)
#endif //_FULL_FLASH_TEST_


//////////////////////////////////////////////////////////////////////////////
//
// function prototypes
//
//////////////////////////////////////////////////////////////////////////////
static ERROR_CODE GetFlashInfo(void);
static ERROR_CODE SetupForFlash(void);
static ERROR_CODE EraseFlash(void);
static ERROR_CODE WriteData( unsigned long ulStart, long lCount, long lStride, int *pnData );
static ERROR_CODE ReadData( unsigned long ulStart, long lCount, long lStride, int *pnData );


static volatile int  AFP_Error 		= 0;			// contains last error encountered
static volatile int  AFP_ManCode 	= -1;			// 0x01 	= Atmel
static volatile int  AFP_DevCode 	= -1;			// 0x38 	= AT49BV040
static volatile bool AFP_Verify 	= false;		// verify writes or not


//////////////////////////////////////////////////////////////////////////////
// int TEST_PARALLEL_FLASH(void)
//
// PURPOSE:		Test the parallel flash device
//
// INPUT: 	none
// OUPTUP:	none
//////////////////////////////////////////////////////////////////////////////
int TEST_PARALLEL_FLASH(void)
{
	int nValue;
	int nIndex = 0;
	int bError = 1; 	// returning 1 indicates a pass, anything else is a fail

	// open the device
	AFP_Error = am29lv081b_Open();

	// setup the device so the DSP can access it
	if (SetupForFlash() != NO_ERR)
		return 0;

	AFP_Error = GetFlashInfo();
	if( (0x01 != AFP_ManCode) || (0x38 != AFP_DevCode) )
		return 0;

	// write incrementing values to each FLASH location
	EraseFlash();
	for(nIndex = 0; nIndex < FLASH_TEST_SIZE; nIndex++ )
	{
		WriteData( FLASH_TEST_START + nIndex, 1, 1, &nIndex );
	}

	// verify incrementing values
	for(nIndex = 0; nIndex < FLASH_TEST_SIZE; nIndex++ )
	{
		ReadData( (FLASH_TEST_START + nIndex), 1, 1, &nValue );
		if( (nIndex & 0xFF) != nValue )
		{
			return 0;
		}
	}

	// write incrementing values to each FLASH location
	EraseFlash();
	nValue = 0xFF;
	for(nIndex = 0; nIndex < FLASH_TEST_SIZE; nIndex++ )
	{
		WriteData( FLASH_TEST_START + nIndex, 1, 1, &nValue );
	}

	// verify incrementing values
	for(nIndex = 0; nIndex < FLASH_TEST_SIZE; nIndex++ )
	{
		nValue = 0;
		ReadData( (FLASH_TEST_START + nIndex), 1, 1, &nValue );
		if( 0xff != nValue )
		{
			return 0;
		}
	}

	// write incrementing values to each FLASH location
	EraseFlash();
	nValue = 0xAA;
	for(nIndex = 0; nIndex < FLASH_TEST_SIZE; nIndex++ )
	{
		WriteData( FLASH_TEST_START + nIndex, 1, 1, &nValue );
	}

	// verify incrementing values
	for(nIndex = 0; nIndex < FLASH_TEST_SIZE; nIndex++ )
	{
		nValue = 0;
		ReadData( (FLASH_TEST_START + nIndex), 1, 1, &nValue );
		if( 0xAA != nValue )
		{
			return 0;
		}
	}


	// write incrementing values to each FLASH location
	EraseFlash();
	nValue = 0x55;
	for(nIndex = 0; nIndex < FLASH_TEST_SIZE; nIndex++ )
	{
		WriteData( FLASH_TEST_START + nIndex, 1, 1, &nValue );
	}

	// verify incrementing values
	for(nIndex = 0; nIndex < FLASH_TEST_SIZE; nIndex++ )
	{
		nValue = 0;
		ReadData( (FLASH_TEST_START + nIndex), 1, 1, &nValue );
		if( 0x55 != nValue )
		{
			return 0;
		}
	}

	// Close the Device
	AFP_Error = am29lv081b_Close();

	return bError;
}


//----------- G e t F l a s h I n f o  ( ) ----------//
//
//  PURPOSE
//  	Get the manufacturer code and device code
//
// 	RETURN VALUE
//  	ERROR_CODE - value if any error occurs
//  	NO_ERR     - otherwise
//

static ERROR_CODE GetFlashInfo(void)
{

	ERROR_CODE ErrorCode = NO_ERR;		//return error code
	static GET_CODES_STRUCT  SGetCodes;	//structure for GetCodes
	COMMAND_STRUCT CmdStruct;

	// setup code so that flash programmer can just read memory instead of call GetCodes().
	CmdStruct.SGetCodes.pManCode = (unsigned long *)&AFP_ManCode;
	CmdStruct.SGetCodes.pDevCode = (unsigned long *)&AFP_DevCode;
	CmdStruct.SGetCodes.ulFlashStartAddr = FLASH_START_ADDR;

	ErrorCode = am29lv081b_Control( CNTRL_GET_CODES, &CmdStruct );

	return(ErrorCode);
}


//----------- S e t u p F o r F l a s h ( ) ----------//
//
//  PURPOSE
// 		Perform necessary setup for the processor to talk to the
//		flash such as external memory interface registers, etc.
//
//  RETURN VALUE
//  	ERROR_CODE - value if any error occurs during Opcode scan
//  	NO_ERR     - otherwise

static ERROR_CODE SetupForFlash(void)
{
#if ( defined(__ADSP21375__) || defined(__ADSP21369__) )
	// setup EPCTL to use bank 2 (MS1) core accesses.
	*pEPCTL = (((*pEPCTL) & (~B1SD)) | EPBRCORE);

	// setup for max waitstates
	// NOTE:  The PKDIS bit is set which makes a 1 to 1 mapping, each 8 bit byte
	//       maps to an address.
	*pAMICTL1 = ( PKDIS | WS31 | HC1 | HC2 | RHC1 | RHC2 | IC7 | AMIEN | AMIFLSH);

	// reset the flash to a known state
	ERROR_CODE ErrorCode = NO_ERR;		// return error code
	COMMAND_STRUCT SCmdStruct;
	SCmdStruct.SReset.ulFlashStartAddr = FLASH_START_ADDR;
	AFP_Error = am29lv081b_Control( CNTRL_RESET, &SCmdStruct );

#elif __ADSP21364__
	sysreg_bit_set(sysreg_MODE1, IRPTEN);
	sysreg_bit_set(sysreg_LIRPTL, PPIMSK);
	interrupt( SIG_PP, dma_int_0 );		// set DMA interrupt vector
#endif

	return NO_ERR;
}


//////////////////////////////////////////////////////////////
// ERROR_CODE EraseFlash()
//
// Sends an "erase all" command to the flash.
//
//////////////////////////////////////////////////////////////

static ERROR_CODE EraseFlash(void)
{
	ERROR_CODE ErrorCode = 	NO_ERR; 		//return error code

	COMMAND_STRUCT CmdStruct;

#ifdef _FULL_FLASH_TEST_
	// erase contents in Main Flash Array
	CmdStruct.SEraseAll.ulFlashStartAddr 	= FLASH_START_ADDR;	//FlashStartAddress
	ErrorCode = am29lv081b_Control( CNTRL_ERASE_ALL, &CmdStruct );
#else
	CmdStruct.SEraseSect.nSectorNum  		= 15;		// Sector Number to erase
	CmdStruct.SEraseSect.ulFlashStartAddr 	= FLASH_START_ADDR;	//FlashStartAddress
	ErrorCode = am29lv081b_Control( CNTRL_ERASE_SECT, &CmdStruct );
#endif //_FULL_FLASH_TEST_

	// erase should be complete
	return ErrorCode;
}


//----------- W r i t e D a t a  ( ) ----------//
//
//  PURPOSE
//  	Write a buffer to flash device.
//
// 	INPUTS
// 		unsigned long ulStart - address in flash to start the writes at
//		long lCount - number of elements to write, in this case bytes
//		long lStride - number of locations to skip between writes
//		int *pnData - pointer to data buffer
//
//  RETURN VALUE
//  	ERROR_CODE - value if any error occurs during writing
//  	NO_ERR     - otherwise

static ERROR_CODE WriteData( unsigned long ulStart, long lCount, long lStride, int *pnData )
{
	long i = 0;						// loop counter
	ERROR_CODE ErrorCode = NO_ERR;	// tells whether there was an error trying to write
	int nCompare = 0;				// value that we use to verify flash
	bool bVerifyError = false;		// lets us know if there was a verify error
	unsigned long ulAbsoluteAddr;   // current address to write
	unsigned long ulSector = 0;		// sector number to verify address

	
	ulAbsoluteAddr = FLASH_START_ADDR + ulStart;

	COMMAND_STRUCT	CmdStruct;	// structure for GetSectStartEnd

	// if the user wants to verify then do it
	if( AFP_Verify == true )
	{
		// write the buffer up to BUFFER_SIZE items
		for (i = 0; ( i < lCount ) && ( ErrorCode == NO_ERR ); i++, ulAbsoluteAddr += lStride)
		{
			// check to see that the address is within a valid sector
			CmdStruct.SGetSectNum.ulOffset = ulAbsoluteAddr;
			CmdStruct.SGetSectNum.pSectorNum = &ulSector;
			ErrorCode = am29lv081b_Control( CNTRL_GET_SECTNUM, &CmdStruct );

			if( NO_ERR == ErrorCode )
			{
				// unlock the flash, do the write, increase shift, and wait for completion
				ErrorCode = am29lv081b_Write( (unsigned short *)&pnData[i], ulAbsoluteAddr, 0x1 );
				ErrorCode = am29lv081b_Read( (unsigned short *)&nCompare, ulAbsoluteAddr, 0x1 );

				if( ( nCompare & 0xFF ) != (pnData[i] & 0xFF) )
				{
					bVerifyError = true;
					break;
				}
			}
			else
			{
				return ErrorCode;
			}
		}

		// return appropriate error code if there was a verification error
		if( bVerifyError == true )
			return VERIFY_WRITE;
	}
	// the user does not want to verify
	else
	{
		// write the buffer up to BUFFER_SIZE items
		for (i = 0; ( i < lCount ) && ( ErrorCode == NO_ERR ); i++, ulAbsoluteAddr += lStride)
		{
			// check to see that the address is within a valid sector
			CmdStruct.SGetSectNum.ulOffset = ulAbsoluteAddr;
			CmdStruct.SGetSectNum.pSectorNum = &ulSector;
			ErrorCode = am29lv081b_Control( CNTRL_GET_SECTNUM, &CmdStruct );

			if( NO_ERR == ErrorCode )
			{
				// unlock the flash, do the write, increase shift, and wait for completion
				ErrorCode = am29lv081b_Write( (unsigned short *)&pnData[i], ulAbsoluteAddr, 0x1 );
			}
			else
			{
				return ErrorCode;
			}
		}
	}

	// return the appropriate error code
	return ErrorCode;
}


//----------- R e a d D a t a  ( ) ----------//
//
//  PURPOSE
//  	Read a buffer from flash device.
//
// 	INPUTS
//		unsigned long ulStart - address in flash to start the reads at
//		long lCount - number of elements to read, in this case bytes
//		long lStride - number of locations to skip between reads
//		int *pnData - pointer to data buffer to fill
//
//	RETURN VALUE
//  	ERROR_CODE - value if any error occurs during reading
//  	NO_ERR     - otherwise

static ERROR_CODE ReadData( unsigned long ulStart, long lCount, long lStride, int *pnData )
{
	long i = 0;						// loop counter
	ERROR_CODE ErrorCode = NO_ERR;	// tells whether there was an error trying to read
	unsigned long ulAbsoluteAddr;   // current address to read
	unsigned long ulSector = 0;		// sector number to verify address
	unsigned long ulMask = 0xffff;

	
	COMMAND_STRUCT	CmdStruct;	//structure for GetSectStartEnd

	ulAbsoluteAddr = FLASH_START_ADDR + ulStart;

	// read the buffer up to BUFFER_SIZE items
	for (i = 0; (i < lCount) && (i < BUFFER_SIZE); i++, ulAbsoluteAddr += lStride)
	{
		// check to see that the address is within a valid sector
		CmdStruct.SGetSectNum.ulOffset = ulAbsoluteAddr;
		CmdStruct.SGetSectNum.pSectorNum = &ulSector;
		ErrorCode = am29lv081b_Control( CNTRL_GET_SECTNUM, &CmdStruct );

		if( NO_ERR == ErrorCode )
		{
			ErrorCode = am29lv081b_Read( (unsigned short *)&pnData[i], ulAbsoluteAddr, 0x1 );
		}
		else
		{
			return ErrorCode;
		}
	}

	// return the appropriate error code
	return ErrorCode;
}
