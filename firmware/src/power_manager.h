#pragma once
#include <Arduino.h>
#include "config.h"

class PowerManager {
public:
    static PowerManager& getInstance();

    void init();
    void update();
    void triggerTurnAlert();

private:
    PowerManager();
    void updateAutoBrightness();
    void handleAudioBeep();

    uint32_t m_lastLdrReadMillis;
    uint32_t m_beepStartMillis;
    bool     m_isBeeping;
    uint8_t  m_beepStep;
    uint8_t  m_currentBrightness;
};

