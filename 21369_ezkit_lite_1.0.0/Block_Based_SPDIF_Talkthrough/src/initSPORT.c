/*********************************************************************************

 Copyright(c) 2012 Analog Devices, Inc. All Rights Reserved.

 This software is proprietary and confidential.  By using this software you agree
 to the terms of the associated Analog Devices License Agreement.

 *********************************************************************************/

/*

NAME:     initSPORT.c (Block-based Talkthrough)
PURPOSE:  Talkthrough framework for sending and receiving samples to the AD1835.
USAGE:    This file uses SPORT0 to receive data from the SPDIF Rx and transmits the
        data to the SPDIF Tx via SPORT3A. DMA Chaining is enabled

*/

#include "ADDS_21369_EzKit.h"

/* TCB blocks for Chaining*/
/* Input ping pong blocks*/
unsigned int InBlock_A[NUM_SAMPLES] ;
unsigned int InBlock_B[NUM_SAMPLES] ;
/* Output ping pong blocks*/
unsigned int OutBlock_C[NUM_SAMPLES] ;
unsigned int OutBlock_D[NUM_SAMPLES] ;

/*Set up the TCBs to rotate automatically*/
int TCB_Block_InA[4] = { 0, sizeof(InBlock_A), 1, 0};;
int TCB_Block_InB[4] = { 0, sizeof(InBlock_B), 1, 0};
int TCB_Block_OutC[4] = { 0, sizeof(OutBlock_C), 1, 0};
int TCB_Block_OutD[4] = { 0, sizeof(OutBlock_D), 1, 0};

void InitSPORT()
{
    /*Proceed from Block A to Block B*/
    TCB_Block_InA[0] = (((int) TCB_Block_InB + 3)& OFFSET_MASK)|PCI ;
    TCB_Block_InA[3] = ((unsigned int) InBlock_A)& OFFSET_MASK ;

    /*Proceed from Block B to Block A*/
    TCB_Block_InB[0] = (((int) TCB_Block_InA + 3)& OFFSET_MASK)|PCI ;
    TCB_Block_InB[3] = ((unsigned int) InBlock_B)& OFFSET_MASK ;

    /*Proceed from Block C to Block D*/
    TCB_Block_OutC[0] = (((int) TCB_Block_OutD + 3)& OFFSET_MASK)|PCI ;
    TCB_Block_OutC[3] = ((unsigned int) OutBlock_C)& OFFSET_MASK ;

    /*Proceed from Block C to Block D*/
    TCB_Block_OutD[0] = (((int) TCB_Block_OutC + 3)& OFFSET_MASK)|PCI ;
    TCB_Block_OutD[3] = ((unsigned int) OutBlock_D)& OFFSET_MASK ;

   /* SPORT Loopback: Use SPORT0 as RX & SPORT3 as TX  */

    /*initially clear SPORT control register */

    *pSPMCTL0 = 0 ;
    *pSPMCTL3 = 0 ;
    *pSPCTL0 = 0 ;
    *pSPCTL3 = 0 ;

    /* Configure SPORT0 as a receiver externally generating clock and frame sync*/
    *pDIV0 = 0;
    *pSPCTL0 = (SPEN_A|    /* Enable Channel A */
         SLEN24|    /* 32-bit word length */
         OPMODE|    /* Enable I2S Mode*/
         L_FIRST|   /* I2S sends left channel first */
         SDEN_A|    /* Enable Channel A DMA */
         SCHEN_A);  /* Enable Channel A DMA Chaining */

    /* Enabling Chaining*/
    /* Block A will be filled first*/
    *pCPSP0A = ((unsigned int) TCB_Block_InA +3 )& OFFSET_MASK;


    /* Configure SPORT3 as a transmitter */
    *pDIV3 = 0;
    *pSPCTL3 = (SPEN_A|    /* Enable Channel A */
         L_FIRST|   /* I2S sends left channel first */
         SLEN32|    /* 32-bit word length */
         SPTRAN|    /* Transmit on enabled channels */
         SDEN_A|    /* Enable Channel A DMA */
         SCHEN_A|   /* Enable Channel A DMA Chaining */
         OPMODE);   /* Enable I2S Mode*/
    *pCPSP3A = ((unsigned int) TCB_Block_OutC +3 )& OFFSET_MASK;


}
