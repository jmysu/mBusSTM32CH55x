 # Based on https://github.com/walac/pyusb/blob/master/docs/tutorial.rst
import time
import usb.core
import usb.util

def hid_set_report(dev, report):
    """ Implements HID SetReport via USB control transfer """
    dev.ctrl_transfer(
        0x21,  # REQUEST_TYPE_CLASS | RECIPIENT_INTERFACE | ENDPOINT_OUT
        9,     # SET_REPORT
        0x200, # "Vendor" Descriptor Type + 0 Descriptor Index
        0,     # USB interface № 0
        report # the HID payload as a byte array -- e.g. from struct.pack()
    )

def hid_get_report(dev):
    """ Implements HID GetReport via USB control transfer """
    return dev.ctrl_transfer(
        0xA1,  # REQUEST_TYPE_CLASS | RECIPIENT_INTERFACE | ENDPOINT_IN
        1,     # GET_REPORT
        0x200, # "Vendor" Descriptor Type + 0 Descriptor Index
        0,     # USB interface № 0
        64     # max reply size
    )

print("List Avaliable USB Devives:")
for dev in usb.core.find(find_all=True):
    print(dev)
print("\n")

# Find our device
dev = usb.core.find(idVendor=0x0483, idProduct=0xCAFE)
# Was it found?
if dev is None:
    raise ValueError('Device not found')

dev.set_configuration()

cfg  = dev[0]
intf = cfg[(0,0)]
ep0  = intf[0]
ep1  = intf[1]

#dev.write(ep.bEndpointAddress, [0x00, 0x00,0x04,0x04,0xFF,0xFF,0xFF,0x00, 0x00], 1000)
#dev.ctrl_transfer(bmRequestType, bRequest, wValue=0, wIndex=0, data_or_wLength=None, timeout=None)

"""
print("print cfg")
print(cfg)
print("print intf")
print(intf)

print("print ep0")
print(ep0)
print("print ep1")
print(ep1)
"""

#Blink 10 times in 300ms
print("\nStart blinking...")
for x in range(10):
    ep1.write([1])
    time.sleep(0.3)
    ep1.write([0])
    time.sleep(0.3)
print("-END-");