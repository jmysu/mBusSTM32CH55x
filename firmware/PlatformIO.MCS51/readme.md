
cdcTouch.WCH51
--------
   CH55x touch key w/ CDC! <br>
<img src="https://github.com/jmysu/mBusSTM32USB/blob/main/pic/CH554Touch.jpg" height=320 > 
   <br>
   CH55x CMDs w/ CDC! <br>
<img src="https://github.com/jmysu/mBusSTM32USB/blob/main/pic/CH554CMDs0706.jpg"> <br>
   - The PlatformIO is not passing SDCC LinkerOptionFlags well!:-( <br>
     Right now, the workaround is adding the extra python script in the .ini!!! <br>
     See issue here: https://github.com/platformio/platform-intel_mcs51/issues/11 <br>
     <br>

  Blinky USB control project: <br>
     > Set/Read/Toggle LED blink time via USB control transfer, code size 1930 bytes! <br>
     > Use _pyBlinky.py_ to send commands to device! <br>
<img src="https://github.com/jmysu/mBusSTM32USB/blob/main/pic/BlinkyUSBcapture.jpg"> <br>
     <br>

   
---
- [CH554 SDK port](https://github.com/Blinkinlabs/ch554_sdcc) CH551/552/554 software development kit for SDCC<br>
