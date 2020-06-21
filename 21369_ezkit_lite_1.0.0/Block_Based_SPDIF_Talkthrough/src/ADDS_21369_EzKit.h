/*********************************************************************************

 Copyright(c) 2012 Analog Devices, Inc. All Rights Reserved.

 This software is proprietary and confidential.  By using this software you agree
 to the terms of the associated Analog Devices License Agreement.

 *********************************************************************************/

/*
 NAME:     ADDS_21369_EzKit,h
 PURPOSE:  Header file with definitions use in the C-based talkthrough examples
*/
#ifndef __ADDS_21369_EZKIT_H__
#define __ADDS_21369_EZKIT_H__
/* include files */

#include <stdio.h>     /* Get declaration of puts and definition of NULL. */
#include <stdint.h>    /* Get definition of uint32_t. */
#include <assert.h>    /* Get the definition of support for standard C asserts. */
#include <builtins.h>  /* Get definitions of compiler built-in functions */
#include <platform_include.h>      /* System and IOP register bit and address definitions. */
#include <processor_include.h>     /* Get definitions of the part being built*/
#include <services/int/adi_int.h>  /* Interrupt Handler API header. */
#include "adi_initialize.h"
#include <sru.h>

#define NUM_SAMPLES 384
#define PCI 0x00080000

#define OFFSET_MASK 0x7FFFF

/* Function prototypes for this talkthrough code*/

extern void processBlock(unsigned int *,unsigned int *);

extern void initPLL(void);
extern void initExternalMemory(void);

extern void InitSRU(void);

extern void InitSPDIF(void);

extern void InitSPORT(void);
extern void TalkThroughISR(uint32_t , void *);
extern void ClearSPORT(void);

static void Delay (int i) ;

volatile int isProcessing;
volatile int blockReady;
unsigned int *insrc_pointer[2];
unsigned int *outsrc_pointer[2];
volatile int int_cntr;

#endif

