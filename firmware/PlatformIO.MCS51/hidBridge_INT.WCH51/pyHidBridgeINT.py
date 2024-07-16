#!/usr/bin/env python3
# ===================================================================================
# Project:   USB HID to I2C Bridge Demo for CH551, CH552, CH554
# Version:   v1.0
# Year:      2022
# Author:    Stefan Wagner
# Github:    https://github.com/wagiminator
# License:   http://creativecommons.org/licenses/by-sa/3.0/
# ===================================================================================
#
# Description:
# ------------
# This simple demo shows the basic functionality of the USB HID to I2C bridge. An
# image is shown on the OLED and then scrolled.
#
# Dependencies:
# -------------
# - pyusb

import sys
import time
import usb.core
import usb.util


# USB device settings
VENDOR_ID   = 0x16C0    # VID
PRODUCT_ID  = 0x05DF    # PID
PACKET_SIZE = 64        # HID packet size
INTERFACE   = 0         # HID interface number
HID_EP_OUT  = 1         # endpoint for data transfer


# ===================================================================================
# Main Function
# ===================================================================================

def _main():
    try:
        print('Test hidBridgeINT, Connecting to device ...')
        LED = Bridge()
    except Exception as ex:
        sys.stderr.write('ERROR: ' + str(ex) + '!\n')
        sys.exit(1)

    try:
        #print('Starting the show ...')
        #oled.senddata(PIC1)
        time.sleep(1)
        #for i in range(64*4+1):
        #    oled.scroll(i)
        #    time.sleep(0.02)
    except Exception as ex:
        sys.stderr.write('ERROR: ' + str(ex) + '!\n')
        LED.exit()
        sys.exit(1)

    print('DONE.')
    #oled.exit()
    #sys.exit(0)


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
                    
            #if self.dev.is_kernel_driver_active(INTERFACE):
            #    self.dev.detach_kernel_driver(INTERFACE)
            time.sleep(1)
        except:
            raise Exception('Could not access USB device')
        self.setup()

    def exit(self):
        usb.util.release_interface(self.dev, INTERFACE)
        self.dev.attach_kernel_driver(INTERFACE)


    def senddata(self, data):
        while len(data) > 0:
            self.dev.write(HID_EP_OUT, [ADDR, DAT_MODE] + data[:(PACKET_SIZE-2)])
            data = data[(PACKET_SIZE-2):]

    def sendcommand(self, cmd):
        if len(cmd) <= PACKET_SIZE-2:
            self.dev.write(HID_EP_OUT, [ADDR, CMD_MODE] + cmd)
        else:
            raise Exception('Command string too long')

    def setup(self):
        print("Sending CMD")
        self.sendcommand(INIT_CMD)
        time.sleep(1)
        print("Sending DAT")
        self.senddata(INIT_DAT)



# ===================================================================================
# OLED Constants
# ===================================================================================

ADDR     = 0xAD    # OLED write address
CMD_MODE = 0xCD   # set command mode
DAT_MODE = 0xDA    # set data mode

# 
INIT_CMD = [
  0xC0, 0xC1,           
  0xC2, 0xC3,           
  0xC4, 0xC5,           
  0xC6, 0xC7]

INIT_DAT = [
  0xD0, 0xD1,           
  0xD2, 0xD3,           
  0xD4, 0xD5,           
  0xD6, 0xD7]
# ===================================================================================
# OLED Picture (Bitmap Converter: http://www.majer.ch/lcd/adf_bitmap.php)
# ===================================================================================

PIC1 = [0]


# ===================================================================================

if __name__ == "__main__":
    _main()
