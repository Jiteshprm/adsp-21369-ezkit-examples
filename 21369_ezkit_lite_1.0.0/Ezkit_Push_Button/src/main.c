/*********************************************************************************

 Copyright(c) 2012 Analog Devices, Inc. All Rights Reserved.

 This software is proprietary and confidential.  By using this software you agree
 to the terms of the associated Analog Devices License Agreement.

 *********************************************************************************/

/*  ADSP-21369 Blink LEDs with the DAI buttons on the EZKIT  */

/*
    Buttons to push on Ezkit:
	IRQ1 blinks LED1&2, IRQ0 blinks LED3&4, DAI_PB19 blinks LED5&6, DAI_PB20 blinks LED7&8.
	DAI_PB19 is associated with interrupt 29 (MISCA1) and Interrupt 29's ISR sets LED5&6.
	DAI_PB20 is associated with interrupt 30 (MISCA2) and Interrupt 30's ISR sets LED7&8.
*/



#include "ADDS_EZkit_21369.h"

/* The following definition allows the SRU macro to check for errors. Once the routings have
 been verified, this definition can be removed to save some program memory space.
 The preprocessor will issue a warning stating this when using the SRU macro without this
 definition*/
#define SRUDEBUG  /*Check SRU Routings for errors.*/

 int LED06_value=0;
 int LED07_value=0;

void main()
{
	adi_initComponents();

	initPLL();   /*Initialize PLL for the correct core clock (CCLK) frequency*/
	initExternalMemory(); /*SDRAM is setup for use, but cannot be accessed until MSEN bit is enabled*/


/*Setting the SRU and route so that Flag pins connects to DPI pin buffers.*/
/*Use Flags 4 to 15 only. Flags 0 to 3 not available on the DPI.*/

    SRU(FLAG6_O,DPI_PB08_I);    /* Connect Flag6 output to DPI_PB08 input (LED1)*/
    SRU(FLAG7_O,DPI_PB13_I);    /* Connect Flag7 output to DPI_PB13 input (LED2)*/
    SRU(FLAG4_O,DPI_PB06_I);    /* Connect Flag4 output to DPI_PB06 input (LED3)*/
    SRU(FLAG5_O,DPI_PB07_I);    /* Connect Flag5 output to DPI_PB07 input (LED4)*/
    SRU(FLAG8_O,DPI_PB14_I);    /* Connect Flag8 output to DPI_PB14 input (LED5)*/

    SRU(LOW,DAI_PB15_I);        /* Connect Input LOW to LED6*/
    SRU(LOW,DAI_PB16_I);        /* Connect Input LOW to LED7*/

/*Enabling the Buffer using the following sequence: High -> Output, Low -> Input*/

    SRU(HIGH,DPI_PBEN08_I);
    SRU(HIGH,DPI_PBEN13_I);
    SRU(HIGH,DPI_PBEN06_I);
    SRU(HIGH,DPI_PBEN07_I);
    SRU(HIGH,DPI_PBEN14_I);
    SRU(HIGH,PBEN15_I);
    SRU(HIGH,PBEN16_I);

/* The DAI signals can be routed to the miscellaneous channels (MISCAx) which are
   associated with DAI interrupts, and used as inputs from the Push Buttons.*/

    *pDAI_IRPTL_PRI=SRU_EXTMISCA1_INT  | SRU_EXTMISCA2_INT;    /*unmask individual interrupts*/

    *pDAI_IRPTL_RE=SRU_EXTMISCA1_INT  | SRU_EXTMISCA2_INT;    /*make sure interrupts latch on the rising edge*/

/*Setting flag pins*/
    sysreg_bit_set(sysreg_FLAGS, FLG3O|FLG4O|FLG5O|FLG6O|FLG7O|FLG8O);


/*Clearing flag pins*/
    sysreg_bit_clr(sysreg_FLAGS, FLG3|FLG4|FLG5|FLG6|FLG7|FLG8);


/*Set up the IRQ pins to use with the Push Buttons*/

    *pSYSCTL |=IRQ0EN|IRQ1EN;//ustat3;
    sysreg_bit_set( sysreg_MODE2,IRQ0E|IRQ1E);



/*Pin Assignments in SRU_PIN3 (Group D)*/

    /*assign pin buffer 19 low so it is an input*/
    SRU(LOW,DAI_PB19_I);

    /*assign pin buffer 20 low so it is an input*/
    SRU(LOW,DAI_PB20_I);


/*Route MISCA signals in SRU_EXT_MISCA (Group E)*/

    /*route so that DAI pin buffer 19 connects to MISCA1*/
    SRU(DAI_PB19_O,MISCA1_I);

    /*route so that DAI pin buffer 20 connects to MISCA2*/
    SRU(DAI_PB20_O,MISCA2_I);

    /*Pin Buffer Disable in SRU_PINEN0 (Group F)*/
    /*assign pin 19 low so it is an input*/
    SRU(LOW,PBEN19_I);

    /*assign pin 20 low so it is an input*/
    SRU(LOW,PBEN20_I);

/*Set up the interrupt hardware*/

    sysreg_bit_clr( sysreg_IRPTL,IRQ1I|IRQ0I);

    adi_int_InstallHandler(ADI_CID_DAIHI,(ADI_INT_HANDLER_PTR )DAIroutine,0,true);

    adi_int_InstallHandler(ADI_CID_IRQ0I,(ADI_INT_HANDLER_PTR )IRQ0routine,0,true);

    adi_int_InstallHandler(ADI_CID_IRQ1I,(ADI_INT_HANDLER_PTR )IRQ1routine,0,true);


    sysreg_bit_set(sysreg_MODE1,IRPTEN);

    *pDAI_IRPTL_PRI=SRU_EXTMISCA1_INT  | SRU_EXTMISCA2_INT ;   /*unmask individual interrupts*/
    *pDAI_IRPTL_RE=SRU_EXTMISCA1_INT  | SRU_EXTMISCA2_INT ;    /*make sure interrupts latch on the rising edge*/


    puts("Press push buttons to see the LED toggle");

    for(;;)
     {
    		NOP();
     }
}


/*Set up interrupt service routines to toggle LEDs 1 - 4.*/
 void IRQ1routine(void)
 {

/*lights LED1 and LED2 when IRQ1 is asserted*/

	 sysreg_bit_tgl(sysreg_FLAGS,FLG4|FLG5);
 }

 void IRQ0routine(void)
 {
/*lights LED3 and LED4 when IRQ0 is asserted*/

	 sysreg_bit_tgl(sysreg_FLAGS,FLG6|FLG7);
 }


/*Set up DAI routine to toggle LEDs 5 - 8 by calling MISCA1 & MISCA2*/
void DAIroutine(void)
{

    static int interrupt_reg;

    interrupt_reg = *pDAI_IRPTL_H;

    /*test for SRU_EXTMISCB1_INT*/
    if ((interrupt_reg & SRU_EXTMISCA1_INT) != 0)
        handle_MISCA1();

    /*test for SRU_EXTMISCB2_INT*/
    if ((interrupt_reg & SRU_EXTMISCA2_INT) != 0)
        handle_MISCA2();
}

/*Calling MISCA1 to light LED5&6*/
 int handle_MISCA1()
{
int temp=0;
int temp1=0;
    /*lights LED5 when DPI_PB14 is asserted*/
    sysreg_bit_tgl(sysreg_FLAGS,FLG8);

    temp=LED06_value;
    temp1=0x1^temp;
    LED06_value=temp1;
    if(LED06_value==1)
    goto Flag06_Set;

    /*lights LED6 when DAI_PB15 is asserted*/

    Flag06_Clear:
        SRU(LOW,DAI_PB15_I);    /*light up LED6*/
     return 0;

    Flag06_Set:
        SRU(HIGH,DAI_PB15_I);   /*Turn off LED6*/
        return 0;

}


/*Calling MISCA2 to light LED7&8*/
 int handle_MISCA2()
{

	 int temp=0;
	 int temp1=0;
    /*lights LED8 when MISCA2 is asserted*/

     sysreg_bit_tgl(sysreg_FLAGS,FLG3);

    temp=LED07_value;
    temp1=0x1^temp;
    LED07_value=temp1;
    if(LED07_value==1)
    goto Flag07_Set;

    /*lights LED7 when DAI_PB16 is asserted*/

    Flag07_Clear:
        SRU(LOW,DAI_PB16_I);    //light up LED7
       return 0;

    Flag07_Set:
        SRU(HIGH,DAI_PB16_I);   //turn off LED7
        return 0;

}


