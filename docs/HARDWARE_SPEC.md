# NavigateMe Hardware Specification: Electrobot ESP32-D35 (3.5" ST7796)

**Selected Board:** [Electrobot 3.5" ESP32 Development Board with ST7796U Touch LCD (320x480)](https://store.electrobot.co.in/product/esp32d35)  
**Model Code:** `ESP32-D35` / `ESP32-3248S035`  
**Price:** ~₹1,800 – ₹2,200 INR (In-stock in India)

---

## 1. Why this board is ideal for NavigateMe

1. **All-In-One Integrated PCB:** The ESP32 MCU, 3.5-inch color display, backlight driver, ambient light sensor (LDR), audio amplifier/speaker, MicroSD slot, and RGB LEDs are all pre-soldered on a single circuit board. **Zero loose jumper wires to vibrate loose on the scooter.**
2. **Generous 3.5" Screen (480x320 Landscape):** 50% more display area than the 2.8" CYD. Easily fits:
   * Huge primary maneuver arrow.
   * Prominent **Next-Next Turn Card** (essential for flyovers).
   * Full road and junction names without cramped text.
   * Speedometer and ETA indicators.
3. **Onboard LDR Sensor:** Pre-wired to `GPIO 34` for automatic night-mode dimming.
4. **Onboard Audio / Speaker:** Pre-wired to `GPIO 26` for turn alert beeps.

---

## 2. Hardwired PCB Pinout Reference

Because this is an integrated board, the pins are hardwired on the PCB as follows:

| Component | Pin Function | ESP32 GPIO | Notes |
| :--- | :--- | :--- | :--- |
| **ST7796 Display** | SPI MOSI (SDA) | **GPIO 13** | Hardware SPI Data |
| | SPI MISO | **GPIO 12** | Hardware SPI Read |
| | SPI SCLK (SCK) | **GPIO 14** | Hardware SPI Clock (40MHz) |
| | Chip Select (CS) | **GPIO 15** | Display Enable |
| | Data/Command (DC)| **GPIO 2** | Register Select |
| | Reset (RST) | **-1 (EN)** | Tied to board reset |
| | Backlight (BL) | **GPIO 27** | **PWM Brightness Control** |
| **Onboard LDR** | Light Sensor | **GPIO 34** | Analog ADC Input |
| **Onboard Audio** | Speaker / Buzzer | **GPIO 26** | Audio Turn Alert Beep |
| **Status RGB LED** | Red | **GPIO 4** | Alert Indicator |
| | Green | **GPIO 16** | Bluetooth Connected |
| | Blue | **GPIO 17** | Searching Indicator |
| **MicroSD Slot** | Card CS | **GPIO 5** | SPI Mode |

---

## 3. Bill of Materials & Budget

| Item | Component | Approx Price (INR) | Sourcing |
| :--- | :--- | :--- | :--- |
| **Core Board** | Electrobot ESP32-D35 (3.5" ST7796) | ₹1,950 | [Electrobot Store](https://store.electrobot.co.in/product/esp32d35) |
| **Power Cable** | 90° Right-Angle USB-C Cable (1m) | ₹150 | Local mobile shop / Amazon |
| **TVS Protection** | SMBJ24A + 1A Blade Fuse *(if tapping 12V)* | ₹80 | Local electronics market (Koti) |
| **Enclosure** | 3D Printed PETG Case with 22mm Sun Hood | ₹350 | Local 3D print service |
| **Mount** | M10 Right-Hand Mirror Stem Clamp | ₹250 | Local motorcycle accessory shop |
| **Total Build Cost** | | **~₹2,780 INR** | **Comfortably under your ₹4,000 budget!** |

---

## 4. Suzuki Access 125 Power Connections

1. **Option 1 (Plug-and-Play via Front USB):**
   * If your Access 125 has the factory USB port in the front glovebox, connect the right-angle USB-C cable directly from the glovebox to the bottom of the NavigateMe pod.
2. **Option 2 (Switched 12V Auxiliary Harness):**
   * Tap the 12V key-switched line behind the front apron through an inline 1A blade fuse, an **SMBJ24A TVS diode** (clamps magneto voltage spikes >24V), and a 12V-to-5V 2A DC-DC buck converter.
