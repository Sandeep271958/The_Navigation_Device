#pragma once
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "config.h"

// ====================================================================
// LovyanGFX Hardware Display Configuration for ESP32-D35 (ST7796 3.5")
// ====================================================================

class LGFX_NavigateMe : public lgfx::LGFX_Device {
    lgfx::Panel_Device* _panel_instance;
    lgfx::Bus_SPI       _bus_instance;
    lgfx::Light_PWM     _light_instance;

public:
    LGFX_NavigateMe() {
        {
            auto cfg = _bus_instance.config();
            cfg.spi_host = HSPI_HOST;     // Standard ESP32 hardware SPI
            cfg.spi_mode = 0;
            cfg.freq_write = 40000000;    // 40MHz write speed for 60fps UI
            cfg.freq_read  = 16000000;
            cfg.pin_sclk = TFT_SCLK_PIN;  // GPIO 14
            cfg.pin_mosi = TFT_MOSI_PIN;  // GPIO 13
            cfg.pin_miso = TFT_MISO_PIN;  // GPIO 12
            cfg.pin_dc   = TFT_DC_PIN;    // GPIO 2
            _bus_instance.config(cfg);
        }

        {
            auto p = new lgfx::Panel_ST7796();
            auto cfg = p->config();
            cfg.pin_cs  = TFT_CS_PIN;     // GPIO 15
            cfg.pin_rst = TFT_RST_PIN;    // -1 (Tied to EN)
            cfg.pin_busy = -1;
            cfg.panel_width  = 320;
            cfg.panel_height = 480;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            cfg.offset_rotation = 1;      // Landscape 480x320
            cfg.readable = false;
            cfg.invert = false;
            cfg.rgb_order = false;
            cfg.dlen_16bit = false;
            cfg.bus_shared = false;
            p->config(cfg);
            _panel_instance = p;
            _panel_instance->setBus(&_bus_instance);
        }

        {
            auto cfg = _light_instance.config();
            cfg.pin_bl = TFT_BL_PIN;      // GPIO 27 on ESP32-D35
            cfg.invert = false;
            cfg.freq   = 44100;
            cfg.pwm_channel = 7;
            _light_instance.config(cfg);
            _panel_instance->setLight(&_light_instance);
        }

        setPanel(_panel_instance);
    }
};
