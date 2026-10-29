import Foundation
import CoreBluetooth

// ====================================================================
// NavigateMe BLE Bluetooth Central Manager
// ====================================================================

class NavigateMeBLEManager: NSObject, ObservableObject, CBCentralManagerDelegate, CBPeripheralDelegate {
    static let shared = NavigateMeBLEManager()

    // Service & Characteristic UUIDs matching firmware config.h
    let navServiceUUID = CBUUID(string: "0000fee0-0000-1000-8000-00805f9b34fb")
    let navDataCharUUID = CBUUID(string: "0000fee1-0000-1000-8000-00805f9b34fb")

    @Published var isConnected = false
    @Published var statusMessage = "Bluetooth Initializing..."

    private var centralManager: CBCentralManager!
    private var targetPeripheral: CBPeripheral?
    private var dataCharacteristic: CBCharacteristic?

    override init() {
        super.init()
        UIDevice.current.isBatteryMonitoringEnabled = true
        centralManager = CBCentralManager(delegate: self, queue: nil)
    }

    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        if central.state == .poweredOn {
            statusMessage = "Scanning for NavigateMe-Dash..."
            centralManager.scanForPeripherals(withServices: [navServiceUUID], options: [CBCentralManagerScanOptionAllowDuplicatesKey: false])
        } else {
            statusMessage = "Bluetooth unavailable (\(central.state.rawValue))"
            isConnected = false
        }
    }

    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        print("[BLE] Discovered: \(peripheral.name ?? "Unknown") [RSSI: \(RSSI)]")
        targetPeripheral = peripheral
        targetPeripheral?.delegate = self
        centralManager.stopScan()
        statusMessage = "Connecting to \(peripheral.name ?? "Device")..."
        centralManager.connect(peripheral, options: nil)
    }

    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        isConnected = true
        statusMessage = "Connected to NavigateMe"
        peripheral.discoverServices([navServiceUUID])
    }

    func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?) {
        isConnected = false
        statusMessage = "Disconnected. Re-scanning..."
        dataCharacteristic = nil
        targetPeripheral = nil
        centralManager.scanForPeripherals(withServices: [navServiceUUID], options: nil)
    }

    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        guard let services = peripheral.services else { return }
        for service in services where service.uuid == navServiceUUID {
            peripheral.discoverCharacteristics([navDataCharUUID], for: service)
        }
    }

    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        guard let characteristics = service.characteristics else { return }
        for char in characteristics where char.uuid == navDataCharUUID {
            dataCharacteristic = char
            print("[BLE] Navigation Data Characteristic ready for transmission!")
        }
    }

    /// Transmits live navigation packet to ESP32 Handlebar Display
    func sendNavigationUpdate(maneuver: Int, distance: Int, currentRoad: String, nextRoad: String, nextManeuver: Int, nextDistance: Int, speed: Int, eta: Int) {
        guard isConnected, let peripheral = targetPeripheral, let char = dataCharacteristic else { return }

        let payload: [String: Any] = [
            "m": maneuver,
            "d": distance,
            "road": currentRoad,
            "next": nextRoad,
            "nm": nextManeuver,
            "nd": nextDistance,
            "spd": speed,
            "eta": eta,
            "bat": max(1, Int(UIDevice.current.batteryLevel >= 0 ? UIDevice.current.batteryLevel * 100 : 85))
        ]

        if let jsonData = try? JSONSerialization.data(withJSONObject: payload, options: []) {
            peripheral.writeValue(jsonData, for: char, type: .withoutResponse)
        }
    }
}

