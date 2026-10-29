#pragma once
#include <Arduino.h>
#include "display_setup.h"
#include "ble_nav_service.h"

class NavDisplay {
public:
    static NavDisplay& getInstance();

    void init();
    void render(const NavigationState& state);
    void setBrightness(uint8_t brightness);

private:
    NavDisplay();
    
    void renderTopBar(const NavigationState& state);
    void renderMainTurn(const NavigationState& state);
    void renderNextNextTurn(const NavigationState& state);
    void renderRoadBanner(const NavigationState& state);
    void renderStandbyScreen();
    
    // Vector Drawing Helpers for High-Visibility Maneuvers
    void drawManeuverIcon(int cx, int cy, int size, uint8_t maneuver, uint32_t color);
    void drawArrowStraight(int cx, int cy, int size, uint32_t color);
    void drawArrowLeft(int cx, int cy, int size, uint32_t color);
    void drawArrowRight(int cx, int cy, int size, uint32_t color);
    void drawArrowSlightLeft(int cx, int cy, int size, uint32_t color);
    void drawArrowSlightRight(int cx, int cy, int size, uint32_t color);
    void drawArrowUTurn(int cx, int cy, int size, uint32_t color);
    void drawArrowRoundabout(int cx, int cy, int size, uint32_t color);
    void drawArrowKeepLeft(int cx, int cy, int size, uint32_t color);
    void drawArrowKeepRight(int cx, int cy, int size, uint32_t color);
    void drawDestinationPin(int cx, int cy, int size, uint32_t color);

    LGFX_NavigateMe m_gfx;
    LGFX_Sprite     m_sprite; // Canvas sprite to eliminate screen flicker
    uint32_t        m_lastRenderMillis;
    bool            m_wasConnected;
};

