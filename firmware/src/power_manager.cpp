#include "power_manager.h"
#include "ui/nav_display.h"

PowerManager& PowerManager::getInstance() {
    static PowerManager instance;
    return instance;
}

PowerManager::PowerManager() 
    : m_lastLdrReadMillis(0), m_beepStartMillis(0), m_isBeeping(false), 
      m_beepStep(0), m_currentBrightness(BRIGHTNESS_DAY) {}

void PowerManager::init() {
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

#if defined(LDR_SENSOR_PIN) && (LDR_SENSOR_PIN >= 0)
    pinMode(LDR_SENSOR_PIN, INPUT);
#endif

    // Startup beep confirmation
    digitalWrite(BUZZER_PIN, HIGH);
    delay(80);
    digitalWrite(BUZZER_PIN, LOW);

    Serial.println("[PowerManager] Initialized power & audio alerts");
}

void PowerManager::triggerTurnAlert() {
    if (!m_isBeeping) {
        m_isBeeping = true;
        m_beepStep = 1;
        m_beepStartMillis = millis();
        digitalWrite(BUZZER_PIN, HIGH);
    }
}

void PowerManager::handleAudioBeep() {
    if (!m_isBeeping) return;

    uint32_t elapsed = millis() - m_beepStartMillis;

    // Pattern: Beep (80ms) -> Silence (60ms) -> Beep (80ms) -> Done
    if (m_beepStep == 1 && elapsed > 80) {
        digitalWrite(BUZZER_PIN, LOW);
        m_beepStep = 2;
        m_beepStartMillis = millis();
    } else if (m_beepStep == 2 && elapsed > 60) {
        digitalWrite(BUZZER_PIN, HIGH);
        m_beepStep = 3;
        m_beepStartMillis = millis();
    } else if (m_beepStep == 3 && elapsed > 80) {
        digitalWrite(BUZZER_PIN, LOW);
        m_isBeeping = false;
        m_beepStep = 0;
    }
}

void PowerManager::updateAutoBrightness() {
#if defined(LDR_SENSOR_PIN) && (LDR_SENSOR_PIN >= 0)
    if (millis() - m_lastLdrReadMillis > 2000) {
        m_lastLdrReadMillis = millis();
        int ldrRaw = analogRead(LDR_SENSOR_PIN);

        // Map analog reading to smooth brightness (Day vs Night)
        // High light -> High brightness; Dark night -> Dim brightness
        uint8_t target = map(ldrRaw, 200, 3500, BRIGHTNESS_NIGHT, BRIGHTNESS_DAY);
        target = constrain(target, BRIGHTNESS_NIGHT, BRIGHTNESS_DAY);

        // Smooth transition (low-pass filter)
        m_currentBrightness = (m_currentBrightness * 3 + target) / 4;
        NavDisplay::getInstance().setBrightness(m_currentBrightness);
    }
#endif
}

void PowerManager::update() {
    handleAudioBeep();
    updateAutoBrightness();

    // Check if BLE requested an alert
    if (g_navState.alertTriggered) {
        g_navState.alertTriggered = false;
        triggerTurnAlert();
    }
}

