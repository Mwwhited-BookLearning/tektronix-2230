# Tektronix 2230 Service Manual — Section 6: Maintenance

OCR transcription of the service manual's Maintenance section
(46 pages), rendered from `Tektronix_2230_Service_Manual-6-
Maintenance.pdf` (not in the repo — see the user's own manual copy).
Primary source material — transcribed as printed, not summarized.
Figures/diagrams/photos are noted with placeholders, not redrawn.

**Signal names with a printed overbar** (indicating an active-low/
inverted signal, e.g. a line over `RESET`) are written with a leading
`/` instead (`/RESET`) — the standard plain-text convention for a
signal that can't be typeset with a real overline, and one that stays
grep-able (searching `RESET` still finds `/RESET`).

<!-- page 6-1 -->

## MAINTENANCE

This section contains information for conducting preventive maintenance, troubleshooting, and corrective maintenance on the instrument. Circuit board removal procedures are included in the corrective maintenance part of this section.

## STATIC-SENSITIVE COMPONENTS

The following precautions are applicable when performing any maintenance involving internal access to the instrument.

**CAUTION**

*Static discharge can damage any semiconductor component in this instrument.*

This instrument contains electrical components that are susceptible to damage from static discharge. Table 6-1 lists the relative susceptibility of various classes of semiconductors. Static voltages of 1 kV to 30 kV are common in unprotected environments.

When performing maintenance, observe the following precautions to avoid component damage:

1. Minimize handling of static-sensitive components.

2. Transport and store static-sensitive components or assemblies in their original containers or on a metal rail. Label any package that contains static-sensitive components or assemblies.

3. Discharge the static voltage from your body by wearing a grounded antistatic wrist strap while handling these components. Servicing static-sensitive components or assemblies should be performed only at a static-free work station by qualified service personnel.

4. Nothing capable of generating or holding a static charge should be allowed on the work station surface.

5. Keep the component leads shorted together whenever possible.

6. Pick up components by their bodies, never by their leads.

### Table 6-1

**Relative Susceptibility to Static-Discharge Damage**

| Semiconductor Classes | Relative Susceptibility Levels |
|---|---|
| MOS or CMOS microcircuits or discretes, or linear microcircuits with MOS inputs (Most Sensitive) | 1 |
| ECL | 2 |
| Schottky signal diodes | 3 |
| Schottky TTL | 4 |
| High-frequency bipolar transistors | 5 |
| JFETs | 6 |
| Linear microcircuits | 7 |
| Low-power Schottky TTL | 8 |
| TTL (Least Sensitive) | 9 |

Voltage equivalent for levels (voltage discharged from a 100 pF capacitor through a resistance of 100 ohms): 1 = 100 to 500 V, 2 = 200 to 500 V, 3 = 250 V, 4 = 500 V, 5 = 400 to 600 V, 6 = 600 to 800 V, 7 = 400 to 1000 V (est), 8 = 900 V, 9 = 1200 V.

<!-- page 6-2 -->

7. Do not slide the components over any surface.

8. Avoid handling components in areas that have a floor or work-surface covering capable of generating a static charge.

9. Use a soldering iron that is connected to earth ground.

10. Use only approved antistatic, vacuum-type desoldering tools for component removal.

## PREVENTIVE MAINTENANCE

### INTRODUCTION

Preventive maintenance consists of cleaning, visual inspection, and checking instrument performance. When performed regularly, it may prevent instrument malfunction and enhance instrument reliability. The severity of the environment in which the instrument is used determines the required frequency of maintenance. An appropriate time to accomplish preventive maintenance is just before instrument adjustment.

### GENERAL CARE

The cabinet minimizes accumulation of dust inside the instrument and should normally be in place when operating the oscilloscope. The front cover supplied with the instrument provides both dust and damage protection for the front panel and crt. The front cover should be on whenever the instrument is stored or is being transported.

### INSPECTION AND CLEANING

The instrument should be visually inspected and cleaned as often as operating conditions require. Accumulation of dirt in the instrument can cause overheating and component breakdown. Dirt on components acts as an insulating blanket, preventing efficient heat dissipation. It also provides an electrical conduction path that could result in instrument failure, especially under high-humidity conditions.

**CAUTION**

*Avoid the use of chemical cleaning agents which might damage the plastics used in this instrument. Use a nonresidue-type cleaner, preferably isopropyl alcohol or a solution of 1% mild detergent with 99% water. Before using any other type of cleaner, consult your Tektronix Service Center or representative.*

#### Exterior

INSPECTION. Inspect the external portions of the instrument for damage, wear, and missing parts; use Table 6-2 as a guide. Instruments that appear to have been dropped or otherwise abused should be checked thoroughly to verify correct operation and performance. Deficiencies found that could cause personal injury or could lead to further damage to the instrument should be repaired immediately.

**CAUTION**

*To prevent getting moisture inside the instrument during external cleaning, use only enough liquid to dampen the cloth or applicator.*

CLEANING. Loose dust on the outside of the instrument can be removed with a soft cloth or small soft-bristle brush. The brush is particularly useful for dislodging dirt on and around the controls and connectors. Dirt that remains can be removed with a soft cloth dampened in a mild detergent-and-water solution. Do not use abrasive cleaners.

A plastic light filter is provided with the oscilloscope. Clean the light filter and the crt face with a soft lint-free cloth dampened with either isopropyl alcohol or a mild detergent-and-water solution.

#### Interior

To gain access to internal portions of the instrument for inspection and cleaning, refer to the "Removal and Replacement Instructions" in the "Corrective Maintenance" part of this section.

INSPECTION. Inspect the internal portions of the instrument for damage and wear, using Table 6-3 as a guide. Deficiencies found should be repaired immediately. The corrective procedure for most visible defects is obvious; however, particular care must be taken if heat-damaged components are found. Overheating usually indicates other trouble in the instrument; therefore, it is important that the cause of overheating be corrected to prevent recurrence of the damage.

<!-- page 6-3 -->

### Table 6-2

**External Inspection Check List**

| Item | Inspect For | Repair Action |
|---|---|---|
| Cabinet, Front Panel, and Cover | Cracks, scratches, deformations, damaged hardware or gaskets. | Touch up paint scratches and replace defective components. |
| Front-panel Controls | Missing, damaged, or loose knobs, buttons, and controls. | Repair or replace missing or defective items. |
| Connectors | Broken shells, cracked insulation, and deformed contacts. Dirt in connectors. | Replace defective parts. Clean or wash out dirt. |
| Carrying Handle | Correct operation. | Replace defective parts. |
| Accessories | Missing items or parts of items, bent pins, broken or frayed cables, and damaged connectors. | Replace damaged or missing items, frayed cables, and defective parts. |

### Table 6-3

**Internal Inspection Checklist**

| Item | Inspect For | Repair Action |
|---|---|---|
| Circuit Boards | Loose, broken, or corroded solder connections. Burned circuit boards. Burned, broken, or cracked circuit-run plating. | Clean solder corrosion with an eraser and flush with isopropyl alcohol. Resolder defective connections. Determine cause of burned items and repair. Repair defective circuit runs. |
| Resistors | Burned, cracked, broken, blistered. | Replace defective resistors. Check for cause of burned component and repair as necessary. |
| Solder Connections | Cold solder or rosin joints. | Resolder joint and clean with isopropyl alcohol. |
| Capacitors | Damaged or leaking cases. Corroded solder on leads or terminals. | Replace defective capacitors. Clean solder connections and flush with isopropyl alcohol. |
| Semiconductors | Loosely inserted in sockets. Distorted pins. | Firmly seat loose semiconductors. Remove devices having distorted pins. Carefully straighten pins (as required to fit the socket), using long-nose pliers, and reinsert firmly. Ensure that straightening action does not crack pins, causing them to break off. |
| Wiring and Cables | Loose plugs or connectors. Burned, broken, or frayed wiring. | Firmly seat connectors. Repair or replace defective wires or cables. |
| Chassis | Dents, deformations, and damaged hardware. | Straighten, repair, or replace defective hardware. |

<!-- page 6-4 -->

If any electrical component is replaced, conduct a Performance Check for the affected circuit and for other closely related circuits (see Section 4). If repair or replacement work is done on any of the power supplies, conduct a complete Performance Check and, if so indicated, an instrument readjustment (see Sections 4 and 5).

**CAUTION**

*To prevent damage from electrical arcing, ensure that circuit boards and components are dry before applying power to the instrument.*

CLEANING. To clean the interior, blow off dust with dry, low-pressure air (approximately 9 psi). Remove any remaining dust with a soft brush or a cloth dampened with a solution of mild detergent and water. A cotton-tipped applicator is useful for cleaning in narrow spaces and on circuit boards. If these methods do not remove all the dust or dirt, the instrument may be spray washed using a solution of 5% mild detergent and 95% water as follows:

1. Gain access to the parts to be cleaned by removing easily accessible shields and panels (see "Removal and Replacement Instructions").

2. Spray wash dirty parts with the detergent-and-water solution; then use clean water to thoroughly rinse them.

3. Dry all parts with low-pressure air.

4. Dry all components and assemblies in an over or drying compartment using low-temperature (125°F to 150°F) circulating air.

SWITCH CONTACTS. The VOLTS/DIV and SEC/DIV switches are mounted on circuit boards within the instrument. Care must be exercised to preserve the high-frequency characteristics of these switches. Switch maintenance is seldom necessary, but if required, use this procedure.

1. Cam-activated VOLTS/DIV Attenuator switches.

**CAUTION**

*Most spray-type circuit coolants contain Freon 12 as a propellant. Because many Freons adversely affect switch contacts, do not use spray-type coolants on the switches or attenuators.*

*The only recommended circuit coolants for the VOLT/DIV attenuators are dry ice (CO₂) and isopropyl alcohol.*

a. Use only isopropyl alcohol as a cleaning agent for switches, especially in the area of the Vertical Attenuator circuit board. Carbon based solvents will damage the board material.

b. Apply the alcohol with a small, camel-hair brush. Do not use cotton tipped applicators as the cotton tends to snag and possibly damage the switch contacts.

2. Rotary-activated SEC/DIV switch contacts.

**CAUTION**

*Use only deionized or distilled water at about 55°C (131°F) to clean the SEC/DIV timing switch. Tap water contains impurities that remain as residual deposits after evaporation.*

a. Spray hot water into the slots at the top of each switch housing while rotating the switch control knob. Use an atomizing spray device, and spray for only about five seconds.

b. Dry the switch and circuit board on which it is mounted with dry low-pressure air.

c. Bake the switch and circuit board in an oven or drying compartment using dry circulating air at about 75°C (167°F) for 15 minutes.

<!-- page 6-5 -->

### LUBRICATION

Most of the potentiometers used in this instrument are permanently sealed and generally do not require periodic lubrication. All switches, both rotary- and lever-type, are installed with proper lubrication applied where necessary and will rarely require any additional lubrication. A regular periodic lubrication program for the instrument is therefore, not recommended.

### SEMICONDUCTOR CHECKS

Periodic checks of the transistors and other semiconductors in the oscilloscope are not recommended. The best check of semiconductor performance is actual operation in the instrument.

### PERIODIC READJUSTMENT

To ensure accurate measurements, check the performance of this instrument every 2000 hours of operation, or if used infrequently, once each year. In addition, replacement of components may necessitate readjustment of the affected circuits.

Complete Performance Check and Adjustment instructions are given in Sections 4 and 5. The Performance Check Procedure can also be helpful in localizing certain troubles in the instrument. In some cases, minor problems may be revealed or corrected by readjustment. If only a partial adjustment is performed, see the interaction chart, Table 5-1, for possible adjustment interaction with other circuits.

## TROUBLESHOOTING

### INTRODUCTION

Preventive maintenance performed on a regular basis should reveal most potential problems before an instrument malfunctions. However, should troubleshooting be required, the following information is provided to facilitate location of a fault. In addition, the material presented in the "Theory of Operation" and "Diagrams" sections of this manual may be helpful while troubleshooting.

### TROUBLESHOOTING AIDS

#### Diagnostic Firmware

The operating firmware in this instrument contains diagnostic routines that aid in locating malfunctions of the digital storage portions of the instrument. When instrument power is applied, power-up kernel tests are performed to verify proper operation of the instrument's microprocessor, RAM and ROM. If a failure is detected, this information is passed on to the operator, if possible. The failure information directs the operator to the failing block of memory. If the failure is such that the processor can still execute the diagnostic routines, the user can call up specific tests to further check the failing circuitry. The specific diagnostic routines are explained later in this section.

#### Schematic Diagrams

Complete schematic diagrams are located on tabbed foldout pages in the "Diagrams" section. Portions of circuitry mounted on each circuit board are enclosed by heavy black lines. The assembly number and name of the circuit are shown near either the top or the bottom edge of the enclosed area.

Functional blocks on schematic diagrams are outlined with a wide grey line. Components within the outlined area perform the function designated by the block label. The "Theory of Operation" uses these functional block names when describing circuit operation as an aid in cross-referencing between the theory and the schematic diagrams.

Component numbers and electrical values of components in this instrument are shown on the schematic diagrams. Refer to the first page of the "Diagrams" section for the reference designators and symbols used to identify components. Important voltages and waveform reference numbers (enclosed in hexagonal-shaped boxes) are also shown on each diagram. Waveform illustrations are located adjacent to their respective schematic diagram.

<!-- page 6-6 -->

#### Circuit Board Illustrations

Circuit board illustrations showing the physical location of each component are provided for use in conjunction with each schematic diagram. Each board illustration is found in the "Diagrams" section on the back of a foldout page, preceding the first schematic diagram(s) to which it relates.

The locations of waveform test points are marked on the circuit board illustrations with hexagonal outlined numbers corresponding to the waveform numbers on both the schematic diagram and the waveform illustrations.

Also provided in the "Diagrams" section is an illustration of the bottom side of the Main circuit board. This illustration aids in troubleshooting by showing the connection pads for the components mounted on the top side of the circuit board. By using this illustration, circuit tracing and probing for voltages and signals that are inaccessible from the top side of the board may be achieved without dismantling portions of the instrument.

#### Circuit Board Locations

The placement of each circuit board in the instrument is shown in board locator illustrations. These illustrations are located on foldout pages along with the circuit board illustration.

#### Circuit Board Interconnections

A circuit board interconnection diagram is provided in the "Diagrams" section to aid in tracing a signal path or power source between boards. All wire, plug, and jack numbers are shown along with their associated wire or pin numbers.

#### Power Distribution

Power Distribution diagrams (diagrams 10, 11, and 21) are provided to aid in troubleshooting power supply problems. This diagram shows the service jumper connections used to apply power to the various circuit boards. Excessive loading on a power supply by a circuit board fault may be isolated by disconnecting the appropriate service jumpers.

#### Grid Coordinate System

Each schematic diagram and circuit board illustration has a grid border along its left and top edges. A table located adjacent to each diagram lists the grid coordinates of each component shown on that diagram. To aid in physically locating components on the circuit board, this table also lists the grid coordinates of each component on the circuit board illustration.

Near each circuit board illustration is an alphanumeric listing of all components mounted on that board. The second column in each listing identifies the schematic diagram in which each component can be found. These component-locator tables are especially useful when more than one schematic diagram is associated with a particular circuit board.

#### Component Color Coding

Information regarding color codes and markings of resistors and capacitors is located on the color-coding illustration (Figure 9-1) at the beginning of the "Diagrams" section.

RESISTOR COLOR CODE. Resistors used in this instrument are carbon-film, composition, or precision metal-film types. They are usually color coded with the EIA color code; however, some metal-film type resistors may have the value printed on the body. The color code is interpreted starting with the stripe nearest to one end of the resistor. Composition resistors have four stripes; these represent two significant digits, a multiplier, and a tolerance value. Metal-film resistors have five stripes representing three significant digits, a multiplier, and a tolerance value.

CAPACITOR MARKINGS. Capacitance values of common disc capacitors and small electrolytics are marked on the side of the capacitor body. White ceramic capacitors are color coded in picofarads, using a modified EIA code.

Dipped tantalum capacitors are color coded in microfarads. The color dot indicates both the positive lead and the voltage rating. Since these capacitors are easily destroyed by reversed or excessive voltage, be careful to observe the polarity and voltage rating when replacing them.

DIODE COLOR CODE. The cathode end of each glass-encased diode is indicated by either a stripe, a series of stripes or a dot. For most diodes marked with a series of stripes, the color combination of the stripes identifies three digits of the Tektronix Part Number, using the resistor color-code system. The cathode and anode ends of a metal-encased diode may be identified by the diode symbol marked on its body.

#### Semiconductor Lead Configurations

Figure 9-2 in the "Diagrams" section shows the lead configurations for semiconductor devices used in the instrument. These lead configurations and case styles are typical of those used at completion of the instrument design. Vendor changes and performance improvement

<!-- page 6-7 -->

changes may result in changes of case styles or lead configurations. If the device in question does not appear to match the configuration shown in Figure 9-2, examine the associated circuitry or consult the manufacturer's data sheet.

#### Multipin Connectors

Multipin connector orientation is indexed by two triangles; one on the holder and one on the circuit board. Slot numbers are usually molded into the holder. When a connection is made to circuit board pins, ensure that the index on the holder is aligned with the index on the circuit board (see Figure 6-1).

#### Storage Board Latch

**WARNING**

*Turn off POWER switch before placing the Storage circuit board in Servicing Position.*

While servicing the interior of the instrument, the Storage circuit board may be latched in the Servicing Position. See the "Storage Circuit Board in Servicing Position" in the "Removal and Replacement Instructions" part of this section. The two signal leads of the four-wire connectors P2111 and P2112 need to be grounded when disconnected from the Storage circuit board. Grounding the signal leads of P2111 and P2112 permits the VERTICAL POSITION controls to work properly.

**[Figure 6-1. Multi-connector holder orientation — photo of a multipin connector holder and circuit board pins showing index-triangle alignment and "DIRECTION OF NOMENCLATURE" markings. (0995-11) 2662-50.]**

The center signal leads may be connected to the outside ground leads of P2111 and P2112 by using four 1-inch long number 22 tinned copper wires (two wires for each connector). Bend the wires in a U-shape and insert the wires between pins 1 and 2, and between pins 3 and 4 of the connectors (see Figure 6-2).

#### Analog Isolation

The digital portion of the instrument may be isolated from the analog portion of the instrument. Use of this procedure enables disabling and isolation of the digital portion of the instrument while permitting troubleshooting on the analog portion.

1. Disconnect connectors P6110, P6120, and P6130 from the right edge of the Input/Output board (A11A1).

2. Disconnect connectors P6410 and P6420 from the right edge of the Vector Generator board (A11A2).

**[Figure 6-2. Grounding the signal lines of P2111 and P2112 — diagram of a 4-pin connector (P2111 and P2112) with two 1-inch wires bent in a U-shape and inserted between pins 1-2 and 3-4. Drawing number 4999-36.]**

<!-- page 6-8 -->

3. Disconnect connector P6421 from the Sweep Interface board (A13).

4. Disconnect connector P9010 from the middle right edge of the Main board (A1).

5. Latch Storage circuit board in the servicing position (see "Storage Circuit Board in the Servicing Position" in the "Removal and Replacement Instructions" part of this section.

6. Ground the two signal leads (pins 2 and 3) of four-wire connectors P2111 and P2112 (see preceding "Storage Board Latch" part of this section).

7. Disconnect connector P9410 from the Sweep Reference board (A16).

8. Connect together pins 2 and 4 of J9410.

9. Disconnect connector P4220 from the middle right side of the Alternate Sweep board (A5).

10. Disconnect connector P4210 from the middle right half of the Main board (A1).

11. Disconnect connector P9050 from the middle of the Main board (A1).

12. Disconnect connector P9060 from the middle of the Main board (A1).

13. Disconnect connector P9320 from the front of the Main board (A1).

14. Disconnect connector P9301 (P8100 if the instrument contains Option 10 or Option 12) from the middle left corner of the X/Y Plotter board.

#### Kernel Isolation

The Kernel (Microprocessor, Clock, and Address Latch) may be isolated from the rest of the circuitry. The Kernel can then be troubleshot. When the Kernel is functional, the power-up diagnostics may be used to further troubleshoot the digital circuitry. Isolate the Kernel by:

1. Removing P9105A.
2. Moving P9105B to its TEST position.
3. Moving P9105C to its TEST position.
4. Moving P9105D to its TEST position.

Figure 6-3 shows the isolated Kernel timing diagrams. After the Kernel is repaired, restore normal operation by using the reverse of the preceding procedure.

#### Switch Interface Voltages

Voltages generated by the interface to front-panel switches may be used to troubleshoot the instrument. Timing switch interface voltages are shown in Table 6-4. VERTICAL VOLTS/DIV switch interface voltages are shown in Table 6-5. Interface voltages for the AC GND DC switches are shown in Table 6-6.

<!-- page 6-9 -->

**[Figure 6-3. Isolated kernel timing — 8 oscilloscope photo traces of pins 12-19 for U9112 and U9114, each showing digital timing waveforms with vertical/horizontal scale annotations (e.g. "5V/div", timebase values). Drawing number 4999-14.]**

<!-- page 6-10 -->

### Table 6-4

**Timing Switch Interface Voltages**

| A and B SEC per DIV | ARES1 J6421 pin 2 Voltage Range | AC1 W6123 pin 1 | AC2 W6123 pin 2 | ARES2 J6421 pin 1 Voltage Range | B RES J6421 pin 5 Voltage Range | B CAPS J6421 pin 4 |
|---|---|---|---|---|---|---|
| EXT CLK | 4.591 to 5.100 | 5 V | 5 V | 3.742 to 4.590 | 2.510 to 3.546 | 3.2 to 5.0 |
| 0.5 s | 4.591 to 5.100 | 0 V | 5 V | 4.591 to 5.100 | 2.510 to 3.546 | 3.2 to 5.0 |
| 0.2 s | 4.591 to 5.100 | 0 V | 5 V | 3.742 to 4.590 | 2.510 to 3.546 | 3.2 to 5.0 |
| 0.1 s | 4.591 to 5.100 | 0 V | 5 V | 2.716 to 3.742 | 2.510 to 3.546 | 3.2 to 5.0 |
| 50 ms | -0.250 to 1.150 | 0 V | 5 V | 4.591 to 5.100 | 2.510 to 3.546 | 3.2 to 5.0 |
| 20 ms | 4.591 to 5.100 | 0 V | 5 V | 1.109 to 2.715 | 1.548 to 2.509 | 3.2 to 5.0 |
| 10 ms | 4.591 to 5.100 | 0 V | 5 V | -0.350 to 1.108 | -0.200 to 0.612 | 3.2 to 5.0 |
| 5 ms | 1.151 to 2.715 | 0 V | 5 V | 4.591 to 5.100 | 0.613 to 1.547 | 3.2 to 5.0 |
| 2 ms | 3.743 to 4.590 | 0 V | 5 V | 4.591 to 5.100 | 4.227 to 4.752 | 3.2 to 5.0 |
| 1 ms | 2.716 to 3.742 | 0 V | 5 V | 4.591 to 5.100 | 3.547 to 4.226 | 3.2 to 5.0 |
| 0.5 ms | -0.250 to 1.150 | 5 V | 0 V | 4.591 to 5.100 | 2.510 to 3.546 | 1.3 to 3.2 |
| 0.2 ms | 4.591 to 5.100 | 5 V | 0 V | 1.109 to 2.715 | 1.548 to 2.509 | 1.3 to 3.2 |
| 0.1 ms | 4.591 to 5.100 | 5 V | 0 V | -0.350 to 1.108 | -0.200 to 0.612 | 1.3 to 3.2 |
| 50 μs | 1.151 to 2.715 | 5 V | 0 V | 4.591 to 5.100 | 0.613 to 1.547 | 1.3 to 3.2 |
| 20 μs | 3.743 to 4.590 | 5 V | 0 V | 4.591 to 5.100 | 4.227 to 4.752 | 1.3 to 3.2 |
| 10 μs | 2.716 to 3.742 | 5 V | 0 V | 4.591 to 5.100 | 3.547 to 4.226 | 1.3 to 3.2 |
| 5 μs | -0.250 to 1.150 | 0 V | 0 V | 4.591 to 5.100 | 2.510 to 3.546 | -1.0 to 1.3 |
| 2 μs | 4.591 to 5.100 | 0 V | 0 V | 1.109 to 2.715 | 1.548 to 2.509 | -1.0 to 1.3 |
| 1 μs | 4.591 to 5.100 | 0 V | 0 V | -0.350 to 1.108 | -0.200 to 0.612 | -1.0 to 1.3 |
| 0.5 μs | 1.151 to 2.715 | 0 V | 0 V | 4.591 to 5.100 | 0.613 to 1.547 | -1.0 to 1.3 |
| 0.2 μs | 3.743 to 4.590 | 0 V | 0 V | 4.591 to 5.100 | 4.227 to 4.752 | -1.0 to 1.3 |
| 0.1 μs | 2.716 to 3.742 | 0 V | 0 V | 4.591 to 5.100 | 3.547 to 4.226 | -1.0 to 1.3 |
| 0.05 μs | 4.591 to 5.100 | 0 V | 0 V | 4.591 to 5.100 | 4.753 to 5.100 | -1.0 to 1.3 |

<!-- page 6-11 -->

### Table 6-5

**Vertical VOLTS/DIV Switch Interface Voltages**

| SWITCH SETTING | CH1 ATN and CH2 ATN (J6111 pin 2 and J6112 pin 2) |
|---|---|
| 2 mV per division | 2.104 to 2.340 |
| 5 mV per division | 4.167 to 4.712 |
| 10 mV per division | 3.199 to 3.440 |
| 20 mV per division | 2.502 to 2.702 |
| 50 mV per division | 0 to 2.104 |
| 0.1 V per division | 2.938 to 3.199 |
| 0.2 V per division | 2.340 to 2.502 |
| 0.5 V per division | 4.712 to 5.000+ |
| 1 V per division | 3.731 to 4.167 |
| 2 V per division | 3.440 to 3.731 |
| 5 V per division | 2.702 to 2.938 |

### Table 6-6

**AC GND DC Switch Interface Voltages**

| Variable VOLTS/DIV | SWITCH POSITION | CH1 STAT and CH2 STAT (J6111 pin 3 and J6112 pin 3) |
|---|---|---|
| OUT OF DETENT | AC | 0 to 2.423 |
| OUT OF DETENT | GND | 2.696 to 3.070 |
| OUT OF DETENT | DC | 3.623 to 4.457 |
| IN DETENT | AC | 2.423 to 2.696 |
| IN DETENT | GND | 3.070 to 3.623 |
| IN DETENT | DC | 4.457 to 5.000+ |

## TROUBLESHOOTING EQUIPMENT

The equipment listed in Table 4-1 of this manual, or equivalent equipment, may be useful when troubleshooting this instrument.

## TROUBLESHOOTING TECHNIQUES

The following procedure is arranged in an order that enables checking simple trouble possibilities before requiring more extensive troubleshooting. The first two steps use diagnostic aids inherent in the instrument's operating firmware and will locate many circuit faults. The next four steps ensure proper control settings, connections, operation, and adjustment. If the trouble is not located by these checks, the remaining steps will aid in locating the defective component. When the defective component is located, replace it using the appropriate replacement procedure given under "Corrective Maintenance" in this section.

**CAUTION**

*Before using any test equipment to make measurements on static-sensitive, current-sensitive, or voltage-sensitive components or assemblies, ensure that any voltage or current supplied by the test equipment does not exceed the limits of the component to be tested.*

### 1. Power-up Tests

The instrument performs automatic verification of the instrument's Microprocessor, ROM, and RAM (the operating kernel) when power is first applied. If all Kernel tests pass, a second level of diagnostic tests are performed. The Diagnostic tests, when passed, give the user a high degree of assurance that the instrument's storage circuitry is functioning properly.

If a diagnostic test fails, the faulty circuitry is identified by a message on the crt (if the instrument is able to produce a display), and by an LED display. If a failure occurs, refer to the "Diagnostics" discussion later in this section for definitions of error messages.

### 2. Diagnostic Test Routines

Many of the diagnostic routines may be selected from the front panel to further clarify the nature of a suspected failure. The desired test is selected using the MENU. The Diagnostics are explained in the "Diagnostics" discussion later in this section.

### 3. Check Control Settings

Incorrect control settings can give a false indication of instrument malfunction. If there is any question about the correct function or operation of any control, refer to either the "Operating Information" in Section 2 of this manual or to the Operators Manual.

### 4. Check Associated Equipment

Before proceeding, ensure that any equipment used with the instrument is operating correctly. Verify that input signals are properly connected and that the interconnecting cables are not defective. Check that the ac-power-source voltage to all equipment is correct.

<!-- page 6-12 -->

### 5. Visual Check

**WARNING**

*To avoid electrical shock, disconnect the instrument from the ac power source before making a visual inspection of the internal circuitry.*

Perform a visual inspection. This check may reveal broken connections or wires, damaged components, semiconductors not firmly mounted, damaged circuit boards, or other clues to the cause of an instrument malfunction.

### 6. Check Instrument Performance and Adjustment

Check the performance of either those circuits where trouble appears to exist or the entire instrument. The apparent trouble may be the result of misadjustment. Complete performance check and adjustment instructions are given in Sections 4 and 5 of this manual.

### 7. Isolate Trouble to a Circuit

To isolate problems to a particular area, use any symptoms noticed to help locate the trouble. Refer to the "Diagnostics" discussion in this section as an aid in locating a faulty circuit.

### 8. Check Power Supplies

**WARNING**

*For safety reasons, an isolation transformer must be connected whenever troubleshooting is done in the Preregulator and Inverter Power Supply sections of the instrument.*

When trouble symptoms appear in more that one circuit, first check the power supplies; then check the affected circuits by taking voltage and waveform readings. Check first for the correct output voltage of each individual supply. These voltages are measured between the power supply test points and ground (see the associated circuit board illustration and Table 6-7).

Voltages levels may be measured either with a DMM or with an oscilloscope. Voltage ripple amplitudes must be measured using an oscilloscope. Before checking power-supply circuitry, set the INTENSITY control to normal brightness, the A and B SEC/DIV switch to 0.1 ms, the HORIZONTAL MODE to B, the ON/OFF READOUT TOGGLE to display the readout, the A TRIGGER Mode to P-P AUTO, and set the VERTICAL MODE switch to CH 1.

When measuring ripple, use a 1X probe having a bayonet ground assembly (see Table 6-7) attached to the probe tip to minimize stray pickup. Insert the bayonet assembly signal tip into the first test point indicated in Table 6-7, and touch the bayonet assembly ground tip to the chassis near the test point. The ripple values listed are based on a system limited in bandwidth to 30 kHz. Using a system with wider bandwidth will result in higher readings.

### Table 6-7

**Power Supply Voltage and Ripple Limits**

| Power Supply | Test Point | Reading (Volts) | P-P Ripple (mV) |
|---|---|---|---|
| -8.6 V | W961 | -8.56 to -8.64 | <1.5 |
| -5.0 V | W9020 | -4.75 to -5.25 | <20 |
| +5.0 V | W9068 | +5.75 to +5.25 | <20 |
| +8.6 V | W960 | +8.43 to +8.77 | <8 |
| +30 V | W956 | +29.1 to +30.9 | <30 |
| +100 V | W954 | +97.0 to +103.0 | <100 |

If the power-supply voltages and ripple are within the ranges listed in Table 6-7, the supply can be assumed to be working correctly. If they are outside the range, the supply may be either misadjusted or operating incorrectly. Use the "Power Supply and CRT Display" subsection in the "Adjustment" procedure to adjust the -8.6 V supply.

A defective component elsewhere in the instrument can create the appearance of a power-supply problem and may also affect the operation of other circuits.

### 9. Check Circuit Board Interconnections

After the trouble has been isolated to a particular circuit, again check for loose or broken connections, improperly seated semiconductors, and heat-damaged components.

### 10. Check Voltages and Waveforms

Often the defective component can be located by checking circuit voltages or waveforms. Typical voltages are listed on the schematic diagrams. Waveforms indicated on the schematic diagrams by hexagonal-outlined numbers are shown adjacent to the diagrams. Waveform test points are shown on the circuit board illustrations.

<!-- page 6-13 -->

**NOTE**

*Voltages and waveforms indicated on the schematic diagrams are not absolute and may vary slightly between instruments. To establish operating conditions similar to those used to obtain these readings, see the "Voltage and Waveform Setup Conditions" preceding the waveform illustrations in the "Diagrams" section.*

Note the recommended test equipment, front-panel control settings, voltage and waveform conditions, and cable-connection instructions. Any special control settings required to obtain a given waveform are noted under the waveform illustration. Changes to the control settings from the initial setup, other than those noted, are not required.

### 11. Check Individual Components

**WARNING**

*To avoid electric shock, always disconnect the instrument from the ac power source before removing or replacing components.*

The following procedures describe methods of checking individual components. Two-lead components that are soldered in place are most accurately checked by first disconnecting one end from the circuit board. This isolates the measurement from the effects of the surrounding circuitry. See Figure 9-1 for component value identification and Figure 9-2 for semiconductor lead configurations.

**CAUTION**

*When checking semiconductors, observe the static-sensitivity precautions located at the beginning of this section.*

TRANSISTORS. A good check of a transistor is actual performance under operating conditions. A transistor can most effectively be checked by substituting a known-good component. However, be sure that circuit conditions are not such that a replacement transistor might also be damaged. If substitute transistors are not available, use a dynamic-type transistor checker for testing. Static-type transistor checkers are not recommended, since they do not check operation under simulated operating conditions.

When troubleshooting transistors in the circuit with a voltmeter, measure both the emitter-to-base and emitter-to-collector voltages to determine whether they are consistent with normal circuit voltages. Voltages across a transistor may vary with the type of device and its circuit function.

Some of these voltages are predictable. The emitter-to-base voltage for a conducting silicon transistor will normally range from 0.6 V to 0.8 V. The emitter-to-collector voltage for a saturated transistor is about 0.2 V. Because these values are small, the best way to check them is by connecting a sensitive voltmeter across the junction rather that comparing two voltages taken with respect to ground. If the former method is used, both leads of the voltmeter must be isolated from ground.

If voltage values measured are less that those just given, either the device is shorted or no current is flowing in the external circuit. If values exceed the emitter-to-base values given, either the junction is reverse biased or the device is defective. Voltages exceeding those given for typical emitter-to-collector values could indicate either a nonsaturated device operating normally or a defective (open-circuited) transistor. If the device is conducting, voltage will be developed across the resistors in series with it; if open, no voltage will be developed across the resistors unless current is being supplied by a parallel path.

**CAUTION**

*When checking emitter-to-base junctions, do not use an ohmmeter range that has a high internal current. High current may damage the transistor. Reverse biasing the emitter-to-base junction with a high current may degrade the current-transfer ratio (Beta) of the transistor.*

A transistor emitter-to-base junction also can be checked for an open or shorted condition by measuring the resistance between terminals with an ohmmeter set to a range having a low internal source current, such as the R X 1 kΩ range. The junction resistance should be very high in one direction and much lower when the meter leads are reversed.

When troubleshooting a field-effect transistor (FET), the voltage across its elements can be checked in the same manner as previously described for other transistors. However, remember that in the normal depletion mode of operation, the gate-to-source junction is reverse biased; in the enhanced mode, the junction is forward biased.

INTEGRATED CIRCUITS. An integrated circuit (IC) can be checked with a voltmeter, test oscilloscope, or by direct substitution. A good understanding of circuit operation is essential when troubleshooting a circuit having IC components. Use care when checking voltages and waveforms around the IC so that adjacent leads are not shorted together. An IC test clip provides a convenient means of clipping a test probe to an IC.

<!-- page 6-14 -->

**CAUTION**

*When checking a diode, do not use an ohmmeter scale that has a high internal current. High current may damage a diode. Checks on diodes can be performed in much the same manner as those on transistor emitter-to-base junctions. Do not check tunnel diodes or back diodes with an ohmmeter; use a dynamic tester, such as the TEKTRONIX 576 Curve Tracer.*

DIODES. A diode can be checked for either an open or a shorted condition by measuring the resistance between terminals with an ohmmeter set to a range having a low internal source current, such as the R X 1 kΩ range. The diode resistance should be very high in one direction and much lower when the meter leads are reversed.

Silicon diodes should have 0.6 V to 0.8 V across their junctions when conducting; Schottky diodes about 0.2 V to 0.4 V. Higher readings indicate that they are either reverse biased or defective.

RESISTORS. Check resistors with an ohmmeter. Refer to the "Replaceable Electrical Parts" list for the tolerances of resistors used in this instrument. A resistor normally does not require replacement unless its measured value varies widely from its specified value and tolerance.

INDUCTORS. Check for open inductors by checking continuity with an ohmmeter. Shorted or partially shorted inductors can usually be found by checking the waveform response when high-frequency signals are passed through the circuit.

CAPACITORS. A leaky or shorted capacitor can best be detected by checking resistance with an ohmmeter set to one of the highest ranges. Do not exceed the voltage rating of the capacitor. The resistance reading should be high after the capacitor is charged to the output voltage of the ohmmeter. An open capacitor can be detected with a capacitance meter or by checking whether the capacitor passes ac signals.

### 12. Repair and Adjust the Circuit

If any defective parts are located, follow the replacement procedures given under "Corrective Maintenance" in this section. After any electrical component has been replaced, the performance of that circuit and any other closely related circuit should be checked. Since the power supplies affect all circuits, performance of the entire instrument should be checked if work has been done on the power supplies or if the power transformer has been replaced. Readjustment of the affected circuitry may be necessary. Refer to the "Performance Check" and "Adjustment Procedure," Sections 4 and 5 of this manual and to Table 5-1 (Adjustment affected by repairs).

## DIAGNOSTICS

### Introduction

A list of the instrument diagnostic tests is shown in Table 6-8. The diagnostics are run automatically during power-up and/or manually via the menu. The location in the menu of each test is shown in Figure 6-4. Only the *digital storage* portion of the instrument is checked. Circuitry checked, and/or used by each test is shown in Table 6-9. During a normal power-up, only the first error of each test is displayed. If the CURSORS SELECT C1/C2 button is held in during power-up (invoking extended DIAGNOSTICS) the first 15 errors from all tests are displayed. If the instrument contains the RS-232-C Option, an ASCII version of all errors found during power-up is sent to the option. In addition to displaying the errors on the crt, the errors are also displayed on U4119 (see Table 6-10). Timing for the error codes displayed on U4119 is shown in Figure 6-5. A list of all possible error messages is shown in Table 6-11.

### Table 6-8

**Diagnostic Messages and Tests**

| MESSAGE (a) | POWER-UP | MENU |
|---|---|---|
| PU : {message} | X | |
| MI : {message} | X | |
| SYS_ROM_n : {message} | | X |
| SYS_RAM : {message} | | X |
| NIB_RAM : {message} | | X |
| ACQ_AB : {message} | X | X |
| ACQ_MEM : {message} | X | X |
| PRC : {message} | X | X |
| HS_ACQ : {message} | X | X |
| TBD {rng} : {message} | X | X |
| MM_ACQ : {message} | X | X |
| XY_ACQ : {message} | X | X |
| CDT : {message} | X | X |
| FP_A2D : {message} | X | X |

(a) Each n, message, and rng depend upon the detected failure.

<!-- page 6-15 -->

### Table 6-9

**Circuitry Checked by Each Test and Exerciser**

| Test | Circuitry Checked |
|---|---|
| PU | KERNEL = Y4100, U4102, U9104, U9102, U9108, U9101, U9111, U9109, U9110, U9112, U9114, U9103, U9107, and U9113 |
| MI | KERNEL, U9200, and U9105 |
| SYS_ROM | KERNEL, and IO_BLOCK_DECODING = U9105, and U9106 |
| SYS_RAM | KERNEL |
| NIB_RAM | KERNEL |
| ACQ_AB | KERNEL, IO_BLOCK_DECODING, U3423, U3424, U3425, U3427, U3428, and U3426 |
| ACQ_MEM | KERNEL, IO_BLOCK_DECODING, U3423, U3424, U3425, U3427, U3428, U3426, U3418, U3419, U3421, U3422, U3417, U3416, U3420, and U3422 |
| PRC | KERNEL, IO_BLOCK_DECODING, U4115, U4116, U4117, U4123, U4124, U4122, U4114, U4121, U4118, and U4119 |
| HS_ACQ | KERNEL, IO_BLOCK_DECODING, PRC, ACQ_AB, ACQ_MEM, U4104, U4102, U4103, U4125, U4114, U4119, U4118, U4126, U4127, U4128, U4227, U4320, U3310, U3306, U3309, U3308, U3307, U3313, U3426, U3229, U3230, U3231, U3232, U3234, U3236, U3239, U4104, U3101, U3105, U3307, U3104, U3105, and U3308 |
| TBD | KERNEL, IO_BLOCK_DECODING, HS_ACQ, U4107, U4108, U4109, U4110, U4111, U4112, U6106, U4105, U4103, U4127, U4113, and U4114 |
| MM_ACQ | HS_ACQ |
| XY_ACQ | HS_ACQ |
| CDT | HS_ACQ, U4230, U4231, U4122, U4232, U4229, U4127, U4120, U4108, U3428, Q4207, Q4203, Q4204, Q4205, Rs, and Cs |
| FP_A2D | KERNEL, IO_BLOCK_DECODING, U6103, U6104, U6106, U6108, U6102, U6101, and R4912 |
| CAL_PU | NMI, U9111, U9109, U9110, U9201, U9202, U9203, U9231, U9232, U9233, U9208, U3310, U6301A, U6301B, U6301C, U6303, U6304, U6305, U6306, U6307, U6308, U6401A, U6401B, U6401C, U6401D, U6401E, U6402A, U6402B, U6402C, U6402D, U6403A, and U6403D |
| OUT_PORTS | U3423, U3424, U3425, (U3427 U3428), U3310, U4119, U4113, and U6104 |

<!-- page 6-16 -->

### Table 6-10

**U4119 Error Code Display**

U4119 Signal and Pin Number: AD7(12), AD6(13), AD5(14), AD4(15), AD0(16), AD1(17), AD2(18), AD3(19)

| Test | AD7 | AD6 | AD5 | AD4 | AD0 | AD1 | AD2 | AD3 |
|---|---|---|---|---|---|---|---|---|
| ACQ TESTS HS | 0 | 0 | 1 | 0 | FILL | ACQ MEM | PRC | EOR |
| TBD HS/2 | 0 | 0 | 1 | 1 | FILL | ACQ MEM | PRC | EOR |
| TBD PS/2 | 0 | 1 | 0 | 0 | FILL | ACQ MEM | PRC | EOR |
| TBD PS/5 | 0 | 1 | 0 | 1 | FILL | ACQ MEM | PRC | EOR |
| TBD PS/10 | 0 | 1 | 1 | 0 | FILL | ACQ MEM | PRC | EOR |
| TBD /10 | 0 | 1 | 1 | 1 | FILL | ACQ MEM | PRC | EOR |
| TBD /100 | 1 | 0 | 0 | 0 | FILL | ACQ MEM | PRC | EOR |
| TBD /1K | 1 | 0 | 0 | 1 | FILL | ACQ MEM | PRC | EOR |
| TBD /10K | 1 | 0 | 1 | 0 | FILL | ACQ MEM | PRC | EOR |
| TBD /100K | 1 | 0 | 1 | 1 | FILL | ACQ MEM | PRC | EOR |
| MM_ACQ | 1 | 1 | 0 | 0 | FILL | ACQ MEM | PRC | EOR |
| XY | 1 | 1 | 0 | 1 | FILL | ACQ MEM | PRC | EOR |
| CDT | 1 | 1 | 1 | 0 | DELT UNCAL | MIN UNCAL | PRE DETRIG | TIMEOUT |
| FPA/D | 1 | 1 | 1 | 1 | GND | DELTA POT | 0 | TIMEOUT |
| MI | 0 | 0 | 0 | 1 | 0 | STUCK HI | NO RESET | TIMEOUT |
| PU | 0 | 0 | 0 | 0 | 1 | 1 | 1 | 1 |
| ACQ AB | 0 | 0 | 0 | 0 | 1 | 0 | 0 | 1 |
| ACQ MEM 1 | 0 | 0 | 0 | 0 | 0 | 1 | 0 | 1 |
| ACQ MEM 2 | 0 | 0 | 0 | 0 | 1 | 1 | 0 | 1 |
| PRC | 0 | 0 | 0 | 0 | 0 | 0 | 1 | 1 |

<!-- page 6-17 -->

### Table 6-11

**Diagnostic Messages**

| Access Group: Power-up | Access Group: Menu | Message |
|---|---|---|
| Power-up | | `PU : ROM/RAM/NMI : {hex_value}` |
| Power-up | | `MI : line stuck high`<br />`MI : Display controller : TIMEOUT`<br />`MI : Display controller : unable to reset mi` |
| | Menu | `SYS_ROM_0 : {actual_check_sum} {} {expected_check_sum}`<br />`SYS_ROM_1 : {actual_check_sum} {} {expected_check_sum}` |
| | Menu | `SYS_RAM : @ {address}` |
| | Menu | `NIB_RAM : @ {address}` |
| Power-up | Menu | `ACQ_AB : read-back {actual} {} {expected}` (this message may appear more than once) |
| Power-up | Menu | `ACQ_MEM : @ {address}` |
| Power-up | Menu | `PRC : read-back {actual} {} {expected}` (this message may appear more than once) |
| Power-up | Menu | `HS_ACQ : latent END_OF_RECORD`<br />`HS_ACQ : acq_mem cntr {mem_actual} {} {mem_expected}`<br />`HS_ACQ : prc {prc_actual} {} {prc_expected}`<br />`HS_ACQ : fill @ {fill_address} : {fill_actual} {} {fill_expected}` |
| Power-up | Menu | `TBD hs/2 : latent END_OF_RECORD`<br />`TBD hs/2 : acq_mem cntr {mem_actual} {} {mem_expected}`<br />`TBD hs/2 : prc {prc_actual} {} {prc_expected}`<br />`TBD hs/2 : fill @ {fill_address} : {fill_actual} {} {fill_expected}` |
| Power-up | Menu | `TBD ps/2 : latent END_OF_RECORD`<br />`TBD ps/2 : acq_mem cntr {mem_actual} {} {mem_expected}`<br />`TBD ps/2 : prc {prc_actual} {} {prc_expected}`<br />`TBD ps/2 : fill @ {fill_address} : {fill_actual} {} {fill_expected}` |
| Power-up | Menu | `TBD ps/5 : latent END_OF_RECORD`<br />`TBD ps/5 : acq_mem cntr {mem_actual} {} {mem_expected}`<br />`TBD ps/5 : prc {prc_actual} {} {prc_expected}`<br />`TBD ps/5 : fill @ {fill_address} : {fill_actual} {} {fill_expected}` |
| Power-up | Menu | `TBD ps/10 : latent END_OF_RECORD`<br />`TBD ps/10 : acq_mem cntr {mem_actual} {} {mem_expected}`<br />`TBD ps/10 : prc {prc_actual} {} {prc_expected}`<br />`TBD ps/10 : fill @ {fill_address} : {fill_actual} {} {fill_expected}` |
| Power-up | Menu | `TBD /10 : latent END_OF_RECORD`<br />`TBD /10 : acq_mem cntr {mem_actual} {} {mem_expected}`<br />`TBD /10 : prc {prc_actual} {} {prc_expected}`<br />`TBD /10 : fill @ {fill_address} : {fill_actual} {} {fill_expected}` |

<!-- page 6-18 -->

### Table 6-11 (cont)

| Access Group: Power-up | Access Group: Menu | Message |
|---|---|---|
| Power-up | Menu | `TBD /100 : latent END_OF_RECORD`<br />`TBD /100 : acq_mem cntr {mem_actual} {} {mem_expected}`<br />`TBD /100 : prc {prc_actual} {} {prc_expected}`<br />`TBD /100 : fill @ {fill_address} : {fill_actual} {} {fill_expected}` |
| Power-up | Menu | `TBD /1k : latent END_OF_RECORD`<br />`TBD /1k : acq_mem cntr {mem_actual} {} {mem_expected}`<br />`TBD /1k : prc {prc_actual} {} {prc_expected}`<br />`TBD /1k : fill @ {fill_address} : {fill_actual} {} {fill_expected}` |
| Power-up | Menu | `TBD /10k : latent END_OF_RECORD`<br />`TBD /10k : acq_mem cntr {mem_actual} {} {mem_expected}`<br />`TBD /10k : prc {prc_actual} {} {prc_expected}`<br />`TBD /10k : fill @ {fill_address} : {fill_actual} {} {fill_expected}` |
| Power-up | Menu | `TBD /100k : latent END_OF_RECORD`<br />`TBD /100k : acq_mem cntr {mem_actual} {} {mem_expected}`<br />`TBD /100k : prc {prc_actual} {} {prc_expected}`<br />`TBD /100k : fill @ {fill_address} : {fill_actual} {} {fill_expected}` |
| Power-up | Menu | `MM_ACQ : latent END_OF_RECORD`<br />`MM_ACQ : prc {prc_actual} {} {prc_expected}`<br />`MM_ACQ : acq_mem cntr {acq_mem_actual} {} {acq_mem_expected}`<br />`MM_ACQ : fill @ {fill_address} : {fill_actual} {} {fill_expected}` |
| Power-up | Menu | `XY_ACQ : latent END_OF_RECORD`<br />`XY_ACQ : prc {prc_actual} {} {prc_expected}`<br />`XY_ACQ : acq_mem cntr {acq_mem_actual} {} {acq_mem_expected}`<br />`XY_ACQ : fill @ {fill_address} : {fill_actual} {} {fill_expected}` |
| Power-up | Menu | `CDT : TIMED-OUT {tb_mode_reg_pattern}`<br />`CDT : PRE_DETRIG {tb_mode_reg_pattern}`<br />`CDT : uncaled : min = {min_actual}`<br />`CDT : uncaled : delta = {delta_actual}` |
| Power-up | Menu | `FP_A2D : cursor :a= {actual} b= {actual}`<br />`FP_A2D : gnd = {actual} {} 5`<br />`FP_A2D : TIME OUT` |

<!-- page 6-19 -->

**[Figure 6-4. Diagnostic Menu — menu tree diagram: SPECIAL FUNCTIONS > DIAGNOSTICS > TESTS, branching into ROM (SYS_ROM_0, SYS_ROM_1), RAM (SYSTEM/SYS_RAM, NIBBLE/NIB_RAM, ACQ_AB, ACQ_MEM), SYSTEM (ACQ_AB, PRC, HS_ACQ, TBD {range}, MM_ACQ, XY_ACQ, CDT, FP_A2D), and OPTION (ROM, RAM, IO) - each leaf shows its own `{message}` output format. Drawing number 4999-13.]**

The following sequence of events occurs during power-up:

Set up temporary interrupt vectors (single task).

Do the power-up (PU) Kernel tests (each sets a bit in a q buffer).

- ROM tests (Send error codes to U4113 and U4119 once for each detected error).
- RAM tests (Send error codes to U4113 and U4119 once for each detected error).
- Non maskable interrupt test (Send error codes to U4113 and U4119 once for each detected error).

Initialize system (two tasks: RAM refresh and diagnostics).

If the CURSORS SELECT C1/C2 button is pressed:

- Enable extended error display.
- Enable RS-232-C error reporting.

If a Menu/DISPLAY ON/OFF button is pressed:

- Enable RS-232-C error reporting.
- Do power-up calibration/diagnostic routines:
  - Rotate ones in control ports (OUT_PORTS).
  - Display the Box without maskable interrupt support (BOX).
  - Run Clock Delay Timer calibration routine (CAL_CLK_DLY).
  - Run Store/Nonstore Position Balance (CAL_V_POS).

Start building the power-up fault display.

- Generate text about PU test results found in PU Q buffer.

Do System Diagnostic tests:

- (when a failure is found, one line of text is generated for later display).
- Maskable interrupt test (MI).
- Acquisition memory address bus (ACQ mem access).
- Acquisition memory (ACQ MEM).
- Post record counter (PRC).
- High speed acquisition (HS ACQ).
- Time base divider (TBD).
- Min/Max acquisition (MM ACQ).
- X/Y acquisition (XY ACQ).
- Clock delay timer (CDT).
- Front panel A/D converter (FP A2D).

Information in Table 6-12 is used to set up the acquisitions used in diagnostic tests. Start Address and Post Record Start data is valid just before ACQENA goes TRUE. Timebase Mode Register, Timebase Divider Register, and Acquisition Mode Register data is valid while ACQENA is TRUE, and causes the Timebase Divider to divide by the Real Divide Ratio. Record Length is the length of the record being acquired. RECCLK Period is the time that ACQENA is TRUE before ENDREC goes TRUE. Fill Test Start is the first value of the data being acquired. Fill Delta (B/CNT) is the increment used to select succeeding data points from the Diagnostic Generator. Effected Sweep Speed is the sweep speed used for the acquisition.

<!-- page 6-20 -->

**[Figure 6-5. Error code timing (U4119) — timing diagram showing a signal starting at 00, rising to FF, holding for 0.7s, then a sequence of 0.7s-wide pulses labeled ERROR 1, ERROR 2, ERROR 3, ..., each returning to 00 between pulses, then rising back to FF/holding at the end. Drawing number 4999-15.]**

### Table 6-12

**Diagnostic Acquisition Values**

| Test | TB MOD REG U4119 | TB DIV REG U4113 | ACQ MOD REG U3310 | Real DIV Ratio | REC LEN | Start ADDR | Post REC Start | Fill Text Start | RECCLK Period | Fill Delta (B/CNT) | Effected SWP Speed |
|---|---|---|---|---|---|---|---|---|---|---|---|
| HS_ACQ | 1E | 00 | 85 | 1. | 260. | 0F9E | F4F | 0FA0 | 50 ns | 1. | 5 μs |
| TBD hs/2 | 1A | 00 | 85 | 2. | 24. | 0FFE | FDB | 0000 | 100 ns | 2. | 10 μs |
| TBD ps/2 | 15 | FC | 85 | 2. | 24. | 0FFE | FDB | 0000 | 200 ns | 2. | 20 μs |
| TBD ps/5 | 15 | FB | 85 | 5. | 24. | 0FFE | FDB | 0000 | 500 ns | 5. | 50 μs |
| TBD ps/10 | 15 | F8 | 85 | 10. | 24. | 0FFE | FDB | 0000 | 1 μs | 10. | 0.1 ms |
| TBD /10 | 15 | F5 | 85 | 10. | 24. | 0FFE | FDB | 0000 | 1 σ | 10. | 0.2-1 ms |
| TBD /100 | 15 | ED | 85 | 91. | 24. | 0FFE | FDB | 0000 | 9.1 μs | 91. | 2-10 ms |
| TBD /1k | 15 | DD | 85 | 901. | 24. | 0FFE | FDB | 0000 | 90.1 μs | 133. | 0.02-0.1 s |
| TBD /10k | 15 | BD | 85 | 9001. | 8. | 0FFE | FEB | 0000 | 900.1 μs | 41. | 0.2-1 s |
| TBD /100k | 15 | 7D | 85 | 90001. | 8. | 0FFE | FEB | 0000 | 9000.1 μs | 145. | 2-5 s |
| MM_ACQ | 16 | E4 | A5 | 200. | 32. | 0FFE | FD3 | 0000 | 100 ns | 199. or 255. | |
| XY_ACQ | 16 | FC | 8C | 2. | 16. | 0FFE | FE3 | 0000 | 100 ns | 1. or 3. | |
| CDT min | 8E | 00 | 8A | 1. | 4082. | 0FFE | 001 | n/a | 50 ns | n/a | |
| CDT max | 8F | 00 | 9A | 1. | 4082. | 0FFE | 001 | n/a | 50 ns | n/a | |

<!-- page 6-21 -->

If there were power-up faults:

Display the power-up faults on U4119.

Display the power-up faults on the crt without maskable interrupt support:

Until a Menu button is changed.

Start normal instrument operation.

### Diagnostic Tests

PU TEST. At power-up, this kernel test does a quick check of the instruments dynamic RAM (random access memory), ROM (read only memory), and NMI (non maskable interrupt) circuitry. If no errors are found, additional diagnostic tests are run.

If errors are found, their code is displayed (at power-up before NMI or MI go HI and before other tests are run) repeatedly, for approximately 2 sec, on U4113 and U4119 (see Table 6-13). The instrument also tries to display the errors on the crt as a four digit hexadecimal number:

`PU : ROM/RAM/NMI : {hex_value}`

For example: if ROM U9110, RAM U9232 and RAM U9231 fail, the instrument will:

1. Flash failure codes on U4113 and U4119:

```
PIN 12 ... PIN 19
0100 0010
1000 0001
1010 0101
```

2. If possible, display error message on the crt (see Figure 6-6):

**NOTE**

*More than one bad RAM usually means that something else is causing the problem.*

MI. The maskable interrupt (MI) diagnostic creates and displays a single dark vector display (low resolution). Then a /INT-RST (U9105 pin 11) is issued followed by a /FRAME (U9105 pin 10). The MI (INTR at U9111 pin 18) should then go TRUE until another /INT-RST is generated. All other MI sources are tested inherently by normal operation. The test sequence is:

```
Microprocessor (CPU) : pulse /INT-RST LO (U9105 pin 11, U9208 pin 10)

DSP : set INTR (U9208 pin 6) FALSE

CPU : check MI by enabling MIs (U9136 pin 8)

Generate fault message.

Pulse FRAME LO (U9105.10, U9208.7)

Enable MIs

DSP : set INTR (U9208 pin 8) TRUE

CPU : if time is too great

Generate fault messages

Disable MIs

CPU : pulse /INT-RST LO (U9105 pin 11, U9208 pin 10)

DSP : set INTR (U9208 pin 6) FALSE

CPU : check MI by enabling MIs (U9103G pin 8)

Generate fault messages

A fault generates one or more of the following error messages:

MI : line stuck high

MI : Display controller : TIMEOUT

MI : Display controller : unable to reset mi

SYS_ROM_n. SYS_ROM_n checks each ROM by calculating and then comparing its checksum to what is stored in the ROM.
```

If an error is found, the calculated value and the value expected are displayed on the crt:

`SYS_ROM_n : calculated_value {} expected_value`

**[Figure 6-6. PU error display — oscilloscope readout screen showing "POWER-UP FAILURES", "PRESS MENU KEYS TO CONTINUE", and "PU : ROM/RAM/NMI @ 00A2 (and any other error detected)" overlaid on a graticule. Drawing number 4999-16.]**

<!-- page 6-22 -->

### Table 6-13

**PU TEST Failure Codes**

| Failed Part | U4113 and U4119 Code: Pins 12/19 | Pins 13/18 | Pins 14/17 | Pins 15/16 | Crt Failure Code In Binary (from pu q) |
|---|---|---|---|---|---|
| ROM U9109 (E0000) | 0 | 0 | 1 | 0 | xxxx xxxx xxxx xxx1 |
| ROM U9110 (E8000) | 0 | 1 | 0 | 0 | xxxx xxxx xxxx xx1x |
| RAM U9203 | 0 | 1 | 0 | 1 | xxxx xxxx xxxx x1xx |
| RAM U9202 | 0 | 1 | 1 | 0 | xxxx xxxx xxxx 1xxx |
| RAM U9233 | 0 | 1 | 1 | 1 | xxxx xxxx xxx1 xxxx |
| RAM U9232 | 1 | 0 | 0 | 0 | xxxx xxxx xx1x xxxx |
| RAM U9201 | 1 | 0 | 0 | 1 | xxxx xxxx x1xx xxxx |
| RAM U9231 | 1 | 0 | 1 | 0 | xxxx xxxx 1xxx xxxx |
| NMI | 1 | 0 | 1 | 1 | xxxx xxx1 xxxx xxxx |

For example, if the calculated value is A4D2 and the value stored in the ROM is 23DA the following error message is displayed on the crt:

`SYS_ROM_1 : A4D2 {} 23DA`

SYS_RAM. This test checks the system RAM. The test writes a 0xAA55 into 100 bytes of display memory. It then checks the data to make sure that the data has not changed. The test is then repeated using 0x55AA.

**NOTE**

*Firmware version 01 only displays the address of the bad RAM.*

If an error is found, the address (greater than 0 but less than 8000) of the error, the actual data found at the address, and the data that was expected at that address are displayed on the crt:

`SYS_RAM : @ {address} {actual data} {} {expected data}`

For example, if the address of the bad cell is 0x4000, the data found at that address is 0x0F, and the expected data for that address is 0x4F the following error message is displayed on the crt:

`SYS_RAM : @ 4000 0F {} 4F`

NIB_RAM. This test checks the nibble RAM. The test procedure and the error message format are the same as for SYS_RAM.

ACQ_AB. This test checks the address bus of the acquisition memory. Twenty one unique patterns are written into the address counters (U3423 U3424 and U3425) and read back through the acquisition address buffers (U3427 U3428).

**NOTE**

*At power-up this test and all others are transient and not active at the time of the power-up failure messages.*

Push the SELECT C1/C2 switch to stop pattern changes. The test loops using the pattern for the first error found. All patterns used are shown in Table 6-14.

If an error is found, the value read back and the value expected are displayed on the crt:

`ACQ_AB : read-back {actual} {} {expected}`

**NOTE**

*The outputs of the Record Counters should be about 50% duty cycle square waves of 1.1 seconds duration.*

If the oscilloscope is operating in extended diagnostics mode, the error display is expanded to include all errors,

<!-- page 6-23 -->

not just the first error. Also, in extended diagnostics mode the RS-232-C Option can be used to send the error reports to a terminal or computer. This enables analysis of the data for pattern recognition. For example, if bit 5 (U3424 pin 15) is shorted to ground all patterns where bit 5 should be a one will have a zero in bit position 5 and therefore fail.

ACQ_MEM. This test checks the acquisition memory and it's microprocessor interface.

**NOTE**

*Software version 01 always claims address 0 is bad no matter what errors are actually found.*

Firmware version 01 fills the acquisition memory with a ramp and then checks to see if the values are correct. The value at each physical_address is the (physical_address-0x48000) mod 256. Firmware version 02 fills the acquisition memory with a checkerboard pattern of AA55 and 55AA and checks to see if the values are correct.

The microprocessor can not reliably write to acquisition memory without clobbering the adjacent byte (the microprocessor has byte wide memory). Each acquisition-memory device is tested separately (U3418 first and then U3419. An error message identifying a faulty address implies the faulty device via its address. An even address value implies that RAM (U3418) or transceiver (U3421) may be faulty. An odd address implies U3419 or U3422.

**NOTE**

*Firmware version 01 only displays the address of an error.*

If an error is found the address of the error, the actual data found at the address, and the data expected at the address are displayed on the crt:

`ACQ_MEM : odd @ {address} {actual data} {} {expected data}`

`ACQ_MEM : even @ {address} {actual data} {} {expected data}`

**NOTE**

*The displayed address is offset from 0x40000 (acq_mem_block) and is a 4 digit hexadecimal number between 0 and 4096.*

For example, if the address of an error is 48008, the actual data found at the address is F0, and the expected

<!-- page 6-24 -->

data at that address is F4, the following error message is displayed on the crt:

`ACQ_MEM : even @ 4008 F0 {} F4`

PRC. This test checks the Post Record Counter write and the B-TRIG read circuitry. Twenty four unique patterns are written into the Post Record Counter (U4115 U4116 and U4117) and read through the B Delay Timer (U4123 U4124).

The B Delay Timer is clocked by a write to the Time Base Divider register (U4114 pin 8 through U4107 pin 4), the inactive B-GATE (U4121B pin 11), and the TRGD (U4121B pin 13) signals.

Push the SELECT C1/C2 switch to stop pattern changes. The test loops using the pattern for the first error found. All patterns used are shown in Table 6-15.

**NOTE**

*If rec-clk is active unpredictable results occur.*

If an error is found, the value read back and the value expected are displayed on the crt:

`PRC : read-back {actual} {} {expected}`

For example, if the value read back is 008 and the value expected is 00F the following error message is displayed on the crt:

`PRC : read-back 008 {} 00f`

If the oscilloscope is operating in extended diagnostics mode, the error display is expanded to include all errors, not just the first error. Also, in extended diagnostics mode the RS-232-C Option can be used to send the error reports to a terminal or computer. This enables analysis of the data for pattern recognition. For example, if bit 5 (U4116 pin 15) is shorted to ground all patterns where bit 5 should be a one will have a zero in bit position 5 and therefore fail.

HS_ACQ. This test checks the High Speed Acquisition using a 260 byte acquisition at the fastest record speed (record clock = convert clock = 20 MHz = 50 ns per byte) sampling the Diagnostic Code generators.

<!-- page 6-25 -->

**NOTE**

*This is the only test which absolutely origins the fill; others only test the slope of the fill.*

Synchronous to NMI the Acquisition Address Counter (U3423, U3424, and U3425) is loaded with 0x1000 - 0x96 (0x0F6A) and the Post Record Counter (U4115, U4116,

and U4117) is loaded with 0xFF1 - 240 + 0x96 (0xF9F). The Time Base Divisor Register (U4113) is set to 0x00, the Acquisition Mode Register (U3310) is set to 0x85 and the Time Base Mode Register (U4119) is set to 0x1E. See Table 6-12 for more acquisition data.

To start the acquisition a 0x10 is ORed into the Time Base Mode Register (U4119), generating ACQENA TRUE synchronous to CONV CLK. Two activities are then done at the same time:

1. The microprocessor polls the Memory Address Buffer bit 16 (U3428 pin 9) (/ENDREC) 4000 times before aborting the second activity.

2. The acquisition runs asynchronous to the microprocessor.

CONV clock propagates through U4103B, U4125A, and U4125B becoming SAVECLK. CONV and SAVECLK propagate through U4104B, U3101A, U3105B, U3105A (becoming ACQWRITE), and U3417 to clock the data from the swap (Acquisition Buffer Sequencer) registers (U3236 and U3239) into the Acquisition Memory (U3418 and U3419) in 16-bit chunks. The signals from U3417 also clock the acquisition Address Counters (U3423, U3424, and U3425).

The microprocessor sets /TEST FALSE (U3310) disabling the DATA IN BUFFER (U3229). A LO /TEST causes the output of the DIAGNOSTIC CODE GENERATORS (U3230 and U3231) to be used instead of the A/D CONVERTER data.

The microprocessor uses the ACQUISITION MODE REGISTER (U3310) to tie MAXCLK and MINCLK (U3309 pin 7 and U3309 pin 9) to /EVENCLK and /ODDCLK (U3101B pin 8 and U3103B pin 8) respectively through U3309. /ODDCLK and /EVENCLK are 50% duty cycle complements of each other and have a period of two CONV clocks. This means that the MIN REGISTER is latched with a test value and 50 ns later the MAX REGISTER is latched with a value one greater. After another 25 ns the swap (Acquisition Buffer/Sequencer) registers (U3236, U3237, U3238, and U3239) latch a 16-bit word comprised of the output of the MIN REGISTER and the MAX REGISTER.

When the Acquisition Address Counter overflows PRE-FULL (U3425 pin 7) goes HI. This in turn makes STO RDY (U4226A pin 5) HI. CALTIMER (from U3310 pin 12) makes multiplexer U4227 select STO RDY and pass it through to U4227 pin 7. Convert clock (/CONV) then passes the signal through U4228A, U4127C, and U4226B making TRIGD (U4226B pin 9) HI. TRIGD enables the Post Record Counter to count at RECCLK (CONV clock) rates.

One RECCLK after the Post Record Counter reaches a hexadecimal count of FF0, U4105B creates /ENDREC (not end of record) LO. When the microprocessor finds /ENDREC LO, the values in the Acquisition Memory Address Counters (U3423, U3424, and U3425) and the Post Record Counter (U4115, U4116, and U4117) are analyzed. Then the Acquisition Memory is checked to see if it contains the proper values.

If an error is found, one of the following messages is displayed on the crt:

`HS_ACQ : latent END_OF_RECORD`

`HS_ACQ : acq_mem cntr {mem_actual} {} {mem_expected}`

`HS_ACQ : prc {prc_actual} {} {prc_expected}`

`HS_ACQ : fill @ {fill_address} : {fill_actual} {} {fill_expected}`

Where:

Latent END_OF_RECORD means the microprocessor polled for an /ENDREC 4000 times and never saw one.

Acq_mem cntr means the completion value of the Acquisition Memory Counter was not what was expected (see Table 6-12).

Prc means the completion value of the Post Record Counter was not what was expected (see Table 6-12).

Fill means the fill value at the indicated address was not what was expected (see Table 6-12).

Prc_actual, prc_expected, mem_actual and mem_expected are all 3 digit hexadecimal numbers.

Fill_address is a 4 digit hexadecimal number representing an offset from 0x48000 (start of Acquisition Memory).

Fill_actual and fill_expected are each 2 digit hexadecimal numbers.

TBD. This test checks the Time Base Divider string using nine different Time Base Divider test ranges (rng).


<!-- page 6-26 -->

An acquisition is run as in HS_ACQ except that U4103B selects an input that makes RECCLK a submultiple of CONV clock. As in HS_ACQ, /ENDREC is polled and the Post Record Counter and Acquisition Memory completion values are checked. Although the acquisition is similar to the HS ACQ acquisition, the fill is different (see Table 6-12).

**NOTE**

*See Table 6-12, (Diagnostic Acquisition Values) for specific signals, register values, and terms used in the following discussion.*

If the SELECT C1/C2 button is held in while the test is running, the test loops on the first error. If an error is detected, one of the following messages is displayed on the crt:

`TBD {rng} : {error}`

Where:

Rng is one of the following:

```
hs/2
ps/2
ps/5
ps/10
/10
/100
/1k
/10k
/100k
```

Error is one of the following:

latent END_OF_RECORD

prc {prc_actual} {} {prc_expected}

acq_mem cntr {mem_actual} {} {mem_expected}

fill @ {address} : {fill_actual} {} {fill_expected}

MM_ACQ. This test checks the acquisition circuitry as it relates to MIN/MAX.

**NOTE**

*This test also runs the XY_ACQ test.*

RECCLK is set using the Time Base Divider to 1/200th of the CONV clock. Then an acquisition is performed as in HS ACQ, /ENDREC is polled, and the Post Record Counter and Acquisition Memory Counter completion values are checked.

Fill testing starts at acq_mem address 0000. The fill is tested for max (odd) byte minus min (even) byte to give either 255 or 200.

**NOTE**

*The error message values are as in HS_ACQ except for the fill values.*

If an error is found, one of the following messages is displayed on the crt:

MM_ACQ : latent END_OF_RECORD

MM_ACQ : prc {prc_actual} {} {prc_expected}

MM_ACQ : acq_mem cntr {acq_mem_actual} {} {acq_mem_expected}

MM_ACQ : fill @ {fill_address} : {fill_actual} {} {fill_expected}

XY_ACQ. This test checks the acquisition circuitry as it relates to X-Y.

**NOTE**

*This test has no menu entry, however it is run by MM_ACQ.*

As in HS_ACQ, an acquisition is performed, /ENDREC is polled, and the Post Record Counter and Acquisition Memory Counter completion values are checked. The fill is tested for n, n+1, n+4,n+5, n+8, n+9, ... starting at Acquisition Memory address 0000.

**NOTE**

*The test and the error message values are as in HS_ACQ except for the fill values.*

If an error is found, one of the following messages is displayed on the crt:

`XY_ACQ : latent END_OF_RECORD`

`XY_ACQ : prc {prc_actual} {} {prc_expected}`

`XY_ACQ : acq_mem cntr {acq_mem_actual} {} {acq_mem_expected}`

`XY_ACQ : fill @ {fill_address} : {fill_actual} {} {fill_expected}`

CDT. This test checks the Clock Delay Timer. The CDT (clock delay timer) is a dual-slope integrator used to

<!-- page 6-27 -->

measure the time between an asynchronous trigger (either the A or the B Gate) and the acquisition systems master clock. The timer divides the 50 ns convert clock (CONV) into 200 time periods.

The CDT diagnostic checks the Clock Delay Timer circuit using two self-triggered acquisitions. Each test acquisition is started when the microprocessor sets CAL-TIMER (U4247 pin 2) TRUE and /TEST (U4228 pin 10) is set first LO and then HI. When PREFULL (U4228 pin 2) goes HI, U4127 pin 4 goes TRUE causing the charge cycle of the CDT (C4201) to start. The discharge cycle begins 100 to 150 ns later when TRGD goes TRUE forward biasing Q4203.

The time that the voltage on C4201 is above the voltage at U4229 pin 2 (set by R4214, R4215 and R4216) during the discharge cycle is proportional to the time difference between U4127 pin 4 going HI and TRGD (U4226 pin 9) going TRUE. This time is counted by U4230 (at the CONV clock rate) and U4231B. The MSB of the CDT word (bit 8) is shared with BYTEINT (the hardware flag signifying that a byte interrupt has occurred). This shared bit is read by the microprocessor through U3428 pin 8.

If an error is found, one of the following messages is displayed on the crt:

`CDT : TIME-OUT {tb_mode_reg_pattern}`

`CDT : PRE-DETRIG {tb_mode_reg_pattern}`

`CDT : uncaled : min = {min_actual}`

`CDT : uncaled : delta = {delta_actual}`

Where:

TIME-OUT is caused by not receiving a /ENDREC.

Tb_mode_reg_pattern is a 2-digit hexadecimal value indicating the pattern used in the Time Base Mode Register during the test acquisition.

PRE-DETRIG is caused by the CDT counter overflowing (CNTCLR U4231 pin 6).

Tb_mode_reg_pattern is a 2-digit hexadecimal value indicating the pattern used in the Time Base Mode Register during the test acquisition.

Min_actual is the value (85.0 to 115.0) read from U4230 + CDT msb (U3428 pin 8) during a test acquisition with /TEST LO.

Delta_actual is the value (200 to 210) read from U4230 + CDT msb (U3428 pin 8) during a test acquisition with /TEST HI minus the value of the previous min cycle.

FP_A2D. This test checks the front panel A/D converter circuitry. A conversion is done on three of the analog inputs (A CURS, U6106 pin 12, B CURS, U6106 pin 13, and ground, U6108 pin 5). The algebraic sum of A CURS and B CURS are checked. Their sum should be between 0x100 and 0x700. Ground is also checked. It should be between 0 and 5 front panel A/D converter counts (5 / 1024 of VREF).

During power-up this test defines a variable (FP_POLLED) that controls how the microprocessor works with the front panel. If during testing a MI is not generated, it is assumed that the front panel will never generate a MI and the microprocessor must poll the front panel to see when to transfer front-panel data.

If an error is found one of the following messages is displayed on the crt:

**NOTE**

*In firmware version 02, the Gnd message should be FP_A2D : gnd = {actual} } 5 (greater than only)*

`FP_A2D : cursor :a= {actual} & b= {actual}`

`FP_A2D : gnd = {actual} {} 5`

`FP_A2D : TIME-OUT`

Where:

Actual is a 3-digit hexadecimal number representing the result of a front-panel digitization.

TIME-OUT indicates A/D INT FLAG (U6101D pin 13) did not occur within 0x800 polls by the microprocessor.

CAL_AIDS. The instrument calibration aids are used to help calibrate the instrument.

CAL_V_POS. This calibration aid is used to calibrate the storage position control (see "Adjustment Procedure").

CAL_CLK_DLY. Clock Delay Timer (CDT) calibration uses a graphic display. The horizontal position of the display cross hairs is attached to the min count and the vertical position is attached to the delta count (see "Adjustment Procedure").

**NOTE**

*Only BOX and OUT_PORTS is run by version 01 software.*

<!-- page 6-28 -->

CAL_PU. Depressing one of the Menu Select/DISPLAY ON/OFF controls during power-up runs four calibration routines, BOX, OUT_PORTS, CAL_CLK_DLY, and the Storage Acquisition Offset. Each routine is run until one of the menu buttons is again pushed. The BOX and OUT-PORTS routines are run at the same time. Each routine is used to adjust the instrument (see "Adjustment Procedure") except for OUT_PORTS. OUT_PORTS is used to check instrument circuitry (see OUT_PORTS).

BOX. This exerciser displays a box (rectangle) on the crt. Two places in the Menu generate the Box. Gains and offsets of the storage display system integrators are set using the Box display (see the "Adjustment Procedure"). The Display Controller (U9208) is synchronously stimulated (at a multiple of NMI) to display the box not using MIs.

CIRCLE. A high resolution circle is displayed on the crt by this exerciser. This is the only diagnostic that uses all 10 bits of the display DACs (U9210 and U9220).

FP_VALUES. Raw internal front-panel data is displayed on the crt by this exerciser. Table 6-16 shows the display format, and Table 6-17 shows the bit definitions for the display.

**NOTE**

*Digital data is intensified when a control is changed. All other data is intensified if the data has changed more than 5 counts since the last display update.*

### Exercisers

Instrument exercisers are used to aid in the repair of the instrument.

CONFIGURATION. This exerciser lists the ROM part numbers used in the instrument and the options installed in the instrument.

**Table 6-16**

**Display Format**

| Data | Signal Names | | | |
|---|---|---|---|---|
| Digital | AD DATA (R6101) | ISTAT (U6103) | SWB1 (U9302) | SWB2 (U9301) |
| Cursors | A CURSOR | B CURSOR | B DELAY | |
| CH 1 | E114 E115 | CH1 ATT | CH1 STAT | CH1 PROBE |
| CH 2 | E164 E165 | CH2 ATT | CH2 STAT | CH2 PROBE |
| A Sweep | ARES1 | ARES2 | | |
| B Sweep | B RES | B CAPS | | |
| Ground | GROUND | | | |

**Table 6-17**

**Display Format Bit Definitions**

| Signal Names | Displayed Bit Positions 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 |
|---|---|---|---|---|---|---|---|---|
| AD DATA | | | CH2 INV | T MAG | PP | /TRL | SS RST | /XY |
| ISTAT | /VALT | SGL SWP | AC1 | AC2 | /CH1 SEL | /CH2 SEL | /CHOP | A/D INT FLAG |
| SWB1 | STORE ON | /B ONLY | HOLD | ROLL | HOR MAG | HOR CAL | PRE/POST | /A ONLY |
| SWB2 | SELECT C1/C2 | MENU ADV | MEM 2 | MENU | 1K/4K | POS/SEL | MEM 1 | MEM 3 |

<!-- page 6-29 -->

OUT_PORTS. All microprocessor output ports of the instrument are exercised by this exerciser. If entered from the menu, rotating the cursor knob selects either a single port or all ports at once. If entered from power-up, the exerciser is run with the box display. Test patterns used in each port are shown in Table 6-18 through Table 6-22.

**NOTE**

*The ones and zeros patterns are observed using an LED dip clip on the registers. The pattern seen on Address Counters U3423, U3424, and U3425 (U3427 and U3428) will occasionally have other data superimposed upon it.*

**Table 6-18**

**ACQ_MEM 0x48000**

| U3427 Pins 2 3 4 5 6 7 8 9 | U3428 Pins 2 3 4 5 6 7 8 9 |
|---|---|
| 0 0 0 x 0 0 0 0 | 0 0 0 0 x x x x |
| 0 0 1 x 0 0 0 0 | 0 0 0 0 x x x x |
| 0 1 0 x 0 0 0 0 | 0 0 0 0 x x x x |
| 1 0 0 x 0 0 0 0 | 0 0 0 0 x x x x |
| 0 0 0 x 1 0 0 0 | 0 0 0 0 x x x x |
| 0 0 0 x 0 1 0 0 | 0 0 0 0 x x x x |
| 0 0 0 x 0 0 1 0 | 0 0 0 0 x x x x |
| 0 0 0 x 0 0 0 1 | 0 0 0 0 x x x x |
| 0 0 0 x 0 0 0 0 | 1 0 0 0 x x x x |
| 0 0 0 x 0 0 0 0 | 0 1 0 0 x x x x |
| 0 0 0 x 0 0 0 0 | 0 0 1 0 x x x x |
| 0 0 0 x 0 0 0 0 | 0 0 0 1 x x x x |
| 0 0 0 x 0 0 0 0 | 0 0 0 0 x x x x |
| 0 0 1 x 0 0 0 0 | 0 0 0 0 x x x x |

**Table 6-19**

**ACQ_MODE 0x437BE**

| U3310 Pins 2 3 4 5 6 7 8 9 |
|---|
| 0 0 0 0 0 0 0 0 |
| 1 0 0 0 0 0 0 1 |
| 0 1 0 0 0 0 1 0 |
| 0 0 1 0 0 1 0 0 |
| 0 0 0 1 1 0 0 0 |
| 0 0 0 0 0 0 0 0 |
| 1 0 0 0 0 0 0 1 |

**Table 6-20**

**TB_MODE 0x407DE**

| U4119 Pins 2 3 4 5 6 7 8 9 |
|---|
| 0 0 0 0 0 0 0 0 |
| 0 0 0 1 1 0 0 0 |
| 0 0 1 0 0 1 0 0 |
| 0 1 0 0 0 0 1 0 |
| 1 0 0 0 0 0 0 1 |
| 0 0 0 0 0 0 0 0 |
| 0 0 0 1 1 0 0 0 |

**Table 6-21**

**TB_SWP_RATE 0x407EE**

| U4113 Pins 2 3 4 5 6 7 8 9 |
|---|
| 0 0 0 0 0 0 0 0 |
| 0 0 0 1 1 0 0 0 |
| 0 0 1 1 1 1 0 0 |
| 0 1 1 1 1 1 1 0 |
| 1 1 1 1 1 1 1 1 |
| 0 0 0 0 0 0 0 0 |
| 0 0 0 1 1 0 0 0 |

**Table 6-22**

**FP_A/D_CTL 0x437F6**

| U6104 Pins 2 3 4 5 6 7 8 9 |
|---|
| 1 1 1 1 1 1 1 1 |
| 1 1 0 1 1 1 1 1 |
| 1 1 0 0 1 1 1 1 |
| 1 1 0 0 0 1 1 1 |
| 1 1 0 0 0 0 1 1 |
| 1 1 0 0 0 0 0 1 |
| 1 1 0 0 0 0 0 0 |
| 0 1 0 0 0 0 0 0 |
| 0 0 0 0 0 0 0 0 |
| 1 1 1 1 1 1 1 1 |
| 1 1 0 1 1 1 1 1 |

<!-- page 6-30 -->

INPUT_PORTS. This exerciser displays the input data for all microprocessor input ports. An explanation of the displayed data is shown in Table 6-23.

A_TO_D_TESTS/SAMPLES. This exerciser sets the number of acquisitions used to test the A/D Converter.

Turn the CURSORS control to select the number of 4096 byte acquisitions (a power of 2 is best) used to test the A/D Converter (see LINEARITY exerciser).

A_TO_D_TESTS/LINEARITY. This exerciser tests the acquisition A/D converter for missing bits.

Inject a highly linear 11 division vertically centered, (2 x 4096 x 50) ns duration triangle wave signal into the CH 1 or X input. Set the trigger so that the oscilloscope triggers close to the negative peak.

If any codes are missing in samples x 4096 acquisitions a message indicating that there were missing codes is displayed (and sent to the communications option if operating in extended diagnostics).

Setting the trigger point close to the positive peak tests negative going conversions.

The display is a histogram with a vertical scale of 5 x samples x (codes converted) per division and a horizontal scale of 0 to 255 codes across 10 divisions left to right (25.5 codes per division).

COM_OPTION/DEBUG. This exerciser is used in debuging the communications option. Debug outputs a test message and displays any incoming messages (data) on the crt.

PICTURES. The picture exercisers use line drawings to exercise the instruments display system. The Tekbug is a line drawing of the Tektronix symbol. The Wizard is a multi function display. The gain of the display controller is controlled by the CURSORS control. The position of the display is controlled by the CH 1 and CH 2 POSITION controls. If the VERTICAL POSITION and CURSORS controls are not turned for about 5 seconds, the display is automatically moved through its gain (CURSORS) and POSITION ranges.

**Table 6-23**

**Display Format Digit Definitions**

| Input Port Name | U Number | Crt Name | Number of Bits Displayed |
|---|---|---|---|
| Acquisition Address Buffer | U3427 and U3428 | ACQ_ADDR_BUF | 16 |
| Clock Delay Register | U4230 | CLK_DELAY_REG | 8 |
| B Delay Timer | U4123 and U4124 | B_DELAY_TIMER | 12 |
| Front Panel Instrument Status | U6103 | FP_INSTAT | 8 |
| Front Panel Address Data | U6102 | FP_AD_DATA | 8 |

## CORRECTIVE MAINTENANCE

<!-- page 6-31 -->

### INTRODUCTION

Corrective maintenance consists of component replacement and instrument repair. This part of the manual describes special techniques and procedures required to replace components in this instrument. If it is necessary to ship your instrument to a Tektronix Service Center for repair or service, refer to the "Repackaging" information in Section 2 of this manual.

### MAINTENANCE PRECAUTIONS

To reduce the possibility of personal injury or instrument damage, observe the following precautions.

1. Disconnect the instrument from the ac-power source before removing or installing components.

2. Verify that the line-rectifier filter capacitors are discharged prior to performing any servicing.

3. Use care not to interconnect instrument grounds which may be at different potentials (cross grounding).

4. When soldering on circuit boards or small insulated wires, use only a 15-watt, pencil-type soldering iron.

### OBTAINING REPLACEMENT PARTS

Most electrical and mechanical parts can be obtained through your local Tektronix Field Office or representative. However, many of the standard electronic components can usually be obtained from a local commercial source. Before purchasing or ordering a part from a source other than Tektronix, Inc., please check the "Replaceable Electrical Parts" list for the proper value, rating, tolerance, and description.

**NOTE**

*Physical size and shape of a component may affect instrument performance, particularly at high frequencies. Always use direct-replacement components, unless it is known that a substitute will not degrade instrument performance.*

#### Special Parts

In addition to the standard electronic components, some special parts are used in the instrument. These components are manufactured or selected by Tektronix, Inc. to meet specific performance requirements, or are manufactured for Tektronix, Inc. in accordance with our specifications. The various manufacturers can be identified by referring to the "Cross Index-Manufacturer's Code number to Manufacturer" at the beginning of the "Replaceable Electrical Parts" list. Most of the mechanical parts used in this instrument were manufactured by Tektronix, Inc. Order all special parts directly from your local Tektronix Field Office or representative.

#### Ordering Parts

When ordering replacement parts from Tektronix, Inc., be sure to include all of the following information:

1. Instrument type (include all modification and option numbers).

2. Instrument serial number.

3. A description of the part (if electrical, include its full circuit component number).

4. Tektronix part number.

#### Selectable Components

Several components in the instrument are selectable to obtain optimum circuit operation. Value selection of these components is done during the initial factory adjustment procedure. Usually, further selection is not necessary for subsequent adjustments unless a component has been changed that affects circuitry for which a selected component has been specifically chosen.

### MAINTENANCE AIDS

The maintenance aids listed in Table 6-24 include items required for performing most of the maintenance procedures in this instrument. Equivalent products may be substituted for those given, provided their characteristics are similar.

<!-- page 6-32 -->

**Table 6-24**

**Maintenance Aids**

| Description | Specification | Usage | Example |
|---|---|---|---|
| 1. Soldering Iron | 15 to 25 W. | General soldering and unsoldering. | Antex Precision Model C. |
| 2. Torx Screwdriver Tips and Handle | Torx tips: #T7, #T9, #T10, #T15, and #T20. Handle: 1/4 inch hex drive. | Assembly and disassembly. | Tektronix Part Numbers: #T7 003-1293-00, #T9 003-0965-00, #T10 003-0814-00, #T15 003-0966-00, #T20 003-0866-00. Handles: 8 1/2 in. 003-0293-00, 3 1/2 in. 003-0445-00. |
| 3. Nutdrivers | 1/4 inch, 5/16 inch, 1/2 inch, and 9/16 inch. | Assembly and disassembly. | Xcelite #8, #10, #16, and #18. |
| 4. Open-end Wrench | 9/16 inch and 1/2 inch. | Channel Input and Ext Trig BNC Connectors. | Tektronix Part Numbers: 9/16 003-0502-00, 1/2 003-0822-00. |
| 5. Hex Wrenches | 0.050 inch, 1/16 inch. | Assembly and disassembly. | Allen Wrenches. |
| 6. Long-nose Pliers | | Component removal and replacement. | Diamalloy Model LN55-3. |
| 7. Diagonal Cutters | | Component removal and replacement. | Diamalloy Model M554-3. |
| 8. Vacuum Solder Extractor | No static charge retention. | Unsoldering static sensitive devices and components on multilayer boards. | Pace Model PC-10. |
| 9. Contact Cleaner | No-Noise R. | Switch and pot cleaning. | Tektronix Part Number 006-0442-02. |
| 10. Pin-Replacement Kit | | Replace circuit board connector pins. | Tektronix Part Number 040-0542-01. |
| 11. IC-Removal Tool | | Removing DIP IC packages. | Augat T114-1. |
| 12. Isopropyl Alcohol | Reagent grade. | Cleaning attenuator and front panel assemblies. | 2-Isopropanol. |
| 13. Isolation Transformer | | Isolate the instrument from the ac power source for safety. | Tektronix Part Number 006-5953-00. |
| 14. 1X Probe | | Power supply ripple check. | TEKTRONIX P6101A. |
| 15. Bayonet Ground Assembly | | Signal interconnect for power supply ripple check. | Tektronix Part Number 013-0085-00. |
| 16. LED Dip Clip | | Troubleshooting. | HP 548A. |

REV MAY 1987

<!-- page 6-33 -->

### INTERCONNECTIONS

Interconnections in this instrument are made with pins soldered onto the circuit boards. Several types of mating connectors are used for the interconnecting pins. The following information provides the replacement procedures for the various type connectors.

#### End-Lead Pin Connectors

Pin connectors used to connect the wires to the interconnect pins are factory assembled. They consist of machine-inserted pin connectors mounted in plastic holders. If the connectors are faulty, the entire wire assembly should be replaced.

#### Multipin Connectors

When pin connectors are grouped together and mounted in a plastic holder, they are removed, reinstalled, or replaced as a unit. If any individual wire or connector in the assembly is faulty, the entire cable assembly should be replaced. To provide correct orientation of a multipin connector, an index arrow is stamped on the circuit board, and either a matching arrow is molded into or the numeral 1 is marked on the plastic housing as a matching index. Be sure these index marks are aligned with each other when the multipin connector is reinstalled (see Figure 6-1).

### TRANSISTORS AND INTEGRATED CIRCUITS

Transistors and integrated circuits should not be replaced unless they are actually defective. If removed from their sockets or unsoldered from the circuit board during routine maintenance, return them to their original board locations. Unnecessary replacement or transposing of semiconductor devices may affect the adjustment of the instrument. When a semiconductor is replaced, check the performance of any circuit that may be affected.

Any replacement component should be of the original type or a direct replacement. Bend transistor leads to fit their circuit board holes, and cut the leads to the same length as the original component. See Figure 9-2 in the "Diagrams" section for lead-configuration illustrations.

The chassis-mounted power supply transistor is insulated from the chassis by a heat-transferring mounting block. Reinstall the mounting block and bushings when replacing these transistors. Use a thin layer of heat-transferring compound between the insulating block and chassis when reinstalling the block.

**NOTE**

*After replacing a power transistor, check that the collector is not shorted to the chassis before applying power to the instrument.*

To remove socketed dual-in-line packaged (DIP) integrated circuits, pull slowly and evenly on both ends of the device. Avoid disengaging one end of the integrated circuit from the socket before the other, since this may damage the pins.

To remove a soldered DIP IC when it is going to be replaced, clip all the leads of the device and remove the leads from the circuit board one at a time. If the device must be removed intact for possible reinstallation, do not heat adjacent conductors consecutively. Apply heat to pins at alternate sides and ends of the IC as solder is removed. Allow a moment for the circuit board to cool before proceeding to the next pin.

### SOLDERING TECHNIQUES

The reliability and accuracy of this instrument can be maintained only if proper soldering techniques are used to remove or replace parts. General soldering techniques, which apply to maintenance of any precision electronic equipment, should be used when working on this instrument.

**WARNING**

*To avoid an electric-shock hazard, observe the following precautions before attempting any soldering: turn the instrument off, disconnect it from the ac power source, and wait at least three minutes for the line-rectifier filter capacitors to discharge.*

Use rosin-core wire solder containing 63% tin and 37% lead. Contact your local Tektronix Field Office or representative to obtain the names of approved solder types.

When soldering on circuits boards or small insulated wires, use only a 15-watt, pencil-type soldering iron. A higher wattage soldering iron may cause etched circuit conductors to separate from the board base material and melt the insulation on small wires. Always keep the soldering-iron tip properly tinned to ensure best heat transfer from the iron tip to the solder joint. Apply only enough solder to make a firm joint. After soldering, clean the area around the solder connection with an approved flux-removing solvent (such as isopropyl alcohol) and allow it to air dry.

<!-- page 6-34 -->

Circuit boards in this instrument may have many conductive layers. Conductive paths between the top and bottom board layers may connect to one or more inner layers. If any inner-layer conductive path becomes broken due to poor soldering practices, the board becomes unusable and must be replaced. Damage of this nature can void the instrument warranty.

**CAUTION**

*Only an experienced maintenance person, proficient in the use of vacuum-type desoldering equipment should attempt repair of any circuit board in this instrument.*

Desoldering parts from multilayer circuit boards is especially critical. Many integrated circuits are static sensitive and may be damaged by solder extractors that generate static charges. Perform work involving static-sensitive devices only at a static-free work station while wearing a grounded antistatic wrist strap. Use only an antistatic vacuum-type solder extractor approved by a Tektronix Service Center.

**CAUTION**

*Attempts to unsolder, remove, and resolder leads from the component side of a circuit board may cause damage to the reverse side of the circuit board. The following techniques should be used to replace a component on a circuit board:*

1. Touch the vacuum desoldering tool to the lead at the solder connection. Never place the iron directly on the board; doing so may damage the board.

**NOTE**

*Some components are difficult to remove from the circuit board due to a bend placed in the component leads during machine insertion. To make removal of machine-inserted components easier, straighten the component leads on the reverse side of the circuit board.*

2. When removing a multipin component, especially an IC, do not heat adjacent pins consecutively. Apply heat to the pins at alternate sides and ends of the IC as solder is removed. Allow a moment for the circuit board to cool before proceeding to the next pin.

**CAUTION**

*Excessive heat can cause the etched circuit conductors to separate from the circuit board. Never allow the solder extractor tip to remain at one place on the board for more than three seconds. Solder wick, spring-actuated or squeeze-bulb solder suckers, and heat blocks (for desoldering multipin components) must not be used. Damage caused by poor soldering techniques can void the instrument warranty.*

3. Bend the leads of the replacement component to fit the holes in the circuit board. If the component is replaced while the board is installed in the instrument, cut the leads so they protrude only a small amount through the reverse side of the circuit board. Excess lead length may cause shorting to other conductive parts.

4. Insert the leads into the holes of the board so that the replacement component is positioned the same as the original component. Most components should be firmly seated against the circuit board.

5. Touch the soldering iron to the connection and apply enough solder to make a firm solder joint. Do not move the component while the solder hardens.

6. Cut off any excess lead protruding through the circuit board (if not clipped to the correct length in step 3).

7. Clean the area around the solder connection with an approved flux-removing solvent. Be careful not to remove any of the printed information from the circuit board.

### REMOVAL AND REPLACEMENT INSTRUCTIONS

The exploded view drawings in the "Replaceable Mechanical Parts" list (Section 9) may be helpful during the removal and reinstallation of individual subassemblies or components. Circuit board and component locations are shown in the "Diagrams" section.

#### Cabinet

**WARNING**

*To avoid electric shock, disconnect the instrument from the ac-power-input source before removing or replacing any component or assembly.*

<!-- page 6-35 -->

To remove the instrument cabinet, perform the following steps:

**NOTE**

*For instruments with a power-cord securing clamp; remove the Phillips-head screw holding the power-cord securing clamp before disconnecting the power cord.*

1. Disconnect the power cord from the instrument.

2. Remove two screws, one each from the right-rear side and bottom front of the cabinet.

3. Remove two screws from the rear panel (located on each side) and remove it from the instrument.

4. Remove four screws from the left rear side of the cabinet securing the side panel to the instrument side chassis.

5. Remove the side panel from the instrument.

6. Pull the front panel and attached chassis forward and out of the cabinet.

**NOTE**

*To ensure that the cabinet is properly grounded to the instrument chassis, the screws at the right-rear side and the bottom front of the cabinet must be tightly secured.*

7. To reinstall the cabinet, perform the reverse of the preceding steps. Ensure that the cabinet is flush with the rear of the chassis and that the cabinet and rear-panel holes are align with the screw holes in the chassis frame.

#### Storage Circuit Board in Servicing Position

The following procedure describes how to secure the Storage circuit board into the servicing position to facilitate instrument disassembly and reinstallation for individual components or subassemblies.

1. Remove the five MEMORY buttons, SELECT WAVEFORM button, four ACQUISITION buttons, STORE button, and extension shafts from their respective switches by inserting a small screwdriver between the extension shaft and the switch shaft. Push down and forward until the extension shaft is disengaged and pull the shafts straight back through the front panel.

2. Disconnect the following two connectors from the Storage circuit board.

a. P2111, a four-wire connector located near the middle left edge of the Storage circuit board.

b. P2112, a four-wire connector located near the middle left edge of the Storage circuit board.

3. Remove three Storage circuit board screws that are identified by the etched words "Remove To Lift Board" (see Figure 6-7 for the location of the Storage board three screws).

4. Lift the Storage circuit board up until the cable of P9430 (on the front edge of the Storage circuit board) clears the back of the CURSORS control.

5. Remove P9430, a six-wire connector from the Storage circuit board by pulling it toward the front panel.

6. Continue to raise up the Storage circuit board to it standing position ensuring that the Board Latch clears the top of the chassis side rail. Place the Board Latch tab in the chassis side rail slot.

To lower the Storage circuit board into the instrument and to reconnect the connectors, perform the reverse of the preceding steps.

#### Support Chassis

The support chassis divides the inside of the instrument into two parts by connecting the center of the rear chassis and the front chassis together. The support chassis can be removed and reinstalled as follows:

1. Perform the "Storage Circuit Board in Servicing Position" removal procedure.

2. Remove the crt anode lead and High-Voltage Multiplier lead connectors from the anode clip on the Power-Supply shield.

3. Remove the anode clip from the Power-Supply shield through the hole in the support chassis.

<!-- page 6-36 -->

4. Remove the two recessed screws from the rear chassis (located directly above the Z-AXIS connector) securing the support chassis.

5. Remove the three screws securing the top attenuator shield to the support chassis.

6. Slide the front of the support chassis toward the center and over the top attenuator shield away from underneath the front chassis bracket.

7. Remove the support chassis from the instrument.

To reinstall the support chassis, perform the reverse of the preceding steps.

#### Side-Chassis Assembly

The Side-Chassis Assembly can be removed and reinstalled as follows:

1. Disconnect the following three connectors from the Side-Chassis Assembly.

a. P4110, a two-wire connector located at the rear of the Side-Chassis Assembly.

b. P6423, a four-wire connector located at the rear of the Side-Chassis Assembly.

c. P9301, a five-wire connector located at the rear of the Side-Chassis Assembly.

2. Remove two screws and ground clip from the top of the side chassis and two screws from the bottom of the side chassis that secures the Side-Chassis Assembly to the instrument.

3. Remove the Side-Chassis Assembly from the instrument.

To reinstall the Side-Chassis Assembly, perform the reverse of the preceding steps.

#### Storage Circuit Board

The Storage circuit board can be removed and reinstalled as follows:

1. Perform the "Storage Circuit Board in Servicing Position" removal procedure.

2. Perform the "Support Chassis" removal procedure.

3. Perform the "Side-Chassis Assembly" removal procedure.

4. Remove the ground clip near the center edge of the Storage chassis (towards the instrument).

5. Unsolder the strap from the ground clip near the center of the Storage chassis and slide the strap through the slot in the chassis when removing the Storage chassis from the instrument in step 7.

6. Remove the four circuit board shield screws from the Storage circuit board (see Figure 6-7 for location of the four circuit board shield screws). Remove the two screws located on top of the Storage circuit board last.

7. Remove the Storage chassis from the instrument by lifting it up out of the bracket spacer. See Figure 6-7 for location of the bracket spacer.

8. Disconnect the following eight connectors from the inside of the instrument. Note cable color, location, and routing for reinstallation reference.

a. P4210, a four-wire connector located on the Main circuit board behind the CH 2 VOLTS/DIV switch.

b. P4220, a two-wire connector located on the right side of the Alternate Sweep circuit board.

c. P9010, a nine-wire connector located on the right side of the Main circuit board between the Timing and Alternate Sweep circuit boards.

d. P9050, a single white-wire connector located between the Alternate Sweep circuit board and the Power-Supply shield.

e. P9060, a single black-wire connector located between the Alternate Sweep circuit board and the Power-Supply shield.

f. P9210, a seven-wire connector located on the Main Board underneath the CRT shield near the delay line.

h. P9320, a four-wire connector located on the front edge of the Main circuit board between the Attenuator and Position Interface circuit boards.

i. P9410, a seven-wire connector located on the Sweep Referenced circuit board.

<!-- page 6-37 -->

**[Figure 6-7: Exploded-view line drawing of the instrument (front panel/crt end), labeling "REMOVE THREE STORAGE BOARD SCREWS," "REMOVE FOUR CIRCUIT BOARD SPACERS," "REMOVE FOUR CIRCUIT BOARD SHIELD SCREWS," and "BRACKET SPACER" with leader lines pointing to their locations on the chassis. Drawing number 4999-37.]**

Figure 6-7. Location of screws and spacers on the Storage circuit board.

<!-- page 6-38 -->

9. With one hand firmly holding the Input/Output and Vector Generator circuit board assembly and with the other hand use a long-nose pliers on the top side to squeeze and push the four circuit board spacers through the holes in the Storage circuit board (see Figure 6-7 for location of the circuit board spacers). Place the Input/Output and Vector Generator circuit board assembly inside the instrument temporarily to be reinstalled later.

10. Release the Board Latch and lower the Storage circuit board into the instrument.

11. Disconnect the ribbon connector (P6100) from the Input/Output and Vector Generator circuit board assembly.

12. Remove the Storage circuit board EMI clip from the side chassis rail located behind the front hinge.

13. Remove both the recessed screw and the chassis mounted rear hinge nearest to the Board Latch from the instrument (see Figure 6-8 for removal of the chassis recessed screw and hinge).

14. Slide the Storage circuit board back until the front and middle hinges separate and lift it out of the instrument. Ensure that P6100 is free from the Storage circuit board and the chassis rail.

**NOTE**

*When installing the circuit board shield ensure that the black spacer tabs and the circuit board bracket are aligned with their respective holes in the shield. Also ensure that the strap (unsoldered in step 5) from the Input/Output circuit board is inserted through the circuit board shield slot to be resoldered to the ground clip.*

To reinstall the Storage circuit board, perform the reverse of the preceding steps.

#### Input/Output and Vector Generator Circuit Boards Assembly

The Input/Output and Vector Generator circuit boards assembly can be removed and reinstalled as follows:

1. Perform the "Storage Circuit Board in Servicing Position" removal procedure.

2. Remove the circuit board shield (see "Storage Circuit Board" removal procedure steps 4 through 7).

**[Figure 6-8: Close-up line drawing of a chassis corner showing "REMOVE RECESSED SCREW AND REAR HINGE" with a leader line to the screw and hinge assembly at the corner. Drawing number 4999-38.]**

Figure 6-8. Recessed screw and rear hinge removal.

3. Disconnect the following five connectors from the Input/Output and Vector Generator circuit boards assembly. Note cable color, location, and routing for reinstallation reference.

a. Disconnect P6410 (ten-wire connector) and P6420 (nine-wire connector) from the Input/Output circuit board.

b. Disconnect P6110 (ten-wire connector), P6120 (nine-wire connector), and P6130 (eight-wire connector) from the Vector Generator circuit board.

4. Perform step 9 of "Storage Circuit Board" removal procedure and place the Input/Output and Vector Generator circuit assembly down inside the instrument temporarily for later removal in step 9 of this procedure.

5. Release the Board Latch and lower the Storage circuit board into the instrument.

6. Disconnect P6100, a ribbon connector on the Storage circuit board from the Input/Output and Vector Generator circuit board assembly.

7. Remove the screw from the chassis mounted hinge nearest to the Board Latch and separate it from the hinge on the Storage circuit board.

8. Unhinge the Storage circuit board from the chassis side rail to remove P6100 from the Storage circuit board. Set the Storage circuit board down on top of the Power Supply shield leaving enough space to lift the Input/Output and Vector Generator circuit boards assembly out of the instrument.

<!-- page 6-39 -->

9. Remove the Input/Output and Vector Generator circuit boards assembly from the inside of the instrument (placed inside the instrument in step 4).

To reinstall the Input/Output and Vector Generator circuit boards assembly, perform the reverse of the preceding steps.

#### Cathode-Ray Tube

**WARNING**

*Use care when handling a crt. Breakage of the crt may cause high-velocity scattering of glass fragments (implosion). Protective clothing and safety glasses should be worn. Avoid striking the crt on any object which may cause it to crack or implode. When storing a crt, either place it in a protective carton or set it face down on a smooth surface in a protected location with a soft mat under the faceplate.*

The crt can be removed and reinstalled as follows:

1. Perform the "Storage Circuit Board in Servicing Position" removal procedure.

2. Perform the "Side-Chassis Assembly" removal procedure.

3. Disconnect four deflection-plate wires at the middle of the crt neck and unplug the Trace Rotation connector (P9006) from the Front-Panel circuit board (note the connection locations and wire colors for reinstallation reference).

**WARNING**

*The crt anode lead and the High-Voltage Multiplier output lead retain a high-voltage charge after the instrument is turned off. To avoid electrical shock, disconnect the High-Voltage Multiplier lead from the crt anode lead and ground both leads to the main instrument chassis.*

4. Unplug the crt anode lead connector from the High-Voltage Multiplier lead located between the support chassis and the crt shield. Discharge both the anode lead connector and the High-Voltage Multiplier lead to chassis ground.

5. Remove two front-panel screws that retain the plastic crt frame and light filter to the front panel. Remove the crt frame and light filter from the instrument.

6. Remove the crt socket cap from the rear of the crt socket. Save the cap for reinstallation.

7. With the rear of the instrument facing you, place the fingers of both hands over the front edge of the front subpanel. Then, using both thumbs, press forward gently on the crt funnel near the front of the crt. When the crt base pins disengage from the socket, remove the crt and the crt shield through the instrument front panel. Place the crt in a safe place until it is reinstalled. If the plastic crt corner pads fall out, save them for reinstallation.

**NOTE**

*When installing the crt into the instrument, reinstall any loose plastic crt corner pads that are out of place. Ensure all crt pins are straight and that the indexing keys on the crt base, socket, and shield are aligned. Ensure that the ground clip makes contact only with the outside of the crt shield.*

To reinstall the crt, perform the reverse of the preceding steps.

#### Power-Supply Shield

The Power-Supply shield can be removed and reinstalled as follows:

1. Turn the instrument over (Main circuit board up) and remove the screw from the plastic power-supply cover (middle of the Main circuit board). Insert a small pointed tool into the hole in the left-rear corner of the rear chassis and gently push the power-supply cover tab in. Remove the power-supply cover by sliding it out from underneath the rear and side chassis.

2. Remove the screw securing the Power-Supply shield to the Main circuit board (located at the bottom of the Main circuit board near the middle of the side chassis frame). Turn the instrument over again (Storage circuit board on top) to continue with the Power-Supply Shield removal procedure.

3. Perform the "Storage Circuit Board in Servicing Position" removal procedure.

4. Perform the "Support Chassis" removal procedure.

<!-- page 6-40 -->

5. Remove one pan-head and two recessed screws securing the Power-Supply shield to the rear chassis frame. See Figure 6-9 for the location of the three screws on the rear chassis frame.

6. Remove the screw from the front upper-right hand corner of the Power-Supply shield.

7. Lift the Power-Supply shield up and out of the chassis frame by removing the right rear corner first.

**NOTE**

*To reinstall the Power-Supply shield, ensure that the shield is placed in the frame guides on the rear chassis above the fuse holder and that the crt socket-wire assembly and crt anode lead are properly placed in their respective cutouts.*

To reinstall the Power-Supply shield, perform the reverse of the preceding steps.

#### Line Filter Circuit Board and Cover

To remove the Line Filter circuit board and cover, perform the following steps:

1. Perform the "Storage Circuit Board in Servicing Position" removal procedure.

2. Remove the Power-Supply shield (see the "Power-Supply Shield" removal procedure).

3. Remove the two recessed screws that secures the Filter circuit board to the rear chassis and lift the Line Filter circuit board out and away from the filter capacitor.

4. Remove the four wires to the Line Filter circuit board by unsoldering two wires from the Main circuit board, one wire from the line filter, and one wire from the fuse holder (pull the protective cap completely off the fuse holder before unsoldering).

To reinstall the Line Filter circuit board and cover, perform the reverse of the preceding steps.

**[Figure 6-9: Line drawing of the rear-chassis interior (line filter/power-entry area), labeling "REMOVE ONE PAN-HEAD AND TWO RECESSED SCREWS" with leader lines to three screw locations near the fan, power-entry module, and fuse holder. Drawing number 4999-39.]**

Figure 6-9. Location of screws securing Power-Supply shield and the support bracket to the rear chassis frame.

<!-- page 6-41 -->

#### Fan

The fan can be removed and reinstalled as follows:

1. Perform the "Storage Circuit Board in Servicing Position" removal procedure.

2. Perform the "Power-Supply Shield" removal procedure.

3. Unsolder the two leads from the fan driver on the Main circuit board.

4. Remove two screws securing the fan to the rear chassis and two recessed screws securing the fan driver to the side chassis.

To reinstall the Fan, perform the reverse of the preceding steps.

#### Thermal Shutdown Circuit Board

1. Perform the "Storage Circuit Board in Servicing Position" removal procedure.

2. Perform the "Power-Supply Shield" removal procedure.

3. Perform the "Fan" removal procedure.

4. Stand the instrument up on its rear chassis (front panel up) and use a vacuum-desoldering tool to unsolder three pins from the Thermal Shutdown circuit board to the Main circuit board (W9070).

To reinstall the Thermal Shutdown circuit board, perform the reverse of the preceding steps.

#### Alternate Sweep Circuit Board

The Alternate Sweep circuit board can be removed and reinstalled as follows:

1. Perform the "Storage Circuit Board in Servicing Position" removal procedure.

2. Disconnect P4220, a two-wire connector located on the right side of the Alternate Sweep circuit board.

3. Remove the cable strap from the Alternate Sweep circuit board that secures the cable harness from the Storage circuit board.

4. Use a vacuum-desoldering tool to unsolder the 27 Alternate Sweep circuit board pins on the Main circuit board (W9400).

5. Unclip the plastic holder from the Power-Supply shield and remove the Alternate Sweep circuit board from the instrument.

To reinstall the Alternate Sweep circuit board, perform the reverse of the preceding steps.

#### Position Interface Circuit Board

The Position Interface circuit board can be removed and reinstalled as follows:

1. Perform the "Storage Circuit Board in Servicing Position" removal procedure.

2. Perform the "Support Chassis" removal procedure.

3. Disconnect P6113, a four-wire connector from Input/Output and Vector Generator circuit boards assembly.

4. Turn the instrument on its side and with a vacuum-desoldering tool, unsolder the six Position Interface circuit board wire straps from the Main circuit board.

5. Remove the Position Interface circuit board from the instrument and clean the wire-strap holes on the Main circuit board of any remaining solder.

To reinstall the Position Interface circuit board, perform the reverse of the preceding steps.

#### Channel 1 Logic and Channel 2 Logic Circuit Boards

The Channel 1 Logic and Channel 2 Logic Circuit Boards can be removed and reinstalled as follows:

1. Perform the "Storage Circuit Board in Servicing Position" removal procedure.

<!-- page 6-42 -->

2. Perform the "Support Chassis" removal procedure.

3. Remove the remaining six screws that secure the top attenuator shield and ground strap (from the Front Panel circuit board) to the Attenuator circuit board and bottom shield.

4. Remove the top attenuator shield from the instrument.

5. Disconnect the following connectors from the Channel 1 Logic and Channel 2 Logic circuit boards, noting their locations for reinstallation reference:

a. P6111, a three-wire connector from Channel 1 Logic circuit board.

b. P6112, a three-wire connector from Channel 2 Logic circuit board.

6. Remove one screw each from the front of the Channel 1 Logic and Channel 2 Logic circuit boards.

7. Unsolder the two-wire strap from the rear of both the Channel 1 Logic and Channel 2 Logic circuit boards.

8. Remove the Channel 1 Logic and Channel 2 Logic circuit boards from the instrument.

To reinstall the Channel 1 Logic and Channel 2 Logic circuit boards, perform the reverse of the preceding steps.

#### Attenuator, Channel 1 Logic and Channel 2 Logic Circuit Boards Assembly

The Attenuator, Channel 1 and Channel 2 Logic Circuit Boards Assembly can be removed and reinstalled as follows:

1. Turn the instrument over (Main circuit board up) and remove two screws securing the Attenuator circuit board to the BNC bracket (located underneath the CH 1 OR X and CH 2 OR Y input connectors).

2. Unsolder the two resistors from the CH 1 OR X and CH 2 OR Y input connectors. Turn the instrument over again (Storage circuit board on top) to continue with the Attenuator, Channel 1 and Channel 2 Logic circuit boards assembly procedure.

3. Perform the "Storage Circuit Board in Servicing Position" removal procedure.

4. Use a 1/16-inch hex wrench to loosen the set screws on both the CH 1 and CH 2 VOLTS/DIV Variable knobs and remove the knobs.

5. Set the CH 1 and CH 2 VOLTS/DIV switches to the same position. Note switch positions for reinstallation reference; then remove the knobs by pulling them straight out from the front panel.

6. Perform the "Support Chassis" removal procedure.

7. Remove the remaining six screws that secure the top attenuator shield and ground strap (from the Front Panel circuit board) to the Attenuator circuit board and bottom shield.

8. Remove the top attenuator shield from the instrument.

9. Disconnect the following connectors from the Channel 1 Logic, Channel 2 Logic and Attenuator circuit boards, noting their locations for reinstallation reference:

a. P6111, a three-wire connector from Channel 1 Logic circuit board.

b. P6112, a three-wire connector from Channel 2 Logic circuit board.

c. P9103, a four-wire connector located behind the CH 1 VOLTS/DIV switch assembly and underneath the Channel 1 Logic circuit board.

d. P9108, a four-wire connector located behind the CH 2 VOLTS/DIV switch assembly and underneath the Channel 2 Logic circuit board.

e. P9991, a three-wire connector located between CH 1 and CH 2 VOLTS/DIV Variable controls and Channel 1 and Channel 2 Logic circuit boards.

10. Remove the screw from the left rear corner of the Attenuator circuit board.

**NOTE**

*The insulator on the left rear corner of the Timing circuit board may be loose. If the insulator is loose, remove and save it for the reinstallment of the Attenuator circuit board.*

<!-- page 6-43 -->

11. Pull the Attenuator, Channel 1 Logic and Channel 2 Logic circuit boards Assembly straight back from the front of the instrument until the circuit boards interconnecting pins are disengaged and the switch shafts are clear of both the Front-Panel circuit board and the two Input Coupling switch shafts (located between the front panel and the subpanel). Then lift out the entire assembly through the top of the instrument.

12. If removal of Channel 1 Logic and Channel 2 Logic circuit boards from the assembly is desired, perform the "Channel 1 Logic and Channel 2 Logic Circuit Boards" removal procedure steps 6 through 8.

**NOTE**

*When reinstalling the Attenuator, Channel 1 and Channel 2 Logic circuit boards Assembly, ensure that the interconnecting pins are aligned with the Front-Panel circuit board connectors and that the two resistors (soldered to the bottom of the Attenuator circuit board) are not touching the Front-Panel circuit board. Push the Attenuator circuit board forward and, at the same time, press the front end of the board down slightly. Align the two Input Coupling switch shafts with the front-panel holes by moving either the Channel 1 or the Channel 2 Input Coupling switch knob.*

To reinstall the Attenuator, Channel 1 and Channel 2 Logic circuit boards assembly, perform the reverse of the preceding steps.

#### Sweep Reference Circuit Board

The Sweep Reference circuit board can be removed and reinstalled as follows:

1. Perform the "Storage Circuit Board in Servicing Position" removal procedure.

2. Disconnect P9410, an seven-wire connector located behind the SEC/DIV Variable control on the Sweep Reference circuit board.

3. Disconnect P5201, a three-wire connector located on the right side of the Sweep Reference circuit board.

4. Unsolder the two resistors from the Timing Circuit board on the right side of the SEC/DIV Variable control.

5. Remove the shaft extension by loosening the setscrew with a 0.50-hex wrench.

6. Remove the SEC/DIV variable control nut with a 9/16 inch open-end wrench.

7. Remove the Sweep Reference circuit board.

To reinstall the Sweep Reference circuit board, perform the reverse of the preceding steps.

#### Timing, Sweep Interface, and Sweep Reference Circuit Boards Assembly

The Timing, Sweep Interface, and Sweep Reference circuit boards assembly can be removed and reinstalled as follows:

1. Perform the "Storage Circuit Board in Servicing Position" removal procedure.

2. Use a 1/16-inch hex wrench to loosen the set screw of the SEC/DIV Variable knob. Remove the SEC/DIV Variable knob.

3. Set both A and B SEC/DIV knobs to the EXT CLK position. Use a 1/16-inch hex wrench to loosen the two set screws that secure the A and B SEC/DIV knob; pull off the knob from the shaft assembly.

4. Use a 1/16-inch hex wrench to loosen two set screws securing the A SEC/DIV dial to the shaft assembly. Remove the dial from the shaft.

5. Disconnect the following connectors from the assembly, noting their locations for reinstallation reference:

a. P9700, a 10-wire connector located on the right edge of the Timing circuit board.

b. P9705, an eight-wire connector located at the rear of the Timing circuit board.

c. P6421, an five-wire connector located on the Sweep Interface circuit board.

d. P9410, an seven-wire connector located behind the SEC/DIV Variable control on the Sweep Reference circuit board.

6. Remove the screw located at the right rear of the Attenuator circuit board (securing both the Attenuator and the Timing circuit boards to the Bottom shield).

<!-- page 6-44 -->

7. Remove the three securing screws from the Timing circuit board (the screws are located at the right front corner, left front side by the SEC/DIV switch shaft, and at the right rear corner of the circuit board).

**NOTE**

*The insulator on the left rear corner of the Timing circuit board may be loose. If the insulator is loose, remove and save it for the reinstallment of the Timing circuit board.*

8. Pull the Timing circuit board straight back from the front of the instrument until the circuit board interconnecting pins are disengaged and the switch shaft is clear of the Front-Panel circuit board.

9. If removal of Sweep Reference circuit board from the assembly is desired, perform the "Sweep Reference Circuit Board" removal procedure steps 3 through 7.

**NOTE**

*Ensure that the Timing circuit board interconnecting pins are aligned to the Front-Panel circuit board connectors before reinstallation.*

To reinstall the Timing, Sweep Interface, and Sweep Reference circuit boards assembly, perform the reverse of the preceding steps.

SWEEP INTERFACE CIRCUIT BOARD SEPARATION. To remove the Sweep Interface circuit board from the Timing circuit board perform the following steps.

1. Use a vacuum-desoldering tool to unsolder the 22-wire strap W1304 from the Sweep Interface to the Timing circuit board.

2. Remove the Sweep Interface circuit board and clean the wire-strap holes in the Timing circuit board.

To reinstall the Sweep Interface circuit board, perform the reverse of the preceding steps.

#### Bottom Shield, Attenuator and Timing Circuit Boards Assembly

The Bottom Shield, Attenuator, and Timing circuit boards assembly can be removed and reinstalled as follows:

1. Place the instrument upside down and remove the three screws and one spacer post securing the Bottom shield to the Main circuit board.

2. Perform steps 1 through 9 of the "Attenuator, Channel 1 Logic and Channel 2 Logic Circuit Board" removal procedure.

3. Perform steps 2 through 5 of the "Timing, Sweep Interface, and Sweep Reference Circuit Boards" removal procedure.

4. Pull the Bottom shield, along with the attached circuit boards straight back from the front of the instrument until the interconnecting pins on the circuit boards are disengaged and the switch shafts are clear of the holes in the Front-Panel circuit board; then lift out the entire assembly through the top of the instrument.

5. If accessibility to the bottom of either the Attenuator or the Timing circuit board is desired, refer to step 10 of the "Attenuator, and Channel 1 and Channel 2 Logic Circuit Boards Assembly" removal procedure and to step 7 of the "Timing, Sweep Interface, and Sweep Reference Circuit Boards Assembly" removal procedure.

To reinstall the Bottom Shield, Attenuator, and Timing circuit boards assembly, perform the reverse of the preceding steps.

#### Front-Panel Circuit Board

The Front-Panel circuit board can be removed and reinstalled as follows:

1. Perform the "Storage Circuit Board in Servicing Position" removal procedure.

2. Perform the "Support Chassis" removal procedure.

3. Perform the "Cathode-Ray Tube" removal procedure.

4. Perform the "Bottom shield, Attenuator and Timing Circuit Boards Assembly" removal procedure.

5. Remove the knobs from the following control shafts by pulling them straight out from the front panel:

a. Channel 1 and Channel 2 POSITION.

b. A/B SWP SEP.

c. Horizontal POSITION.

d. B TRIGGER LEVEL.

<!-- page 6-45 -->

6. Use a 1/16-inch hex wrench to loosen the setscrew of the HF REJECT knob. Remove the HF REJECT knob.

7. Use a 1/16-inch hex wrench to loosen the setscrew of the A TRIGGER LEVEL knob. Remove the A TRIGGER LEVEL knob.

8. Unsolder both the resistor to the EXT INPUT center connector and the wire strap to the EXT INPUT ground lug.

9. Unsolder the two wire straps from VAR HOLDOFF control.

10. Unsolder the single wire from the PROBE ADJUST connector and the two wires from the VAR HOLDOFF control (leading to the Front-Panel circuit board).

11. Remove the following screws:

a. Three screws (and ground strap) securing the upper part of the Front-Panel circuit board to the front panel.

b. Two recessed frame-securing screws at the left-rear corner of the chassis frame.

c. Two bottom screws securing the Main circuit board to the left bottom side of the chassis frame.

d. One screw securing the delay line to the chassis frame on the left side of the instrument.

e. Two recessed frame-securing screws at the right-front corner.

**NOTE**

*At this point, any component on the Front-Panel circuit board may be accessed for removal and replacement. If circuit board replacement is intended, continue with the last two steps 10 and 11.*

12. Pull the left-front frame assembly apart from the right-rear frame assembly.

**NOTE**

*If a vacuum-desoldering tool is not available, lift each strap out of the Main circuit board as the joint is heated.*

13. Use a vacuum-desoldering tool to unsolder the 45 (W9001) wire straps from the Main circuit board (connecting to the Front-Panel circuit board).

14. Remove the Front-Panel circuit board from the instrument and clean the wire-strap holes on the Main circuit board of any remaining solder.

To reinstall the Front-Panel circuit board, perform the reverse of the preceding steps.

#### Main Circuit Board

All components on the Main circuit board are accessible either directly or by removing either the Storage circuit board, the crt, the Bottom shield, Attenuator, Timing circuit-boards assembly, and the Power-Supply shield. Removal of the Main circuit board is required only when it is necessary to replace the circuit board with a new one.

The Main circuit board can be removed and reinstalled as follows:

1. Perform the "Storage Circuit Board in Servicing Position" removal procedure.

2. Perform the "Support Chassis" removal procedure.

3. Perform the "Side-Chassis Assembly" removal procedure.

4. Perform step 3 under the "Input/Output and Vector Generator Boards Assembly" removal procedure.

5. Disconnect the three-wire B DELAY TIME POSITION potentiometer connector (P9644) from the Main circuit board (located on the right side of the Main circuit board).

6. Perform the "Alternate Sweep Circuit Board" removal procedure.

7. Disconnect the connectors from the Attenuator and Timing circuit boards assembly, noting their locations for reinstallation reference.

8. Remove three screws and one spacer securing the Bottom shield to the Main circuit board.

<!-- page 6-46 -->

9. Perform the "Power-Supply Shield" removal procedure.

10. Unsolder two wires from the Main circuit board to the Filter circuit board.

11. Unsolder the rear-panel EXT Z AXIS connector wire from the Main circuit board.

12. Unsolder the two leads on the Main circuit board from the fan driver.

13. Unsolder the three leads on the chassis mounted CR970 from the Main circuit board.

14. Disconnect P9070, a three-wire connector from the Main circuit board to the heat-sink mounted Q9070.

15. Remove the FOCUS control shaft assembly by pulling it straight out from the front panel.

16. Remove the POWER switch extension-shaft assembly by first pressing in the POWER button to the ON position. Then insert a scribe (or similar tool) into the notch between the end of the switch shaft and the end of the extension shaft and gently pry the connection apart. Push the extension shaft forward, then sideways, to clear the switch shaft. Finally, pull the extension shaft back and out of the instrument.

17. Remove two recessed screws securing the power-supply transistor heat-sink assembly to the right side of the chassis frame.

**WARNING**

*The crt anode lead and the output terminal to the High-Voltage Multiplier will retain a high-voltage charge after the instrument is turned off. To avoid electrical shock, ground the crt side of the anode lead to the main instrument chassis.*

18. Disconnect the crt anode lead from the High-Voltage Multiplier anode lead by carefully pulling the anode plug out of the jack. Discharge the plug tip to the chassis.

19. Unsolder two sets of crt socket wires from the Main circuit board, noting wire color and position for reinstallation reference.

20. Unsolder two sets of delay-line wires from the Main circuit board, noting wire color and position for reinstallation reference.

21. Remove three screws securing the Main circuit board to the instrument chassis frame (one under the EXT Z AXIS connector and two along the left side of the Main circuit board).

22. Use a vacuum-desoldering tool to unsolder the 45 wire straps (W9001) connecting the Main circuit board to the Front-Panel circuit board) from the Main circuit board.

**NOTE**

*If a vacuum-desoldering tool is not available, lift each wire strap out of the Main circuit board as the joint is heated. Use care to maintain, as nearly as possible, the original shape and spacing of the wire straps to facilitate replacing the circuit board.*

23. Push the wire-strap connection end of the Main circuit board down until it is clear of all wire strap ends; then remove it through the bottom of the instrument frame. Ensure that the wire straps are not bent out of place.

**NOTE**

*When installing the Main circuit board, ensure that the circuit board is in the guides at the rear and right side of the frame and that the 45 wire straps of W9001 are inserted into their corresponding holes.*

To reinstall the Main circuit board, perform the reverse of the preceding steps.
