
#ifndef _PB_LED_TEST_H_
#define _PB_LED_TEST_H_



/////////////////////////        Constants        ////////////////////////////////


typedef enum LEDS_tag{
	LED1 = 0x01,
	LED2 = 0x02,
	LED3 = 0x04,
	LED4 = 0x08,
	LED5 = 0x10,
	LED6 = 0x20,
	LED7 = 0x40,
	LED8 = 0x80,
	LAST_LED = 0x100
}enLED;

#define BLINK_FAST	1000000
#define BLINK_SLOW	BLINK_FAST*4


///////////////////////////   Function Prototypes     ///////////////////////////
void LED_BAR(const int iSpeed);
int TEST_LED_and_PB(void);

void Blink_LED(const int enleds, const int iSpeed);
void ClearSet_LED(const enLED led, const int bState);
void ClearSet_LED_Bank(const int enleds, const int iState);

void Delay(const int iVal);

void IRQ0_routine(int sig_int);
void IRQ1_routine(int sig_int);
void DAIroutine(int sig_int);
void Setup_leds(void);
void Setup_Ints(void);



#endif //_PB_LED_TEST_H_
