#pragma once
#include <Arduino.h>

// ====================================================================
// NavigateMe Firmware Configuration
// Target: Electrobot ESP32-D35 / ESP32-3248S035 (3.5" ST7796 480x320)
// ====================================================================

#define DEVICE_NAME             "NavigateMe-Dash"
#define FIRMWARE_VERSION        "1.1.0"

// ====================================================================
// BLE GATT UUIDs
// ====================================================================
#define NAV_SERVICE_UUID        "0000fee0-0000-1000-8000-00805f9b34fb"
#define NAV_DATA_CHAR_UUID      "0000fee1-0000-1000-8000-00805f9b34fb"
#define NAV_TELEMETRY_CHAR_UUID "0000fee2-0000-1000-8000-00805f9b34fb"

// ====================================================================
// Maneuver Codes
// ====================================================================
enum ManeuverType : uint8_t {
    MANEUVER_UNKNOWN        = 0,
    MANEUVER_STRAIGHT       = 1,
    MANEUVER_TURN_LEFT      = 2,
    MANEUVER_TURN_RIGHT     = 3,
    MANEUVER_SHARP_LEFT     = 4,
    MANEUVER_SHARP_RIGHT    = 5,
    MANEUVER_SLIGHT_LEFT    = 6,
    MANEUVER_SLIGHT_RIGHT   = 7,
    MANEUVER_U_TURN         = 8,
    MANEUVER_ROUNDABOUT     = 9,
    MANEUVER_KEEP_LEFT      = 10,   // Crucial for Hyderabad Flyovers vs Service Road
    MANEUVER_KEEP_RIGHT     = 11,   // Crucial for Flyover entry
    MANEUVER_FLYOVER_RAMP   = 12,   // Specific flyover indicator
    MANEUVER_DESTINATION    = 13,
    MANEUVER_REROUTING      = 14
};

// ====================================================================
// Electrobot ESP32-D35 (ESP32-3248S035) Hardwired Pinout
// ====================================================================
#define TFT_MOSI_PIN    13
#define TFT_MISO_PIN    12
#define TFT_SCLK_PIN    14
#define TFT_CS_PIN      15
#define TFT_DC_PIN      2
#define TFT_RST_PIN     -1      // Tied to board reset (EN)
#define TFT_BL_PIN      27      // Backlight PWM on 3.5" CYD board

// Onboard Peripherals on ESP32-D35
#define BUZZER_PIN      26      // Onboard audio DAC / Speaker output
#define LDR_SENSOR_PIN  34      // Onboard ambient light sensor (LDR)
#define LED_RGB_RED     4       // Onboard status LED
#define LED_RGB_GREEN   16
#define LED_RGB_BLUE    17
#define SD_CS_PIN       5       // Onboard MicroSD slot

// Display Resolution
#define DISPLAY_WIDTH   480
#define DISPLAY_HEIGHT  320

// ====================================================================
// System Timing & Power Config
// ====================================================================
#define BLE_DISCONNECT_SLEEP_TIMEOUT_MS  (10 * 60 * 1000)
#define TURN_ALERT_DISTANCE_METERS       120
#define BRIGHTNESS_DAY                   255
#define BRIGHTNESS_NIGHT                 65
