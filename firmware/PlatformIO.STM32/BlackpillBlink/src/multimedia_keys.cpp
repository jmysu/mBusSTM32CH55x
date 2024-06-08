/*
 * USB Composite device consisting of keyboard and consumer HID
 * Extends the features of Keyboard library by adding
 * multimedia keys support. Depends on MediaKeyboard library.
 * 
 * A rotary encoder and four push buttons are required. See the schematic at
 * https://www.onetransistor.eu/2020/03/usb-multimedia-keys-on-arduino-stm32.html
 *
 * RotaryEncoder library by Matthias Hertel is required for successful compilation. 
 * You can find it in Library Manager or at:
 * http://www.mathertel.de/Arduino/RotaryEncoderLibrary.aspx
 *
 * USB HID support must be enabled from Tools menu.
 * 
 * Copyright (C) 2020 One Transistor (https://www.onetransistor.eu).
 */
#include <Arduino.h>

#include <MediaKeyboard.h>
#include <Keyboard.h>
 

#define PIN_MUTE PA0
 

#define DEBOUNCE_DELAY 250

void setup() {
  pinMode(PIN_MUTE, INPUT_PULLUP);


  MediaKeyboard.begin();
  delay(1000); // wait long enough to let the host configure device

  // Keyboard class can be used in the same time
  // Do not call both Keyboard.begin() and MediaKeyboard.begin()
  char *s = "Hello from STM32 USB Keyboard.\r\n"
            "Use the rotary encoder to change volume and\r\n"
            "the buttons to play and change media.\0";
    int i = 0;
    while (s[i] != 0) {
    Keyboard.write(s[i]);
    i++;
    }
}

void loop() {
  // evaluate potential button presses
  if (digitalRead(PIN_MUTE) == LOW) {
    MediaKeyboard.press(VOLUME_MUTE);
    MediaKeyboard.release();
    delay(DEBOUNCE_DELAY);
  }



}
