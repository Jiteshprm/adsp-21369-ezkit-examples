////////////////////////////////////////////////////////////////////////////
//
//  Program to check the functionality of the UART
//    device
//

#ifdef __ADSP21371__
	#include <Cdef21371.h>
	#include <def21371.h>
#elif __ADSP21375__
	#include <Cdef21375.h>
	#include <def21375.h>
#elif __ADSP21369__
	#include <Cdef21369.h>
	#include <def21369.h>
#endif

#include "post_common.h"

//////////////////////////////////////////////////////////////////////////////
//
// common defines
//
//////////////////////////////////////////////////////////////////////////////
//Baud rate = SCLK/(16 × divisor), where SCLK is the system clock frequency
//and the divisor can be a value from 1 to 65536
#define BAUD_RATE 9600
#define CCLK 393000000 //(24.576 * 16)
#define SCLK (CCLK / 2)
#define BAUD_RATE_DIVISOR	(SCLK / (BAUD_RATE * 16))



//////////////////////////////////////////////////////////////////////////////
//
// standalone test jig
//
//////////////////////////////////////////////////////////////////////////////
#ifdef _STANDALONE_ // use this to run standalone tests
int main(void)
{
	int bPassed = 0;

	Config_SRU_UART();

	while(1)
	{
		bPassed = TEST_UART();

		if( 0 == bPassed )
		{
			break;
		}
	}



	return 0;
}
#endif //#ifdef _STANDALONE_



//////////////////////////////////////////////////////////////////////////////
// int TEST_UART(void)
//
// PURPOSE:		Test the UART
//////////////////////////////////////////////////////////////////////////////
int TEST_UART(void)
{
	int nTimeout = 1000000;
	int n;

	Config_SRU_UART();

	int TX_Buffer[] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA};
	int RX_Buffer[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};


	(*pUART0LCR) = UARTDLAB; //enables access to Divisor register to set baud rate for UART0

	(*pUART0DLL) = (BAUD_RATE_DIVISOR & 0xFF);
	(*pUART0DLH) = ((BAUD_RATE_DIVISOR >> 8) & 0xFF); //0x28b = 651 for divisor value and gives a baud rate of9600

	(*pUART0LCR) = (UARTWLS8 | UARTPEN | UARTSP);         // sets UART0 Line with one stop bit,  odd parity and with baud rate of 9600

	(*pUART0MODE) = UARTPACK;	// enable packing mode


	/// DMA method

	/* UART0 receiver is configured in DMA mode*/
	(*pUART0RXCTL) = 0;       // clears the UART recieve control register

	// set the DMA parameter registers
	(*pIIUART0RX) = (int)RX_Buffer;
	(*pIMUART0RX) = 1;
	(*pCUART0RX) = sizeof(RX_Buffer);

	(*pUART0RXCTL) = (UARTEN| UARTDEN);        // Enables UART0 in receive DMA mode


	/* UART0 transmitter is configured  in DMA mode */
	(*pUART0TXCTL) = 0;        // clears the UART0 transmit control register

	// Set the DMA transmit parameter registers
	(*pIIUART0TX) = (int)TX_Buffer;
	(*pIMUART0TX) = 1;
	(*pCUART0TX)  = sizeof(TX_Buffer);

	(*pUART0TXCTL) = (UARTEN | UARTDEN);         // Enables  UART0 in transmit DMA mode;


	// UART0 receiver Status polling
	while( ((*pUART0RXSTAT) & UARTDMASTAT)  )
	{
		if( nTimeout-- < 0 )
		{
			return 0;
		}
	}

	(*pUART0RXCTL) = 0;           // Disable UART0 receive
	(*pUART0TXCTL) = 0;       // Disable UART0 transmit


	/// end DMA method

	for(n = 0; n < sizeof(TX_Buffer); n++ )
	{
		if( TX_Buffer[n] != RX_Buffer[n] )
		{
			return 0;
		}
	}

	return 1;
}



