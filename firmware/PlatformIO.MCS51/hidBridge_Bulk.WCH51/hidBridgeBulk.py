#!/usr/bin/env python3
# ===================================================================================
# Project:   USB Vendor Class to I2C Bridge Demo for CH551, CH552, CH554
# Version:   v1.0
# Year:      2022
# Author:    Stefan Wagner
# Github:    https://github.com/wagiminator
# License:   http://creativecommons.org/licenses/by-sa/3.0/
# ===================================================================================
#
# Description:
# ------------
# This simple demo shows the basic functionality of the USB vendor class to I2C bridge.
# An image is shown on the OLED and then scrolled.
#
# Dependencies:
# -------------
# - pyusb

import sys
import time
import usb.core

# USB device settings
VENDOR_ID   = 0x16C0      # VID (idVendor)
PRODUCT_ID  = 0x05DC      # PID (idProduct)
BULK_EP_OUT = 0x01        # (bEndpointAddress) for bulk writing to device
BULK_EP_IN  = 0x81        # (bEndpointAddress) for bulk reading from device

# USB vendor class control requests (bRequest)
VEN_REQ_BOOTLOADER  = 1   # enter bootloader
VEN_REQ_LED17_ON    = 0x71   # turn on LED17
VEN_REQ_LED17_OFF   = 0x70   # turn off LED17
VEN_REQ_I2C_START   = 4   # set start condition on I2C bus
VEN_REQ_I2C_STOP    = 5   # set stop condition on I2C bus

VEN_REQ_WRITE = 0x40      # (bRequestType): vendor host to device
VEN_REQ_READ  = 0xC0      # (bRequestType): vendor device to host

# ===================================================================================
# Main Function
# ===================================================================================

def _main():
    try:
        print('Reset & set config...')
        LED = Bridge()
    except Exception as ex:
        sys.stderr.write('ERROR: ' + str(ex) + '!\n')
        sys.exit(1)

    try:
        #LED.OnOff()
        #time.sleep(.1)
        #LED.OnOff()
        #time.sleep(.1)
        LED.sendcommand(INIT_CMD)
        time.sleep(.2)
        LED.senddata(INIT_DAT)
    except Exception as ex:
        sys.stderr.write('ERROR: ' + str(ex) + '!\n')
        sys.exit(1)

    print('DONE.')
    sys.exit(0)


# ===================================================================================
# Bridge Class
# ===================================================================================

class Bridge():
    def __init__(self):
        self.dev = usb.core.find(idVendor = VENDOR_ID, idProduct = PRODUCT_ID)
        if self.dev is None:
            raise Exception('Device not found')
        try:
            print("Device Reset...")
            self.dev.reset()
            time.sleep(1)
            print("Device Config...")
            self.dev.set_configuration()
        except:
            raise Exception('Could not access USB device')
        
        self.setup()
       
 

    def sendcontrol(self, ctrl):
        self.dev.ctrl_transfer(VEN_REQ_WRITE, ctrl, 0, 0)

    def sendstream(self, stream):
        print("Cntrl-Out:Start...")
        self.sendcontrol(VEN_REQ_I2C_START)
        #time.sleep(.1)
        print("Bulk-Out: EP", BULK_EP_OUT)
        self.dev.write(BULK_EP_OUT, stream, 100)
        print("Cntrl-Out:Stop...", end="\n\n")
        self.sendcontrol(VEN_REQ_I2C_STOP)

    def senddata(self, data):
        #self.sendstream([ADDR, DAT_MODE] + data)
        self.dev.write(BULK_EP_OUT, data, 100)

    def sendcommand(self, cmd):
        #self.sendstream([ADDR, CMD_MODE] + cmd)
        self.dev.write(BULK_EP_OUT, cmd, 100)

    def setup(self):
        hex_string = "".join("|%02X" % b for b in INIT_CMD)
        print(hex_string, end="|\n")
        self.sendstream(INIT_CMD)
        time.sleep(.5)
        self.sendstream(INIT_DAT)
        time.sleep(.5)

        #hex_string = "".join("|%02X" % b for b in INIT_DAT)
        #print(hex_string, end="|\n")
        #self.senddata(INIT_DAT)
        #time.sleep(.5)

    def OnOff(self):
        print("Cntrl-Out:LED_On")
        self.sendcontrol(VEN_REQ_LED17_ON)
        time.sleep(0.5)
        self.sendcontrol(VEN_REQ_LED17_OFF)
        print("Cntrl-Out:LED_Off")
        
    def boot(self):
        self.sendcontrol(VEN_REQ_BOOTLOADER)


# ===================================================================================
# OLED Constants
# ===================================================================================

ADDR     = 0xAD    # 
CMD_MODE = 0xCC    #  
DAT_MODE = 0xDD    # 

# OLED initialisation sequence
INIT_CMD = [
  0xA8, 0x3F,           # set multiplex ratio  
  0x8D, 0x14,           # set DC-DC enable  
  0x20, 0x00,           # set horizontal memory addressing mode
  0xC8, 0xA1,           # flip screen
  0xDA, 0x12,           # set com pins
  0xAF                  # display on
]
INIT_DAT =[
    0xDE, 0xED,
    0xBE, 0xEF
]
# ===================================================================================
# OLED Picture (Bitmap Converter: http://www.majer.ch/lcd/adf_bitmap.php)
# ===================================================================================

PIC1 = [0x00]


# ===================================================================================

if __name__ == "__main__":
    _main()
