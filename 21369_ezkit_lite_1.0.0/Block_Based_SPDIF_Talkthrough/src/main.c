/*********************************************************************************

 Copyright(c) 2012 Analog Devices, Inc. All Rights Reserved.

 This software is proprietary and confidential.  By using this software you agree
 to the terms of the associated Analog Devices License Agreement.

 *********************************************************************************/

/*
  NAME:     main.c (Block_Based_SPDIF_Talkthrough)
  PURPOSE:  Talkthrough framework for sending and receiving samples to the SPDIF peripherals.
  USAGE:    This file contains the main routine calls functions to set up the talkthrough
            routine. It receives an input signal from the SPDIF Rx via SPORT0A and outputs
            to DAC's via SPORT3A.
*/

#include "ADDS_21369_EzKit.h"

void main(void)
{

	int count=0;
	/* Initialize managed drivers and/or services at the start of main(). */
	adi_initComponents();

    /* SPORT0 interrupt calls TalkThroughISR*/
    adi_int_InstallHandler(ADI_CID_SP0I,(ADI_INT_HANDLER_PTR )TalkThroughISR,0,true);
    /*Initialize PLL to run at CCLK= 331.776 MHz & SDCLK= 165.888 MHz.*/
    initPLL();
    initExternalMemory();

    /* Need to initialize DAI because the sport signals need to be routed*/
    InitSRU();

    /* Set up SPDIF transmitter and receiver.*/
    InitSPDIF();

    /* Finally setup the sport to receive / transmit the data*/
    InitSPORT();

    /* Be in infinite loop and do nothing until done.*/

    while(1)
    {

        if(blockReady)
        {
        		if(count==10)
        			  {
        			        puts("Talkthrough is running successfully");
        			   }
        		processBlock(insrc_pointer[int_cntr],outsrc_pointer[int_cntr]);
        		count++;
        }
     }
}
