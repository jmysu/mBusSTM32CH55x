/*  _cdcHID.c
 *
 *    HID CDC monitor
 * 
 *    CH55x USB HID monitor w/ CDC, use USB-CDC to monitor the HID input/output. 
 * 
 *    Flash: [==        ]  24.4% (used 4001 bytes from 16384 bytes)
 *    
 *    Name             Start    End      Size     Max     
 *    ---------------- -------- -------- -------- --------
 *    PAGED EXT. RAM                         0      256   
 *    EXTERNAL RAM     0x0200   0x020e      15      512   
 *    ROM/EPROM/FLASH  0x0000   0x0fa0    4001    16384   
 * 
  ;--------------------------------------------------------
  ; external ram data
  ;--------------------------------------------------------
	.area XSEG    (XDATA)
  _EP0_bufUSB	  =	0x0000
  _EP1_bufHID	  =	0x000a
  _EP1_bufHIDTX	=	0x004a
  _EP2_bufCDC	  =	0x008a
  _EP2_bufCDCTX	=	0x00ca
  ___EP_end__	  =	0x010b
  ;--------------------------------------------------------
 *   
 *    by Jimmy.Su@2024
 * 
**/
// ===================================================================================
// Project:   DAPLink - CMSIS-DAP compliant debugging probe with VCP based on CH552
// Version:   v1.0
// Year:      2023
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// The CH552-based DAPLink is a CMSIS-DAP compliant debugging probe with SWD and JTAG
// protocol support. It can be used to program Microchip SAM and other ARM-based
// microcontrollers. The additional Virtual COM Port (VCP) provides an additional
// debugging feature. The SWD-part of the firmware is based on Ralph Doncaster's 
// DAPLink-implementation for CH55x microcontrollers and Deqing Sun's CH55xduino port.
//
// References:
// -----------
// - Blinkinlabs: https://github.com/Blinkinlabs/ch554_sdcc
// - Deqing Sun: https://github.com/DeqingSun/ch55xduino
// - Ralph Doncaster: https://github.com/nerdralph/ch554_sdcc
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
// - ARMmbed DAPLink: https://github.com/ARMmbed/DAPLink
// - picoDAP: https://github.com/wagiminator/CH552-picoDAP
//
// Compilation Instructions:
// -------------------------
// - Chip:  CH552
// - Clock: 16 MHz internal
// - Adjust the firmware parameters in src/config.h if necessary.
// - Make sure SDCC toolchain and Python3 with PyUSB is installed.
// - Press BOOT button on the board and keep it pressed while connecting it via USB
//   with your PC.
// - Run 'make flash' immediatly afterwards.
//
// Operating Instructions:
// -----------------------
// Connect the DAPLink to the target board via the pin header. You can supply power
// via the 3V3 pin or the 5V pin (max 400 mA). Plug the DAPLink into a USB port on 
// your PC. Since it is recognized as a Human Interface Device (HID), no driver 
// installation is required. However, Windows users may need to install a CDC driver
// for the Virtual COM Port (VCP). The DAPLink should work with any debugging software
// that supports CMSIS-DAP (e.g. OpenOCD or PyOCD). The virtual COM port (8N1 only)
// can be used with any serial monitor.

#undef F_CPU
#define F_CPU 24000000
#warning "F_CPU 24MHz"
// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================

// Libraries
//CH554
#include "system.h"                     // system functions
#include "delay.h"                      // delay functions
#include "gpio.h"
 
//USB
#include "usb.h"
#include "usb_hid_data.h"               // for USB HID data
#include "usb_cdc.h"                    // USB CDC functions
//Utils
#include "ansiColor.h"
#include "xPRINTF.h"
 
#ifdef _TITLE_STAMP_
 #warning "W/ TITLE_STAMP"
 #include "myDate.h"
#else
 #warning "W/O TITLE_STAMP"
#endif

int putchar(int c) {
    CDC_write(c & 0xFF);
    return c;
} 
// Prototypes for used interrupts
void USB_interrupt(void);
void USB_ISR(void) __interrupt(INT_NO_USB) {
  USB_interrupt();
}
 
__code uint16_t __at(0X3FFA) chid_hx;
__code uint16_t __at(0X3FFE) chid_hi;
__code uint16_t __at(0X3FFC) chid_lo;
// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
uint8_t len,counter;

  // Setup
  CLK_config();                             // configure system clock
  DLY_ms(10);                               // wait for clock to settle
  
  USB_init();                               // UEP0
  HID_EP_init();                            // EP1
 
  CDC_init();                               // UEP2 UEP3
  DLY_ms(500);                              // WIN10 NEED wait .5 sec.!!!

  while (!CDC_ready){}                      // wait CDC driver ready
  //=========================================================================
  //TIMESTAMP & ID & TITLE  (3434-2530)bytes
  #ifdef _TITLE_STAMP_
    PRINTF("\033[H\033[J");                                                             // Clear screen
    PRINTF("\033[" ANSI_GREEN "m");                                                     // Green
    //PRINTF("\033[1m");                                                                  
    PRINTF(__TIMESTAMP__);                                                              // Build timestamp
    PRINTF("   CH5%2x:%02X-%04X-%04X\n", CHIP_ID, chid_hx & 0x00FF, chid_hi, chid_lo);  // Chip ID
    PRINTF("\033[" ANSI_YELLOW "m");                                                    // Yellow
    getMyDate();                                                                        // Get date in number from __DATE__
    PRINTF("\033[1m");                                                                  // BOLD
    PRINTF("___%s.%02u%02u%02u___\n", "cdcHID-Mon", _year,_month,_day);                 // Project Name
    PRINTF("\033[0m\a");                                                                // Normal + beep
   
    CDC_flush();
  #endif                                               // Normal
  //=========================================================================
  PIN_output(P17);

  // Loop
  while(1) {
   
  if (HID_available()) {                 // received data packet?
      PIN_high(P17);
      len = HID_available();              // get number of bytes in packet
      counter=0;
      PRINTF("HID[%02d]:",  len);
      while(len--) {
        uint8_t c=HID_read();  
        PRINTF("|%02X",  c);
        if ((counter++%8==0) && (counter>0))PRINTF("| ");
        else if (len==0) PRINTF("|");
        }
      PRINTF("\n");
      DLY_ms(50);
      PIN_low(P17);

      CDC_flush();
      }

 
  }
}
