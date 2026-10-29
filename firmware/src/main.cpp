#include <Arduino.h>
#include "config.h"
#include "ble_nav_service.h"
#include "ui/nav_display.h"
#include "power_manager.h"

// FreeRTOS task handles & frame limiter
static uint32_t s_lastFrameMillis = 0;
const uint32_t TARGET_FRAME_TIME_MS = 33; // ~30 FPS smooth rendering

void setup() {
    Serial.begin(115200);
    delay(200);

    Serial.println("\n==========================================");
    Serial.printf("  %s v%s\n", DEVICE_NAME, FIRMWARE_VERSION);
    Serial.println("  Smart Motorcycle / Scooter Navigation HUD");
    Serial.println("==========================================\n");

    // 1. Initialize Display Subsystem
    NavDisplay::getInstance().init();

    // 2. Initialize Power & Audio Subsystem
    PowerManager::getInstance().init();

    // 3. Initialize Bluetooth Low Energy Subsystem
    BleNavService::getInstance().init();

    Serial.println("[System] Boot sequence complete. Entering main loop.");
}

void loop() {
    uint32_t now = millis();

    // Update Bluetooth state and incoming packet parsing
    BleNavService::getInstance().update();

    // Update power, brightness, and audio beeps
    PowerManager::getInstance().update();

    // Render Display at target frame rate
    if (now - s_lastFrameMillis >= TARGET_FRAME_TIME_MS) {
        s_lastFrameMillis = now;
        NavDisplay::getInstance().render(g_navState);
    }

    // Yield to FreeRTOS watchdog & background BLE stack
    vTaskDelay(pdMS_TO_TICKS(5));
}

