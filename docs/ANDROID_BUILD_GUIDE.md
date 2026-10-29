# NavigateMe: Android Companion Build Guide (Chronos-Navio Architecture)

This build creates a dedicated, low-cost (under ₹2,500) smart motorcycle navigation display for Android phones using the battle-tested open-source **`fbiego/chronos-navio`** ecosystem.

---

## 1. System Overview

```
┌────────────────────────────────────────────────────────┐
│            Friend's Android Phone (In Pocket)          │
│                                                        │
│   • Google Maps Navigation running                     │
│   • Chronos App (Notification Listener Service)        │
│   • Encodes maneuver icons, distance, street names     │
│   • Transmits over BLE 5.0 (low power, zero lag)       │
└──────────────────────────┬─────────────────────────────┘
                           │ BLE GATT Service
                           ▼
┌────────────────────────────────────────────────────────┐
│       Handlebar Unit (ESP32-S3 Display Pod)            │
│                                                        │
│   • Firmware: fbiego/chronos-navio (PlatformIO + LVGL) │
│   • Turn Card: Large maneuver arrow + distance countdown│
│   • Street & Junction: Current road & next road name   │
│   • Dashboard: Current speed, trip ETA, battery state  │
│   • Media Widget: Shows current Spotify/YT Music track │
│   • Powered by scooter USB / 12V ignition rail         │
└────────────────────────────────────────────────────────┘
```

---

## 2. Bill of Materials (BOM) & Hyderabad Sourcing

| Component | Part / Model | Approx Cost (INR) | Where to Buy (Hyd / Online) |
| :--- | :--- | :--- | :--- |
| **All-in-One Display Board** | **ESP32-2432S028 (CYD 2.8")** *or* **ESP32-3248S035 (3.5" IPS)** | ₹1,350 – ₹1,950 | Robu.in, ElectronicsComp, or Gujarati Galli (Koti) |
| **Power Cable** | 90° Right-angle USB Type-C Cable (1 meter) | ₹120 – ₹180 | Local mobile shop / Amazon |
| **Vehicle Power Protection** | 1A in-line blade fuse + SMBJ24A TVS diode *(if hardwiring to 12V)* | ₹60 – ₹100 | Gujarati Galli (Koti) |
| **Enclosure** | 3D-Printed PETG casing with 20mm sun visor | ₹250 – ₹350 | Local 3D print service |
| **Handlebar Mount** | M10 scooter mirror thread clamp bracket | ₹200 – ₹300 | Local two-wheeler accessory shop |
| **Total BOM** | | **₹1,980 – ₹2,880** | **Comfortably under ₹3,000!** |

---

## 3. Software Repositories & Tools

1. **ESP32 Display Firmware:**
   * Repository: [`fbiego/chronos-navio`](https://github.com/fbiego/chronos-navio)
   * Core Library: [`fbiego/chronos-esp32`](https://github.com/fbiego/chronos-esp32)
   * Build Tool: **PlatformIO IDE** (VS Code extension) or Arduino IDE.
2. **Android Phone Companion App:**
   * App Name: **Chronos** (Free on Google Play Store or APK via [GitHub: fbiego/Chronos-App](https://github.com/fbiego/chronos-chat)).
   * Permission Required: Notification Access (Settings $\rightarrow$ Apps $\rightarrow$ Special App Access $\rightarrow$ Notification Access $\rightarrow$ Enable Chronos).

---

## 4. Step-by-Step Implementation Workflow

### Step 1: Flash Firmware onto the ESP32 Board
1. Open VS Code on your Mac and install the **PlatformIO IDE** extension.
2. Clone the `chronos-navio` repository:
   ```bash
   git clone https://github.com/fbiego/chronos-navio.git
   ```
3. In `platformio.ini`, select your target display board (e.g., `esp32-2432S028R` for the 2.8" CYD, or configure your specific ST7789/ST7796 pinout).
4. Connect the ESP32 board to your Mac via USB and click **Upload & Monitor**.
5. Once booted, the screen will show the default dashboard with a Bluetooth pairing icon.

### Step 2: Configure the Android Phone
1. Install the **Chronos** app on the Android phone.
2. Open Chronos $\rightarrow$ Grant **Notification Access** and **Bluetooth permissions**.
3. In the Chronos app, tap **Search Devices** $\rightarrow$ Select `Chronos-Navio` (or your ESP32's BLE name).
4. Under **Navigation Settings** in Chronos, ensure **Google Maps** is toggled ON.

### Step 3: Bench Test on Desk
1. Keep the ESP32 powered on your desk.
2. Open **Google Maps** on the Android phone and start navigation to any destination (e.g., "Gachibowli to Charminar").
3. Within 1 second of starting navigation, the ESP32 screen will switch to the **Navigation UI**:
   - Turn icon updates immediately when maneuver changes.
   - Distance decrements in real-time.
   - Street names update seamlessly.
   - Lock the Android phone and put it in a bag—the ESP32 screen continues updating!

### Step 4: Scooter Installation (Suzuki Access 125)
1. Mount the 3D-printed enclosure under the right-side M10 mirror stem locknut.
2. Route the right-angle USB-C cable from the handlebar pod into the front glovebox USB port.
3. Turn on the scooter key: device powers on, auto-connects to the Android phone over BLE within 3 seconds, and is ready for the ride.

