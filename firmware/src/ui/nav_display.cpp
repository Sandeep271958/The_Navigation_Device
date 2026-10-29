#include "nav_display.h"

// Color definitions (High-contrast outdoor palette)
#define COLOR_BG          0x000000 // Pure Black
#define COLOR_CARD_BG     0x101820 // Dark Charcoal
#define COLOR_ACCENT      0x00E5FF // Neon Cyan
#define COLOR_PRIMARY     0xFFFFFF // Bright White
#define COLOR_ALERT       0xFFD600 // Amber Yellow
#define COLOR_SUCCESS     0x00E676 // Bright Green
#define COLOR_MUTED       0x8892B0 // Slate Gray

NavDisplay& NavDisplay::getInstance() {
    static NavDisplay instance;
    return instance;
}

NavDisplay::NavDisplay() 
    : m_sprite(&m_gfx), m_lastRenderMillis(0), m_wasConnected(false) {}

void NavDisplay::init() {
    m_gfx.init();
    m_gfx.setRotation(1); // Landscape
    m_gfx.setColorDepth(16);
    setBrightness(BRIGHTNESS_DAY);

    // Create full-screen double-buffer sprite
    m_sprite.setColorDepth(16);
    m_sprite.createSprite(m_gfx.width(), m_gfx.height());
    m_sprite.setTextWrap(false);

    Serial.printf("[Display] Initialized %dx%d resolution with double buffering\n", 
                  m_gfx.width(), m_gfx.height());
}

void NavDisplay::setBrightness(uint8_t brightness) {
    m_gfx.setBrightness(brightness);
}

void NavDisplay::render(const NavigationState& state) {
    m_sprite.fillScreen(COLOR_BG);

    if (!state.isConnected) {
        renderStandbyScreen();
    } else if (!state.isNavigating) {
        // Connected but waiting for Google Maps navigation to start
        renderTopBar(state);
        
        m_sprite.setTextDatum(MC_DATUM);
        m_sprite.setTextColor(COLOR_PRIMARY);
        m_sprite.setTextSize(2);
        m_sprite.drawString("NavigateMe Connected", m_gfx.width() / 2, m_gfx.height() / 2 - 20);
        
        m_sprite.setTextColor(COLOR_ACCENT);
        m_sprite.setTextSize(1);
        m_sprite.drawString("Start a route in Google Maps on phone", m_gfx.width() / 2, m_gfx.height() / 2 + 15);
        
        m_sprite.drawRoundRect(20, m_gfx.height() / 2 - 40, m_gfx.width() - 40, 80, 10, COLOR_ACCENT);
    } else {
        // Full Active Navigation UI
        renderTopBar(state);
        renderMainTurn(state);
        renderNextNextTurn(state);
        renderRoadBanner(state);
    }

    // Push double-buffered frame to display via DMA
    m_sprite.pushSprite(0, 0);
}

void NavDisplay::renderTopBar(const NavigationState& state) {
    int w = m_gfx.width();
    
    // Top bar background
    m_sprite.fillRect(0, 0, w, 28, COLOR_CARD_BG);
    m_sprite.drawFastHLine(0, 28, w, 0x223344);

    // Bluetooth Status Icon
    m_sprite.fillCircle(12, 14, 5, state.isConnected ? COLOR_SUCCESS : 0xFF5252);

    // Speedometer
    m_sprite.setTextDatum(ML_DATUM);
    m_sprite.setTextColor(COLOR_PRIMARY);
    m_sprite.setTextSize(1);
    char speedBuf[24];
    snprintf(speedBuf, sizeof(speedBuf), "%d km/h", state.speedKmh);
    m_sprite.drawString(speedBuf, 24, 14);

    // Trip ETA
    if (state.isNavigating && state.etaMinutes > 0) {
        m_sprite.setTextDatum(MC_DATUM);
        m_sprite.setTextColor(COLOR_ACCENT);
        char etaBuf[24];
        snprintf(etaBuf, sizeof(etaBuf), "ETA %d min", state.etaMinutes);
        m_sprite.drawString(etaBuf, w / 2, 14);
    }

    // Phone Battery
    if (state.phoneBatteryPercent > 0) {
        m_sprite.setTextDatum(MR_DATUM);
        m_sprite.setTextColor(COLOR_MUTED);
        char batBuf[16];
        snprintf(batBuf, sizeof(batBuf), "%d%%", state.phoneBatteryPercent);
        m_sprite.drawString(batBuf, w - 8, 14);
    }
}

void NavDisplay::renderMainTurn(const NavigationState& state) {
    int w = m_gfx.width();
    int h = m_gfx.height();
    
    // Scale proportionally for 480x320 (3.5" screen) vs 320x240 (2.8" screen)
    int iconX = (w > 400) ? 85 : 65;
    int iconY = (h > 280) ? 120 : 100;
    int iconSize = (h > 280) ? 58 : 42;

    // 1. Draw Maneuver Vector Icon
    uint32_t iconColor = (state.distanceMeters <= TURN_ALERT_DISTANCE_METERS) ? COLOR_ALERT : COLOR_ACCENT;
    drawManeuverIcon(iconX, iconY, iconSize, state.maneuver, iconColor);

    // 2. Draw Distance
    m_sprite.setTextDatum(ML_DATUM);
    m_sprite.setTextColor(COLOR_PRIMARY);
    
    char distBuf[16];
    char unitBuf[8];
    if (state.distanceMeters >= 1000) {
        float km = state.distanceMeters / 1000.0f;
        snprintf(distBuf, sizeof(distBuf), "%.1f", km);
        snprintf(unitBuf, sizeof(unitBuf), "km");
    } else {
        snprintf(distBuf, sizeof(distBuf), "%lu", (unsigned long)state.distanceMeters);
        snprintf(unitBuf, sizeof(unitBuf), "m");
    }

    // Large Distance Numbers (Size 5 on 480x320, Size 4 on 320x240)
    int distTextSize = (w > 400) ? 5 : 4;
    int distPosX = (w > 400) ? 165 : 130;
    m_sprite.setTextSize(distTextSize);
    m_sprite.drawString(distBuf, distPosX, iconY - 10);

    // Unit string (m / km)
    int distWidth = m_sprite.textWidth(distBuf);
    m_sprite.setTextSize(distTextSize > 4 ? 3 : 2);
    m_sprite.setTextColor(COLOR_MUTED);
    m_sprite.drawString(unitBuf, distPosX + distWidth + 8, iconY);

    // 3. Proximity Progress Bar (fills up as you approach 0m)
    int barX = 20;
    int barY = (h > 280) ? 190 : 158;
    int barW = w - 40;
    int barH = (h > 280) ? 8 : 6;
    m_sprite.drawRoundRect(barX, barY, barW, barH, 4, 0x334455);
    
    if (state.distanceMeters < 500) {
        float fillRatio = 1.0f - ((float)state.distanceMeters / 500.0f);
        if (fillRatio < 0.05f) fillRatio = 0.05f;
        int fillW = (int)(barW * fillRatio);
        m_sprite.fillRoundRect(barX, barY, fillW, barH, 4, iconColor);
    }
}

void NavDisplay::renderNextNextTurn(const NavigationState& state) {
    // Top-Right Next-Next Turn Card (Essential for Hyderabad Flyovers)
    if (state.nextManeuver != MANEUVER_UNKNOWN && state.nextManeuver != 0) {
        int w = m_gfx.width();
        int cardW = (w > 400) ? 115 : 75;
        int cardH = (w > 400) ? 65 : 50;
        int cardX = w - cardW - 12;
        int cardY = (w > 400) ? 42 : 36;

        m_sprite.fillRoundRect(cardX, cardY, cardW, cardH, 8, COLOR_CARD_BG);
        m_sprite.drawRoundRect(cardX, cardY, cardW, cardH, 8, 0x223344);

        m_sprite.setTextDatum(MC_DATUM);
        m_sprite.setTextColor(COLOR_MUTED);
        m_sprite.setTextSize(1);
        m_sprite.drawString("THEN", cardX + cardW / 2, cardY + 12);

        // Draw small mini icon
        int miniIconSize = (w > 400) ? 20 : 14;
        drawManeuverIcon(cardX + cardW * 0.30, cardY + cardH * 0.65, miniIconSize, state.nextManeuver, COLOR_PRIMARY);

        // Distance to next-next turn
        char nextDistBuf[12];
        if (state.nextDistanceMeters >= 1000) {
            snprintf(nextDistBuf, sizeof(nextDistBuf), "%.1fkm", state.nextDistanceMeters / 1000.0f);
        } else {
            snprintf(nextDistBuf, sizeof(nextDistBuf), "%lum", (unsigned long)state.nextDistanceMeters);
        }
        m_sprite.setTextDatum(ML_DATUM);
        m_sprite.setTextColor(COLOR_PRIMARY);
        m_sprite.setTextSize(w > 400 ? 2 : 1);
        m_sprite.drawString(nextDistBuf, cardX + cardW * 0.52, cardY + cardH * 0.65);
    }
}

void NavDisplay::renderRoadBanner(const NavigationState& state) {
    int w = m_gfx.width();
    int h = m_gfx.height();
    int bannerH = (h > 280) ? 75 : 52;
    int bannerY = h - bannerH - 10;

    // Dark Card for road info
    m_sprite.fillRoundRect(10, bannerY, w - 20, bannerH, 10, COLOR_CARD_BG);
    m_sprite.drawRoundRect(10, bannerY, w - 20, bannerH, 10, 0x223344);

    // Target Next Road / Turn Street
    m_sprite.setTextDatum(ML_DATUM);
    m_sprite.setTextColor(COLOR_PRIMARY);
    m_sprite.setTextSize((w > 400) ? 3 : 2);
    
    // Truncate road string if too long for screen
    char displayNextRoad[36];
    strncpy(displayNextRoad, state.nextRoad[0] ? state.nextRoad : state.currentRoad, sizeof(displayNextRoad) - 1);
    displayNextRoad[sizeof(displayNextRoad) - 1] = '\0';
    
    m_sprite.drawString(displayNextRoad, 22, bannerY + ((h > 280) ? 26 : 18));

    // Current Road label
    m_sprite.setTextSize(1);
    m_sprite.setTextColor(COLOR_MUTED);
    m_sprite.drawString(state.currentRoad, 22, bannerY + ((h > 280) ? 55 : 38));
}

void NavDisplay::renderStandbyScreen() {
    int cx = m_gfx.width() / 2;
    int cy = m_gfx.height() / 2;

    m_sprite.setTextDatum(MC_DATUM);
    m_sprite.setTextColor(COLOR_ACCENT);
    m_sprite.setTextSize(3);
    m_sprite.drawString("NavigateMe", cx, cy - 35);

    m_sprite.setTextColor(COLOR_PRIMARY);
    m_sprite.setTextSize(1);
    m_sprite.drawString("SMART SCOOTER HUD", cx, cy);

    // Blinking Bluetooth status indicator
    static uint32_t lastBlink = 0;
    static bool blinkState = false;
    if (millis() - lastBlink > 500) {
        blinkState = !blinkState;
        lastBlink = millis();
    }

    m_sprite.fillCircle(cx - 60, cy + 35, 4, blinkState ? COLOR_ALERT : 0x334455);
    m_sprite.setTextColor(COLOR_MUTED);
    m_sprite.drawString("Searching for phone...", cx + 15, cy + 35);

    // Footer info
    m_sprite.setTextDatum(BC_DATUM);
    m_sprite.drawString("v1.0 • Suzuki Access 125", cx, m_gfx.height() - 8);
}

// ====================================================================
// Vector Maneuver Icons (Clear, thick, high-contrast)
// ====================================================================

void NavDisplay::drawManeuverIcon(int cx, int cy, int size, uint8_t maneuver, uint32_t color) {
    switch (maneuver) {
        case MANEUVER_STRAIGHT:
            drawArrowStraight(cx, cy, size, color);
            break;
        case MANEUVER_TURN_LEFT:
        case MANEUVER_SHARP_LEFT:
            drawArrowLeft(cx, cy, size, color);
            break;
        case MANEUVER_TURN_RIGHT:
        case MANEUVER_SHARP_RIGHT:
            drawArrowRight(cx, cy, size, color);
            break;
        case MANEUVER_SLIGHT_LEFT:
            drawArrowSlightLeft(cx, cy, size, color);
            break;
        case MANEUVER_SLIGHT_RIGHT:
            drawArrowSlightRight(cx, cy, size, color);
            break;
        case MANEUVER_U_TURN:
            drawArrowUTurn(cx, cy, size, color);
            break;
        case MANEUVER_ROUNDABOUT:
            drawArrowRoundabout(cx, cy, size, color);
            break;
        case MANEUVER_KEEP_LEFT:
            drawArrowKeepLeft(cx, cy, size, color);
            break;
        case MANEUVER_KEEP_RIGHT:
        case MANEUVER_FLYOVER_RAMP:
            drawArrowKeepRight(cx, cy, size, color);
            break;
        case MANEUVER_DESTINATION:
            drawDestinationPin(cx, cy, size, color);
            break;
        default:
            drawArrowStraight(cx, cy, size, color);
            break;
    }
}

void NavDisplay::drawArrowStraight(int cx, int cy, int size, uint32_t color) {
    int w = size / 2;
    int h = size;
    m_sprite.fillRect(cx - 3, cy - h/3, 7, h * 2/3, color);
    m_sprite.fillTriangle(cx, cy - h/2, cx - w, cy - h/6, cx + w, cy - h/6, color);
}

void NavDisplay::drawArrowLeft(int cx, int cy, int size, uint32_t color) {
    int s = size / 2;
    // L-shaped road line
    m_sprite.fillRect(cx - 4, cy - 4, 8, s + 4, color);
    m_sprite.fillRect(cx - s, cy - 4, s + 4, 8, color);
    // Arrow head
    m_sprite.fillTriangle(cx - s - 8, cy, cx - s + 4, cy - s/2 - 4, cx - s + 4, cy + s/2 + 4, color);
}

void NavDisplay::drawArrowRight(int cx, int cy, int size, uint32_t color) {
    int s = size / 2;
    // R-shaped road line
    m_sprite.fillRect(cx - 4, cy - 4, 8, s + 4, color);
    m_sprite.fillRect(cx - 4, cy - 4, s + 4, 8, color);
    // Arrow head
    m_sprite.fillTriangle(cx + s + 8, cy, cx + s - 4, cy - s/2 - 4, cx + s - 4, cy + s/2 + 4, color);
}

void NavDisplay::drawArrowSlightLeft(int cx, int cy, int size, uint32_t color) {
    m_sprite.fillCircle(cx, cy, 3, color);
    m_sprite.fillTriangle(cx - size/2, cy - size/2, cx - size/2 + 12, cy - size/4, cx - size/4, cy - size/2 + 12, color);
    m_sprite.fillQuad(cx - 3, cy + size/3, cx + 3, cy + size/3, cx - size/4 + 3, cy - size/4, cx - size/4 - 3, cy - size/4, color);
}

void NavDisplay::drawArrowSlightRight(int cx, int cy, int size, uint32_t color) {
    m_sprite.fillCircle(cx, cy, 3, color);
    m_sprite.fillTriangle(cx + size/2, cy - size/2, cx + size/2 - 12, cy - size/4, cx + size/4, cy - size/2 + 12, color);
    m_sprite.fillQuad(cx - 3, cy + size/3, cx + 3, cy + size/3, cx + size/4 + 3, cy - size/4, cx + size/4 - 3, cy - size/4, color);
}

void NavDisplay::drawArrowUTurn(int cx, int cy, int size, uint32_t color) {
    int r = size / 3;
    if (r < 4) r = 4;
    int inner_r = (r > 4) ? (r - 4) : 1;
    m_sprite.drawArc(cx, cy - r/2, r, inner_r, 0, 180, color);
    m_sprite.fillRect(cx + r - 3, cy - r/2, 4, size/2, color);
    m_sprite.fillRect(cx - r, cy - r/2, 4, size/2, color);
    m_sprite.fillTriangle(cx - r + 2, cy + size/2 + 4, cx - r - 6, cy + size/2 - 2, cx - r + 10, cy + size/2 - 2, color);
}

void NavDisplay::drawArrowRoundabout(int cx, int cy, int size, uint32_t color) {
    int r = size / 2;
    m_sprite.drawCircle(cx, cy, r, color);
    m_sprite.drawCircle(cx, cy, r - 1, color);
    m_sprite.fillTriangle(cx + r, cy - 5, cx + r + 6, cy + 6, cx + r - 6, cy + 6, color);
}

void NavDisplay::drawArrowKeepLeft(int cx, int cy, int size, uint32_t color) {
    // Fork in road (Left branch highlighted)
    m_sprite.fillRect(cx - 3, cy, 6, size/2, color);
    m_sprite.fillTriangle(cx - size/2, cy - size/2, cx - size/4, cy - size/4, cx - size/2 + 6, cy, color);
    // Right branch dimmed
    m_sprite.drawFastVLine(cx + size/3, cy - size/3, size/2, 0x445566);
}

void NavDisplay::drawArrowKeepRight(int cx, int cy, int size, uint32_t color) {
    // Fork in road (Right branch / Flyover highlighted)
    m_sprite.fillRect(cx - 3, cy, 6, size/2, color);
    m_sprite.fillTriangle(cx + size/2, cy - size/2, cx + size/4, cy - size/4, cx + size/2 - 6, cy, color);
    // Left branch dimmed
    m_sprite.drawFastVLine(cx - size/3, cy - size/3, size/2, 0x445566);
}

void NavDisplay::drawDestinationPin(int cx, int cy, int size, uint32_t color) {
    m_sprite.fillCircle(cx, cy - size/4, size/3, color);
    m_sprite.fillTriangle(cx - size/3, cy - size/4, cx + size/3, cy - size/4, cx, cy + size/2, color);
    m_sprite.fillCircle(cx, cy - size/4, size/6, COLOR_BG);
}

