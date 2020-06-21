
/*NAME:     initSPORT.c

  USAGE:    This file initializes the transmit and receive serial ports (SPORTS). It uses
			SPORT0 to receive data from the SPDIF Rx and transmits the data to SRC1 Input
			via SPORT1A, then uses SPORT2A to receive data from SRC1 Output and transmits
			to the DAC's via SPORT3A, SPORT3B, SPORT4A and SPORT4B.
*/

#include "tt.h"

void initSport()
{
    /* Make sure that the multi-channel mode registers are cleared*/

int i=0;
   
	*pSPMCTL0 = i;
	*pSPMCTL1 = i;
	*pSPMCTL2 = i;
	*pSPMCTL3 = i;
	*pSPMCTL4 = i;
	*pSPCTL0 = i;
	*pSPCTL1 = i;
	*pSPCTL2 = i;
	*pSPCTL3 = i;
	*pSPCTL4 = i;

   /* Configure SPORT 0 for input from ADC
       OPMODE = I2S mode
       L_FIRST = Send the Left word first (per I2S requirement)
       SLEN24 = 24 bit of data in each 32-bit word
       SPEN_A = Enable data channel A
    */


    *pSPCTL0 = OPMODE | L_FIRST | SLEN24 | SPEN_A;


    /* Configure SPORT 1 as a transmitter

        SPTRAN = Transmit on serial port
        OPMODE = I2S mode
        L_FIRST = Send the Left word first (per I2S requirement)
        SLEN24 = 24 bit of data in each 32-bit word
        SPEN_A = Enable data channel A
   */

    *pSPCTL1 = SPTRAN | OPMODE | L_FIRST | SLEN24 | SPEN_A;


    /* Configure SPORT 2 as a receiver

      OPMODE = I2S mode
      L_FIRST = Send the Left word first (per I2S requirement)
      SLEN24 = 24 bit of data in each 32-bit word
      SPEN_A = Enable data channel A
   */

    *pSPCTL2 = OPMODE | L_FIRST | SLEN24 | SPEN_A | SPEN_B;



    /* Configure SPORTs 3 & 4 as transmitters to DACs 1-4

        SPTRAN = Transmit on serial port
        OPMODE = I2S mode
        L_FIRST = Send the Left word first (per I2S requirement)
        SLEN24 = 24 bit of data in each 32-bit word
        SPEN_A = Enable data channel A
   */

   *pSPCTL3 = SPTRAN | OPMODE | L_FIRST | SLEN24 | SPEN_A | SPEN_B;
   *pSPCTL4 = SPTRAN | OPMODE | L_FIRST | SLEN24 | SPEN_A | SPEN_B;
   

}

