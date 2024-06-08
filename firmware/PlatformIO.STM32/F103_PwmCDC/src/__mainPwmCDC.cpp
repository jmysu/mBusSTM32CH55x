#include <Arduino.h>            // Load Libraries
#include "softPwmStm32.h"
 
#include "USBAPI.h"
#include "PluggableUSB.h"
//#include "MIDIUSB.h"


#define KEY PA0
//#define LED PA9 /*STLINK/V2*/
//LED 
#define LED PC13

 
void setup() {
  Serial.begin(115200);         // Start Serial Communication  
  delay(500);
  while (!Serial) {delay(100);}
  Serial.println();
  Serial.printf("STM32duino Core:%X.%X.%x\n",
     STM32_CORE_VERSION_MAJOR, STM32_CORE_VERSION_MINOR, STM32_CORE_VERSION_PATCH);
  pinMode(LED_BUILTIN, OUTPUT);
 
  #if defined(STM32F1)
    Serial.print("F103: Blinking LED...");
    //#if defined(USER_BTN)
    // pinMode(USER_BTN, INPUT_PULLUP);
    //#endif
  #elif defined(STM32F4)
    Serial.print("F4:Blinking PC13 & checking User KeyA0.\n");
    pinMode(KEY, INPUT_PULLUP);
  #else
    Serial.print("STM32:");
  #endif

  softPwmSTM32Init();
  softPwmSTM32Attach(LED_BUILTIN, 50);
}

void loop() {
  for (int i=0;i<3;i++) {
    //digitalWrite(LED, HIGH);   
    softPwmSTM32Set(LED_BUILTIN,0);
    delay(300);                       
    //digitalWrite(LED, LOW);   
    softPwmSTM32Set(LED_BUILTIN,100);
    delay(300);   
    }
  
  for(uint8_t i=100;i>0;i--){
    softPwmSTM32Set(LED_BUILTIN,i);
    delay(10);
  }
  for(uint8_t i=0;i<100;i++){
    softPwmSTM32Set(LED_BUILTIN,i);
    delay(10);
  }
  delay(1000);
//#if defined(USER_BTN)
//  if (digitalRead(USER_BTN)==LOW)
//    Serial.println("User key pressed!");
//#endif
    
}