Analog Devices, Inc. CrossCore(R) Embedded Studio Application Example

Example:  Digital_In_Analog_Out_With_SRC (J8=Input, J5 DACs 3 & 4=Output)
Target:   ADSP-21369
Hardware: ADSP-21369 EZ-KIT LITE

Description: 

	Multichannel talk-through (two stereo pairs in -> four stereo pairs out)
	
	TalkThrough reads audio samples from the on-chip SPDIF receiver and it
	passes the unmodified data through to the four DACs of the AD1835.
	
	There are two clock domains in this project.
	The first is derived from the SPDIF input stream. Each interrupt reads a single
	24-bit sample (alternating left and right) using SPORT0A and immediately writes
	the same value to SPORT1A. All audio is transmitted in I2S mode.
	
	The second clock is provided by the AD1835. Each interrupt reads a single 24-bit
	sample (alternating left and right) using SPORT2 and immediately writes the same
	value to both the A and B data channels of SPORT3 and SPORT4. All audio is
	transmitted in I2S mode.
	
	The ADC data is being received on SPORT2B while the SPDIF is being received on 
	SPORT2A. The ADC data is being copied to DACs 1 & 2, and the SPDIF data is being 
	copied to DACs 3 & 4.
	
	An Asynchronous SRC is being used to convert the data between the two clock
	domains so that the input data from the SPDIF receiver can be sent to the DACs.
	This setup will work for encoded data if the decoding takes place in the SPORT0
	interrupt.


Instructions: 

	Download the executable to the EZ-KIT.  There are no user
	controls or visual outputs.

Required Switch Settings: 

	SW3 - 1=Off, 2=On, 3=On, 4=On (This is the default setting)

		
NOTE:  		If the application is halted within the CCES, the executable should 
      		be reloaded to the target to ensure proper operation.

Reference:	ADSP-213xx Hardware Reference Manual
			ADSP-21369 EZ-Kit lite Manual
		
(c) 2012 Analog Devices, Inc.  All rights reserved.
