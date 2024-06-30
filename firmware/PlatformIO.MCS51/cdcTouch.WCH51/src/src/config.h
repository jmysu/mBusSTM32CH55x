// ===================================================================================
// User Configurations
// ===================================================================================

#pragma once

// Pin definitions
#define PIN_ADC             P11       // pin used as ADC input
#define PIN_LED             P16       // pin connected to LED

// Touchkey configuration
#define TOUCH_TH_LOW        3600      // key pressed threshold
#define TOUCH_TH_HIGH       4000      // key released threshold

// USB device descriptor
#define USB_VENDOR_ID       0xFFFF    // VID (shared www.voti.nl)
#define USB_PRODUCT_ID      0xCDC1    // PID (shared CDC-ACM)
#define USB_DEVICE_VERSION  0x0100    // v1.0 (BCD-format)

// USB configuration descriptor
#define USB_MAX_POWER_mA    50        // max power in mA 

// USB descriptor strings
#define MANUFACTURER_STR    'j','i','m','m','y','C','r','a','f','t'
#define PRODUCT_STR         'C','H','5','5','x',' ','D','e','v','B','o','a','r','d'
#define SERIAL_STR          'C','H','5','5','x','C','D','C'
#define INTERFACE_STR       'C','D','C','-','S','e','r','i','a','l'
