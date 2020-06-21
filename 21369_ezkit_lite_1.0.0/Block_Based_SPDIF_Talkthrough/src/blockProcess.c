/*********************************************************************************

 Copyright(c) 2012 Analog Devices, Inc. All Rights Reserved.

 This software is proprietary and confidential.  By using this software you agree
 to the terms of the associated Analog Devices License Agreement.

 *********************************************************************************/
#include "ADDS_21369_EzKit.h"

/* Place the audio processing algorithm here. The input and output are given
   as unsigned integer pointers.*/

void processBlock(unsigned int *inblock_ptr,unsigned int *outblock_ptr)
{
    int i;
    float temp_out;

    /*Clear the Block Ready Semaphore*/
    blockReady = 0;

    /*Set the Processing Active Semaphore before starting processing*/
    isProcessing = 1;

    /* Input data is 24-bit, we need to set the block start bit for the first word
    of the block and make 32-bit words. Data is contained in upper 24-bits of output
    word */
    /* Shift left by 8 bits and set block start bit*/
    *(outblock_ptr) =  ((*(inblock_ptr))<<8 | 0x10);
    *(outblock_ptr+1) =  ((*(inblock_ptr+1))<<8 | 0x10);

    for(i=2; i<NUM_SAMPLES; i++)
    {
        /* Shift left by 8 bits*/
       *(outblock_ptr+i) =  ((*(inblock_ptr+i))<<8);
    }

    /*Clear the Processing Active Semaphore after processing is complete*/
    isProcessing = 0;
}
