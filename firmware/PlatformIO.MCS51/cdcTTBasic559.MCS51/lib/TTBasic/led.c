/*
	Filename: led.c
	Description: LED API
*/
#include "gpio.h"

void led_init(){
	pinMode(1, 5, OUTPUT);
	digitalWrite(1, 5, 1);
}

void led_write(short no, unsigned char sw){
	pinMode(1, no, OUTPUT);
	digitalWrite(1, no, sw);
}
