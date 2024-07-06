#ifndef _EEPROM_H
#define _EEPROM_H

#include <stdint.h>

#include "ch554.h"

extern void eeprom_write_byte(uint8_t addr, uint8_t val);
extern uint8_t eeprom_read_byte(uint8_t addr);
#endif 