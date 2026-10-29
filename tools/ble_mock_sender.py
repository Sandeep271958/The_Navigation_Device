#!/usr/bin/env python3
"""
NavigateMe BLE Mock Trip Simulator
Simulates a live motorcycle ride through Hyderabad (Banjara Hills -> Flyover -> HITEC City)
Broadcasting live JSON navigation packets to test the ESP32 display pod on your desk.

Requires: pip install bleak
Usage: python3 ble_mock_sender.py
"""

import asyncio
import json
import time
import sys
# Top-level imports

# Match the UUIDs from firmware/include/config.h
NAV_SERVICE_UUID = "0000fee0-0000-1000-8000-00805f9b34fb"
NAV_DATA_CHAR_UUID = "0000fee1-0000-1000-8000-00805f9b34fb"
TARGET_DEVICE_NAME = "NavigateMe-Dash"

# Maneuver Enum (matches firmware config.h)
MANEUVER_STRAIGHT = 1
MANEUVER_TURN_LEFT = 2
MANEUVER_TURN_RIGHT = 3
MANEUVER_SLIGHT_LEFT = 6
MANEUVER_SLIGHT_RIGHT = 7
MANEUVER_U_TURN = 8
MANEUVER_ROUNDABOUT = 9
MANEUVER_KEEP_LEFT = 10
MANEUVER_KEEP_RIGHT = 11      # Flyover Ramp
MANEUVER_DESTINATION = 13

# Realistic Hyderabad Waypoint Sequence
SIMULATED_ROUTE = [
    {
        "stage": "Departing Banjara Hills",
        "m": MANEUVER_STRAIGHT,
        "d_start": 400,
        "road": "Road No. 1, Banjara Hills",
        "next": "Road No. 36 Junction",
        "nm": MANEUVER_TURN_RIGHT,
        "nd": 450,
        "spd": 35,
        "eta": 22
    },
    {
        "stage": "Turning Right onto Road 36",
        "m": MANEUVER_TURN_RIGHT,
        "d_start": 180,
        "road": "Approaching Jubilee Hills Checkpost",
        "next": "Road No. 36",
        "nm": MANEUVER_STRAIGHT,
        "nd": 800,
        "spd": 25,
        "eta": 20
    },
    {
        "stage": "Cruising toward Durgam Cheruvu",
        "m": MANEUVER_STRAIGHT,
        "d_start": 900,
        "road": "Road No. 36, Jubilee Hills",
        "next": "Cable Stayed Bridge Ramp",
        "nm": MANEUVER_SLIGHT_LEFT,
        "nd": 950,
        "spd": 48,
        "eta": 16
    },
    {
        "stage": "Approaching Flyover / Bridge Fork",
        "m": MANEUVER_KEEP_RIGHT,  # Highlights the Flyover entry!
        "d_start": 250,
        "road": "Durgam Cheruvu Approach",
        "next": "Cable Stayed Bridge (Stay Right)",
        "nm": MANEUVER_STRAIGHT,
        "nd": 600,
        "spd": 40,
        "eta": 12
    },
    {
        "stage": "Exiting onto Mindspace Junction",
        "m": MANEUVER_TURN_LEFT,
        "d_start": 120,
        "road": "Bridge Exit, Inorbit Mall Road",
        "next": "HITEC City Main Road",
        "nm": MANEUVER_KEEP_LEFT,
        "nd": 40,
        "spd": 30,
        "eta": 8
    },
    {
        "stage": "Arriving at Destination",
        "m": MANEUVER_DESTINATION,
        "d_start": 60,
        "road": "Cyber Towers, HITEC City",
        "next": "You have arrived!",
        "nm": 0,
        "nd": 0,
        "spd": 15,
        "eta": 1
    }
]

async def run_simulator():
    from bleak import BleakScanner, BleakClient
    print("=" * 60)
    print("  NavigateMe BLE Mock Trip Simulator (Hyderabad)")
    print(f"  Scanning for target device: '{TARGET_DEVICE_NAME}'...")
    print("=" * 60)

    device = await BleakScanner.find_device_by_filter(
        lambda d, ad: d.name and TARGET_DEVICE_NAME.lower() in d.name.lower()
    )

    if not device:
        print(f"\n[ERROR] Could not find '{TARGET_DEVICE_NAME}'.")
        print("Make sure your ESP32 display pod is powered on and advertising.\n")
        sys.exit(1)

    print(f"\n[FOUND] Device: {device.name} [{device.address}]")
    print("[BLE] Connecting to GATT server...")

    async with BleakClient(device) as client:
        print(f"[BLE] Connected: {client.is_connected}")
        print("[SIMULATOR] Starting live simulated route...\n")

        for leg_idx, leg in enumerate(SIMULATED_ROUTE, 1):
            print(f"\n>>> Stage {leg_idx}/{len(SIMULATED_ROUTE)}: {leg['stage']}")
            print(f"    Target: {leg['next']} (Maneuver Code: {leg['m']})")
            
            dist = leg['d_start']
            step_meters = 25  # Decrement per tick

            while dist >= 0:
                payload = {
                    "m": leg['m'],
                    "d": dist,
                    "road": leg['road'],
                    "next": leg['next'],
                    "nm": leg['nm'],
                    "nd": leg['nd'],
                    "spd": leg['spd'],
                    "eta": leg['eta'],
                    "bat": 88
                }

                json_bytes = json.dumps(payload).encode('utf-8')
                await client.write_gatt_char(NAV_DATA_CHAR_UUID, json_bytes, response=False)
                
                print(f"    [BLE TX] Distance: {dist:3d}m | Speed: {leg['spd']} km/h | Road: {leg['road'][:28]}", end="\r")
                
                dist -= step_meters
                await asyncio.sleep(0.8) # 800ms per distance step

            print()

        print("\n[SIMULATOR] Route simulation complete! Destination reached.")

async def run_dry_run():
    print("=" * 60)
    print("  NavigateMe Mock Trip Simulator (DRY-RUN / TEST MODE)")
    print("  Validating route telemetry, JSON packets & timings...")
    print("=" * 60)

    total_packets = 0
    for leg_idx, leg in enumerate(SIMULATED_ROUTE, 1):
        print(f"\n>>> Stage {leg_idx}/{len(SIMULATED_ROUTE)}: {leg['stage']}")
        print(f"    Maneuver: {leg['m']} | Next Road: {leg['next']}")
        
        dist = leg['d_start']
        step_meters = 50

        while dist >= 0:
            payload = {
                "m": leg['m'],
                "d": dist,
                "road": leg['road'],
                "next": leg['next'],
                "nm": leg['nm'],
                "nd": leg['nd'],
                "spd": leg['spd'],
                "eta": leg['eta'],
                "bat": 88
            }

            # Verify JSON serialization
            json_str = json.dumps(payload)
            json_bytes = json_str.encode('utf-8')
            assert len(json_bytes) <= 256, f"Packet too large for BLE buffer: {len(json_bytes)} bytes"
            total_packets += 1
            
            print(f"    [SIM PKT #{total_packets:02d}] {dist:3d}m -> {leg['next'][:25]:25s} | Payload: {len(json_bytes)}B", end="\r")
            dist -= step_meters
            await asyncio.sleep(0.05)

        print()

    print(f"\n[VERIFIED] All {total_packets} simulation packets generated and validated successfully!")

if __name__ == "__main__":
    if "--test" in sys.argv or "--dry-run" in sys.argv:
        asyncio.run(run_dry_run())
    else:
        try:
            # Top-level imports
            asyncio.run(run_simulator())
        except ImportError:
            print("[INFO] 'bleak' library not detected. Running telemetry validation dry-run instead.")
            print("To enable live BLE transmission to your ESP32, run: pip3 install bleak\n")
            asyncio.run(run_dry_run())
        except KeyboardInterrupt:
            print("\n[Simulator stopped by user]")

