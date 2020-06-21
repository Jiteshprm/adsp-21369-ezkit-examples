/********************************************************************************/
/*																				*/
/*   (C) Copyright 2012 - Analog Devices, Inc.  All rights reserved.			*/
/*																				*/
/*    FILE:     am29lv081b.h													*/
/*																				*/
/*  PURPOSE:  This header file defines items specific to the am29lv081b flash.	*/
/*																				*/
/********************************************************************************/

#ifndef __ADI_AM29LV081B_H__
#define __ADI_AM29LV081B_H__

#include "../common/flash.h"

#define NUM_SECTORS 	16	// number of sectors in the flash device

/* interface function prototypes */
ERROR_CODE am29lv081b_Open(void);						/* open the flash device */
ERROR_CODE am29lv081b_Close(void);						/* close the flas device */
ERROR_CODE am29lv081b_Read(	unsigned short *pusData,		/* read some bytes from flash */
							unsigned long ulStartAddress,
							unsigned int uiCount );
ERROR_CODE am29lv081b_Write(unsigned short *pusData,	/* write some bytes to flash */
							unsigned long ulStartAddress,
							unsigned int uiCount );
ERROR_CODE am29lv081b_Control(  unsigned int uiCmd,		/* send a command to device */
								COMMAND_STRUCT *pCmdStruct);


#endif	// __ADI_AM29LV081B_H__
