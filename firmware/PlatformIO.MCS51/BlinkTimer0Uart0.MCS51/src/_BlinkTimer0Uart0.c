// ============================================================================
// Project:   Example for CH559
// Version:   v1.0
// Year:      2022
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ============================================================================
//
// Description:
// ------------
// Blink example.
//
// References:
// -----------
// - Deqing Sun: https://github.com/DeqingSun/ch55xduino
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// ============================================================================
/*
 *  _BlinkTimer0Uart0.c
 * 
 *    CH559 Dev board test!
 * 
 *    -Blink LED P1.5 (100ms gpio toggle)
 *    -Blink LED P1.4 (1sec Timer0)
 *    -UART0 @9600baud (P3.0Tx / P3.1Rx) w/ 12Mhz FREQ_SYS
 *    -Bootloader when P4.6 low
 * 
 *  by Jimmy.Su @2024-0614 (Lastbuy day of Z80)
 */
// ============================================================================
// Libraries, Definitions and Macros
// ============================================================================
// Libraries
#include "system.h"                               // system functions
#include "delay.h"                                // delay functions
#include "gpio.h"                                 // GPIO functions
#include <stdio.h>

// Pin definitions CH559 Dev board w/ 2LEDs
#define PIN_LED4  P14                             // define LED pin
#define PIN_LED5  P15    

#define PIN_BOOT  P46

//12MHz for UART0 9600Baud @Mode01
#define FREQ_SYS 12000000 
// ============================================================================
// Timer0 interrupt
// ============================================================================
volatile uint16_t counter;
void timer0_interrupt(void) __interrupt(INT_NO_TMR0){
    //TL0 = 0xe0; //24MHz 1sec
    TL0 = 0xf0; //12MHz 1sec
    TH0 = 0xff;
    // once timer triggers this interrupt, we'll set the TL and TH values, so 
    // it will start to increment from there
    // The formula used to calculate TL and TH values in regards to the 16 bit `counter`:
    // Timer0/1 will _always_ increment TL/TH every 12th clock cycle:
    // max_val_of_counter+1 * ((0xffff-0xTHTL) * (1/(FREQ_SYS/12)) = seconds for the counter to overflow
    // our `counter` is 16 bit, so the max value is 0xffff:
    // (0xffff+1)*(0xffff-0xffe0)*(1/(24000000/12)) = 1.015 seconds

    // we will update this `counter` on every interrupt
    // and once it overflows, we'll toggle LED pin
    counter++;
    if(!counter) {
        //LED4 = !LED4;
        PIN_toggle(PIN_LED4); 
        }
}

// ============================================================================
// Hardcoded delay Function
// ============================================================================
/*
static inline void delay100ms(void) {
    uint32_t i;
    for (i = 0; i < (120000UL); i++){}
        __asm__("nop");
}*/

int i=0;
// ============================================================================
// Main Function
// ============================================================================
void main(void) {
  // Setup
  CLK_config();                                     // configure system clock
  PIN_output(PIN_LED4);                             // set LED pin as output
  PIN_output(PIN_LED5); 
  PIN_current_low(PIN_LED4);                        // set low current P1

  PIN_input(PIN_BOOT);
 
  // Timer0--------------------------
  EA = 1; // global interrupt enable
  ET0 = 1; // enable timer0 interrupt
  //TMOD |= bT0_M0; // mode 01 (16bit)
  TR0 = 1; // start the timer

  // Uart0---------------------------
	uart0_init();
	printf("Hello World!\n");
  

  //===========================================================================
  while(1) {
    PIN_toggle(PIN_LED5);                          // toggle LED
    //PIN_toggle(PIN_LED4); 
    DLY_ms(100);                                  // wait a bit
    //delay100ms();

    if (!PIN_read(PIN_BOOT)) BOOT_now();

    printf("Hello %d\n", i++);
  }
}
