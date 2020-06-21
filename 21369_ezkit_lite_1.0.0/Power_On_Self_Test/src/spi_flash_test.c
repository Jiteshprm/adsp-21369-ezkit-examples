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

#include <sru.h>
#include <sysreg.h>
#include <stdlib.h>						// malloc includes
#include "common\flash.h"
#include "m25p20\m25p20.h"
#include "post_common.h"


//////////////////////////////////////////////////////////////////////////////
//
// COMMON DEFINES
//
//////////////////////////////////////////////////////////////////////////////
#define BAUD_RATE_DIVISOR 	100
#define BUFFER_SIZE			2

#ifdef _AT25F512_
	#define NUM_SECTORS		2
	#define SECTOR_SIZE		0x8000
#else
	#define NUM_SECTORS		4
	#define SECTOR_SIZE		0x10000
#endif



#define FLASH_START_ADDR  		0x0000000		// start address
#define FLASH_SIZE	 		(SECTOR_SIZE * NUM_SECTORS)	// size of FLASH in 8-bit words.  (i.e. 512K x 8)





#ifdef 	_FULL_FLASH_TEST_
	#define FLASH_TEST_SIZE		FLASH_SIZE
	#define FLASH_TEST_START	0
#else
	#define FLASH_TEST_SIZE		SECTOR_SIZE
	#define FLASH_TEST_START	(SECTOR_SIZE * (NUM_SECTORS-1) )	// test last sector
#endif //_FULL_FLASH_TEST_


static volatile int  AFP_Error 		= 0;			// contains last error encountered
static volatile int  AFP_ManCode 	= -1;			// 0x1F 	= Atmel
static volatile int  AFP_DevCode 	= -1;			// 0x63 = AT25F2048, 0x60 = AT25F512
static volatile bool AFP_Verify 	= false;		// verify writes or not

#define	MAX_DEV				3	// number of flash type we support

const unsigned int g_FlashDevCode[MAX_DEV] = 
{
	0x12,
	0x60,
	0x63 
};	

//////////////////////////////////////////////////////////////////////////////
//
// Function prototypes
//
//////////////////////////////////////////////////////////////////////////////
static ERROR_CODE OpenFlashDevice(void);
static ERROR_CODE SetupForFlash(void);
static ERROR_CODE GetFlashInfo(void);
static ERROR_CODE EraseFlash(void);
static ERROR_CODE WriteData( unsigned long ulStart, long lCount, long lStride, int *pnData );
static ERROR_CODE ReadData( unsigned long ulStart, long lCount, long lStride, int *pnData );


//////////////////////////////////////////////////////////////////////////////
// int TEST_SPI_FLASH(void)
//
// PURPOSE:		Test the SPI flash
//
//////////////////////////////////////////////////////////////////////////////
int TEST_SPI_FLASH(void)
{
	int nManCode = 0;
	int nDevCode = 0;
	int nValue;
	int nTestValue;


	int nIndex = 0;
	int bError = 1; 	// returning 1 indicates a pass, anything else is a fail


    sysreg_bit_clr( sysreg_IRPTL, SPIHI);    // clear any pending SPI interrupt latch
    sysreg_bit_clr( sysreg_IMASK, SPIHI);    // enable SPI receive interrupt
    sysreg_bit_clr( sysreg_MODE1, IRPTEN);   // enable global interrupts

    *pSPIDMAC = 0;

	// setup the device so the DSP can access it
	if (SetupForFlash() != NO_ERR)
		return 0;

	// open flash and get flash info
	AFP_Error = OpenFlashDevice();
	AFP_Error = GetFlashInfo();

	// All A's
	EraseFlash();

	// write incrementing values to each FLASH location
	nTestValue = 0xAA;
	for(nIndex = 0; nIndex < FLASH_TEST_SIZE; nIndex++ )
	{
		if(NO_ERR != WriteData( (FLASH_TEST_START + nIndex), 1, 1, &nTestValue ) )
		{
			return 0;
		}
	}

	// verify incrementing values
	nTestValue = 0xAA;
	for(nIndex = 0; nIndex < FLASH_TEST_SIZE; nIndex++ )
	{
		if( NO_ERR != ReadData( (FLASH_TEST_START + nIndex), 1, 1, &nValue ) )
		{
			return 0;
		}

		if( (0xFF & nTestValue) != nValue )
		{
			return 0;
		}
	}

	// close the device
	AFP_Error = m25p20_Close();

	return bError;
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
	ERROR_CODE ErrorCode = NO_ERR;

	int nStatus;

	SRU(SPI_CLK_O,DPI_PB03_I);
	SRU(HIGH,DPI_PBEN03_I);

	// for the flag pins to act as chip select
	SRU(FLAG4_O, DPI_PB05_I);
	SRU(HIGH, DPI_PBEN05_I);

	// first set flag 4 as an output
	sysreg_bit_set( sysreg_FLAGS, FLG4O );
	sysreg_bit_set( sysreg_FLAGS, FLG4 );
	*pSPIBAUD = BAUD_RATE_DIVISOR;

	return ErrorCode;
}


//----------- O p e n F l a s h D e v i c e ( ) ----------//
//
//  PURPOSE
//  	Opens the appropriate flash device.
//
// 	RETURN VALUE
//  	ERROR_CODE - value if any error occurs during Opcode scan
//  	NO_ERR     - otherwise
//

ERROR_CODE OpenFlashDevice(void)
{
	ERROR_CODE ErrorCode = 	NO_ERR; 		//return error code
	int n;
	
	for( n=0; n<MAX_DEV; n++)
	{
		// open the device	
	   	ErrorCode = m25p20_Open();

		// get flash manufacturer & device codes, title & desc
		if( ErrorCode == NO_ERR )
		{
			ErrorCode = GetFlashInfo();
		}				

		if( AFP_DevCode == g_FlashDevCode[n] )	
		{
		
			// reset the flash to a known state
			COMMAND_STRUCT SCmdStruct;
			SCmdStruct.SReset.ulFlashStartAddr = FLASH_START_ADDR;
			ErrorCode = m25p20_Control( CNTRL_RESET, &SCmdStruct );
			return ErrorCode;  
		}
	}	
	
	return(ErrorCode);
}

//----------- G e t F l a s h I n f o  ( ) ----------//
//
//  PURPOSE
//  	Get the manufacturer code and device code
//
// 	RETURN VALUE
//  	ERROR_CODE - value if any error occurs
//  	NO_ERR     - otherwise

static ERROR_CODE GetFlashInfo(void)
{
	ERROR_CODE ErrorCode = NO_ERR;		//return error code
	static GET_CODES_STRUCT  SGetCodes;	//structure for GetCodes
	COMMAND_STRUCT CmdStruct;

	//setup code so that flash programmer can just read memory instead of call GetCodes().
	CmdStruct.SGetCodes.pManCode = (unsigned long *)&AFP_ManCode;
	CmdStruct.SGetCodes.pDevCode = (unsigned long *)&AFP_DevCode;
	CmdStruct.SGetCodes.ulFlashStartAddr = FLASH_START_ADDR;

	AFP_Error = m25p20_Control( CNTRL_GET_CODES, &CmdStruct );

	return(ErrorCode);
}

//////////////////////////////////////////////////////////////
// ERROR_CODE EraseFlash()
//
// Sends an "erase all" command to the flash.
//
//////////////////////////////////////////////////////////////

static ERROR_CODE EraseFlash(void)
{
    COMMAND_STRUCT CmdStruct;
    ERROR_CODE ErrorCode = 	NO_ERR; 		// return error code

#ifdef _FULL_FLASH_TEST_	// erase the entire flash
	CmdStruct.SEraseAll.ulFlashStartAddr 	= FLASH_START_ADDR;	//FlashStartAddress
	ErrorCode = m25p20_Control( CNTRL_ERASE_ALL, &CmdStruct );
#else	// only erase last sector
	CmdStruct.SEraseSect.nSectorNum  		= (NUM_SECTORS - 1);	// Sector Number to erase
	CmdStruct.SEraseSect.ulFlashStartAddr 	= FLASH_START_ADDR;		// FlashStartAddress
	ErrorCode = m25p20_Control( CNTRL_ERASE_SECT, &CmdStruct );
#endif
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

ERROR_CODE WriteData( unsigned long ulStart, long lCount, long lStride, int *pnData )
{
	long i = 0;						// loop counter
	ERROR_CODE ErrorCode = NO_ERR;	// tells whether there was an error trying to write
	int nCompare = 0;				// value that we use to verify flash
	bool bVerifyError = false;		// lets us know if there was a verify error
	unsigned long ulAbsoluteAddr;   // current address to write
	unsigned long ulSector = 0;		// sector number to verify address

	ulAbsoluteAddr = FLASH_START_ADDR + ulStart;

	COMMAND_STRUCT	CmdStruct;	//structure for GetSectStartEnd

	// if the user wants to verify then do it
	if( AFP_Verify == true )
	{
		// write the buffer up to BUFFER_SIZE items
		for (i = 0; ( i < lCount ) && ( ErrorCode == NO_ERR ); i++, ulAbsoluteAddr += lStride)
		{

			// check to see that the address is within a valid sector
			CmdStruct.SGetSectNum.ulOffset = ulAbsoluteAddr;
			CmdStruct.SGetSectNum.pSectorNum = &ulSector;
			ErrorCode = m25p20_Control( CNTRL_GET_SECTNUM, &CmdStruct  );

			if( NO_ERR == ErrorCode )
			{
				// unlock the flash, do the write, increase shift, and wait for completion
				ErrorCode = m25p20_Write( (unsigned short *)&pnData[i], ulAbsoluteAddr, 0x1 );
				ErrorCode = m25p20_Read( (unsigned short *)&nCompare, ulAbsoluteAddr, 0x1 );

				if( ( nCompare ) != (pnData[i] & 0xFF) )
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
			ErrorCode = m25p20_Control( CNTRL_GET_SECTNUM, &CmdStruct  );

			if( NO_ERR == ErrorCode )
			{
				// unlock the flash, do the write, increase shift, and wait for completion
				ErrorCode = m25p20_Write( (unsigned short *)&pnData[i], ulAbsoluteAddr, 0x1 );
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
		ErrorCode = m25p20_Control( CNTRL_GET_SECTNUM, &CmdStruct );

		if( NO_ERR == ErrorCode )
		{
			ErrorCode = m25p20_Read( (unsigned short *)&pnData[i], ulAbsoluteAddr, 0x1 );
		}
		else
		{
			return ErrorCode;
		}
	}

	// return the appropriate error code
	return ErrorCode;
}

