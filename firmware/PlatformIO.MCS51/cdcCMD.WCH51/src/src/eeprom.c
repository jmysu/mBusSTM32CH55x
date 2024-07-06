/*
 * eeprom.c
 *
 *    CH55x internal EE r/w
 * 
 *          Modified from CH55xduino
 * 
 *    by Jimmy.su@2024
 */

/*
 created by Deqing Sun for use with CH55xduino
 */
#include "eeprom.h"


void eeprom_write_byte(uint8_t addr, uint8_t val) {

  if (addr >= 128) {
    return;
  }

  SAFE_MOD = 0x55;
  SAFE_MOD = 0xAA;        // Enter Safe mode
  GLOBAL_CFG |= bDATA_WE; // Enable DataFlash write
  SAFE_MOD = 0;           // Exit Safe mode
  ROM_ADDR_H = DATA_FLASH_ADDR >> 8;
  ROM_ADDR_L = addr << 1;
  ROM_DATA_L = val;
  if (ROM_STATUS & bROM_ADDR_OK) { // Valid access Address
    ROM_CTRL = ROM_CMD_WRITE;      // Write
  }
  SAFE_MOD = 0x55;
  SAFE_MOD = 0xAA;         // Enter Safe mode
  GLOBAL_CFG &= ~bDATA_WE; // Disable DataFlash write
  SAFE_MOD = 0;            // Exit Safe mode
}

uint8_t eeprom_read_byte(uint8_t addr) {

  ROM_ADDR_H = DATA_FLASH_ADDR >> 8;
  ROM_ADDR_L = addr << 1; // Addr must be even
  ROM_CTRL = ROM_CMD_READ;
  return ROM_DATA_L;
}