/*
 *  cdc_Touch.c
 *
 *    CH55x Touch test w/ USB-CDC
 * 
 *      P1.1 TIN1
 *      P1.6 TIN4
 *      P1.7 TIN5
 * 
 *  by Jimmy.Su@2024
 *  Github:   https://github.com/jmysu
 */

// ===================================================================================
// Project:   ADC Transmitter Demo for CH551, CH552 and CH554
// Version:   v1.1
// Year:      2023
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Continuously transmits ADC sample values (pin P1.4) via USB-CDC.
//
// References:
// -----------
// - Blinkinlabs: https://github.com/Blinkinlabs/ch554_sdcc
// - Deqing Sun: https://github.com/DeqingSun/ch55xduino
// - Ralph Doncaster: https://github.com/nerdralph/ch554_sdcc
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Chip:  CH551, CH552 or CH554
// - Clock: 16 MHz internal
// - Adjust the firmware parameters in src/config.h if necessary.
// - Make sure SDCC toolchain and Python3 with PyUSB is installed.
// - Press BOOT button on the board and keep it pressed while connecting it via USB
//   with your PC.
// - Run 'make flash' immediatly afterwards.
// - To compile the firmware using the Arduino IDE, follow the instructions in the 
//   .ino file.
//
// Operating Instructions:
// -----------------------
// - Connect the board via USB to your PC. It should be detected as a CDC device.
// - Open a serial monitor or a serial plotter and select the correct serial port
//   (BAUD rate doesn't matter).
// - The ADC sample values are continuously transmitted via CDC.
// - Use a variable resistor (5V - P1.4 - GND) to change values.

#undef F_CPU
#define F_CPU 24000000
#pragma warning 'F_CPU 24MHz'
// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================

// Libraries
#include "src/ch554.h"
#include "src/config.h"                   // user configurations
#include "src/gpio.h"                     // GPIO functions
#include "src/system.h"                   // system functions
#include "src/delay.h"                    // delay functions
#include "src/usb_cdc.h"                  // USB-CDC serial functions
 
#include <stdio.h>                        // for printf


// ===================================================================================
// Prototypes for used interrupts
// ===================================================================================
void USB_interrupt(void);
void USB_ISR(void) __interrupt(INT_NO_USB) {
  USB_interrupt();
}

//**********************************************
// Touch Keys
//**********************************************
#include "src/touchKey.h"
extern UINT8 	TK_CH[TOUCH_NUM];
/*******************************************************************************
* Function Name  : TK_int_ISR
* Description    : Touch key interrupt routing for touch key scan.
* Input          : None.
* Return         : None.
*******************************************************************************/
void TK_int_ISR( void ) __interrupt(INT_NO_TKEY) 
{
	static UINT8 tch = 0;
	UINT16 KeyData;

	KeyData = TKEY_DAT;
	
	if( KeyData < ( Key_FreeBuf[tch] - TH_VALUE ) )	{
		Touch_CH |=  1 << ( TK_CH[tch] - 1 );
	  }
	
	//printf( "ch[%d]=%d\t", (UINT16)(TK_CH[ch] - 1), (UINT16)KeyData );

	if( ++tch >= TOUCH_NUM )	{
		//printf("\n");
		tch = 0;
	  }	
	TK_SelectChannel( tch );
}
// ===================================================================================
// printf
// ===================================================================================
//#define printf printf_tiny
#define ALWAYS_PRINT_UNSIGNED

#if SDCC < 370
void putchar(char c) {
  CDC_write(c);
}
#else
int putchar(int c) {
  CDC_write(c & 0xFF);
  return c;
}
#endif

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Setup
  CLK_config();                           // configure system clock
  DLY_ms(10);                             // wait for clock to settle
  CDC_init();                             // init USB CDC
  //PIN_input(PIN_ADC);                     // set ADC pin to INPUT
  //ADC_input(PIN_ADC);                     // set ADC pin as ADC input
  //ADC_enable();                           // enable ADC
  DLY_ms(1000);

  while (!CDC_ready()) {} //Wait any key...
  DLY_ms(100);            //need some delay
  CDC_EP_init();          //reset CDC EP again!!!

  printf("\nStart Touch... CH:543210\n");
  printf(  "========================\n");
  CDC_flush();
  //TK_Init(BIT1+BIT4+BIT5);		// Init TIN1 (P1.1), TIN2 (P1.4) and TIN3 (P1.5)
	TK_Init(BIT1+BIT6+BIT7,  1, 1 );		      /* Init TIN1 (P1.1), TIN4 (P1.6) and TIN5 (P1.7)for touchkey input. enable interrupt. */
	TK_SelectChannel(0);											/* NOTICE: ch is not compatible with the IO actually. */
	EA = 1;		

  //DLY_ms(10);
  Touch_CH=0;
  // Loop
  while(1) {
    
    if ( Touch_CH != 0 ) {
			printf("Touched CH = 0x%02x ", Touch_CH);
      for (int ch=5;ch>=0;ch--) {
        //if (i==2 || i==3) {printf("");continue; }//skip bit2/bit3
        printf("%c", Touch_CH&(1<<ch)? 'v':'-');
        }
      printf("\n");
      CDC_flush();
      DLY_ms(100); //deBounce

			Touch_CH = 0;
		  }
    DLY_ms(100);
    }

}
