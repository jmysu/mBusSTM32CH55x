// ===================================================================================
// USB Descriptors and Definitions
// ===================================================================================
//
// Definition of USB descriptors and endpoint sizes and addresses.
//
// The following must be defined in config.h:
// USB_VENDOR_ID            - Vendor ID (16-bit word)
// USB_PRODUCT_ID           - Product ID (16-bit word)
// USB_DEVICE_VERSION       - Device version (16-bit BCD)
// USB_MAX_POWER_mA         - Device max power in mA
// All string descriptors.
//
// In the makefile the following microcontroller settings must be made:
// XRAM_LOC   = 0x0100
// XRAM_SIZE  = 0x0300

#pragma once
#include <stdint.h>
#include "usb.h"
#include "config.h"

// ===================================================================================
// USB Endpoint Definitions
//
//  EP0 8 bytes CONTROL
//  EP1 8 bytes HID In/OUT
//  EP2 64 bytes CDC RX
//  EP2TX 64 bytes CDC TX
//
// ===================================================================================
#define EP0_SIZE        8
#define EP1_SIZE        8
#define EP2_SIZE        64 /*MUST BE 64!!!*/

#define EP0_BUF_SIZE    EP_BUF_SIZE(EP0_SIZE)
#define EP1_BUF_SIZE    EP_BUF_SIZE(EP1_SIZE) 
#define EP2_BUF_SIZE    EP_BUF_SIZE(EP2_SIZE)*2 /*64+64 buffer*/
#define EP_BUF_SIZE(x)  (x+2<64 ? x+2 : 64)     /*DMA will add two bytes CRC at the end when less than 64*/
// bUEPn_RX_EN & bUEPn_TX_EN & bUEPn_BUF_MOD: USB endpoint 1/2/3 buffer mode, buffer start address is UEPn_DMA
//   0 0 x:  disable endpoint and disable buffer
//   1 0 0:  64 bytes buffer for receiving (OUT endpoint)
//   1 0 1:  dual 64 bytes buffer by toggle bit bUEP_R_TOG selection for receiving (OUT endpoint), total=128bytes
//   0 1 0:  64 bytes buffer for transmittal (IN endpoint)
//   0 1 1:  dual 64 bytes buffer by toggle bit bUEP_T_TOG selection for transmittal (IN endpoint), total=128bytes
// * 1 1 0:  64 bytes buffer for receiving (OUT endpoint) + 64 bytes buffer for transmittal (IN endpoint), total=128bytes
//   1 1 1:  dual 64 bytes buffer by bUEP_R_TOG selection for receiving (OUT endpoint) + dual 64 bytes buffer by bUEP_T_TOG selection for transmittal (IN endpoint), total=256bytes

#define EP0_ADDR        0
#define EP1_ADDR        (EP0_ADDR + EP0_BUF_SIZE)
#define EP2_ADDR        (EP1_ADDR + EP1_BUF_SIZE)
#define EP2_ADDRTX      (EP2_ADDR + 64)
#define EP_END          (EP2_ADDR + EP2_BUF_SIZE)

__xdata __at (EP0_ADDR) uint8_t EP0_bufCNTRL[EP0_BUF_SIZE]; //8 CONTROL  
__xdata __at (EP1_ADDR) uint8_t EP1_buffer[EP1_BUF_SIZE];   //8 NOT USE
__xdata __at (EP2_ADDR) uint8_t EP2_bufCDCRX[EP2_SIZE];     //64 CDC RX
__xdata __at (EP2_ADDRTX) uint8_t EP2_bufCDCTX[EP2_SIZE];   //EP2+64 CDC TX
__xdata __at (EP_END)   uint8_t ___EP_END___[1];            //Mark

// ===================================================================================
// Device and Configuration Descriptors
// ===================================================================================
typedef struct _USB_CFG_DESCR_CDC {
  USB_CFG_DESCR config;
  USB_IAD_DESCR association;
  USB_ITF_DESCR interface0;
  uint8_t functional[19];
  USB_ENDP_DESCR ep1IN;
  USB_ITF_DESCR interface1;
  USB_ENDP_DESCR ep2OUT;
  USB_ENDP_DESCR ep2IN;
} USB_CFG_DESCR_CDC, *PUSB_CFG_DESCR_CDC;
typedef USB_CFG_DESCR_CDC __xdata *PXUSB_CFG_DESCR_CDC;

extern __code USB_DEV_DESCR DevDescr;
extern __code USB_CFG_DESCR_CDC CfgDescr;

// ===================================================================================
// String Descriptors
// ===================================================================================
extern __code uint16_t LangDescr[];
extern __code uint16_t ManufDescr[];
extern __code uint16_t ProdDescr[];
extern __code uint16_t SerDescr[];
extern __code uint16_t InterfDescr[];

#define USB_STR_DESCR_i0    (uint8_t*)LangDescr
#define USB_STR_DESCR_i1    (uint8_t*)ManufDescr
#define USB_STR_DESCR_i2    (uint8_t*)ProdDescr
#define USB_STR_DESCR_i3    (uint8_t*)SerDescr
#define USB_STR_DESCR_i4    (uint8_t*)InterfDescr
#define USB_STR_DESCR_ix    (uint8_t*)SerDescr
