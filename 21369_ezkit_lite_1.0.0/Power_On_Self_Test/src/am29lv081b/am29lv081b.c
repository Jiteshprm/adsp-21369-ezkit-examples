/*********************************************************************************

Copyright(c) 2012 Analog Devices, Inc. All Rights Reserved.

This software is proprietary and confidential.  By using this software you agree
to the terms of the associated Analog Devices License Agreement.

*********************************************************************************/

//----- I n c l u d e s  -----//

#include "am29lv081b.h"
#include "../common/flash.h"
#ifdef __ADSP21262__
	#include <def21262.h>
	#include <Cdef21262.h>
#elif __ADSP21364__
	#include <def21364.h>
	#include <Cdef21364.h>
#elif __ADSP21371__
	#include <def21371.h>
	#include <Cdef21371.h>
#elif __ADSP21375__
	#include <def21375.h>
	#include <Cdef21375.h>
#endif
	
//---- c o n s t a n t   d e f i n i t i o n s -----//

static char 	*pFlashDesc		=	"AMD AM29LV081B";
static char 	*pDeviceCompany	=	"AMD";

//---- F u n c t i o n   P r o t o t y p e s  ----//
//            (for  Helper Functions)             //

static ERROR_CODE EraseFlash(unsigned long ulStartAddr);
static ERROR_CODE EraseBlock( int nBlock, unsigned long ulStartAddr );
static ERROR_CODE GetCodes(int *pnManCode, int *pnDevCode, unsigned long ulStartAddr);
static ERROR_CODE GetSectorNumber( unsigned long ulAddr, int *pnSector );
static ERROR_CODE GetSectorStartEnd( unsigned long *ulStartOff, unsigned long *ulEndOff, int nSector );
static ERROR_CODE PollToggleBit( const unsigned long ulOffset, const unsigned short usValue );
static ERROR_CODE ReadFlash(const unsigned long ulOffset, unsigned short *pusValue );
static ERROR_CODE ResetFlash(unsigned long ulStartAddr);
static ERROR_CODE WriteFlash(const unsigned long ulOffset, const unsigned short usValue );
static unsigned long GetFlashStartAddress( unsigned long ulAddr );


/**
 *****************************************************************************
 * initialize anything needed before accessing the device
 *
 * @return				value if any error occurs
 */
ERROR_CODE am29lv081b_Open(void)
{
	return (NO_ERR);
}


/**
 *****************************************************************************
 * do cleanup needed to close connection to the device
 *
 * @return				value if any error occurs
 */
ERROR_CODE am29lv081b_Close(void)
{
	return (NO_ERR);
}


/**
 *****************************************************************************
 * Read from the flash device
 *
 * @param	pusData			pointer to the buffer to store flash contents in
 * @param	ulStartAddress	flash start address to read
 * @param	uiCount			number of locations to read( each byte is stored
 * 							as 16 bits so the upper byte is 0x00 )
 *
 * @return					value if any error occurs
 */
ERROR_CODE am29lv081b_Read( unsigned short *pusData,
							unsigned long ulStartAddress,
							unsigned int uiCount )
{
	ERROR_CODE Result = NO_ERR;
	unsigned int i = 0;
	unsigned short *pusCurrentData = pusData;
	unsigned long ulCurrentAddress = ulStartAddress;
	
	for (i = 0; i < uiCount; i++, ulCurrentAddress++, pusCurrentData++)
	{
		Result = ReadFlash( ulCurrentAddress, pusCurrentData );
	}

	return(Result);
}


/**
 *****************************************************************************
 * Write the flash device
 *
 * @param	pusData			pointer to the buffer to write
 * @param	ulStartAddress	flash start address to write
 * @param	uiCount			number of locations to write
 *
 * @return				value if any error occurs
 */
ERROR_CODE am29lv081b_Write( unsigned short *pusData,
							 unsigned long ulStartAddress,
							 unsigned int uiCount )
{
ERROR_CODE Result = NO_ERR;
	unsigned int i = 0;
	unsigned short *pusCurrentData = pusData;
	unsigned long ulCurrentAddress = ulStartAddress;
	unsigned long ulFlashStartAddr;			//flash start address
	ulFlashStartAddr = GetFlashStartAddress(ulStartAddress);

	for (i = 0; i < uiCount; i++, ulCurrentAddress++, pusCurrentData++)
	{

#if ( defined(__ADSP21371__) || defined(__ADSP21375__) || defined(__ADSP21369__) )
		WriteFlash( ulCurrentAddress, 0x00 );
		WriteFlash( ulCurrentAddress, 0xaa );
		WriteFlash( ulCurrentAddress, 0x55 );
		WriteFlash( ulCurrentAddress, 0xa0 );
#else
		WriteFlash( ulFlashStartAddr ,  0xa055aa00 );
#endif
		// program our actual value now
		Result = WriteFlash( ulCurrentAddress, *pusCurrentData);

		// make sure the write was successful
		Result = PollToggleBit(ulCurrentAddress, *pusCurrentData & 0xff);
	}

	return(Result);
}


/**
 *****************************************************************************
 * This function takes a command and calls the appropriate function
 *
 * @param	uiCmd			command being processed
 * @param	pCmdStruct		information needed to process command
 *
 * @return					value if any error occurs
 */
ERROR_CODE am29lv081b_Control( unsigned int uiCmd, COMMAND_STRUCT *pCmdStruct)
{
	ERROR_CODE ErrorCode = NO_ERR;

	// switch on the command
	switch ( uiCmd )
	{
		// erase all
		case CNTRL_ERASE_ALL:
			ErrorCode = EraseFlash(pCmdStruct->SEraseAll.ulFlashStartAddr);
			break;

		// erase sector
		case CNTRL_ERASE_SECT:
			ErrorCode = EraseBlock( pCmdStruct->SEraseSect.nSectorNum, pCmdStruct->SEraseSect.ulFlashStartAddr );
			break;

		// get manufacturer and device codes
		case CNTRL_GET_CODES:
			ErrorCode = GetCodes((int *)pCmdStruct->SGetCodes.pManCode, (int *)pCmdStruct->SGetCodes.pDevCode, (unsigned long)pCmdStruct->SGetCodes.ulFlashStartAddr);
			break;

		case CNTRL_GET_DESC:
			//Filling the contents with data
			//pCmdStruct->SGetDesc.pTitle = pEzKitTitle;
			pCmdStruct->SGetDesc.pDesc  = pFlashDesc;
			pCmdStruct->SGetDesc.pFlashCompany  = pDeviceCompany;
			break;

		// get sector number based on address
		case CNTRL_GET_SECTNUM:
			ErrorCode = GetSectorNumber( pCmdStruct->SGetSectNum.ulOffset, (int *)pCmdStruct->SGetSectNum.pSectorNum );
			break;

		// get sector number start and end offset
		case CNTRL_GET_SECSTARTEND:
			ErrorCode = GetSectorStartEnd( pCmdStruct->SSectStartEnd.pStartOffset, pCmdStruct->SSectStartEnd.pEndOffset, pCmdStruct->SSectStartEnd.nSectorNum );
			break;

		// reset
		case CNTRL_RESET:
			ErrorCode = ResetFlash(pCmdStruct->SReset.ulFlashStartAddr);
			break;

		// no command or unknown command do nothing
		default:
			// set our error
			ErrorCode = UNKNOWN_COMMAND;
			break;
	}

	// return
	return(ErrorCode);

}


/**
 *****************************************************************************
 * Reset the flash device
 *
 * @return			value if any error occurs
 */
ERROR_CODE ResetFlash(unsigned long ulAddr)
{

	unsigned long ulFlashStartAddr;		//flash start address

	// get flash start address from absolute address
	// The ulAddr should ideally be pointing to the flash start
	// address. However we just verify it here again.
	ulFlashStartAddr = GetFlashStartAddress(ulAddr);

	// send the reset command to the flash
	// return to standard operation mode
#if ( defined(__ADSP21371__) || defined(__ADSP21375__) || defined(__ADSP21369__) )
	WriteFlash( ulFlashStartAddr + 0, 0x00 );
	WriteFlash( ulFlashStartAddr + 1, 0x00 );
	WriteFlash( ulFlashStartAddr + 2, 0x00 );
	WriteFlash( ulFlashStartAddr + 3, 0xF0 );
#else
	WriteFlash( ulFlashStartAddr, 0xF0000000 );
#endif

	// reset should be complete
	return NO_ERR;
}


/**
 *****************************************************************************
 * Erase the entire flash device
 *
 * @return				value if any error occurs
 */
ERROR_CODE EraseFlash(unsigned long ulAddr)
{
	ERROR_CODE ErrorCode = NO_ERR;	// tells us if there was an error erasing flash
	unsigned long ulFlashStartAddr;	// flash start address

	// get flash start address from absolute address
	// The ulAddr should ideally be pointing to the flash start
	// address. However we just verify it here again.
	ulFlashStartAddr = GetFlashStartAddress(ulAddr);

	// erase contents in Main Flash Array
#if ( defined(__ADSP21371__) || defined(__ADSP21375__) || defined(__ADSP21369__) )
	WriteFlash( ulFlashStartAddr + 0, 0x00 );
	WriteFlash( ulFlashStartAddr + 1, 0x00 );
	WriteFlash( ulFlashStartAddr + 2, 0xaa );
	WriteFlash( ulFlashStartAddr + 3, 0x55 );

	WriteFlash( ulFlashStartAddr + 0, 0x80 );
	WriteFlash( ulFlashStartAddr + 1, 0xaa );
	WriteFlash( ulFlashStartAddr + 2, 0x55 );
	WriteFlash( ulFlashStartAddr + 3, 0x10 );
#else
	WriteFlash( ulFlashStartAddr + 0, 0x55aa0000 );
	WriteFlash( ulFlashStartAddr + 1, 0x1055aa80 );
#endif

		
		// poll until the command has completed
	ErrorCode = PollToggleBit(ulFlashStartAddr, 0xFF);

	// erase should be complete
	return ErrorCode;
}


/**
 *****************************************************************************
 * Erase just the block specified
 *
 * @param	nBlock		block to erase
 *
 * @return				value if any error occurs
 */
ERROR_CODE EraseBlock( int nBlock, unsigned long ulAddr )
{

	ERROR_CODE 	  ErrorCode   = NO_ERR;		//tells us if there was an error erasing flash
	unsigned long ulSectStart = 0x0;		//stores the sector start offset
	unsigned long ulSectEnd   = 0x0;		//stores the sector end offset(however we do not use it here)
	unsigned long ulFlashStartAddr;			//flash start address

	// get flash start address from absolute address
	// The ulAddr should ideally be pointing to the flash start
	// address. However we just verify it here again.
	ulFlashStartAddr = GetFlashStartAddress(ulAddr);

	// Get the sector start offset
	// we get the end offset too however we do not actually use it for Erase sector
	GetSectorStartEnd( &ulSectStart, &ulSectEnd, nBlock );

	// send the erase block command to the flash
#if ( defined(__ADSP21371__) || defined(__ADSP21375__) || defined(__ADSP21369__) )
	WriteFlash( (ulFlashStartAddr + ulSectStart + 0), 0x00 );
	WriteFlash( (ulFlashStartAddr + ulSectStart + 1), 0x00 );
	WriteFlash( (ulFlashStartAddr + ulSectStart + 2), 0xaa );
	WriteFlash( (ulFlashStartAddr + ulSectStart + 3), 0x55 );

	WriteFlash( (ulFlashStartAddr + ulSectStart + 0), 0x80 );
	WriteFlash( (ulFlashStartAddr + ulSectStart + 1), 0xaa );
	WriteFlash( (ulFlashStartAddr + ulSectStart + 2), 0x55 );
	WriteFlash( (ulFlashStartAddr + ulSectStart + 3), 0x30 );
#else
	WriteFlash( (ulFlashStartAddr + ulSectStart ), 0x55aa0000 );
	WriteFlash( (ulFlashStartAddr + ulSectStart ), 0x3055aa80 );
#endif
	
	// poll until the command has completed
	ErrorCode = PollToggleBit(ulFlashStartAddr + ulSectStart, 0xFF);

	// block erase should be complete
	return ErrorCode;
}


/**
 *****************************************************************************
 * Checks to see if the operationg has completed
 *
 * @param	ulOffset	offset to poll
 * @param	usValue		value that we are comparing to see if the polling
 *						is complete
 *
 * @return				value if any error occurs
 */
ERROR_CODE PollToggleBit( const unsigned long ulOffset, const unsigned short usValue )
{
	bool bError = true;				// flag to indicate error
	bool bPass = false;				// flag indicating passing
	int nTimeOut = 0x1FFFFF;		// timeout after a while
	ERROR_CODE ErrorCode = NO_ERR;	// flag to indicate error
	unsigned short nReadVal = 0;

	while( bError && !bPass )
	{
		// read the data
		ReadFlash( ulOffset, &nReadVal );

		// see if the data read == data written
		if( (nReadVal & 0xFF) != usValue )
		{
			// check DQ5 bit for error
			if( (nReadVal & 0x20) == 0x20 )
				bError = false;
		}
		else
			bPass = true;
	}

	// if we didn't pass yet then make sure DQ7 was
	// not changing simultaneously with DQ5
	if( !bPass )
	{
		ReadFlash( ulOffset, &nReadVal );

		// see if the data read == data written
		if( (nReadVal & 0xFF) == usValue )
			bPass = true;
	}

	if( !bPass )
		ErrorCode = POLL_TIMEOUT;

	// we can return
	return ErrorCode;
}


/**
 *****************************************************************************
 * Erase just the block specified
 *
 * @param	pnManCode		pointer to manufacture code
 * @param	pnDevCode		pointer to device code
 * @param	ulAddr			flash start address
 *
 * @return					value if any error occurs
 */
ERROR_CODE GetCodes(int *pnManCode, int *pnDevCode, unsigned long ulAddr)
{

	unsigned long ulFlashStartAddr;		//flash start address

	// get flash start address from absolute address
	// The ulAddr should ideally be pointing to the flash start
	// address. However we just verify it here again.
	ulFlashStartAddr = GetFlashStartAddress(ulAddr);

	// send the auto select command to the flash
	// return to standard operation mode
#if ( defined(__ADSP21371__) || defined(__ADSP21375__) || defined(__ADSP21369__) )
	WriteFlash( ulFlashStartAddr + 0, 0x00 );
	WriteFlash( ulFlashStartAddr + 1, 0xAA );
	WriteFlash( ulFlashStartAddr + 2, 0x55 );
	WriteFlash( ulFlashStartAddr + 3, 0x90 );
#else
	WriteFlash( ulFlashStartAddr, 0x9055aa00 );
#endif

	// read the manufacturer code
	ReadFlash( ulFlashStartAddr , (unsigned short *)pnManCode );
	*pnManCode &= 0x00FF;

	ReadFlash( ulFlashStartAddr + 1, (unsigned short *)pnDevCode );
	*pnDevCode &= 0x00FF;

	// we need to issue another command to get the part out
	// of auto select mode so issue a reset which just puts
	// the device back in read mode
	ResetFlash(ulAddr);

	// ok
	return NO_ERR;
}


/**
 *****************************************************************************
 * Gets a sector number based on the offset.
 *
 * @param	ulAddr			absolute address
 * @param	pnSector		pointer to sector number
 *
 * @return					value if any error occurs
 */
ERROR_CODE GetSectorNumber( unsigned long ulAddr, int *pnSector )
{
	int nSector = 0;
	
	// get offset from absolute address
	unsigned long ulMask  = 0x3fffff;		 //offset mask
	unsigned long ulOffset= ulAddr & ulMask; //offset
	
	// determine the sector
	nSector = ulOffset & 0xffff0000;
	nSector = ulOffset >> 16;
	nSector = nSector & 0x000f;

	// if it is a valid sector, set it
	if ( (nSector >= 0) && (nSector < NUM_SECTORS) )
		*pnSector = nSector;

	// else it is an invalid sector
	else
		return INVALID_SECTOR;

	// ok
	return NO_ERR;
}


/**
 *****************************************************************************
 * Gets a sector start and end address based on the sector number.
 *
 * @param	ulStartOff		pointer to the start offset
 * @param	ulEndOff		pointer to the end offset
 * @param	nSector			sector number
 *
 * @return					value if any error occurs
 */
ERROR_CODE GetSectorStartEnd( unsigned long *ulStartOff, unsigned long *ulEndOff, int nSector )
{
	// The blocks in the flash memory are asymmetrically arranged
	// Thus we use block sizes to determine the block addresses
	unsigned long  ulBlkSize 	= 0x10000;	// Block size 0

	// block is in main flash
	if( ( nSector < NUM_SECTORS ))
	{
		*ulStartOff =(nSector * ulBlkSize );
		*ulEndOff = ( (*ulStartOff) + ulBlkSize ) - 1;
	}
	// no such sector
	else
	{
		return INVALID_SECTOR;
	}

	// ok
	return NO_ERR;
}


/**
 *****************************************************************************
 * Gets the start address of teh device
 *
 * @param	ulAddr			believed flash start addr
 *
 * @return					value if any error occurs
 */
unsigned long GetFlashStartAddress( unsigned long ulAddr )
{

	ERROR_CODE 	  ErrorCode = NO_ERR;		// tells us if there was an error erasing flash
	unsigned long ulFlashStartAddr;			// flash start address
	unsigned long ulSectStartAddr;			// sector start address
	unsigned long ulSectEndAddr;			// sector end address
	unsigned long ulMask;					// offset mask

	// get flash start address from absolute address
	GetSectorStartEnd( &ulSectStartAddr, &ulSectEndAddr, (NUM_SECTORS-1));
	ulMask      	  = ~(ulSectEndAddr);
	ulFlashStartAddr  =  ulAddr & ulMask;

	return(ulFlashStartAddr);
}


/**
 *****************************************************************************
 * Reads a value from flash
 *
 * @param	ulAddr			address to read
 * @param	pusValue		ptr to the value read
 *
 * @return					value if any error occurs
 */
ERROR_CODE ReadFlash( const unsigned long ulAddr, unsigned short *pusValue )
{
#if ( defined(__ADSP21371__) ||  defined(__ADSP21375__) || defined(__ADSP21369__) )
	// set our flash address to where we want to read
	volatile unsigned short *pFlashAddr = (volatile unsigned short *)(ulAddr);

	// read the value
	*pusValue = (unsigned short)*pFlashAddr;

#else //if __ADSP21364__ & __ADSP21262__
	
	//--- Core-Driven Transfer to read from Parallel Port ---//
	unsigned short val = 0;

	unsigned short ReadEnable = PPDUR32 | PPEN;  // enables PP read
	
	// Initiate parallel port registers ---/
	*pEMPP = 0;		// setup DMA modify
	*pEIPP = ulAddr;// setup DMA destination
	*pICPP = 1;
	*pECPP = 1;
			
	// Initialize PPCTL register with appropriate settings
	*pPPCTL = ReadEnable;  // enables PP read	

	// Wait two core-clock cycles after PPEN is set for the parallel 
	// port to fetch two 32-bit data words
	asm("nop;");
	asm("nop;");

	val = (*pRXPP) & 0xFF;
			
	// poll to ensure parallel port has completed the transfer
	do{
	;}while( (*pPPCTL & (PPBS|PPDS)) != 0);

	// Fetch data from RXPP
	*pPPCTL = 0; // clear dma control register

	*pusValue = val; // put the value at the location passed in
#endif

	// ok
	return NO_ERR;
}


/**
 *****************************************************************************
 * Writes a value to flash
 *
 * @param	ulAddr		address to write
 * @param	usValue		value that will be written
 *
 * @return				value if any error occurs
 */
ERROR_CODE WriteFlash( const unsigned long ulAddr, const unsigned short usValue )
{

#if ( defined(__ADSP21371__) || defined(__ADSP21375__) || defined(__ADSP21369__) )
	// set the address
	volatile unsigned short *pFlashAddr = (volatile unsigned short *)(ulAddr);

	*pFlashAddr = usValue;
	
#else // if __ADSP21364__ or __ADSP21262__

	//--- Core-Driven Transfer to write to Parallel Port ---//
	unsigned short WriteEnable = PPDUR32 | PPTRAN  | PPEN;  // enables PP write

	//--- Initiate parallel port registers ---/
	*pEMPP = 0;
	*pEIPP = ulAddr;
	*pICPP = 1;
	*pECPP = 4;
	
	// Initialize PPCTL register with appropriate settings
	*pPPCTL = WriteEnable;
	*pTXPP = usValue;
	*pPPCTL |= PPDEN|PPEN;
	
	//poll to ensure parallel port has completed the transfer
	do{
	;}while( (*pPPCTL & (PPBS|PPDS)) != 0);

	// clear dma control register
	*pPPCTL = 0;
	
#endif
	// ok
	return NO_ERR;
}
