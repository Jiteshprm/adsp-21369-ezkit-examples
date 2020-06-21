/*********************************************************************************

 Copyright(c) 2012 Analog Devices, Inc. All Rights Reserved.

 This software is proprietary and confidential.  By using this software you agree
 to the terms of the associated Analog Devices License Agreement.

 *********************************************************************************/

/*
	 NAME:     initSRU.c (Block-based Talkthrough)
	 PURPOSE:  Talkthrough framework for sending and receiving samples
	 USAGE:    This file initializes the DAI/DPI for accessing the SPDIF stream
*/

#include "ADDS_21369_EzKit.h"

/* 	The following definition allows the SRU macro to check for errors. Once the routings have
 	been verified, this definition can be removed to save some program memory space.
 	The preprocessor will issue a warning stating this when using the SRU macro without this
	definition*/

#define SRUDEBUG  /* Check SRU Routings for errors.*/
#include <SRU.h>


void InitSRU()
{

/* Disable the pull-up resistors on all 20 pins*/
    *pDAI_PIN_PULLUP = 0x000FFFFF;

/*  Connect the SPDIF Receiver*/
    SRU(DAI_PB18_O, DIR_I);


/*  Connect the SRC 0 inputs.*/

    SRU(DIR_DAT_O, SPORT0_DA_I);          //  Data in from the SPDIF RX*/
    SRU(DIR_FS_O, SPORT0_FS_I);            //  Input Frame sync from the SPDIF RX*/
    SRU(DIR_CLK_O, SPORT0_CLK_I);          //  Input clock from the SPDIF RX*/

/*  Connect the SPDIF Transmitter Inputs*/

    SRU(SPORT3_DA_O, DIT_DAT_I);           /*  Data in from SPORT 3A*/
    SRU(DIR_FS_O, DIT_FS_I);               /*  Frame sync from SPDIF receiver*/
    SRU(DIR_CLK_O, DIT_CLK_I);             /*  Clock from SPDIF receiver*/
    SRU(DIR_CLK_O, SPORT3_CLK_I);          /*  Clock from SPDIF receiver*/
    SRU(DIR_TDMCLK_O, DIT_HFCLK_I);        /*  Over sampling Clock from SPDIF receiver*/
    SRU(DIR_FS_O, SPORT3_FS_I );           /*  FS from SPDIF receiver*/


  /*  Connect the SPDIF Transmitter Output.*/
    SRU(DIT_O, DAI_PB01_I);


 /* Tie the pin buffer inputs LOW for DAI pin 18.  Even though
    these pins are inputs to the SHARC, tying unused pin buffer inputs
    LOW is "good coding style" to eliminate the possibility of
    termination artifacts internal to the IC.  Note that signal
    integrity is degraded only with a few specific SRU combinations.
    In practice, this occurs VERY rarely, and these connections are
    typically unnecessary.*/

    SRU(LOW, DAI_PB18_I);


/*  Tie the pin buffer enable inputs LOW for DAI pin 18 so
    that they are always input pins.*/

    SRU(LOW, PBEN18_I);


/* Tie the pin buffer enable high*/
    SRU(HIGH, PBEN01_I);

}
