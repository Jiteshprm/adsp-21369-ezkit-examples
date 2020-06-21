/********************************************************************************/
/*																				*/
/*   (C) Copyright 2012 - Analog Devices, Inc.  All rights reserved.			*/
/*																				*/
/*    FILE:     m25p20.h													*/
/*																				*/
/*  PURPOSE:  This header file defines items specific to the m25p20 flash.	*/
/*																				*/
/********************************************************************************/

#ifndef __ADI_M25P20_H__
#define __ADI_M25P20_H__

//---- I n c l u d e s  ----//
#include "../common/flash.h"

#define NUM_SECTORS 	4			// number of sectors in the flash device

/* interface function prototypes */
ERROR_CODE m25p20_Open(void);							/* open the flash device */
ERROR_CODE m25p20_Close(void);							/* close the flas device */
ERROR_CODE m25p20_Read(	unsigned short *pusData,		/* read some bytes from flash */
						unsigned long ulStartAddress,
						unsigned int uiCount );
ERROR_CODE m25p20_Write(unsigned short *pusData,		/* write some bytes to flash */
						unsigned long ulStartAddress,
						unsigned int uiCount );
ERROR_CODE m25p20_Control( unsigned int uiCmd,			/* send a command to device */
						   COMMAND_STRUCT *pCmdStruct);


#endif	// __ADI_M25P20_H__
