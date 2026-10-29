// ====================================================================
// NavigateMe: Parametric Handlebar Pod Enclosure (OpenSCAD)
// Tailored for Electrobot ESP32-D35 (ESP32-3248S035, 3.5" ST7796)
// Product: https://store.electrobot.co.in/product/esp32d35
// ====================================================================

$fn = 60;

// Electrobot ESP32-D35 3.5" Exact Dimensions (in mm)
disp_width  = 96.0;   // ESP32-D35 PCB width
disp_length = 60.0;   // ESP32-D35 PCB length
disp_depth  = 12.5;   // Total depth of PCB + screen
wall_thick  = 3.0;    // Rugged shell wall thickness
visor_depth = 22.0;   // 22mm sun hood overhang for Hyderabad sun
corner_rad  = 6.0;    // Smooth rounded outer corners

total_w = disp_width + (wall_thick * 2) + 2.0;   // ~104mm
total_l = disp_length + (wall_thick * 2) + 2.0;  // ~68mm
total_d = disp_depth + wall_thick + 6.0;         // ~21.5mm

// Helper: Rounded Box
module rounded_box(w, l, d, r) {
    translate([r, r, 0])
    minkowski() {
        cube([w - 2*r, l - 2*r, d/2]);
        cylinder(r=r, h=d/2);
    }
}

// --------------------------------------------------------------------
// 1. FRONT BEZEL WITH SUN HOOD VISOR (3.5" Screen)
// --------------------------------------------------------------------
module front_bezel_with_sunhood() {
    difference() {
        union() {
            // Main face frame
            rounded_box(total_w, total_l, wall_thick + 2, corner_rad);
            
            // Integrated Sun Hood Visor (Extends 22mm at top, angled cheeks)
            hull() {
                translate([0, total_l - wall_thick*2, 0])
                    cube([total_w, wall_thick*2, wall_thick + 2]);
                translate([0, total_l - wall_thick*2, visor_depth])
                    cube([total_w, wall_thick*2, 2]);
                translate([0, 0, 0])
                    cube([wall_thick, total_l, 4]);
                translate([total_w - wall_thick, 0, 0])
                    cube([wall_thick, total_l, 4]);
            }
        }
        
        // 3.5" Screen Viewing Window Cutout (Active TFT: 73.4mm x 49.0mm)
        translate([total_w/2 - 37, total_l/2 - 25, -1])
            cube([74, 50, wall_thick + 10]);
            
        // 2mm Acrylic Lens Recess Lip (80mm x 56mm x 2mm)
        translate([total_w/2 - 40, total_l/2 - 28, wall_thick])
            cube([80, 56, 3]);

        // M3 Screw Mounting Holes (4 corners)
        translate([5, 5, -2]) cylinder(r=1.6, h=10);
        translate([total_w - 5, 5, -2]) cylinder(r=1.6, h=10);
        translate([5, total_l - 5, -2]) cylinder(r=1.6, h=10);
        translate([total_w - 5, total_l - 5, -2]) cylinder(r=1.6, h=10);
    }
}

// --------------------------------------------------------------------
// 2. REAR HOUSING & MOUNTING BOSS
// --------------------------------------------------------------------
module rear_housing() {
    difference() {
        union() {
            // Outer casing
            rounded_box(total_w, total_l, total_d, corner_rad);
            
            // Rear Mounting Boss for M10 Mirror Clamp (Ball/Lug Socket)
            translate([total_w/2 - 14, total_l/2 - 14, -10])
                cube([28, 28, 10]);
        }
        
        // Inner electronics cavity for ESP32-D35
        translate([wall_thick, wall_thick, wall_thick])
            cube([disp_width + 1.5, disp_length + 1.5, total_d]);

        // Continuous Silicone O-Ring Gasket Groove (1.8mm wide x 1.5mm deep)
        translate([wall_thick/2, wall_thick/2, total_d - 1.5])
            difference() {
                rounded_box(total_w - wall_thick, total_l - wall_thick, 2, corner_rad - 1);
                translate([1.8, 1.8, -0.5])
                    rounded_box(total_w - wall_thick - 3.6, total_l - wall_thick - 3.6, 3, corner_rad - 2);
            }

        // Right-Angle USB-C Cable Grommet Port (Bottom Edge)
        translate([total_w/2 - 7, -2, wall_thick + 3])
            cube([14, wall_thick + 4, 8]);

        // M3 Heat-Set Insert Holes (4 corners)
        translate([5, 5, total_d - 6]) cylinder(r=2.1, h=7);
        translate([total_w - 5, 5, total_d - 6]) cylinder(r=2.1, h=7);
        translate([5, total_l - 5, total_d - 6]) cylinder(r=2.1, h=7);
        translate([total_w - 5, total_l - 5, total_d - 6]) cylinder(r=2.1, h=7);

        // Through-bolt hole for M10 Mirror Stem bracket
        translate([total_w/2, total_l/2, -12])
            cylinder(r=4.2, h=15); // M8/M10 mount bolt
    }
}

// --------------------------------------------------------------------
// 3. M10 MIRROR STEM CLAMP BRACKET
// --------------------------------------------------------------------
module mirror_stem_m10_bracket() {
    difference() {
        union() {
            // Mirror stem base collar
            cylinder(r=14, h=8);
            // Arm extending toward pod
            translate([-8, 0, 0])
                cube([16, 40, 8]);
            // Pivot lug
            translate([0, 40, 4])
                cylinder(r=10, h=8, center=true);
        }
        // Center M10 mirror thread hole (10.2mm clear hole for Suzuki Access 125)
        translate([0, 0, -2])
            cylinder(r=5.2, h=12);
            
        // Pivot bolt hole for angle adjustment
        translate([0, 40, -2])
            cylinder(r=2.6, h=12); // M5 locknut pivot
    }
}
