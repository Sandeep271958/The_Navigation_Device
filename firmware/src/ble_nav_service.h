#pragma once
#include <Arduino.h>
#include <NimBLEDevice.h>
#include "config.h"

// ====================================================================
// Navigation State Structure
// ====================================================================
struct NavigationState {
    bool isConnected;
    bool isNavigating;
    
    uint8_t maneuver;              // Primary upcoming turn (ManeuverType)
    uint32_t distanceMeters;       // Distance to primary turn
    char currentRoad[64];          // e.g. "Road No. 36"
    char nextRoad[64];             // e.g. "PVNR Expressway"
    
    // Next-Next Turn Preview (Crucial for Flyovers)
    uint8_t nextManeuver;
    uint32_t nextDistanceMeters;
    
    // Scooter & Trip Telemetry
    uint16_t speedKmh;
    uint16_t etaMinutes;
    uint32_t totalDistanceRemaining;
    uint8_t phoneBatteryPercent;
    
    uint32_t lastPacketMillis;
    bool alertTriggered;
};

// Global state instance
extern NavigationState g_navState;

class BleNavService : public NimBLEServerCallbacks, public NimBLECharacteristicCallbacks {
public:
    static BleNavService& getInstance();
    
    void init();
    void update();
    bool isClientConnected() const { return m_connected; }
    
    // Callbacks
    void onConnect(NimBLEServer* pServer) override;
    void onDisconnect(NimBLEServer* pServer) override;
    void onWrite(NimBLECharacteristic* pCharacteristic) override;

private:
    BleNavService();
    void parseIncomingPacket(const uint8_t* data, size_t length);
    void parseJsonPacket(const char* jsonStr);
    void parseBinaryPacket(const uint8_t* buffer, size_t length);

    NimBLEServer* m_pServer;
    NimBLECharacteristic* m_pDataChar;
    NimBLECharacteristic* m_pTelemetryChar;
    bool m_connected;
    uint32_t m_lastConnectedMillis;
};

