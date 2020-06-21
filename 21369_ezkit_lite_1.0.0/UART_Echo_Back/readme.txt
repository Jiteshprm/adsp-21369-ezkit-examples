  
  Analog Devices, Inc. CrossCore(R) Embedded Studio Application Example


Example:  UART_Echo_Back
Target:   ADSP-21369
Hardware: ADSP-21369 EZ-KIT LITE


Description:

	This program communicates with the PC using a terminal program on
	your PC and echoes back whatever you type in via the terminal.
	The data communication takes place at a Baud rate of 19200Hz
	with the core clock rate set to 400MHz.


Switch settings:   SW6 all OFF and SW5 OFF, ON, OFF, ON

Instructions: 

	Connect the PC serial port with the UART connector (J6) on your
	Ez-kit using a non-crossover serial cable.
	Open the terminal program of your choice on your PC.
	Choose the serial port on your PC to which you are connected.
	

	The settings used in this example by default are as follows.
	Bits per Second  -> 19200
	Data Bits        -> 8
	Parity           -> odd
	Stop Bits        -> 2
	Flow Control     -> None


	After downloading the executable to the EZ-KIT and running the program, the
	terminal will display the following message: 

			Hello!
			This program echoes back whatever you type in.
			
    after which it will echo any data that is typed or sent by the terminal program.


The source files contained in this directory are:

	a)UART_Echo_Back.c  - echoes back the typed value on the hyperterminal

		Functions present in the source file:

			UARTisr() - Receive interrupt service routine for UART
			initDPI() - Connects the UART receive and transmit signals to DPI
			main()    - The Program starts from here
			initUART() - Configures the UART
			Transmit() - transmits the welcome message

	b)initPLL_SDRAM.c - initializes the PLL to 400 Mhz and the SDRAM to 160 Mhz





NOTE:  	If the application is halted within the CCES, the executable should 
   	be reloaded to the target to ensure proper operation.

Reference:   ADSP-213xx Hardware Reference Manual
             ADSP-21369 EZ-Kit lite Manual


(c) 2012 Analog Devices, Inc.  All rights reserved.


