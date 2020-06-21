/*********************************************************************************

 Copyright(c) 2012 Analog Devices, Inc. All Rights Reserved.

 This software is proprietary and confidential.  By using this software you agree
 to the terms of the associated Analog Devices License Agreement.

 *********************************************************************************/
/*
NAME:     tt.h
PURPOSE:  Header file with definitions use in the C-based talkthrough examples
*/

#ifndef _TT_H_
#define _TT_H_

#include <stdio.h>     /* Get declaration of puts and definition of NULL. */
#include <stdint.h>    /* Get definition of uint32_t. */
#include <assert.h>    /* Get the definition of support for standard C asserts. */
#include <builtins.h>  /* Get definitions of compiler builtin functions */
#include <platform_include.h>      /* System and IOP register bit and address definitions. */
#include <processor_include.h>     /* Get definitions of the part being built*/
#include <services/int/adi_int.h>  /* Interrupt Hndler API header. */
#include "adi_initialize.h"
#include <sru.h>
#include "ad1835.h"


/* Function prototypes for this talkthrough code*/

extern void initPLL(void);
extern void initExternalMemory(void);

extern void InitSRU(void);
extern void Init1835viaSPI(void);
extern void InitSRC(void);
extern void initSport(void);
extern void InitSPDIF(void);
extern void _talkThroughISR_SP0(uint32_t , void *);
extern void _talkThroughISR_SP2(uint32_t , void *);


extern  void SetupSPI1835 (void) ;
extern  void DisableSPI1835 (void) ;
extern  void Configure1835Register (int i) ;
extern unsigned int Get1835Register (int i) ;

extern  void Delay (int i) ;


#endif
