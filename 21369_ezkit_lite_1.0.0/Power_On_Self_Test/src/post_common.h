//////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////////

#ifndef _POST_CONFIG_H_
#define _POST_CONFIG_H_

#include <sru.h>
#include "pb_led_test.h"


//////////////////////////////////////////////////////////////////////////////
//
// COMMON DEFINES
//
//////////////////////////////////////////////////////////////////////////////
#define SDMODIFY  (BIT_17)
#define PI						((float)3.141592765309)
#define MAX_SAMPLES	 			256

//////////////////////////////////////////////////////////////////////////////
//
// COMMON enumerations
//
//////////////////////////////////////////////////////////////////////////////
typedef enum TESTS_tag{					// LED PATTERN
	TEST_1  = LED1,							//00001
	TEST_2  = LED2,							//00010
	TEST_3  = (LED1 | LED2),				//00011
	TEST_4  = LED3,							//00100
	TEST_5  = (LED3 | LED1),				//00101
	TEST_6  = (LED3 | LED2),				//00110
	TEST_7  = (LED3 | LED2 | LED1),			//00111
	TEST_8  = LED4,							//01000
	TEST_9  = (LED4 | LED1),				//01001
	TEST_10 = (LED4 | LED2),				//01010
	TEST_11 = (LED4 | LED2 | LED1),			//01011
	TEST_12 = (LED4 | LED3),				//01100
	TEST_13 = (LED4 | LED3 | LED1),			//01101
	TEST_14 = (LED4 | LED3 | LED2),			//01110
	TEST_15 = (LED4 | LED3 | LED2 | LED1),	//01111
	TEST_16 = LED5,							//10000
	LAST_TEST = 0x100
}enTESTS;

typedef enum TEST_STATES_tag{					// LED PATTERN
	TEST_1_CLR = 0,
	TEST_1_SET = 1,
	TEST_1_TGL = 2,
	TEST_1_EXT = 3,

	TEST_2_CLR = (TEST_1_CLR << 2),
	TEST_2_SET = (TEST_1_SET << 2),
	TEST_2_TGL = (TEST_1_TGL << 2),
	TEST_2_EXT = (TEST_1_EXT << 2),

	TEST_3_CLR = (TEST_2_CLR | TEST_1_CLR),
	TEST_3_SET = (TEST_2_SET | TEST_1_SET),
	TEST_3_TGL = (TEST_2_TGL | TEST_1_TGL),
	TEST_3_EXT = (TEST_2_EXT | TEST_1_EXT),

	TEST_4_CLR = (TEST_1_CLR << 4),
	TEST_4_SET = (TEST_1_SET << 4),
	TEST_4_TGL = (TEST_1_TGL << 4),
	TEST_4_EXT = (TEST_1_EXT << 4),

	TEST_5_CLR = (TEST_4_CLR | TEST_1_CLR),
	TEST_5_SET = (TEST_4_SET | TEST_1_SET),
	TEST_5_TGL = (TEST_4_TGL | TEST_1_TGL),
	TEST_5_EXT = (TEST_4_EXT | TEST_1_EXT),

	TEST_6_CLR = (TEST_4_CLR | TEST_2_CLR),
	TEST_6_SET = (TEST_4_SET | TEST_2_SET),
	TEST_6_TGL = (TEST_4_TGL | TEST_2_TGL),
	TEST_6_EXT = (TEST_4_EXT | TEST_2_EXT),

	TEST_7_CLR = (TEST_4_CLR | TEST_2_CLR | TEST_1_CLR),
	TEST_7_SET = (TEST_4_SET | TEST_2_SET | TEST_1_SET),
	TEST_7_TGL = (TEST_4_TGL | TEST_2_TGL | TEST_1_TGL),
	TEST_7_EXT = (TEST_4_EXT | TEST_2_EXT | TEST_1_EXT),

	TEST_8_CLR = (TEST_1_CLR << 6),
	TEST_8_SET = (TEST_1_SET << 6),
	TEST_8_TGL = (TEST_1_TGL << 6),
	TEST_8_EXT = (TEST_1_EXT << 6),

	TEST_9_CLR = (TEST_8_CLR | TEST_1_CLR),
	TEST_9_SET = (TEST_8_SET | TEST_1_SET),
	TEST_9_TGL = (TEST_8_TGL | TEST_1_TGL),
	TEST_9_EXT = (TEST_8_EXT | TEST_1_EXT),

	TEST_10_CLR = (TEST_8_CLR | TEST_2_CLR),
	TEST_10_SET = (TEST_8_SET | TEST_2_SET),
	TEST_10_TGL = (TEST_8_TGL | TEST_2_TGL),
	TEST_10_EXT = (TEST_8_EXT | TEST_2_EXT),

	LAST_TEST_STATES
}enTEST_STATES;



//////////////////////////////////////////////////////////////////////////////
//
// COMMON variables
//
//////////////////////////////////////////////////////////////////////////////
extern volatile bool g_bLeftRight;
extern volatile int  g_iSampleIndex;
extern volatile int  g_iSampleCount;

extern volatile float g_fSineWaveOut[MAX_SAMPLES];
extern volatile float g_fSineWaveIn_Left[MAX_SAMPLES];
extern volatile float g_fSineWaveIn_Right[MAX_SAMPLES];


//////////////////////////////////////////////////////////////////////////////
//
// function prototypes
//
//////////////////////////////////////////////////////////////////////////////
void InitPLL(void);
void Config_SRU_DAI(void); //void Config_AD1835(void);
void Config_SRU_AT25F2048(void);
void Config_SRU_LEDS(void);
void Config_SRU_INTS(void);
void Config_SRU_SPDIF(void);
void Config_SRU_UART(void);

void Delay(const int iVal);
void CreateSinTable(const float fAmp, const float fFreq, const float fSampleRate);
int Test_Channel(float* pfRealIn, const int iMaxSamples, const int iSampleRate, const float fExpFreq, const float fFreqTol, const float fExpAmp, const float fAmpTol);


#endif //#ifndef _POST_CONFIG_H_
