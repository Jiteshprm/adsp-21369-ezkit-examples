/*********************************************************************************

 Copyright(c) 2012 Analog Devices, Inc. All Rights Reserved.

 This software is proprietary and confidential.  By using this software you agree
 to the terms of the associated Analog Devices License Agreement.

 *********************************************************************************/
/*
PURPOSE:  Header file with definitions use in the Block
*/
#ifndef __ADDS_21369_EZKIT_H__
#define __ADDS_21369_EZKIT_H__

/* include files */

#include <stdio.h>     /* Get declaration of puts and definition of NULL. */
#include <stdint.h>    /* Get definition of uint32_t. */
#include <assert.h>    /* Get the definition of support for standard C asserts. */
#include <builtins.h>  /* Get definitions of compiler builtin functions */
#include <platform_include.h>      /* System and IOP register bit and address definitions. */
#include <processor_include.h>	   /* Get definitions of the part being built*/
#include <services/int/adi_int.h>  /* Interrupt Hndler API header. */
#include "adi_initialize.h"
#include <SRU.h>
#include <sysreg.h>
#include <signal.h>

/* Function prototypes for this talkthrough code */
void DAIroutine(void);
void IRQ0routine(void);
void IRQ1routine(void);
int handle_MISCA1(void);
int handle_MISCA2(void);
void initPLL(void);
void initExternalMemory(void);

#endif
