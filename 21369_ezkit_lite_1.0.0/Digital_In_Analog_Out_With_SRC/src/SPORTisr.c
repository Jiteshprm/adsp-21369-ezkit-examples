/*********************************************************************************

 Copyright(c) 2012 Analog Devices, Inc. All Rights Reserved.

 This software is proprietary and confidential.  By using this software you agree
 to the terms of the associated Analog Devices License Agreement.

 *********************************************************************************/
/*
NAME:     SPORTisr.c (Block-based Talkthrough)
PURPOSE:  Talkthrough framework for sending and receiving samples to the AD1835.
*/


#include "tt.h"
#include <stdio.h>

void _talkThroughISR_SP0(uint32_t iid, void *handlerarg) /* No argument is passed in this ISR */
{
    int temp1;
    temp1=*pRXSP0A;		// Read new sample from SPDIF Rx*/
   *pTXSP1A=temp1;		// write to SRC1-Input*/
	
}

void _talkThroughISR_SP2(uint32_t iid, void *handlerarg) /* No argument is passed in this ISR */
{	
	int temp2,temp3;
	
	temp2=*pRXSP2A;		/* read new sample from SRC1 Output*/
    temp3=*pRXSP2B;		/* read new sample from ADC*/
	
	*pTXSP3A=temp2;			/* write to DAC4*/
	*pTXSP3B=temp2;			/* write to DAC3*/
	*pTXSP4A=temp3;			/* write to DAC2*/
	*pTXSP4B=temp3;			/* write to DAC1*/

}
