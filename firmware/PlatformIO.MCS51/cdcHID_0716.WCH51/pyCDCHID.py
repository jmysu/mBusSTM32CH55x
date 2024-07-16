import usb.core
import sys
import time

dev = usb.core.find(idVendor=0xFFFF, idProduct=0xCDCD)

# was it found?
if dev is None:
    raise ValueError('Device not found')
else:
    try:
        dev.reset
    except Exception as e:
        print( 'reset', e)
    time.sleep(1)

dev.set_configuration()
print(dev)

cfg  = dev[0]
#print(cfg)
intf = cfg[(0,0)]
#print(intf)
print("***")
epIn    = intf[0]
print(epIn)
epOut   = intf[1]
print(epOut)
#dev.write(ep.bEndpointAddress, [0x00, 0x00,0x04,0x04,0xFF,0xFF,0xFF,0x00, 0x00], 1000)
# dev.ctrl_transfer(bmRequestType, bRequest, wValue=0, wIndex=0, data_or_wLength=None, timeout=None)
data = [0x55,0xAA,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0xBE,0xEF]
print("Writing data:", bytes(data).hex().upper())
epOut.write(data)