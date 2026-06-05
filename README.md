# ESP-USB

## What is this? 

This is a "wireless" USB-cable for USB devices. It can "wirelessify" (almost) any USB device with only 2 ESP32-S3's. It can be powered by 5V or even by a 3.7V LiPo battery on the senders side (the side with the USB device). I also features a minimal latency of 2ms, or sometimes even less!!!

### Features: 
- send HID-Protocols wirelessly
- pass the HID-Protocol to the PC with tinyusb
- 1 USB-C
- 1 Status-LED
- 3.7V to 5V booster
- 5V to 3.3V converter
- LiPo battery charger and protector
- 2 buttons, boot and reset
- custom USB-C-CC-Pins-handling

---

## Why? 

I got a new keyboard and wanted it to be wireless. It wasnt possible to do this from the firmware side, because it uses a rpi pico 2 as MCU and upgrading it to a rpi pico 2W wont do much because ZMK doesnt like the bluetooth feature of it. So I thought if it was possible to send the HID-reports, which the keyboard is sending constantly over 2.4Ghz, so the wifi bandwith. After some research I found out that the ESP32-S3 features USB-OTG, so it can read these reports and send them over ESP-Now, a custom "wifi" specifically for ESPs. This will reduce latency by alot. 

--- 

## How?

There are 2 ways of making this project. 1 DIY-version and 1 PCB-version. If you have 2 ESP32-S3-Wroom-1 Devboards, feel free to take the DIY road. 

### DIY:

To replicate this project with 2 ESP32-S3-Devkit-C's you simply just need to download the 2 firmwares, the senders and the recievers. The [ESP-IDF-extension](https://docs.espressif.com/projects/vscode-esp-idf-extension/en/latest/index.html) for [VS-Code](https://code.visualstudio.com/) is also required to flash the firmware. Flashing the firmware is also really easy. After sucessfully installing the ESP-IDF-extension, open the firmware of the reciever by going to the top left and selecting file > open folder > and open the folder containing the reciever firmware. Then go to the bottom bar and select UART for the flash method, its the button with the small little star. We also need to specifiy the COM-port of the ESP. Finding that out is OS-specific, but there are also alot of tutorials for this. For the IDF-Target, select ESP32-S3, since thats out device. Now we are almost finished. The last step we need to take is building and flashing the firmware. We can archieve this by clicking the fire icon a bit more on the right. This will build and flash the firmware. Now repeat this exact precess with the sender and you will have a working ESP-USB-cable. 

<details>
  <summary>
    <h4>TL;DR</h4>
  </summary> 

  - download VS-Code and the ESP-IDF extension
  - download the firmware
  - open the folder with the firmware
  - select UART for flash method
  - select the COM-Port
  - select ESP32-S3 as device
  - press the small fire to build and flash
  - repeat for 2nd device
</details>

### PCB: 

When deciding to use my custom PCB, which is smaller, we need to first order it. JLCPCB for example is a pretty good option. After having it in person, we need to connect it to our PC and hold the boot button while pressing the reset button once. After pressing the reset button we can release the boot button for now. This will set the ESP into download mode and we can flash it exactly like a ESP32-S3-Devkit-C. How this works is explained in the DIY-section above. After flashing both the reciever and the sender we need to decide on a way to power our sender (the one on the USB device side), because it needs to power our USB-device. There are 2 ways, one is connecting a LiPo battery and the other is using 2 of the 4 test pads. The LiPo battery is straight forward, but for the test pads you need 2 cables and a usb breakout board to power the pcb with 5V and GND. After all these steps you can have fun with your wireless usb cable then. 

<details>
  <summary>
    <h4>TL;DR</h4>
  </summary> 

  - order PCB
  - hold boot button
  - press reset button while boot button is hold down
  - release boot button
  - flash firmware (look [here](https://github.com/Lumethra/ESP-USB/blob/main/README.md#diy))
  
</details>

---

## Renders

| PCB V1 |
| --- |
| <img alt="ESP-USB Front with Components" src="https://github.com/Lumethra/ESP-USB/blob/main/Images/PCB-V1-Front.png" /> |
| <img alt="ESP-USB Back with Components" src="https://github.com/Lumethra/ESP-USB/blob/main/Images/PCB-V1-Back.png" /> |

| PCB V2 |
| --- |
| <img alt="ESP-USB Front with Components" src="https://github.com/Lumethra/ESP-USB/blob/main/Images/PCB-V2-Front.png" /> |
| <img alt="ESP-USB Back with Components" src="https://github.com/Lumethra/ESP-USB/blob/main/Images/PCB-V2-Back.png" /> |

---

## PCB + Schematics

| PCB V1 |
| --- |
| <img alt="ESP-USB PCB" src="https://github.com/Lumethra/ESP-USB/blob/main/Images/PCB-V1.png" /> |

| PCB V2 |
| --- |
| <img alt="ESP-USB PCB" src="https://github.com/Lumethra/ESP-USB/blob/main/Images/PCB-V2.png" /> |

| Schematics |
| --- |
| <img alt="ESP-USB Schematics" src="https://github.com/Lumethra/ESP-USB/blob/main/Images/ESP-USB-Schematics.png" /> |

---

<details>
  <summary>
    <h2>Zine</h2> 
  </summary>

| Zine |
| --- |
| <img alt="ESP-USB-Zine" src="https://github.com/Lumethra/ESP-USB/blob/main/Images/zine.png" /> |

---
</details>

## BOM DIY

| Part | Purpose | Cost | Quantity | Total Cost | LCSC/JLCPCB Number | Link | 
| --- | --- | --- | --- | --- | --- | --- |
| ESP-32-Devkit-C | run things | $15.07 | 2 | $30.14 | ~ | [AMAZON](https://www.amazon.de/diymore-DevKitC-Development-Bluetooth-Soldered-1-piece/dp/B0CYZVQR27/?language=en_GB&currency=USD) | 
| Powered USB Hub | the usb port doesnt provide 5V and GND | ~ (already owned) | 1 | ~ (already owned) | ~ | 
| USB Device | the device u wanna make wireless | ~ (already owned) | 1 | ~ (already owned) | ~ | 
| Computer | ur computer that u wanna connect the usb device to | ~ (already owned) | 1 | ~ (already owned) | ~ | 

## BOM PCB 

| Part | Purpose | Cost | Quantity | Total Cost | LCSC/JLCPCB Number | Link | 
| --- | --- | --- | --- | --- | --- | --- |
| PCB | connecting every component | ~$2.00 | 1 (5) | ~$2.00 | ~ | [JLCPCB](https://jlcpcb.com/) |
| Capacitors 100nF | clean up power | $0.0084 | 2 | $0.0168 | C14663 | [JLCPCB](https://jlcpcb.com/partdetail/C14663) |
| Capacitors 4.7uF | cleaning power | $0.0140 | 2 | $0.0280 | C19666 | [JLCPCB](https://jlcpcb.com/partdetail/C19666) |
| Capacitor 22uF | make power clean | $0.0149 | 1 | $0.0149 | C59461 | [JLCPCB](https://jlcpcb.com/partdetail/C59461) |
| Capacitors 10uF | power gets clean | $0.0139 | 2 | $0.0278 | C19702 | [JLCPCB](https://jlcpcb.com/partdetail/C19702) |
| S2B-PH-SM4-TB | battery connector | $0.2040 | 1 | $0.2040 | C295747 | [JLCPCB](https://jlcpcb.com/partdetail/C295747) |
| SK6812 | LED | $0.0894 | 1 | $0.0894 | C5378720 | [JLCPCB](https://jlcpcb.com/partdetail/C5378720) |
| SS14 Schottky Diodes | good "one way door" | $0.0169 | 2 | $0.0338 | C2480 | [JLCPCB](https://jlcpcb.com/partdetail/C2480) |
| Inductor 4.7uH | energy storage | $0.0324 | 1 | $0.0324 | C395016 | [JLCPCB](https://jlcpcb.com/partdetail/C395016) |
| FS8205A | mosfet | $0.0575 | 1 | $0.0575 | C2830320 | [JLCPCB](https://jlcpcb.com/partdetail/C2830320) |
| Resistors 5.1k | prevent booom | $0.0016 | 4 | $0.0064 | C23186 | [JLCPCB](https://jlcpcb.com/partdetail/C23186) |
| Resistors 1k | no booom | $0.0016 | 3 | $0.0048 | C21190 | [JLCPCB](https://jlcpcb.com/partdetail/C21190) |
| Resistor 100k | aww | $0.0018 | 1 | $0.0018 | C25803 | [JLCPCB](https://jlcpcb.com/partdetail/C25803) |
| Resistor 33k | ahh, we want boom | $0.0015 | 1 | $0.0015 | C4216 | [JLCPCB](https://jlcpcb.com/partdetail/C4216) |
| Resistors 56k | no we dont | $0.0016 | 2 | $0.0032 | C23206 | [JLCPCB](https://jlcpcb.com/partdetail/C23206) |
| Resistor 100 | so resistors exist | $0.0019 | 1 | $0.0019 | C22775 | [JLCPCB](https://jlcpcb.com/partdetail/C22775) |
| RST Button | restarts the esp | $0.0206 | 1 | $0.0206 | C318884 | [JLCPCB](https://jlcpcb.com/partdetail/C318884) |
| BOOT Button | lets esp go into download mode | $0.0206 | 1 | $0.0206 | C318884 | [JLCPCB](https://jlcpcb.com/partdetail/C318884) |
| ESP32-S3-WROOM-1 | handeling firmware | $5.3893 | 1 | $5.3893 | C2913202 | [JLCPCB](https://jlcpcb.com/partdetail/C2913202) |
| 74LVC1G3157DW-7 | switch for cc pins | $0.0685 | 2 | $0.1370 | C176651 | [JLCPCB](https://jlcpcb.com/partdetail/C176651) |
| DW01A | battery protector | $0.0428 | 1 | $0.0428 | C351410 | [JLCPCB](https://jlcpcb.com/partdetail/C351410) |
| MCP73831T-2ACI/OT | battery charger | $0.8122 | 1 | $0.8122 | C424093 | [JLCPCB](https://jlcpcb.com/partdetail/C424093) |
| MT3608 | 3.7v to 5v | $0.0786 | 1 | $0.0786 | C84817 | [JLCPCB](https://jlcpcb.com/partdetail/C84817) |
| AMS1117-3.3 | 5v to 3.3v | $0.2036 | 1 | $0.2036 | C6186 | [JLCPCB](https://jlcpcb.com/partdetail/C6186) |
| USB-C Connector | connects the dongle to pcb or whatever | $0.0741 | 1 | $0.0741 | C2765186 | [JLCPCB](https://jlcpcb.com/partdetail/C2765186) |
| **Total** |  | **~$9.48**  |  | **~$9.48** <br> _+ PCBA cost_  |  |  |

---

## Contribute

Want to make it better? Found some bugs? Feel free to open a PR (pull request) and lets make the project better. ^_^
> the libraries are all imported with [easyeda2kicad](https://github.com/uPesy/easyeda2kicad.py)

--- 

## Thank you

huge thank you too all these persons helping me making this project possible, thank you helping me sanity check, thank you for all these support. 
> Thank you, <3 u all ^_^
