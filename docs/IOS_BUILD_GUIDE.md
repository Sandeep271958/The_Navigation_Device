# NavigateMe: iOS Companion App Build Guide (iPhone 16)

This guide walks you through compiling and running the native **NavigateMe Companion** app on your **iPhone 16** using your **Mac**.

---

## 1. Why MapKit is 100% Free for iOS Developers

Unlike Google Maps (which charges per API call after free quotas), Apple’s **MapKit framework is 100% free** for all iOS developers:
* No credit card or billing account needed.
* Unlimited route calculations in Hyderabad.
* Runs continuously in your pocket with background location & BLE updates.

---

## 2. Opening & Compiling in Xcode on Mac

### Step 1: Open the Project
1. Open the project folder on your Mac:
   ```bash
   cd /Users/mouryasaisandeep/Desktop/Projects/NavigateMe/mobile-app/ios/NavigateMeCompanion
   ```
2. Open **Xcode**.
3. Select **File $\rightarrow$ Open** and choose the `NavigateMeCompanion` directory.

### Step 2: Configure Free Apple ID Signing
1. In Xcode, click on the top-level **NavigateMeCompanion** project in the left sidebar.
2. Go to the **Signing & Capabilities** tab.
3. Check **Automatically manage signing**.
4. In the **Team** dropdown, select your personal Apple ID (e.g., `Your Name (Personal Team)`).
5. Ensure the Bundle Identifier is unique (e.g., `com.mourya.navigateme`).

### Step 3: Install onto iPhone 16
1. Connect your iPhone 16 to your Mac using a USB-C cable (or Wi-Fi sync if enabled).
2. In Xcode's top destination bar, select your connected **iPhone 16**.
3. Click the **Play (Run)** button (or press `Cmd + R`).
4. On your iPhone 16, unlock and go to:
   * **Settings $\rightarrow$ General $\rightarrow$ VPN & Device Management**
   * Under Developer App, tap your Apple ID email and select **Trust**.
5. Launch **NavigateMe** from your iPhone home screen!

---

## 3. How to Avoid the 7-Day Free Developer Expiration (SideStore Setup)

Apps signed with a free personal Apple ID expire after **7 days**, requiring you to re-sign them from Xcode. 

To make the app run **permanently without plugging into your Mac every week**:

1. Install **[SideStore](https://sidestore.io)** on your iPhone 16.
2. SideStore uses a local VPN loopback on the iPhone to automatically refresh the app's signing certificates over your home Wi-Fi in the background!
3. You can ride every day without worrying about certificate expiration.

---

## 4. Testing Your iPhone 16 with NavigateMe

1. Power on your NavigateMe display pod (on desk or scooter).
2. Open the **NavigateMe** app on your iPhone 16.
3. The top banner will immediately show: `Connected to NavigateMe-Dash` (Green dot).
4. Tap **"HITEC City"** or type any destination in Hyderabad into the search bar.
5. Tap **Start Navigation**.
6. Lock your iPhone 16 and put it in your pocket.
7. Look at your handlebar screen:
   * The upcoming turn maneuver arrow updates live.
   * Distance countdown decrements smoothly.
   * Next-Next turn preview displays upcoming branch lanes.
   * Speedometer and ETA update in real time.

