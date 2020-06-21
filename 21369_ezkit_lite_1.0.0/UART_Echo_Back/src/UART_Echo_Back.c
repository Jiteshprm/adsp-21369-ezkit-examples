/*********************************************************************************

 Copyright(c) 2012 Analog Devices, Inc. All Rights Reserved.

 This software is proprietary and confidential.  By using this software you agree
 to the terms of the associated Analog Devices License Agreement.

 *********************************************************************************/
/*
	NAME:     UART Echo Back
	PURPOSE:  UART Example for the ADSP-21369 Ez-kit
	USAGE:    Connect the UART connector (P1) to your PC and interact with the EZKit
          	  when using the following settings.
          	  Bits per Second  -> 19200
          	  Data Bits        -> 8
          	  Parity           -> odd
          	  Stop Bits        -> 2
         	  Flow Control     -> None
*/

#include <stdio.h>     /* Get declaration of puts and definition of NULL. */
#include <stdint.h>    /* Get definition of uint32_t. */
#include <assert.h>    /* Get the definition of support for standard C asserts. */
#include <builtins.h>  /* Get definitions of compiler built-in functions */
#include <platform_include.h>      /* System and IOP register bit and address definitions. */
#include <processor_include.h>	   /* Get definitions of the part being built*/
#include <services/int/adi_int.h>  /* Interrupt Handler API header. */
#include "adi_initialize.h"
#include <sru.h>


/* The following definition allows the SRU macro to check for errors. Once the routings have
	been verified, this definition can be removed to save some program memory space.
 	The preprocessor will issue a warning stating this when using the SRU macro without this
    definition*/

#define SRUDEBUG  /* Check SRU Routings for errors.*/

static void initDPI(void);		/*makes UART0 signal connections to DPI pins*/
static void initUART(void);	/*function for initializing the UART*/
static void UARTisr(void);			/*function for receive interrupt*/
static void xmitUARTmessage(char*, int); /* function to transmit the welcome message*/

extern void initPLL(void);    /*Initialize PLL to 393 MHz and set up AMI and SDRAM controller*/
extern void initExternalMemory(void);

char welcomemessage[] = {"\fHello! \n\r"
                  "This program echoes back whatever you type in...\n\r"};

volatile int count,test;


main()
{
	/* Initialize managed drivers and/or services at the start of main(). */
	adi_initComponents();

	*pPICR2 &= ~(0x3E0); /*Sets the UART0 receive interrupt to P13*/

	*pPICR2 |= (0x13<<5);

	initPLL();
	initExternalMemory();

	*pUART0LCR=0;

    *pUART0IER= UARTRBFIE;  /*enables UART0 receive interrupt*/

    adi_int_InstallHandler(ADI_CID_P13I,(ADI_INT_HANDLER_PTR )UARTisr,0,true);

    initDPI();

	initUART();

    while(1)
    {
    	NOP();
     }

}

static void initDPI()
{

     SRU2(UART0_TX_O,DPI_PB09_I); /* UART transmit signal is connected to DPI pin 9*/
     SRU2(HIGH,DPI_PBEN09_I);
     SRU2(DPI_PB10_O,UART0_RX_I); /* connect the pin buffer output signal to the UART0 receive*/
     SRU2(LOW,DPI_PB10_I);
     SRU2(LOW,DPI_PBEN10_I);      /* disables DPI pin10 as input*/
}

static void initUART()
{
	 /* Sets the Baud rate for UART0 */
	 *pUART0LCR = UARTDLAB;  /*enables access to Divisor register to set baud rate*/
	 *pUART0DLL = 0x1c;      /*0x21c = 540 for divisor value and gives a baud rate of19200 for core clock 331.776MHz*/
     *pUART0DLH = 0x02;

     /* Configures UART0 LCR */
     *pUART0LCR = UARTWLS8| 				/*word length 8*/
                 UARTPEN| 				/* parity enable ODD parity*/
                 UARTSTB ; 				/* Two stop bits*/


     *pUART0RXCTL = UARTEN;       /*enables UART0 in receive mode*/

     *pUART0TXCTL = UARTEN;       /*enables UART0 in core driven mode*/

     xmitUARTmessage(welcomemessage,sizeof(welcomemessage));
}


static void UARTisr()
{
	  int value;

 	  value = *pUART0RBR;

 	  /* echoes back the value on to the hyper-terminal screen*/


  	  while ((*pUART0LSR & UARTTHRE) == 0)
  	  {
  	     NOP(); /* Loop until condition */
  	  }

  	  *pUART0THR = value;

  	  /* poll to ensure UART has completed the transfer */
  	  while ((*pUART0LSR & UARTTEMT) == 0)
  	  {
  	  	  NOP(); /* Loop until condition */
  	  }

}

static void xmitUARTmessage(char*xmit, int SIZE)
{
	  int i;

	  /* loop to transmit source array in core driven mode */
	  for (i=0; i<SIZE; i++)
	  {
      /* Wait for the UART transmitter to be ready*/

        while ((*pUART0LSR & UARTTHRE) == 0)
        {
    	  NOP(); /* Loop until condition */
        }

      /*Transmit a byte*/
      *pUART0THR = xmit[i];

	  }

      /* poll to ensure UART has completed the transfer */
	  while ((*pUART0LSR & UARTTEMT) == 0)
	  {
	  	   NOP(); /* Loop until condition */
	  }

}






