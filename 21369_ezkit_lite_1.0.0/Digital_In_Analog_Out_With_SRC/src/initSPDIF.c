/*********************************************************************************

 Copyright(c) 2012 Analog Devices, Inc. All Rights Reserved.

 This software is proprietary and confidential.  By using this software you agree
 to the terms of the associated Analog Devices License Agreement.

 *********************************************************************************/
/*
    File:   initSPDIF.c
    Use:    Testing ADSP-21369 EZkit SPDIF Interfaces
    Note:   Initialize the SPDIF receiver.
*/

#include "tt.h"

void InitSPDIF()
{
    /*SPDIF Setup code goes here*/
    /* Use default setting of SPDIF*/
    *pDIRCTL=0x0;

}
