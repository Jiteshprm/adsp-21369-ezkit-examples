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

#include "ADDS_21369_EzKit.h"

void InitSPDIF()
{
*pDITCTL = (DIT_EN|DIT_IN_I2S);

// SPDIF Setup code goes here
// Use default setting of SPDIF
*pDIRCTL=0x0;

}
