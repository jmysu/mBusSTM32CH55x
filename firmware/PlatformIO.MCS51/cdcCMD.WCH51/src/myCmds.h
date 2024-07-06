/*
 *  cdcCMDs.h
 *
 *    CH55x CDC CMDs
 * 
 *    by jimmy.su @ 2024
 * 
 */
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "src/ch554.h"
#include "src/system.h"
#include "src/delay.h"
#include "src/eeprom.h"
#include "src/gpio.h"

#include "xprintf.h"
#include "ansiColor.h"

//forware declaration
extern void cmdHelp();
extern void cmdHeap();
extern void cmdBL();
extern void cmdP1W();
extern void cmdP1R();
extern void cmdEER();
extern void cmdEEW();
extern void cmdReset();

extern void cmdMatch(char *s);
extern char* cmdParam(char c);
extern __xdata char counterParam;
extern __xdata uint8_t buf[];
extern __xdata uint8_t buf1[];