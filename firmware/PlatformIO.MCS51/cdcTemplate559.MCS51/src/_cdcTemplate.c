/*
 *  _cdcTemplate.c
 *
 *    CH55x CDC Template  w/ CDC printf & Serial.printf(UART0)
 *
 *    by jimmy.su @ 2024
 *
 *    LINKFLAGS: -mmcs51 --iram-size 256 --xram-size 6144 --code-size 65536 --out-fmt-ihx --xram-size 6144 --xram-loc 0 --model-large --stack-auto    
 * 
 *    Flash: [==        ]  22.1% (used 14503 bytes from 65536 bytes)
 * 
 *          __   __
 *    P3.2 -|o\_/ |- 3.3V (connect to GND via 0.1uF)
 *    P1.4 -|  C  |- 5.0V (connect to USB VBUS, connect to GND via 0.1uF)
 *    P1.5 -|  H  |- GND  (to USB GND)
 *    P1.6 -|  5  |- USB D-
 *    P1.7 -|  5  |- USB D+ (short 3.3v to BOOT)
 *    Reset-|  2  |- P3.4
 *    P3.1 -|  G  |- P3.3  
 *    P3.0 -|_____|- P1.1  
 * 
 * 
 * 
 */
#include <stdio.h>
#include <stdarg.h>

//lib/ch559
#include "ch559.h"
#include "serial.h"
#include "io.h"
#include "usb/cdc_device.h"

//lib/Utils
//#include "ansiColor.h"
#ifdef _TITLE_STAMP_
 #warning "W/ TITLE_STAMP"
 #include "myDate.h"
#else
 #warning "W/O TITLE_STAMP"
#endif

//ToYoShiKi TinyBasic
//#include "ttbasic.h"


__xdata uint8_t cdcBuffer[64]; //cdc buffer
///////////////////////////////////////////////////////////////////////////////
static const char kString01Manufacturer[] = "jimmyCraft";
static const char kString02Product[]      = "CDC Template";
static const char kString03SerialNumber[] = "CH559";

static uint8_t get_string_length(uint8_t no) {
  switch (no) {
    case 1:
      return sizeof(kString01Manufacturer) - 1;
    case 2:
      return sizeof(kString02Product) - 1;
    case 3:
      return sizeof(kString03SerialNumber) - 1;
    }
  return 0;
}
static const char* get_string(uint8_t no) {
  switch (no) {
    case 1:
      return kString01Manufacturer;
    case 2:
      return kString02Product;
    case 3:
      return kString03SerialNumber;
    }
  return 0;
}

int icdcCount=0; 
void recv(const uint8_t* buffer, uint8_t len) {
  // Echo back Serial/UART0
  //Serial.printf("recv: %x, %d\n", *buffer, len);

  // Echo back to CDC
  //cdc_device_send(buffer, len);

  // cdcBuffer capture, assume len<64
  Serial.printf("CDC[%d]:", len);
  icdcCount=len;
  int i=0;
  while (len--){
    cdcBuffer[i++]=*(buffer++);
    Serial.printf("|%x",cdcBuffer[i-1]);
    }
  Serial.println("|");
 
}

///////////////////////////////////////////////////////////////////////////////
// Re-direct printf to USB-CDC
int putchar(int c) {
    if (RESET_KEEP) {//Normal, putc to USB-CDC
      cdc_device_send(&c,1);
      }
    else { //Bootloader, direct to Serial UART
      Serial.putc(c);
      }
    return c;
}


///////////////////////////////////////////////////////////////////////////////
// get ONE char from CDC buffer
int getchar(void) {
  if (icdcCount) {
    icdcCount=0;  //reset counter
    return (int)*cdcBuffer;
    }
  else return 0;
}

extern void basic(void);
///////////////////////////////////////////////////////////////////////////////
//main
///////////////////////////////////////////////////////////////////////////////
void main(void) {
  initialize(); //Also init UART, USB, toggling next boot
 
  //CDC Device Defines
  struct cdc_device device;
  device.id_vendor = 0xFFFF;
  device.id_product = 0xcdcd;
  device.bcd_device = 0x0100;
  device.get_string_length = get_string_length;
  device.get_string = get_string;
  device.recv = recv;
  cdc_device_init(&device);
  delay(100);
  stampingTitle("cdcTemplate559");

  while (1) {
    //basic();

    printf("Hello CDC!\r\n");
    //Serial.printf("Hi!\n");
    for (int i=0;i<30;i++) {
        if (icdcCount)  {
            printf("\tCDC:%d\r\n",icdcCount);
            icdcCount=0;
            }
        else
          delay(100);
        }
    }
}