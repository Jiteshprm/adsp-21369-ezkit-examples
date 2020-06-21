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

#include "tt.h"

/* The following definition allows the SRU macro to check for errors. Once the routings have
  been verified, this definition can be removed to save some program memory space.
  The preprocessor will issue a warning stating this when using the SRU macro without this
  definition*/
#define SRUDEBUG  /* Check SRU Routings for errors.*/
#include <SRU.h>

void InitSRU(){

/* Disable the pull-up resistors on all 20 pins*/
    *pDAI_PIN_PULLUP = 0x000FFFFF;

/*  Tie the pin buffer input LOW.*/
    SRU(LOW,DAI_PB17_I);

/*  Tie the pin buffer enable input LOW*/
    SRU(LOW,PBEN17_I);


  /*
    The Steps will be as follows.
    1. Connect DIR to SPORT0
    2. Connect SPORT1 to SRC1-INPUT
    3. Connect SRC1-OUTPUT to SPORT2
    4. Connect SPORT3&4 to DAC's
    */

    /* Tie the pin buffer input LOW.*/
    SRU(LOW,DAI_PB18_I);

    /* Tie the pin buffer enable input LOW*/
    SRU(LOW,PBEN18_I);

    /*  Connect the SPDIF Receiver*/
    SRU(DAI_PB18_O,DIR_I);


/*Input to the DSP*/

    /* Clock in from SPDIF RX*/
    SRU(DIR_CLK_O,SPORT0_CLK_I);    /*Connect CLK_O to SPO_CLK*/
    SRU(DIR_CLK_O,SPORT1_CLK_I);    /*Connect CLK_O to SP1_CLK*/
    SRU(DIR_CLK_O,SRC1_CLK_IP_I);   /*Connect CLK_O to SRC1_CLK*/

    /* Frame sync from SPDIF RX*/
    SRU(DIR_FS_O,SPORT0_FS_I);      /*Connect FS_O to SPO_FS*/
    SRU(DIR_FS_O,SPORT1_FS_I);      /*Connect FS_O to SP1_FS*/
    SRU(DIR_FS_O,SRC1_FS_IP_I);     /*Connect FS_O to SRC1_FS*/

    /*  Data in from SPDIF RX*/
    SRU(DIR_DAT_O,SPORT0_DA_I);     /*Connect DAT_O to SPO_DA*/
    SRU(SPORT1_DA_O,SRC1_DAT_IP_I); /*Connect SP1_DA to SRC1_DAT*/


/*Output from the DSP*/


     SRU(DAI_PB07_O,DAI_PB13_I);
     SRU(DAI_PB08_O,DAI_PB14_I);

     /*  Clock in from ADC*/
     SRU(DAI_PB07_O,SRC1_CLK_OP_I);
     SRU(DAI_PB07_O,SPORT2_CLK_I);
     SRU(DAI_PB07_O,SPORT3_CLK_I);
     SRU(DAI_PB07_O,SPORT4_CLK_I);

    /*  Frame sync from ADC*/
    SRU(DAI_PB08_O,SRC1_FS_OP_I);
    SRU(DAI_PB08_O,SPORT2_FS_I);
    SRU(DAI_PB08_O,SPORT3_FS_I);
    SRU(DAI_PB08_O,SPORT4_FS_I);


    /*  Data from the DSP*/
    SRU(SRC1_DAT_OP_O,SPORT2_DA_I);   
    SRU(SPORT3_DA_O,DAI_PB09_I);
    SRU(SPORT3_DB_O,DAI_PB10_I);
    SRU(SPORT4_DA_O,DAI_PB11_I);
    SRU(SPORT4_DB_O,DAI_PB12_I);

    /*AD1835 MCLK*/
    SRU(DAI_PB17_O,DAI_PB06_I);

    /*ADC data on DAI Pin 05*/
    SRU(DAI_PB05_O,SPORT2_DB_I);
    SRU(LOW,PBEN05_I);


/* Tie the pin buffer enable inputs HIGH to make DAI pins 9-14 outputs.
   and pins 7, 8 & 18 LOW to make them inputs*/

    SRU(HIGH,PBEN06_I);
    SRU(LOW,PBEN07_I);
    SRU(LOW,PBEN08_I);
    SRU(HIGH,PBEN09_I);
    SRU(HIGH,PBEN10_I);
    SRU(HIGH,PBEN11_I);
    SRU(HIGH,PBEN12_I);
    SRU(HIGH,PBEN13_I);
    SRU(HIGH,PBEN14_I);
    SRU(LOW,PBEN18_I);

/* Route SPI signals to AD1835.*/

    SRU(SPI_MOSI_O,DPI_PB01_I)      //Connect MOSI to DPI PB1.
    SRU(DPI_PB02_O, SPI_MISO_I)     //Connect DPI PB2 to MISO.
    SRU(SPI_CLK_O, DPI_PB03_I)      //Connect SPI CLK to DPI PB3.
    SRU(SPI_FLG3_O, DPI_PB04_I)     //Connect SPI FLAG3 to DPI PB4.

/* Tie pin buffer enable from SPI peripherals to determine whether they are
   inputs or outputs*/

    SRU(SPI_MOSI_PBEN_O, DPI_PBEN01_I);
    SRU(SPI_MISO_PBEN_O, DPI_PBEN02_I);
    SRU(SPI_CLK_PBEN_O, DPI_PBEN03_I);
    SRU(SPI_FLG3_PBEN_O, DPI_PBEN04_I);
}
