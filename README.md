<img width="1407" height="769" alt="image" src="https://github.com/user-attachments/assets/61638e1a-289f-4237-b738-ba45863bde8f" /># ESP-USB

## What is this? 

### Features: 

---

## Why? 

--- 

## How?

---

## Renders

---

## PCB + Schematics

---

## Zine

---

## BOM

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

--- 

## Thank you
