/*
 *  cdcTemplate.c
 *
 *    CH55x CDC Template w/ xprintf
 *
 *    by jimmy.su @ 2024
 *
 *    LINKFLAGS: -mmcs51 --iram-size 256 --xram-size 256 --code-size 16384 --out-fmt-ihx --xram-size 768 --xram-loc 256
 *
 *    Flash: [==        ]  21.9% (used 3580 bytes from 16384 bytes) // w/ TITLE_STAMP + Touch
 *    Flash: [==        ]  21.0% (used 3434 bytes from 16384 bytes) // w/ TITLE_STAMP
 *    Flash: [==        ]  15.4% (used 2530 bytes from 16384 bytes) // w/o TITLE_STAMP
 * 
 *          __   __
 *    P3.2 -|o\_/ |- 3.3V (connect to GND via 0.1uF)
 *    P1.4 -|  C  |- 5.0V (connect to USB VBUS, connect to GND via 0.1uF)
 *    P1.5 -|  H  |- GND  (to USB GND)
 *    P1.6 -|  5  |- USB D-
 *    P1.7 -|  5  |- USB D+ (short 3.3v to BOOT)
 *    Reset-|  2  |- P3.4
 *    P3.1 -|  G  |- P3.3  
 *    P3.0 -|_____|- P1.1  
 * 
 */

#undef F_CPU
#define F_CPU 24000000
#warning "F_CPU 24MHz"

// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
// #include <stdio.h>                        // for printf
// #include <stdbool.h>

// Libraries
#include "config.h"   // user configurations


#include "delay.h"    // delay functions
#include "gpio.h"     // GPIO functions
#include "system.h"   // system functions
#include "touch.h"
#include "usb_cdc.h"  // USB-CDC serial functions

#include "ansiColor.h"
#include "xprintf.h"
 
#ifdef _TITLE_STAMP_
 #warning "W/ TITLE_STAMP"
 #include "myDate.h"
#else
 #warning "W/O TITLE_STAMP"
#endif

// Prototypes for used interrupts
void USB_interrupt(void);
void USB_ISR(void) __interrupt(INT_NO_USB) {
    USB_interrupt();
}

//Arduino simillar millis at Timer0
//#define bT0_CLK           0x10      // timer0 internal clock frequency selection: 
//                                    //  0=standard clock, Fsys/12, 
//                                    //  1=faster clock, Fsys/4 if bTMR_CLK=0 or Fsys if bTMR_CLK=1
//
//8051 Interrupt latency: ~10~20 cycles
uint32_t millis, last; 
void	mTimer0Interrupt(void) __interrupt (INT_NO_TMR0)
{                                              
	TH0 = (65536 - 2380)/256;  // Reload
	TL0 = (65536 - 2380)%256;  // Reload    
	millis++;
}
void Timer0Init(void){
	TMOD = 0x11;
	TH0 = (65536 - 2380)/256;  	// for start value
	TL0 = (65536 - 2380)%256;  	// for start value 
	TR0 = 1;    				// Start timer 0 (TR1 for timer 1)
	ET0 = 1;    				// Enable Timer 0 interrupt
	//EA  = 1;    				// Activate global interrupt, later enable in USB_init()   
}
// ===================================================================================
// printf
// ===================================================================================
// #define printf printf_tiny
//#define ALWAYS_PRINT_UNSIGNED

#define DBGCMD 1

int putchar(int c) {
    CDC_write(c & 0xFF);
    return c;
}
/*
int putchar (int ch)
{
  if(ch == '\n') putchar('\r');
  for(SBUF0 = ch; !TI0; );
  TI0 = 0;
  return ch;
}

int getchar (void)
{
  while(!RI0);
  RI0 = 0;
  return SBUF0;
}
*/
//__xdata uint8_t buf[32];  //CDC input buffer
//__xdata uint8_t buf1[32];  //CDC input buffer copy
//__xdata uint8_t counterParam;

__code uint16_t __at(ROM_CHIP_ID_HX) chid_hx;
__code uint16_t __at(ROM_CHIP_ID_HI) chid_hi;
__code uint16_t __at(ROM_CHIP_ID_LO) chid_lo;
 
// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
    // Setup
    CLK_config();  // configure system clock
    DLY_ms(10);    // wait for clock to settle
    Timer0Init(); 
    CDC_init();    // init USB CDC
    DLY_ms(1000);  // WAIT 1-second

    while (!CDC_ready()) {}  // wait USBCDC ready to write1
    //DLY_ms(500);
    //=========================================================================
    //TIMESTAMP & ID & TITLE  (3434-2530)bytes
    #ifdef _TITLE_STAMP_
      PRINTF("\033[H\033[J");                                                             // Clear screen
      PRINTF("\033[" ANSI_GREEN "m");                                                     // Green
      PRINTF(__TIMESTAMP__);                                                              // Build timestamp
      PRINTF("   CH5%2x:%02X-%04X-%04X\n", CHIP_ID, chid_hx & 0x00FF, chid_hi, chid_lo);  // Chip ID
      PRINTF("\033[" ANSI_YELLOW "m");                                                    // Yellow
      getMyDate();                                                                        // Get date in number from __DATE__
      PRINTF("\033[1m");                                                                  // BOLD
      PRINTF("___%s.%02u%02u%02u___\n", "cdcTemplate", _year,_month,_day);                // Project Name
      PRINTF("\033[0m\a");                   
      CDC_flush();
    #endif                                                                                // Normal
    //=========================================================================
    PIN_output(P17);

    TOUCH_start(P16);
    TOUCH_enable();
    int iTouchOld = TOUCH_sample(P16);
    PRINTF("Try Touch1.6...[%d]\n", iTouchOld);
    CDC_flush();
    // Loop
    while (1) {
              
      if (millis-last>1000){
          PIN_toggle(P17);
          last=millis;      
          }
      int iTouchNew = TOUCH_sample(P16);
      if ( iTouchNew < (iTouchOld>>1)) { //pressed!
        PIN_high(P17);
        PRINTF("Touch1.6: %u\n", iTouchNew);
        CDC_flush();
        
        DLY_ms(200); //debounce
        }
      //else {
      //  PIN_toggle(P17);
      //  DLY_ms(500);
      //  }
    }
}
