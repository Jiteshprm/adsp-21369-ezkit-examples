/*********************************************************************************

Copyright(c) 2012 Analog Devices, Inc. All Rights Reserved.

This software is proprietary and confidential.  By using this software you agree
to the terms of the associated Analog Devices License Agreement.

*********************************************************************************/

//---- I n c l u d e s  ----//

#include <stdio.h>
#include "../common/flash.h"
#include "../m25p20/m25p20.h"

#ifdef __ADSP21369__
	#include <cdef21369.h>
	#include <def21369.h>
#elif __ADSP21364__
	#include <cdef21364.h>
	#include <def21364.h>
#elif __ADSP21262__
	#include <cdef21262.h>
	#include <def21262.h>
#elif __ADSP21371__
	#include <cdef21371.h>
	#include <def21371.h>
#elif __ADSP21375__
	#include <cdef21375.h>
	#include <def21375.h>
#else
	#error "*** The Flash Driver does not yet support this processor ***"
#endif	
#include <sru.h>
#include <sysreg.h>
	
//---- c o n s t a n t   d e f i n i t i o n s -----//

static char 	*pFlashDesc = "STMicro M25P20";
static char 	*pDeviceCompany	= "STMicroelectronics";
static int		gNumSectors = NUM_SECTORS;
static unsigned long  	ulBlkSize = 0x10000;	// Block size 0

#undef TIMEOUT
#undef DELAY


//---- F u n c t i o n   P r o t o t y p e s  ----//
//            (for  Helper Functions)             //

static ERROR_CODE EraseFlash(unsigned long ulStartAddr);
static ERROR_CODE EraseBlock( int nBlock, unsigned long ulStartAddr );
static ERROR_CODE GetCodes(int *pnManCode, int *pnDevCode, unsigned long ulStartAddr);
static ERROR_CODE GetSectorNumber( unsigned long ulAddr, int *pnSector );
static ERROR_CODE GetSectorStartEnd( unsigned long *ulStartOff, unsigned long *ulEndOff, int nSector );
static ERROR_CODE ReadFlash(unsigned long ulOffset, unsigned short *pusValue );
static ERROR_CODE ResetFlash(void);
static ERROR_CODE WriteFlash(unsigned long ulOffset, unsigned short usValue );
static unsigned long GetFlashStartAddress( unsigned long ulAddr);

//            (for  Private Functions)             //
static ERROR_CODE ReadStatusRegister(int *pStatus);
static ERROR_CODE Wait_For_SPIF(void);
static ERROR_CODE SendSingleCommand( const int nCommand );
static ERROR_CODE Wait_For_RDY( void );
static void Assert_SPI_CS(void);
static void Clear_SPI_CS(void);
static ERROR_CODE WriteByteToSPI(const int byByte, const int msb_lsb);
static ERROR_CODE ReadByteFromSPI(int *pbyByte, const int msb_lsb);


//Flash commands
#define SPI_WREN			(0x06)	//Set Write Enable Latch
#define SPI_WRDI			(0x04)	//Reset Write Enable Latch
#define SPI_RDID			(0x9F)	//Read Identification
#define SPI_RDSR			(0x05)	//Read Status Register
#define SPI_WRSR			(0x01)	//Write Status Register
#define SPI_READ			(0x03)	//Read data from memory
#define SPI_FAST_READ		(0x0B)	//Read data from memory
#define SPI_PP				(0x02)	//Program Data into memory
#define SPI_SE				(0xD8)	//Erase one sector in memory
#define SPI_BE				(0xC7)	//Erase all memory
#define WIP					(0x1)	//Check the write in progress bit of the SPI status register
#define WEL					(0x2)	//Check the write enable bit of the SPI status register

//Flash Properties
#define NRDY			BIT_0
#define PAGE_LENGTH		64  //(in 32-bit words)

//Application definitions.
#define COMMON_SPI_SETTINGS (SPIEN|SPIMS|SENDZ|CPHASE|CLKPL)
#define COMMON_SPI_DMA_SETTINGS (INTEN|SPIDEN)

// 21262 SPI Baud Rate = Core Clock Rate /(4)x(BAUDR)
// 21364 SPI Baud Rate = Peripheral clock rate /(2)x(BAUDR)
// 21369 SPI Baud Rate = Peripheral clock rate /(2)x(BAUDR)
// 21375 SPI Baud Rate = Peripheral clock rate /(2)x(BAUDR)
#define BAUD_RATE_DIVISOR	40


/**
 *****************************************************************************
 * initialize anything needed before accessing the device
 *
 * @return				value if any error occurs
 */
ERROR_CODE m25p20_Open(void)
{
	/* setup baud rate */
	*pSPIBAUD = BAUD_RATE_DIVISOR;

	return (NO_ERR);
}


/**
 *****************************************************************************
 * do cleanup needed to close connection to the device
 *
 * @return				value if any error occurs
 */
ERROR_CODE m25p20_Close(void)
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
ERROR_CODE m25p20_Read( unsigned short *pusData,
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
ERROR_CODE m25p20_Write( unsigned short *pusData,
						 unsigned long ulStartAddress,
						 unsigned int uiCount )
{
	ERROR_CODE Result = NO_ERR;
	unsigned int i = 0;
	unsigned short *pusCurrentData = pusData;
	unsigned long ulCurrentAddress = ulStartAddress;

	for (i = 0; i < uiCount; i++, ulCurrentAddress++, pusCurrentData++)
	{
		// program our actual value now
		Result = WriteFlash( ulCurrentAddress, *pusCurrentData);
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
ERROR_CODE m25p20_Control( unsigned int uiCmd, COMMAND_STRUCT *pCmdStruct)
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

		// get the number of sectors
		case CNTRL_GETNUM_SECTORS:
			pCmdStruct->SGetNumSectors.pnNumSectors[0] = gNumSectors;
			break;

		// reset
		case CNTRL_RESET:
			ErrorCode = ResetFlash();
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
ERROR_CODE ResetFlash(void)
{
	ERROR_CODE ErrorCode = NO_ERR;
	int nStatus;

	ErrorCode = ReadStatusRegister(&nStatus);

	return ErrorCode;
}


/**
 *****************************************************************************
 * Erase the entire flash device
 *
 * @return			value if any error occurs
 */
ERROR_CODE EraseFlash(unsigned long ulAddr)
{
	int nTimeout = 1000;

	if( NO_ERR != SendSingleCommand( SPI_WREN ) )	// write enable
	{
		return POLL_TIMEOUT;
	}

	if( NO_ERR != SendSingleCommand( SPI_BE ) )	// erase command
	{
		return POLL_TIMEOUT;
	}

	// The Wait_For_RDY() function will timeout after 1000 loops,
	// however that is not long enough for an erase, so it's enclosed
	// here to give it 1000 * 1000 loops, long enough for an erase  operation
	while(nTimeout-- > 0 )
	{
		if( NO_ERR == Wait_For_RDY() )
		{
			// send the write disable command
			return SendSingleCommand( SPI_WRDI );	// write disable
		}
	}

	return POLL_TIMEOUT;
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
	int nTimeout = 1000;
	int nSecAddr = 0;

	nSecAddr = (nBlock * ulBlkSize);

	SendSingleCommand( SPI_WREN );	// write enable

	Assert_SPI_CS();

	// 1 byte of data
	if( NO_ERR != WriteByteToSPI( SPI_SE, MSBF ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// 1 byte of address
	if( NO_ERR != WriteByteToSPI( (nSecAddr >> 16), MSBF ) )
{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// 1 byte of address
	if( NO_ERR != WriteByteToSPI( (nSecAddr >> 8), MSBF ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// 1 byte of address
	if( NO_ERR != WriteByteToSPI( nSecAddr, MSBF ) )
		{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	Clear_SPI_CS();

	// The Wait_For_RDY() function will timeout after 1000 loops,
	// however that is not long enough for an erase, so it's enclosed
	// here to give it 1000 * 1000 loops, long enough for an erase  operation
	while(nTimeout-- > 0 )
	{
		if( NO_ERR == Wait_For_RDY() )
		{
			// send the write disable command
			return SendSingleCommand( SPI_WRDI );	// write disable
		}
		}

	return POLL_TIMEOUT;

}


/**
 *****************************************************************************
 * Erase just the block specified
 *
 * @param	pnManCode		pointer to manufacture code
 * @param	pnDevCode		pointer to device code
 *
 * @return					value if any error occurs
 */
ERROR_CODE GetCodes(int *pnManCode, int *pnDevCode, unsigned long ulAddr)
{
	int wWord = 0;

	Assert_SPI_CS();

	if( NO_ERR != WriteByteToSPI( SPI_RDID, MSBF ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// This is a dummy read which pulls in the
	// SO data clocked in from the write.
	if( NO_ERR != ReadByteFromSPI(&wWord, MSBF) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// this is the real data after the write
	if( NO_ERR != ReadByteFromSPI(pnManCode, MSBF) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// This is a dummy read which pulls in the
	// SO data clocked in from the write.
	if( NO_ERR != ReadByteFromSPI(&wWord, MSBF) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// this is the real data after the write
	if( NO_ERR != ReadByteFromSPI(pnDevCode, MSBF) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	Clear_SPI_CS();

	return ResetFlash();

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

	nSector = ulOffset & 0xffff0000;
	nSector = ulOffset >> 16;
	nSector = nSector & 0x000f;

	// if it is a valid sector, set it
	if ( (nSector >= 0) && (nSector < NUM_SECTORS) )
	{
		*pnSector = nSector;
	}
	else	// else it is an invalid sector
	{
		return INVALID_SECTOR;
	}
	
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
	unsigned long ulSectorSize = 0x10000;

	if( ( nSector >= 0 ) && ( nSector < gNumSectors ) ) // 32 sectors
	{
		*ulStartOff = nSector * ulSectorSize;
		*ulEndOff = ( (*ulStartOff) + ulSectorSize ) - 1;
	}
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
unsigned long GetFlashStartAddress( unsigned long ulAddr)
{

	ERROR_CODE 	  ErrorCode = NO_ERR;		//tells us if there was an error erasing flash
	unsigned long ulFlashStartAddr;			//flash start address
	unsigned long ulSectStartAddr;			//sector start address
	unsigned long ulSectEndAddr;			//sector end address
	unsigned long ulMask;					//offset mask

	// get flash start address from absolute address
	GetSectorStartEnd( &ulSectStartAddr, &ulSectEndAddr, (NUM_SECTORS-1));
	ulMask				= ~(ulSectEndAddr);
	ulFlashStartAddr	= ulAddr & ulMask;

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

	Assert_SPI_CS();

	// 1 byte of command
	if( NO_ERR != WriteByteToSPI( SPI_READ, MSBF ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// 1 byte of address
	if( NO_ERR != WriteByteToSPI( (ulAddr >> 16), MSBF ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// 1 byte of address
	if( NO_ERR != WriteByteToSPI( (ulAddr >> 8), MSBF ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// 1 byte of address
	if( NO_ERR != WriteByteToSPI( ulAddr, MSBF ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// 1 byte of garbage data
	if( NO_ERR != ReadByteFromSPI( (int*)pusValue, 0 ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// 1 byte of GOOD data
	if( NO_ERR != ReadByteFromSPI( (int*)pusValue, 0 ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	Clear_SPI_CS();

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

	int iTempCmd;

	SendSingleCommand( SPI_WREN );	// write enable

	Assert_SPI_CS();

	// 1 byte of command
	if( NO_ERR != WriteByteToSPI( SPI_PP, MSBF ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// 1 byte of address
	if( NO_ERR != WriteByteToSPI( (ulAddr >> 16), MSBF ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// 1 byte of address
	if( NO_ERR != WriteByteToSPI( (ulAddr >> 8), MSBF ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// 1 byte of address
	if( NO_ERR != WriteByteToSPI( ulAddr, MSBF ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// 1 byte of data
	if( NO_ERR != WriteByteToSPI( usValue, 0 ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	Clear_SPI_CS();

	// wait for the write to complete.
	if( NO_ERR != Wait_For_RDY() )
	{
		return POLL_TIMEOUT;
	}


	// send the write disable command
	return SendSingleCommand( SPI_WRDI );	// write disable

}


/**
 *****************************************************************************
 * Sends a single command to the SPI flash
 *
 * @param	iCommand			Command to send
 *
 */
ERROR_CODE SendSingleCommand( const int iCommand )
{
	Assert_SPI_CS();

	if( NO_ERR != WriteByteToSPI( iCommand, MSBF ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	Clear_SPI_CS();

	return NO_ERR;
}



/**
 *****************************************************************************
 * Read the status register
 *
 * @param	pStatus		pointer to the status
 *
 * @return				value if any error occurs
 */
ERROR_CODE ReadStatusRegister(int *pStatus)
{
	int wWord = 0;

	// clear the RX buffer
	*pSPICTL |= (RXFLSH);
	asm("nop;");
	asm("nop;");
	asm("nop;");

	Assert_SPI_CS();

	if( NO_ERR != WriteByteToSPI( SPI_RDSR, MSBF ) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// This is a dummy read which pulls in the
	// SO data clocked in from the write.
	if( NO_ERR != ReadByteFromSPI(pStatus, MSBF) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	// this is the real data after the write
	if( NO_ERR != ReadByteFromSPI(pStatus, MSBF) )
	{
		Clear_SPI_CS();
		return POLL_TIMEOUT;
	}

	Clear_SPI_CS();


	return NO_ERR;
}


/**
 *****************************************************************************
 *
 * checks if Spi single word xfer is complete
 *
 */
ERROR_CODE Wait_For_SPIF(void)
{
	int nTimeout = 10000;
	// status updates can be delayed up to 10 cycles
	// so wait at least 10 cycles before even
	// checking them
	int n;

	// make sure nothing is waiting to be sent
	while( !(SPIF & *pSPISTAT) )
	{
		if( nTimeout-- < 0 )
		{
			return POLL_TIMEOUT;
		}
	}

	return NO_ERR;
}


/**
 *****************************************************************************
 *
 * checks status register to see if NRDY is clear
 *
 */
ERROR_CODE Wait_For_RDY( void )
{
	int nTimeout = 10000;
	int n;
	int iTest;

	while(nTimeout-- > 0)
	{
		ReadStatusRegister(&iTest);
		if( !(iTest & NRDY) )
		{
			return NO_ERR;
		}
	}

	// we can return
	return POLL_TIMEOUT;
}


/**
 *****************************************************************************
 * Writes a byte to SPI
 *
 * @param	byByte		byte to write
 * @param	msb_lsb		specifies lsb or msb
 *
 * @return				value if any error occurs
 */
ERROR_CODE WriteByteToSPI(const int byByte, const int msb_lsb)
{
	int nTimeOut = 100000;
	int n;

	if( NO_ERR != Wait_For_SPIF() )
	{
		return POLL_TIMEOUT;
	}

	while( (TXS & *pSPISTAT) )
	{
		if( nTimeOut-- < 0 )
		{
			return POLL_TIMEOUT;
		}
	}

	*pSPICTL = (SPIEN|SPIMS|SENDZ|TIMOD1|WL8|msb_lsb);
	asm("nop;");
	asm("nop;");
	asm("nop;");
	*pTXSPI = byByte;

	if( NO_ERR != Wait_For_SPIF() )
	{
		return POLL_TIMEOUT;
	}

	return NO_ERR;
}


/**
 *****************************************************************************
 * Reads a byte from SPI
 *
 * @param	pbyByte		ptr to byte being read
 * @param	msb_lsb		specifies lsb or msb
 *
 * @return				value if any error occurs
 */
ERROR_CODE ReadByteFromSPI(int *pbyByte, const int msb_lsb)
{
	int nTimeOut = 1000;

	if( NO_ERR != Wait_For_SPIF() )
	{
		return POLL_TIMEOUT;
	}

	// don't read until there is something to read.
	nTimeOut = 1000;
	while( !(RXS & *pSPISTAT) )
	{
		if( nTimeOut-- < 0 )
		{
			return POLL_TIMEOUT;
		}
	}

	*pSPICTL = (SPIEN|SPIMS|SENDZ|WL8|msb_lsb);
	asm("nop;");
	asm("nop;");
	asm("nop;");
	*pbyByte = *pRXSPI;

	return NO_ERR;

}


/**
 *****************************************************************************
 * Asserts the SPI chip select
 *
 * @return				value if any error occurs
 */
void Assert_SPI_CS(void)
{
	int n;

#if ( defined(__ADSP21371__) || defined(__ADSP21375__) || defined(__ADSP21369__) )
	//Then control the level of flag 4
	sysreg_bit_clr( sysreg_FLAGS, FLG4 );  //logic low
#elif (__ADSP21364__) || (__ADSP21262__)
	//Then control the level of flag 0
	sysreg_bit_clr( sysreg_FLAGS, FLG0 );  //logic low
#endif

	*pSPIBAUD = BAUD_RATE_DIVISOR;
}

/**
 *****************************************************************************
 * De-asserts the SPI chip select
 *
 * @return				value if any error occurs
 */
void Clear_SPI_CS(void)
{
	int n;

#if ( defined(__ADSP21371__) || defined(__ADSP21375__) || defined(__ADSP21369__) )
	//Then control the level of flag 4
	sysreg_bit_set( sysreg_FLAGS, FLG4 );  //Logic high
#elif (__ADSP21364__) || (__ADSP21262__)
	//Then control the level of flag 0
	sysreg_bit_set( sysreg_FLAGS, FLG0 );  //Logic high
#endif
	*pSPIBAUD = 0;
}
