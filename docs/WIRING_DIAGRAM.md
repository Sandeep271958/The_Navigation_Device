# NavigateMe: Electrical Wiring & Power Conditioning Guide

## Vehicle Target: Suzuki Access 125 (BS6 / Special Edition)

---

## 1. Overview of Two-Wheeler Electrical Environment

Two-wheelers (scooters and motorcycles) have notoriously "dirty" 12V DC electrical systems. When the magneto/stator charges the battery or when the electric starter cranks, transient voltage spikes (**load dumps**) can surge from **25V up to 45V+** for several milliseconds.

If you connect a standard sensitive 5V microcontroller directly without automotive-grade protection, it will burn out within weeks. 

NavigateMe uses an automotive transient suppression network:
1. **1A Fast-Blow In-line Blade Fuse:** Prevents battery shorts.
2. **SMBJ24A TVS Diode:** Clamps any voltage surge above 24V instantly to ground.
3. **470µF 50V Low-ESR Bulk Capacitor:** Filters engine ignition noise and absorbs starter-crank brownout sags.
4. **Automotive DC-DC Step-Down Regulator (MP1584EN / TPS54302):** Converts 9V–36V DC into a regulated, clean 5.1V 2A output.

---

## 2. Power Delivery Options for Suzuki Access 125

### Option A: Plug-and-Play Factory USB Socket (Zero Wire Cutting)
Most recent Suzuki Access 125 models (BS6 Special Edition / Ride Connect) have a 2A USB port inside the front left glovebox pocket.
* **Cable:** 1-meter right-angle (90-degree) braided USB-C cable.
* **Routing:** Plug into the glovebox USB port, route the cable along the inner handlebar plastic shroud up to the mirror stem mount.
* **Ignition Behavior:** The factory USB port powers ON with the ignition key and shuts off when you turn the key OFF.

```
[Access 125 Factory USB Port (5V 2A)]
                 │
                 ▼ (Right-angle USB-C Cable)
   [NavigateMe Display Pod on Handlebar]
```

---

### Option B: Hardwired 12V Ignition-Switched Harness (Permanent Clean Install)
If your Access 125 does not have the factory USB port, tap the key-switched auxiliary coupler behind the front apron.

```
Scooter Wiring Loom (Behind Front Apron)
  [12V Key-Switched ACC Line] ────> [1A Blade Fuse] ───┐
                                                       │
  [Chassis Ground (GND)] ──────────────────────────────┼──────┐
                                                       │      │
                                   ┌───────────────────┴──────┴───────────────────┐
                                   │       Power Conditioning Board               │
                                   │                                              │
                                   │  [SMBJ24A TVS Diode] (across 12V and GND)    │
                                   │  [470µF 50V Low-ESR Capacitor]               │
                                   │                                              │
                                   │  [MP1584EN Step-Down Buck Regulator]         │
                                   │   Input: 9V - 36V DC                         │
                                   │   Output: Clean 5.1V 2A DC                   │
                                   └───────────────────┬──────┬───────────────────┘
                                                       │      │
                                                       ▼      ▼
                                              [+5V]  [GND] to ESP32 Display Pod
```

---

## 3. Microcontroller Pinout Table

### Target 1: ESP32-2432S028 ("Cheap Yellow Display" 2.8" TFT)
The CYD board has all pins pre-routed on its PCB:

| Function | ESP32 GPIO Pin | Description |
| :--- | :--- | :--- |
| **SPI SCLK** | GPIO 14 | Hardware SPI Clock |
| **SPI MOSI** | GPIO 13 | Master Out Slave In |
| **SPI MISO** | GPIO 12 | Display Data Read |
| **TFT CS** | GPIO 15 | Display Chip Select |
| **TFT DC** | GPIO 2 | Data / Command Select |
| **Backlight PWM** | GPIO 21 | Hardware Brightness Dimming |
| **Active Buzzer** | GPIO 26 | Turn Proximity Audio Beep |
| **LDR Sensor** | GPIO 34 | Ambient Light Analog Input |
| **Status RGB LED** | Red: GPIO 4, Green: GPIO 16, Blue: GPIO 17 | Bluetooth / Alert Indicators |

---

### Target 2: ESP32-S3 with 3.5" ST7796 IPS Display

| Function | ESP32-S3 Pin | Display Pin |
| :--- | :--- | :--- |
| **+5V Power** | 5V / VBUS | VCC |
| **Ground** | GND | GND |
| **SPI SCLK** | GPIO 12 | SCL / SCK |
| **SPI MOSI** | GPIO 11 | SDA / MOSI |
| **SPI MISO** | GPIO 13 | MISO (Optional) |
| **TFT CS** | GPIO 10 | CS |
| **TFT DC** | GPIO 9 | DC / RS |
| **TFT Reset** | EN / 3.3V | RESET |
| **Backlight** | GPIO 14 | BLK / LED (PWM controlled) |
| **Piezo Buzzer** | GPIO 4 | Audio Alert Pin |
| **Ambient Light**| GPIO 1 | LDR / TEMT6000 Analog Pin |

