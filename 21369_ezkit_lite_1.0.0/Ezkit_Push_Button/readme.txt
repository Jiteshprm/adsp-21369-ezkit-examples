Analog Devices, Inc. CrossCore(R) Embedded Studio Application Example
	
	Example:  Ezkit_Push_Button
	Target:   ADSP-21369
	Hardware: ADSP-21369 EZ-KIT LITE
	
Description:
	
	This directory contains an Assembly code example pushbutton blink for use with
	the ADSP-21369 Ez-Kit Lite Development System.
	
	The project illustrates how to use the push buttons on the board in order to
	blink the on board LEDs.  On the board the pushbuttons are connected to the
	IRQ0, IRQ1, DAI_PB19, and DAI_PB20 signals on the DSP. The DAI signals can be
	routed to the miscellaneous channels (MISCAx) which are associated with DAI
	interrupts. The IRQ signals have dedicated interrupts. In the example code,
	DAI_PB19 is routed to MISCA1 which is associated with DAI interrupt 29 and
	DAI_PB20 is routed to MISCA2 which is associated with DAI interrupt 30.
	
Instruction:

	Pressing PB1 lights LED1&2 and pressing PB2 lights LED3&4.
	Pressing PB3 lights LED5&6 and pressing PB4 lights LED7.

NOTE:  		If the application is halted within the CCES, the executable should 
      		be reloaded to the target to ensure proper operation.

Reference:	ADSP-213xx Hardware Reference Manual
			ADSP-21369 EZ-Kit lite Manual
		
(c) 2012 Analog Devices, Inc.  All rights reserved.

	

	
	
	
