/*

=============================================================================
BluePIll F103 TinyBasicPlus2

*/
#include <Arduino.h>            // Load Libraries
#include "TinyBasicPlus2.h"
//#include "softPwmStm32.h"
 
#include "USBAPI.h"
//#include "PluggableUSB.h"
//#include "MIDIUSB.h"
 
//#define KEY PA0 /*F4 BlackPill*/
//#define LED PA9 /*STLINK/V2*/
//LED 
#define LED PC13

 
void setup()
{
  Serial.begin(115200);         // Start Serial Communication  
  delay(500);
  while (!Serial) {delay(100);}
  Serial.println();
  Serial.printf("STM32duino Core:%X.%X.%x\n",
     STM32_CORE_VERSION_MAJOR, STM32_CORE_VERSION_MINOR, STM32_CORE_VERSION_PATCH);
  pinMode(LED, OUTPUT);
 
  Serial.print("F103: Starting TinyBasicPlus2...");

   setupTinyBasicPlus2();
}

 
void loop()
{        
   while (Serial.available())
      processTinyBasicPlus2();
 
   digitalWrite(LED, !digitalRead(LED));
   delay(500);
}