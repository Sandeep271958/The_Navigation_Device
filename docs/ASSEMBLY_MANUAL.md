# NavigateMe: Physical Assembly & Weatherproofing Manual

---

## 1. 3D Printing Specifications (Hyderabad Climate Considerations)

> [!WARNING]
> **DO NOT USE PLA.**
> In Hyderabad, scooters parked outdoors experience direct solar heating up to **55°C–65°C**. Standard PLA filament softens and deforms at 55°C. 

### Recommended Filament
* **PETG (Recommended for ease of printing):** Heat deflection up to 75°C, high UV resistance, excellent layer bonding.
* **ABS or ASA (Best):** Heat deflection up to 95°C, completely immune to summer heat.

### Slicer Settings
* **Layer Height:** 0.20 mm (or 0.16 mm for smoother visor slope).
* **Perimeters / Wall Loops:** 4 walls (crucial for water impermeability).
* **Top & Bottom Layers:** 5 solid layers.
* **Infill:** 25% – 30% Gyroid or Grid.
* **Print Orientation:**
  * `front_bezel_with_sunhood.stl`: Print flat face-down on build plate with support for the sun hood visor overhang.
  * `back_enclosure.stl`: Print flat bottom-down.
  * `mirror_stem_m10_mount.stl`: Print on its flat side with 100% infill for maximum structural rigidity.

---

## 2. Front Lens & Water Sealing

1. **Protective Lens Material:**
   * Cut a piece of **2.0 mm clear scratch-resistant Acrylic (Plexiglass)** or Polycarbonate:
     * Dimensions: **70.0 mm × 52.0 mm** (with 2mm rounded corners).
2. **Sealing the Lens to the Front Bezel:**
   * Apply a thin, continuous bead of **Black RTV Neutral-Cure Silicone** (e.g., Anabond or Permatex Ultra Black) into the lens recess lip of the 3D-printed front bezel.
   * Press the acrylic lens firmly into the recess.
   * Wipe away any excess silicone immediately using isopropyl alcohol on a lint-free cloth.
   * Allow to cure for 12 hours.

---

## 3. Perimeter Gasket Installation

1. **Silicone Cord:**
   * Take a length of **1.5 mm solid silicone O-ring cord** (available at local hardware stores or online).
2. **Installation into Back Enclosure:**
   * Lay the silicone cord into the continuous perimeter groove of the rear enclosure.
   * Cut the cord with a 1mm overlap, applying a drop of cyanoacrylate (superglue) at the joint to form a continuous watertight loop.
3. **USB Cable Gland:**
   * Insert the 90° right-angle USB-C cable through the bottom port cutout.
   * Seal the entry port with a molded rubber grommet or a dab of black RTV silicone around the cable jacket.

---

## 4. Handlebar Mirror Stem Mounting (Suzuki Access 125)

The Suzuki Access 125 uses **M10 Right-Hand Thread** rear-view mirror stems.

```
       [Rear-View Mirror Assembly]
                   │
                   ▼
       [M10 Mirror Locknut]
                   │
                   ▼
     ┌───────────────────────────────┐
     │  NavigateMe M10 Base Collar   │  <── Clamped tight between locknut
     └─────────────┬─────────────────┘      and handlebar plastic cowl
                   │ (Extension Arm)
                   ▼
     ┌───────────────────────────────┐
     │ 2-Axis Tilt Pivot (M5 Bolt)   │  <── Adjust viewing angle to rider's eye
     └─────────────┬─────────────────┘
                   │
                   ▼
     [NavigateMe Display Pod Enclosure]
```

### Installation Steps:
1. Loosen the right-side rear-view mirror locknut using a 14mm/17mm spanner.
2. Unscrew the mirror stem.
3. Slide the `mirror_stem_m10_mount` bracket collar over the mirror thread.
4. Thread the mirror back into the scooter handlebar socket and tighten the locknut firmly over the bracket collar.
5. Bolt the display pod to the bracket arm using an M5 × 25mm stainless steel bolt with a nylon locknut.
6. Sit on your scooter and tilt the display pod until the screen is perpendicular to your line of sight. Tighten the M5 pivot bolt.

