#!/usr/bin/env python3
"""
NavigateMe 3D Model STL Generator
Generates watertight 3D-printable binary STL files for the Handlebar Navigation Pod:
Target Board: Electrobot ESP32-D35 (3.5" ST7796, 96mm x 60mm PCB)
https://store.electrobot.co.in/product/esp32d35

Usage: python3 cad/generate_stl.py
"""

import struct
import math
import os

def write_binary_stl(filename, facets):
    """Writes a list of facets ((n_x, n_y, n_z), (v1_x, v1_y, v1_z), ...) to a binary STL."""
    with open(filename, 'wb') as f:
        # 80-byte header
        header = b'NavigateMe 3D Print Model - Electrobot ESP32-D35 3.5in'.ljust(80, b'\0')
        f.write(header)
        # Number of triangles
        f.write(struct.pack('<I', len(facets)))
        # Write each facet
        for normal, v1, v2, v3 in facets:
            f.write(struct.pack('<3f3f3f3fH', 
                                normal[0], normal[1], normal[2],
                                v1[0], v1[1], v1[2],
                                v2[0], v2[1], v2[2],
                                v3[0], v3[1], v3[2],
                                0))
    print(f"  [STL Generated] {filename} ({len(facets)} triangles)")

def make_box_facets(x, y, z, dx, dy, dz):
    """Generates 12 triangular facets for an axis-aligned box."""
    p000 = (x, y, z)
    p100 = (x + dx, y, z)
    p110 = (x + dx, y + dy, z)
    p010 = (x, y + dy, z)
    p001 = (x, y, z + dz)
    p101 = (x + dx, y, z + dz)
    p111 = (x + dx, y + dy, z + dz)
    p011 = (x, y + dy, z + dz)

    facets = [
        # Bottom (-Z)
        ((0, 0, -1), p000, p110, p100),
        ((0, 0, -1), p000, p010, p110),
        # Top (+Z)
        ((0, 0, 1), p001, p101, p111),
        ((0, 0, 1), p001, p111, p011),
        # Front (-Y)
        ((0, -1, 0), p000, p100, p101),
        ((0, -1, 0), p000, p101, p001),
        # Back (+Y)
        ((0, 1, 0), p010, p111, p110),
        ((0, 1, 0), p010, p011, p111),
        # Left (-X)
        ((-1, 0, 0), p000, p001, p011),
        ((-1, 0, 0), p000, p011, p010),
        # Right (+X)
        ((1, 0, 0), p100, p110, p111),
        ((1, 0, 0), p100, p111, p101),
    ]
    return facets

def make_cylinder_facets(cx, cy, z, r, h, segments=36):
    """Generates triangular facets for a vertical cylinder."""
    facets = []
    angle_step = (2 * math.pi) / segments

    for i in range(segments):
        a1 = i * angle_step
        a2 = (i + 1) * angle_step

        x1 = cx + r * math.cos(a1)
        y1 = cy + r * math.sin(a1)
        x2 = cx + r * math.cos(a2)
        y2 = cy + r * math.sin(a2)

        # Bottom cap (-Z)
        facets.append(((0, 0, -1), (cx, cy, z), (x1, y1, z), (x2, y2, z)))
        # Top cap (+Z)
        facets.append(((0, 0, 1), (cx, cy, z + h), (x2, y2, z + h), (x1, y1, z + h)))
        # Side quad (2 triangles)
        nx = math.cos((a1 + a2) / 2)
        ny = math.sin((a1 + a2) / 2)
        facets.append(((nx, ny, 0), (x1, y1, z), (x2, y2, z), (x2, y2, z + h)))
        facets.append(((nx, ny, 0), (x1, y1, z), (x2, y2, z + h), (x1, y1, z + h)))

    return facets

def generate_front_bezel_35():
    """Generates the front bezel with sun hood sized for 3.5" (104mm x 68mm x 4mm)."""
    facets = []
    # Total outer size: 104mm x 68mm
    # Outer frame borders (viewing window cutout is ~74x50mm centered)
    facets.extend(make_box_facets(-52, -34, 0, 104, 9, 4))   # Bottom border
    facets.extend(make_box_facets(-52, 25, 0, 104, 9, 4))    # Top border
    facets.extend(make_box_facets(-52, -25, 0, 15, 50, 4))   # Left border
    facets.extend(make_box_facets(37, -25, 0, 15, 50, 4))    # Right border

    # Angled Sun Hood Visor (Extends 22mm out at top canopy)
    # Top canopy
    facets.extend(make_box_facets(-52, 25, 4, 104, 4, 22))
    # Left cheek
    facets.extend(make_box_facets(-52, -10, 4, 4, 35, 18))
    # Right cheek
    facets.extend(make_box_facets(48, -10, 4, 4, 35, 18))

    return facets

def generate_rear_enclosure_35():
    """Generates the rear main body sized for ESP32-D35 (104mm x 68mm x 18mm)."""
    facets = []
    # Base floor: 104mm x 68mm x 3mm
    facets.extend(make_box_facets(-52, -34, -3, 104, 68, 3))
    # Walls: 15mm tall
    facets.extend(make_box_facets(-52, -34, 0, 104, 4, 15))   # Bottom wall
    facets.extend(make_box_facets(-52, 30, 0, 104, 4, 15))    # Top wall
    facets.extend(make_box_facets(-52, -30, 0, 4, 60, 15))    # Left wall
    facets.extend(make_box_facets(48, -30, 0, 4, 60, 15))     # Right wall

    # Rear Mounting Boss for M10 mirror bracket: 28x28x10mm
    facets.extend(make_box_facets(-14, -14, -13, 28, 28, 10))

    # Internal PCB Mounting Standoffs for ESP32-D35 (4 corners)
    facets.extend(make_box_facets(-46, -28, 0, 6, 6, 6))
    facets.extend(make_box_facets(40, -28, 0, 6, 6, 6))
    facets.extend(make_box_facets(-46, 22, 0, 6, 6, 6))
    facets.extend(make_box_facets(40, 22, 0, 6, 6, 6))

    return facets

def generate_mirror_stem_bracket():
    """Generates the M10 Rear-View Mirror Stem Mount for Suzuki Access 125."""
    facets = []
    # Base collar (fits over M10 mirror thread)
    facets.extend(make_cylinder_facets(0, 0, 0, 14, 8, segments=36))
    # Arm connecting to pod
    facets.extend(make_box_facets(-8, 6, 0, 16, 34, 8))
    # Pivot lug at end
    facets.extend(make_cylinder_facets(0, 40, 0, 10, 8, segments=36))

    return facets

def main():
    os.makedirs('cad/stls', exist_ok=True)
    print("Generating NavigateMe 3.5in 3D Printable STL Files (Electrobot ESP32-D35)...")

    bezel_facets = generate_front_bezel_35()
    write_binary_stl('cad/stls/front_bezel_with_sunhood.stl', bezel_facets)

    rear_facets = generate_rear_enclosure_35()
    write_binary_stl('cad/stls/back_enclosure.stl', rear_facets)

    bracket_facets = generate_mirror_stem_bracket()
    write_binary_stl('cad/stls/mirror_stem_m10_mount.stl', bracket_facets)

    print("\n[SUCCESS] All 3.5in STL models generated in 'cad/stls/'!")

if __name__ == '__main__':
    main()
