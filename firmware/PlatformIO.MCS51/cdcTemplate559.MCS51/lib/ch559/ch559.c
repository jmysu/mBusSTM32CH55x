// Copyright 2021 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ch559.h"

#include <stdarg.h>

#include "io.h"
#include "serial.h"
 

void (*runBootloader)(void) = 0xf400;

/*
int putchar(int c) {
  while (!TI)
    ;
  TI = 0;
  SBUF = c & 0xff;
  return c;
}
*/


static void delayU8us(uint8_t us) {
  // clang-format off
  us;
__asm
  mov r7,dpl
loop1$:
  mov a,#8
loop2$:
  dec a 
  jnz loop2$
  nop
  nop
  dec r7
  mov a,r7
  jnz loop1$
__endasm;
  // clang-format on
}

static inline void enter_safe_mode(void) {
  SAFE_MOD = 0x55;
  SAFE_MOD = 0xaa;
}

static inline void leave_safe_mode(void) {
  SAFE_MOD = 0;
}

void initialize(void) {
  // Clock
  // Fosc = 12MHz, Fpll = 288MHz, Fusb4x = 48MHz by PLL_CFG default
  enter_safe_mode();
  CLOCK_CFG = (CLOCK_CFG & ~MASK_SYS_CK_DIV) | 6;  // Fsys = 288MHz / 6 = 48MHz
  PLL_CFG =
      ((24 << 0) | (6 << 5)) & 0xff;  // PLL multiplier 24, USB clock divisor 6
  leave_safe_mode();

#ifndef _NO_UART0
  // UART0 115200 TX at P0.3
  P0_DIR |= 0x08;            // Set P0.3(TXD) as output
  P0_PU |= 0x08;             // Pull-up P0.3(TXD)
  PIN_FUNC |= bUART0_PIN_X;  // RXD0/TXD0 enable P0.2/P0.3
#endif

  SM0 = 0;  // 8-bits data
  SM1 = 1;  // variable baud rate, based on timer

  TMOD |= bT1_M1;               // Timer1 mode2
  T2MOD |= bTMR_CLK | bT1_CLK;  // use original Fsys, timer1 faster clock
  PCON |= SMOD;                 // fast mode
  TH1 = 230;                    // 256 - Fsys(48M) / 16 / baudrate(115200)

  TR1 = 1;  // Start timer1
  TI = 1;   // Set transmit interrupt flag for the first transmit

  // GPIO
  PORT_CFG = 0x00;  // 5mA push-pull for port 0-3 by default

  // SerialLibrary
  myserial_init();
  
/*
 * 起動時にはBootloaderが走りP4.6が押されてなければユーザープログラムの実行を開始する。
 * リセット時にはユーザープログラムが直接走るので、
 * 必要なら二回目の起動時には0xF400にジャンプすればBootloaderに処理が渡せて便利。
 * RESET_KEEPというPON時に0でリセット時は保持されるレジスタがあるので、
 * これを使って起動回数を判定するコードが組める。
 */
  if (RESET_KEEP) { //Default 0, changle to 1 for bootloading
    RESET_KEEP = 0;
    Serial.print("\033[2J\033[H ***Bootloader***\n\r");
    EA =0;
    runBootloader();
    }
  else
    Serial.print("\033[2J\033[H ---APP---\n\r");

  RESET_KEEP = 1; //bootloader next time
}

void delayMicroseconds(uint32_t us) {
  while (us > 255) {
    delayU8us(255);
    us -= 255;
  }
  delayU8us(us & 0xff);
}

void delay(uint32_t ms) {
  for (uint32_t i = 0; i < ms; ++i)
    delayMicroseconds(1000);
}

///////////////////////////////////////////////////////////////////////////////
/* Unique ID CH554 
#define ROM_CHIP_ID_HX    0x3FFA    // chip ID number highest byte (only low byte valid)
#define ROM_CHIP_ID_LO    0x3FFC    // chip ID number low word
#define ROM_CHIP_ID_HI    0x3FFE    // chip ID number high word
__code uint16_t __at(ROM_CHIP_ID_HX) chid_hx;
__code uint16_t __at(ROM_CHIP_ID_HI) chid_hi;
__code uint16_t __at(ROM_CHIP_ID_LO) chid_lo;
*/

/*Unique ID CH559*/
#define ROM_CHIP_ID_ADDR 0x20
__code uint16_t __at(ROM_CHIP_ID_ADDR+0) chid_d0;
__code uint16_t __at(ROM_CHIP_ID_ADDR+1) chid_d1;
__code uint16_t __at(ROM_CHIP_ID_ADDR+2) chid_d2;
__code uint16_t __at(ROM_CHIP_ID_ADDR+3) chid_d3;
__code uint16_t __at(ROM_CHIP_ID_ADDR+4) chid_d4;
__code uint16_t __at(ROM_CHIP_ID_ADDR+5) chid_d5;
__code uint16_t __at(ROM_CHIP_ID_ADDR+6) chid_d6;
__code uint16_t __at(ROM_CHIP_ID_ADDR+7) chid_d7;
/*******************************************************************************
* Function Name  : GetChipID(void)
* Description    : ��ȡID�ź�ID�ź�У��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint32_t GetChipID(void)
{
	uint8_t	d0, d1;
	uint16_t	xl, xh;
	E_DIS = 1;                                      //��������ж�

	d0 = chid_d0;
	d1 = chid_d1;                                   //ID�ŵ���
	xl = ( d1 << 8 ) | d0;
	d0 = chid_d2;
	d1 = chid_d3;                                   //ID�Ÿ���
	xh = ( d1 << 8 ) | d0;
	d0 = chid_d6;
	d1 = chid_d7;                                   //IDУ���
	E_DIS = 0;
	if ( (uint16_t)( xl + xh ) != (uint16_t)( ( d1 << 8 ) | d0 ) ) return( 0xFFFFFFFF );//У��ID��
	return( ( (uint32_t)xh << 16 ) | xl );
}
 