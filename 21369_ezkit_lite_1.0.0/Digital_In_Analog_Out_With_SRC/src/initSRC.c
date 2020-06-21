/*********************************************************************************

 Copyright(c) 2012 Analog Devices, Inc. All Rights Reserved.

 This software is proprietary and confidential.  By using this software you agree
 to the terms of the associated Analog Devices License Agreement.

 *********************************************************************************/

#include "tt.h"

void InitSRC()
{
	
    /* Configure SRC Control Register (SRCCTL0).

       SRC1_IN_I2S : SRC1 Serial Input Format= I2S mode
       SRC1_OUT_I2S: SRC1 Serial Output Format= I2S mode
       SRC1_OUT_24 : Output Word Length= 24 bits

    */

   *pSRCCTL0=SRC1_IN_I2S | SRC1_OUT_I2S | SRC1_OUT_24;


/* Enable SRC1 in a different cycle than setting the configuration*/
 
   *pSRCCTL0 |= SRC1_ENABLE;

}
