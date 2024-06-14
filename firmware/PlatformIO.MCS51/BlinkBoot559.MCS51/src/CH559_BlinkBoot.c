/*
  CH559_BlinkBoot.c

  Blink p1.4/1.5 @100ms & wait P4.6 to entering bootloader!

  for CH559 Dev board w/ 2-LEDs & 2-buttons

  by jimmy.su @ 2024-06-14
*/
#include <ch554.h>
#include <debug.h>
 
typedef void(* __data FunctionReference)();
extern FunctionReference runBootloader;
FunctionReference runBootloader = (FunctionReference)0xF400;

//#define LED_PIN4 4
//#define LED_PIN5 5
//SBIT(LED4, 0x90, LED_PIN4); //P1.4
//SBIT(LED5, 0x90, LED_PIN5); //P1.4
SFR(P4_IN,	0xC1);	// ReadOnly: port 4 input

void main() {
  CfgFsys();
 
 
    //P1_MOD_OC = P1_MOD_OC & ~(1<<4);
    //P1_DIR_PU = P1_DIR_PU |	(1<<4);		
 	
    //==================================
    while (1) {
      P1=0b00100000; //P1.4
    	mDelaymS(100);

      P1=0b00010000; //P1.5
      mDelaymS(100);

      if(!(P4_IN & (1 << 6))) runBootloader(); //Jump to bootloader when p4.6 low
    }
}