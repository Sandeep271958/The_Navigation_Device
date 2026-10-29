# NavigateMe 🛵📍
**DIY Smart Scooter Navigation HUD for Suzuki Access 125**

NavigateMe is a rugged, low-latency, sunlight-readable motorcycle navigation display. It solves the critical safety and equipment risks of riding in Indian cities like Hyderabad—keeping your smartphone securely in your pocket while streaming high-contrast, glanceable turn guidance, distance countdowns, and flyover lane splits over Bluetooth Low Energy (BLE) to an M10 mirror-mounted handlebar pod.

---

## 🌟 Key Highlights

* **Next-Next Turn Preview:** Displays the maneuver *after* the upcoming turn (e.g., *Left in 80m $\rightarrow$ Immediate Right in 25m*), completely eliminating missed flyover ramps and metro-corridor exits in Hyderabad.
* **Vibration & OIS Safe:** Phone stays in your pocket or bag—zero risk to your phone's camera optical image stabilization (OIS).
* **Hyderabad Sun & Weather Shield:** 22mm integrated anti-glare sun visor prevents noon sun washouts; sealed PETG casing with silicone O-ring gasket handles monsoon rain.
* **Dual Ecosystem Support:**
  * **Android:** Plug-and-play with Google Maps via the free open-source `Chronos` notification bridge (built for your friend!).
  * **iOS (iPhone 16):** Native Apple MapKit companion app with zero API billing and background BLE streaming.
* **Automotive-Grade Protection:** Clamped with an SMBJ24A TVS diode and 1A in-line blade fuse to absorb scooter magneto and starter-crank voltage spikes.
* **Ultra-Budget:** Complete hardware build costs **₹1,800 – ₹2,500 INR**!

---

## 📐 System Architecture

```
┌────────────────────────────────────────────────────────┐
│        Smartphone in Pocket (Android or iPhone 16)     │
│                                                        │
│  • Google Maps (via Chronos) OR Apple Maps (MapKit)    │
│  • Phone screen remains LOCKED and OFF                 │
│  • Transmits 16-byte telemetry packet over BLE 5.0     │
└──────────────────────────┬─────────────────────────────┘
                           │ BLE GATT (fee0 / fee1)
                           ▼
┌────────────────────────────────────────────────────────┐
│     Handlebar Display Pod (ESP32-S3 / CYD Board)       │
│                                                        │
│  • High-Contrast Vector Arrows (Left/Right/Flyover)    │
│  • Real-Time Distance Countdown (140 m ... 120 m)      │
│  • Next-Next Turn Card ("THEN: [Right] 30m")           │
│  • Road & Junction Name Banner                         │
│  • Speedometer & Trip ETA readout                      │
│  • Ambient Light Auto-Dimming & Audio Proximity Beep   │
└──────────────────────────┬─────────────────────────────┘
                           ▲
             5V DC Power (USB or Fused 12V Rail)
                           │
      [Suzuki Access 125 Front Apron Electricals]
```

---

## 📁 Repository Directory Structure

```
NavigateMe/
├── firmware/                        # ESP32-S3 & CYD 2.8" Firmware
│   ├── platformio.ini              # Multi-board build environments
│   ├── include/
│   │   ├── config.h                # Hardware pinouts, UUIDs, maneuver enums
│   │   └── display_setup.h         # LovyanGFX high-speed DMA display setup
│   └── src/
│       ├── main.cpp                # Core boot & FreeRTOS loop
│       ├── ble_nav_service.h/.cpp  # BLE GATT server & packet parser
│       ├── power_manager.h/.cpp    # Auto-brightness & audio alert beeps
│       └── ui/
│           ├── nav_display.h       # High-contrast UI rendering engine
│           └── nav_display.cpp     # Vector arrow primitives & cards
│
├── cad/                             # 3D Printable Enclosure & Mounts
│   ├── generate_stl.py             # Script that generates STL files directly
│   ├── navigateme_case.scad        # Parametric OpenSCAD CAD source
│   └── stls/
│       ├── front_bezel_with_sunhood.stl # 22mm anti-glare visor faceplate
│       ├── back_enclosure.stl           # Main body with gasket groove & USB port
│       └── mirror_stem_m10_mount.stl    # Suzuki Access 125 M10 mirror bracket
│
├── mobile-app/                      # Mobile Companion Apps
│   ├── android/                    # Android Chronos setup & APK guides
│   └── ios/                        # Native iOS (iPhone 16) Xcode Project
│       └── NavigateMeCompanion/    # Complete SwiftUI + MapKit app
│
├── tools/                           # Desk Testing & Simulation
│   └── ble_mock_sender.py          # Simulates live Hyderabad scooter ride
│
└── docs/                            # Comprehensive Engineering Guides
    ├── ANDROID_BUILD_GUIDE.md      # Step-by-step Android build for your friend
    ├── IOS_BUILD_GUIDE.md          # Xcode build & SideStore setup for iPhone 16
    ├── HARDWARE_SPEC.md            # BOM, component sourcing & pricing (India)
    ├── WIRING_DIAGRAM.md           # Access 125 12V/USB power conditioning & pinouts
    └── ASSEMBLY_MANUAL.md          # PETG 3D printing, acrylic lens & weatherproofing
```

---

## 🚀 Quick Start Guide

### 1. Build the Firmware (Desk Prototype)
1. Open `firmware/` in **VS Code** with the **PlatformIO** extension.
2. Plug your ESP32 display board (e.g. CYD 2.8" or ESP32-S3 3.5") into your Mac via USB.
3. Click **Upload & Monitor**. The display will boot with the NavigateMe standby screen.

### 2. Test Without Going Outside! (Mock Trip Simulator)
Run the desktop simulator on your Mac to stream a simulated ride through Hyderabad (Banjara Hills $\rightarrow$ Cable Bridge $\rightarrow$ Flyover $\rightarrow$ HITEC City):
```bash
python3 -m pip install bleak
python3 tools/ble_mock_sender.py
```
Watch the handlebar screen immediately switch into navigation mode, count down meters, show upcoming flyover branch cues, and update speed!

### 3. Pair with Android (Friend's Phone)
1. Install **Chronos** on Android.
2. Grant **Notification Access** and pair with `NavigateMe-Dash`.
3. Start any route in **Google Maps**—it updates on screen within 1 second! (See [docs/ANDROID_BUILD_GUIDE.md](file:///Users/mouryasaisandeep/Desktop/Projects/NavigateMe/docs/ANDROID_BUILD_GUIDE.md)).

### 4. Pair with iPhone 16
1. Open `mobile-app/ios/NavigateMeCompanion` in **Xcode**.
2. Run on your iPhone 16 with your free personal Apple ID.
3. Search your destination in Hyderabad and tap **Start Route**! (See [docs/IOS_BUILD_GUIDE.md](file:///Users/mouryasaisandeep/Desktop/Projects/NavigateMe/docs/IOS_BUILD_GUIDE.md)).

### 5. 3D Print & Mount on Suzuki Access 125
1. Slices the models in `cad/stls/` using **PETG or ABS** (do not use PLA).
2. Install a 70×52mm 2mm acrylic lens with RTV silicone.
3. Clamp the M10 bracket under the right-side mirror locknut and connect the USB cable into your glovebox port! (See [docs/ASSEMBLY_MANUAL.md](file:///Users/mouryasaisandeep/Desktop/Projects/NavigateMe/docs/ASSEMBLY_MANUAL.md)).
