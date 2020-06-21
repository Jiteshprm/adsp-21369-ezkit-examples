 Analog Devices, Inc. CrossCore(R) Embedded Studio Application Example
 
	
	Example:  Block_Based_SPDIF_Talkthrough
	Target:   ADSP-21369
	Hardware: ADSP-21369 EZ-KIT LITE, Digital input source (DVD player with test media), headphones or speakers

Description:

	This project contains a talkthrough example using the onchip SPDIF receiver to
	acquire and output an audio stream. The digital audio data is available for
	processing in the file SPORTisr.c. The block size is 384 samples to match the
	block size for the SPDIF stream.
	
	The SPDIF receiver derives the MCLK, BCLK, and LRCLK from the data stream and
	passes these to the SPDIF receiver.
	
Instructions:
	
	 Apply a digital input signal to J8 on the ADSP-21369 EZ-KIT LITE, and
	 attach a digital output device to the selected channel of J7.
	
Source Files contained in this directory:
	
	initSRU.c           Set up the SRU to connect to the AD1835
	initSPDIF.c         Set up the SPDIF transmitter and receiver
	main.c              Main section to call setup routines
	initSPORT.c         Initialize the SPORT DMA to communicate with the SPDIF tx & rx
	initPLL_SDRAM.c     Configures the DSP to run at CCLK= 393.21 MHz & SDCLK= 157.28 MHz
	SPORTisr.c          Process SPORT 0 interrupts
	blockProcess.c      Process the audio data in the current block
	ADDS_21369_EzKit.h  Includes and external declarations used for all files
	
NOTE:  		If the application is halted within the CCES, the executable should 
      		be reloaded to the target to ensure proper operation.

Reference:	ADSP-213xx Hardware Reference Manual
			ADSP-21369 EZ-Kit lite Manual

(c) 2012 Analog Devices, Inc.  All rights reserved.
	
	