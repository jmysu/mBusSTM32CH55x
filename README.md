# mBusSTM32USB
mBus sockets w/ **8051&Arm-M3**, work in process!
Please check ![WIKI](https://github.com/jmysu/mBusSTM32CH55x/wiki) for the latest changes!

<img src="pic/mBusSTM32USB_0608.jpg" height=320 width=45%> &nbsp;&nbsp; <img src="pic/mBusSTM32USB_0607.jpg" height=320 width=45%>
<br>
<br>
Z80 ~~is calling~~ called **_LastBuy_** by 06/14-2024, what's changed in 50 years!<br>
<img src="pic/Z80vsGB200.jpg" width=400 > <img src="pic/Z80-RIP2024.jpg"  width=40%  >


With **_Shared_** MCUs in China, the hardware price is quite affordable!<br>
有了大陸国内共享MCU，硬件价格实在是贼便宜的!!<br>
<img src="pic/LCSC_CH552APM32.jpg" height=400 >
<br>
<br>

## 0715 Updates:
- Update circuit layout in hardware. <br>
- Add more CH55x USB projects (USB Control/Interrupt/Bulk transfer w/ PlatformIO-WCH51) in firmware folder. <br>
  Also attached the pyUSB Host test jig for individual testing in each folder (NEED _libUSB_ as USB driver). <br>
  Finally, add the HID+CDC project that monitoring HID traffics with USB-CDC serial.<br>
  
  <img src="pic/mBusSTM32CH55x_0715Top.jpg" width=40% >  <img src="pic/mBusSTM32Ch55x_0715Bottom.jpg" width=40% > <br>

  <br>
---


## References <br>

- [SDCC](https://sdcc.sourceforge.net/) SDCC - Small Device C Compiler. <br>
- [MCS-51s](https://github.com/SoCXin/MCS-51) 嵌入式应用的单片微型计算机的经典体系结构. <br> 
- [Fake pills](https://github.com/keirf/Greaseweazle/wiki/STM32-Fakes) About fake pills. <br>
- [CH559 baremetal programming](https://kprasadvnsi.com/tags/ch559/)
- [CH559 DOC](https://kprasadvnsi.github.io/CH559_Doc_English/docs/16-usb/) Brief CH559 english document.
- [CH55xG Pinout](https://oshwlab.com/wagiminator/ch55xg-development-board) CH55xG Dev Board.
- [CH55x Getting start](https://rabid-inventor.blogspot.com/2020/05/getting-started-with-ch55x.html) Getting started with the CH55x Microcontroller.
