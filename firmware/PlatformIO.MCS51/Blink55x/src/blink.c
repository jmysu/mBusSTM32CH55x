/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the simpleCH552
  it is attached to digital pin P3.3

  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman
  modified 13 Jun 2020
  by Deqing Sun for use with CH55xduino

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/

#include <8052.h>
#include <stdint.h>

// #define LED_BUILTIN 33
// Blink an LED connected to pin 1.7

#include <ch554.h>
#include <debug.h>

#define LED_PIN 7
SBIT(LED, 0x90, LED_PIN);

void main() {
    CfgFsys();

    // Configure pin 1.7 as GPIO output
    P1_DIR_PU &= 0x0C;
    P1_MOD_OC = P1_MOD_OC & ~(1<<LED_PIN);
    P1_DIR_PU = P1_DIR_PU |	(1<<LED_PIN);

    while (1) {
    	mDelaymS(100);
        LED = !LED;
    }
}