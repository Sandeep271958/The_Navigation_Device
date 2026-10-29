#include "ble_nav_service.h"
#include <ArduinoJson.h>

NavigationState g_navState;

BleNavService& BleNavService::getInstance() {
    static BleNavService instance;
    return instance;
}

BleNavService::BleNavService() 
    : m_pServer(nullptr), m_pDataChar(nullptr), m_pTelemetryChar(nullptr),
      m_connected(false), m_lastConnectedMillis(0) {}

void BleNavService::init() {
    memset(&g_navState, 0, sizeof(NavigationState));
    g_navState.maneuver = MANEUVER_STRAIGHT;
    g_navState.distanceMeters = 0;
    strncpy(g_navState.currentRoad, "Waiting for phone...", sizeof(g_navState.currentRoad));

    NimBLEDevice::init(DEVICE_NAME);
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); // Max TX power for robust motorcycle connectivity

    m_pServer = NimBLEDevice::createServer();
    m_pServer->setCallbacks(this);

    // Create Navigation Service
    NimBLEService* pService = m_pServer->createService(NAV_SERVICE_UUID);

    // Characteristic for incoming navigation directives
    m_pDataChar = pService->createCharacteristic(
        NAV_DATA_CHAR_UUID,
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR | NIMBLE_PROPERTY::NOTIFY
    );
    m_pDataChar->setCallbacks(this);

    // Characteristic for telemetry (Speed, ETA, etc.)
    m_pTelemetryChar = pService->createCharacteristic(
        NAV_TELEMETRY_CHAR_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );

    pService->start();

    // Setup Advertising
    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(NAV_SERVICE_UUID);
    pAdvertising->setName(DEVICE_NAME);
    pAdvertising->setScanResponse(true);
    pAdvertising->start();

    Serial.println("[BLE] NavigateMe GATT server initialized & advertising started");
}

void BleNavService::onConnect(NimBLEServer* pServer) {
    m_connected = true;
    m_lastConnectedMillis = millis();
    g_navState.isConnected = true;
    g_navState.lastPacketMillis = millis();
    strncpy(g_navState.currentRoad, "Connected. Ready to navigate", sizeof(g_navState.currentRoad));
    Serial.println("[BLE] Client connected successfully!");
}

void BleNavService::onDisconnect(NimBLEServer* pServer) {
    m_connected = false;
    g_navState.isConnected = false;
    g_navState.isNavigating = false;
    strncpy(g_navState.currentRoad, "Bluetooth Disconnected", sizeof(g_navState.currentRoad));
    Serial.println("[BLE] Client disconnected. Restarting advertising...");
    NimBLEDevice::startAdvertising();
}

void BleNavService::onWrite(NimBLECharacteristic* pCharacteristic) {
    std::string val = pCharacteristic->getValue();
    if (val.length() > 0) {
        parseIncomingPacket((const uint8_t*)val.data(), val.length());
    }
}

void BleNavService::parseIncomingPacket(const uint8_t* data, size_t length) {
    g_navState.lastPacketMillis = millis();
    g_navState.isConnected = true;

    // Check if JSON packet
    if (data[0] == '{') {
        char buffer[256];
        size_t copyLen = length < sizeof(buffer) - 1 ? length : sizeof(buffer) - 1;
        memcpy(buffer, data, copyLen);
        buffer[copyLen] = '\0';
        parseJsonPacket(buffer);
    } else {
        // Binary packet
        parseBinaryPacket(data, length);
    }
}

void BleNavService::parseJsonPacket(const char* jsonStr) {
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, jsonStr);
    if (err) {
        Serial.printf("[BLE] JSON Parse Error: %s\n", err.c_str());
        return;
    }

    g_navState.isNavigating = true;

    if (doc["m"].is<uint8_t>()) {
        g_navState.maneuver = doc["m"].as<uint8_t>();
    }
    if (doc["d"].is<uint32_t>()) {
        uint32_t newDist = doc["d"].as<uint32_t>();
        // Check if crossed turn threshold to trigger audio/haptic beep
        if (newDist <= TURN_ALERT_DISTANCE_METERS && g_navState.distanceMeters > TURN_ALERT_DISTANCE_METERS) {
            g_navState.alertTriggered = true;
        }
        g_navState.distanceMeters = newDist;
    }
    if (doc["road"].is<const char*>()) {
        strncpy(g_navState.currentRoad, doc["road"].as<const char*>(), sizeof(g_navState.currentRoad) - 1);
        g_navState.currentRoad[sizeof(g_navState.currentRoad) - 1] = '\0';
    }
    if (doc["next"].is<const char*>()) {
        strncpy(g_navState.nextRoad, doc["next"].as<const char*>(), sizeof(g_navState.nextRoad) - 1);
        g_navState.nextRoad[sizeof(g_navState.nextRoad) - 1] = '\0';
    }
    
    // Next-Next Turn
    if (doc["nm"].is<uint8_t>()) {
        g_navState.nextManeuver = doc["nm"].as<uint8_t>();
    }
    if (doc["nd"].is<uint32_t>()) {
        g_navState.nextDistanceMeters = doc["nd"].as<uint32_t>();
    }

    // Telemetry
    if (doc["spd"].is<uint16_t>()) {
        g_navState.speedKmh = doc["spd"].as<uint16_t>();
    }
    if (doc["eta"].is<uint16_t>()) {
        g_navState.etaMinutes = doc["eta"].as<uint16_t>();
    }
    if (doc["bat"].is<uint8_t>()) {
        g_navState.phoneBatteryPercent = doc["bat"].as<uint8_t>();
    }
}

void BleNavService::parseBinaryPacket(const uint8_t* buffer, size_t length) {
    if (length < 8) return;

    g_navState.isNavigating = true;
    uint8_t version = buffer[0];
    if (version != 0x01) return;

    g_navState.maneuver = buffer[1];
    g_navState.distanceMeters = (buffer[2] << 8) | buffer[3];
    g_navState.nextManeuver = buffer[4];
    g_navState.nextDistanceMeters = (buffer[5] << 8) | buffer[6];
    g_navState.speedKmh = buffer[7];

    if (length > 8) {
        size_t strLen = length - 8;
        if (strLen >= sizeof(g_navState.nextRoad)) {
            strLen = sizeof(g_navState.nextRoad) - 1;
        }
        memcpy(g_navState.nextRoad, &buffer[8], strLen);
        g_navState.nextRoad[strLen] = '\0';
    }
}

void BleNavService::update() {
    // Check for packet timeout (e.g. Navigation stopped on phone)
    if (m_connected && g_navState.isNavigating) {
        if (millis() - g_navState.lastPacketMillis > 15000) {
            // 15 seconds without packet -> phone stopped route
            g_navState.isNavigating = false;
            strncpy(g_navState.currentRoad, "Route Paused / Arrived", sizeof(g_navState.currentRoad));
        }
    }
}

