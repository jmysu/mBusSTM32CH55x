import usb.core

dev = usb.core.find(idVendor=0xffff, idProduct=0x001e)

# was it found?
if dev is None:
    raise ValueError('Device not found')
else:
    try:
        #COMMAND_TOGGLE_BLINK 0xD0
        dev.ctrl_transfer(0x40, 0xD1,0xdead,0xbeef)
        #COMMAND_SET_BLINK_TIME 0xD3
        #dev.ctrl_transfer(0x40, 0xD3,0xdead,0xbeef,4)

        #COMMAND_READ_BLINK_TIME
        ret=dev.ctrl_transfer(0xC0, 0xD0, 0x0, 0x0, 2)
        print("Blink time:",ret[0]+ret[1]*256)

    except usb.core.USBError as e:
        print('Apply except:',  e)
