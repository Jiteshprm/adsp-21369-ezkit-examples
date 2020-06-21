/*********************************************************************************

 Copyright(c) 2012 Analog Devices, Inc. All Rights Reserved.

 This software is proprietary and confidential.  By using this software you agree
 to the terms of the associated Analog Devices License Agreement.

 *********************************************************************************/

/*

 NAME:     main.c (Block-based Talkthrough)
 PURPOSE:  Talkthrough framework for sending and receiving samples to the AD1835.
 USAGE:    This file contains the main routine calls functions to set up the talkthrough
           routine. It receives an input signal from the SPDIF Rx via SPORT0A and outputs
           to DAC's via SPORT1A, SPORT1B, SPORT2A, and SPORT2B.
*/

#include "tt.h"

void main(void)
{
	 int count=0;

	adi_initComponents();

    /*Initialize PLL to run at CCLK= 331.776 MHz & SDCLK= 165.888 MHz.*/
    initPLL();
    initExternalMemory();
    /* Need to initialize DAI because the sport signals need to be routed*/
    InitSRU();
    
    /*This function will configure the codec on the kit*/
    Init1835viaSPI();

    /* Initialize SPDIF receiver*/
    InitSPDIF();

   
    /* Finally setup the sport to receive / transmit the data*/
    initSport();
    
    InitSRC();

    adi_int_InstallHandler(ADI_CID_SP0I,(ADI_INT_HANDLER_PTR )_talkThroughISR_SP0,0,true);

    adi_int_InstallHandler(ADI_CID_SP2I,(ADI_INT_HANDLER_PTR )_talkThroughISR_SP2,0,true);


    /* Be in infinite loop and do nothing until done.*/
    while(1)
    	{
    	    count++;
    		if(count==10)
    		{
    			puts("Talkthrough is running successfully");
    		}

    	}

}
