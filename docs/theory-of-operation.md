# Tektronix 2230 Service Manual — Section 3: Theory of Operation

OCR transcription of the service manual's Theory of Operation section
(52 pages), rendered from `Tektronix_2230_Service_Manual-3-Theory_of_
Operation.pdf` (not in the repo — see the user's own manual copy).
Primary source material — transcribed as printed, not summarized.
Figures/diagrams/photos are noted with placeholders, not redrawn.

**Signal names with a printed overbar** (indicating an active-low/
inverted signal, e.g. a line over `RESET`) are written with a leading
`/` instead (`/RESET`) — the standard plain-text convention for a
signal that can't be typeset with a real overline, and one that stays
grep-able (searching `RESET` still finds `/RESET`).

<!-- page 3-1 -->

## SECTION ORGANIZATION

This section contains a functional description of the 2230 Digital Storage Oscilloscope. The discussion begins with a summary of instrument functions. Following the general description, each major circuit is explained in detail. Functional block diagrams and schematic diagrams are used to show the interconnections between parts of the circuitry, to indicate circuit components, and to identify interrelationships with the front-panel controls.

Schematic diagrams and the overall block diagrams are located in the tabbed "Diagrams" section at the back of this manual. The schematic diagram associated with each description is identified in the text and indicated on the tab of the appropriate foldout page by a numbered diamond symbol. For best understanding of the circuit being described, refer to both the appropriate schematic diagram and the functional block diagram.

## INTEGRATED CIRCUIT DESCRIPTIONS

### Digital Logic Conventions

Digital logic circuits perform many functions within the instrument. Functions and operation of the logic circuits are represented by logic symbology and terminology. Most logic functions are described using the positive-logic convention. Positive logic is a system where the more positive of two levels is the TRUE (or 1) state; the more negative level is the FALSE (or 0) state. In this logic description, the TRUE state is HI, and the FALSE state is LO. The specific voltages which constitute a HI or a LO state vary between specific devices. For specific device characteristics, refer to the manufacturer's data book.

### Linear Devices

The operation of individual linear integrated circuit devices is described in this section using waveforms or graphic techniques to illustrate their circuit action.

## GENERAL DESCRIPTION

### Introduction

In the following overall functional description of the instrument, refer to the basic block diagram, Figure 3-1, and to the detailed block diagrams located in the "Diagrams" section of this manual. Each major block in the diagram represents a major circuit within the instrument. In Figure 3-1, the numbered diamond symbol in each block indicates the schematic diagram number. Much of the analog portion of the oscilloscope operates without direction from the Microprocessor circuitry. These portions of the instrument are described first, with appropriate references to areas that either provide information to the Microprocessor or are controlled by the instrument's storage circuitry. The Microprocessor and Storage circuit descriptions follow the more conventional portions of the instrument's circuitry.

### Vertical

Signals to be displayed on the crt (cathode-ray tube) are applied to either or both the CH 1 OR X and the CH 2 OR Y input connectors. The signals may be coupled to the attenuator either directly (DC) or through an input-coupling capacitor (AC). The inputs may also be disconnected, and the input to the attenuators grounded, by switching to the GND position of the input coupling switch. In the GND position, the ac-coupling capacitor is allowed to precharge to the dc level present at the input connector. This precharging prevents large trace shifts of the display when switching from GND to AC coupling. The Attenuators are switched by the front-panel VOLTS/DIV switches and scale the applied signal level to obtain the desired display amplitude. Information about the Input Coupling switch and the channel VOLTS/DIV switch positions is read by the Microprocessor. These signals control the STORE mode ground-reference acquisition and the crt readout displays of the Input Coupling and VOLTS/DIV switch settings of the active channel(s).

Scaled output signals from the Attenuators are applied to the Vertical Preamplifiers for amplification. The Channel 2 Preamplifier has additional circuitry, permitting the operator to invert the Channel 2 display on the cathode-ray tube (crt). Trigger pickoffs in each channel supply a

<!-- page 3-2 -->

**[Figure 3-1. Simplified block diagram — overall instrument block diagram showing: CH1/CH2 Attenuators & Preamplifiers (1,2) -> Delay Line -> Vertical Output Ampl (3) -> CRT; Trigger Generator (4) fed by Trig Controls/Line/Ext Input -> A&B Sweep Generator & Display Logic (5,6) -> Horizontal Output Ampl (7) -> CRT; Status ADC & Bus Interface (19) and Microprocessor & ROM (14) tied to an Address/Data/Control bus connecting Digital Timebase (18), Digital Acquisition & Memory (17), Storage Acquisition (16), Digital Display (15), Vector Generator (20), and Communications (22,23,24,25) blocks; Z Axis (9), HV Power Supply (9) and LV Power Supply (8,9) blocks; outputs to RS232/GPIB port (option) and XY Plotter Outputs. Drawing number 4999-01.]**

<!-- page 3-3 -->

trigger signal to the Trigger Amplifier when internal triggering is selected. Other signal pickoffs provide vertical position information to the Position Signal Conditioning circuitry for vertically positioning the stored signal. The final stage of the Vertical Preamplifier for each channel provides one of two signals; either the vertical channel signal for the analog presentation on the crt or the vertical acquisition signal to be digitized by the storage circuitry.

Channel signals either for direct analog presentation on the crt or for application to the Storage digitizing circuitry are selected by the analog Channel Switch under control of the front-panel VERTICAL MODE switches. The switching signals from the Channel Switch Logic control a diode gate (Channel Switch) that selects the channel signal(s) to be applied to the Delay-line Driver. If ADD is selected, both channel signals are applied to the Delay-line Driver where the signals are summed together. The Delay-Line Driver provides the proper signal-driving level and impedance match to the Delay Line, where the vertical signal is delayed approximately 100 ns with respect to the trigger signal. The vertical signal delay allows time for the Horizontal circuitry to start the sweep before the vertical signal is applied to the crt.

Whenever STORE mode is selected, analog signals from the Storage circuitry are supplied to the Channel Switch circuit. Under control of the Channel Switch Logic, which is in turn switched by signals from the Display Controller, the analog display signal out of the final Vertical Preamplifier stage in each channel is biased off. The Channel 1 and Channel 2 Acquisition signals from the final preamplifiers are then biased on to pass the signals to be digitized to the Storage circuitry. At the same time, the Channel Switch (diode gate) is switched to pass the Storage vertical signal to the Delay Line Driver input.

Final amplification of the vertical signal (either STORE or NON STORE) is done by the Vertical Output Amplifier. This stage produces the signal levels that vertically deflect the crt electron beam. This amplifier stage also contains the vertical trace separation circuitry that separates the Nonstore A Intensified trace from the B Delayed trace when Alt Horizontal display mode is selected. The amount of trace separation is controlled using the front panel TRACE SEP knob. Another circuit feature in the Vertical Output Amplifier is the nonstore bandwidth limit (BW LIMIT) circuitry that follows the Delay Line. Either the full 100 MHz bandwidth or the limited 20 MHz bandwidth for the nonstore signal display may be selected. STORE mode signals are picked off in the Preamplifier and are not bandwidth limited by the BW LIMIT switch.

### Triggering

The Triggering circuitry uses either the Internal Trigger signal obtained from the input signal(s), an External Trigger signal, or a Line Trigger signal derived from the ac-power-source to develop trigger signals for the Sweep Generator. The Auto Trigger circuit sets the range of the Trigger Level to conform approximately to the peak-to-peak amplitude of the selected trigger signal when either Auto or TV Field Trigger mode is selected. In Norm mode, the TRIGGER LEVEL control must be adjusted to the signal level before a sweep will be triggered. ROLL Storage (selectable at the slower sweep speeds in STORE mode) overrides the triggering circuit functions; a continuous signal acquisition is made and the signal displayed without the need of a trigger signal.

The triggering circuitry contains the TV Field Sync circuit. This circuit provides stable triggering on television vertical-sync pulses when in the TV Field triggering mode. TV Line triggering is possible using P-P AUTO trigger mode.

Signal pickoffs from the Internal Trigger circuitry provide the X-Axis signal for the nonstore X-Y display mode and the B trigger signal for triggered B Sweeps.

### A Sweep

The A Sweep Generator and Logic circuits control the nonstore sweep generation and both the Store and the nonstore A Sweep timing. When the A TRIGGER mode switches are set to either P-P AUTO or TV FIELD and no trigger signal is present, the Auto Baseline circuit causes the Sweep Logic circuit to produce a sweep for reference purposes. In the NORM setting, the Auto Baseline circuit is disabled and nonstore sweeps are not generated until a trigger event occurs. NORM trigger mode is used to obtain stable triggering on low-repetition rate signals that do not provide a trigger before an auto baseline is generated. SGL SWP (single sweep) trigger mode allows only one sweep to be generated after being reset and is used to obtain the waveform from a one-shot event.

ROLL and SCAN Storage modes are useful in capturing low-frequency and low-repetition rate waveforms. In SCAN mode, receiving a trigger causes the pretrigger portion of the waveform to update as a block. The posttrigger waveform updates from the trigger point to the right edge of the screen as new data is acquired. ROLL Storage acquisitions differ from the Nonstore sweeps and SCAN Storage mode in that a trigger signal is not used for acquisition of the signal or displaying the waveform. The A Sweep Logic circuitry provides gating and holdoff signals used by the Storage circuitry to control its acquisition and display cycles for all storage modes, except ROLL.

The /A GATE signal applied to the A Miller Sweep Generator circuit starts the Nonstore linear sweep with a ramp time that is controlled by the A SEC/DIV switch setting.

<!-- page 3-4 -->

Switch position pickoffs supply the SEC/DIV switch setting information to the Microprocessor for use in STORE mode horizontal timing. The A SEC/DIV switch setting is also displayed on the crt for both Store and Nonstore operation.

### B Sweep

The Alternate B Sweep Circuitry controls the Nonstore BOTH and B Delayed Horizontal mode displays. This circuitry includes the B Miller Sweep Generator and B Sweep Logic circuitry. STORE mode B timing is controlled by the B SEC/DIV switch. BOTH Horizontal mode is not available with STORE. In STORE mode, the BOTH selection displays an A Intensified Trace only. The intensified zone on the A trace indicates the position and approximate amount of the A trace that is displayed by the B Delayed Display.

### Horizontal

Nonstore A and B Sweep signals (or the X-Axis signal from the X-Y Amplifier in the nonstore X-Y Display mode) are applied to the Horizontal Preamplifier where one is selected and amplified. Gain in the Preamplifier is switchable between X1 and X10. The X10 gain is used for Nonstore X10 Magnification. STORE mode X10 expansion is done digitally and reflected in the horizontal deflection signals supplied after the Horizontal Preamplifier. Horizontal positioning of both the Store and the nonstore display is done by applying a horizontal position dc offset to the Horizontal Preamplifier. The amplified nonstore horizontal signal is applied to the Horizontal Mux circuit where it is available for selection.

STORE mode horizontal deflection signals are also applied to the Horizontal Mux. Selection of either the nonstore sweep signals or the store deflection signals is done by control signals from the Channel Switch Logic in the Vertical circuitry. The selected horizontal deflection signals are then amplified by the Horizontal Output Amplifier to the levels needed to drive the crt's horizontal deflection plates.

### Microprocessor

The Microprocessor (MPU) controls the digital storage and display sections of the oscilloscope. Under firmware control (firmware is the programmed instructions contained in read-only memory), the Microprocessor monitors the operation of the instrument and sets up the circuitry to perform as dictated by the front-panel control settings. Data transfer to and from the Microprocessor and address selection of a device to be communicated with are done over a 20-line I/O bus. The lower eight lines (AD0 through AD7) form a combined address/data bus while the remaining 12 lines (A8 through A19) are for addressing only. Timing for the execution of instructions, addressing, and data transfers is provided by an external, crystal-controlled oscillator that drives the Microprocessor clock generator. The Microprocessor clock circuit further divides the input clock frequency to generate two lower clock frequencies. The clock circuit also generates the Ready and Reset control signals to the Microprocessor.

Storage front-panel control settings are passed to the Microprocessor via eight-bit bus drivers. Settings of the analog front-panel controls and switches are also provided to the MPU, but via different bus drivers. The Status ADC and Bus Interface circuitry provides the interfaces from the analog front-panel controls to the data bus.

### Status ADC and Bus Interface

Switch settings and status bits are applied directly to bus drivers. Each data bit then corresponds to a switch setting (either open or closed) or a status bit logic level (either HI or LO). Analog front-panel information is multiplexed to an analog-to-digital converter where it is converted to a digital value and applied to a bus driver. When the Microprocessor reads the bus, it obtains a data byte that represents the position value for a single control rather than the switch or status data bits of the digital-type information. The Microprocessor determines the control settings from the value of the data bytes or status bits received and sets up the digital storage circuits accordingly.

### Storage Acquisition

Input signals to be digitized are selected by the Channel Switch. Either or both (for ADD) of the input signals picked off from the Vertical Preamplifier may be selected. The differential output signal from the Channel Switch is converted to a single-ended signal for application to the Sample-and-Hold amplifier. The input diode bridge in the Sample-and-Hold circuit is strobed to pass a sample of the signal to charge the hold capacitor. While the signal sample is held for conversion, the diode bridge is reverse biased, and the charge on the Hold capacitor remains at a fixed level. The sample buffer amplifier applies the voltage level on the Hold capacitor to the Analog-to-Digital Converter stage for conversion to an eight-bit digital signal. The output signals are then shifted from the emitter-coupled logic (ECL) level obtained from the ADC to the transistor-transistor-logic level (TTL) and passed to the digitized signal bus for transfer to the Acquisition Memory.

### Digital Acquisition

Digitized waveforms are transferred from the ECL-to-TTL level shifters via the digitized data bus to the A/D Buffer of the Acquisition Memory circuit. The buffered data is applied to two identical registers; the Min Register and the Max Register. Data is alternately clocked into the

<!-- page 3-5 -->

actual clocking that occurs depends on the sampling mode (Min/Max, Sampling, or X-Y). The same waveform data is also applied to opposite comparator inputs of two eight-bit magnitude comparators. Output data from the Min and Max Registers is applied to the other comparator's input pins, with the Min Register data going to the Min Comparator and the Max Register data going to the Max Comparator.

In Min/Max mode, the first data byte taken in a sample window (set by the SEC/DIV switch setting) is clocked into both registers. That data byte is then compared with the next data sample or samples (determined by the sample window) being applied to the inputs of the Min and Max Registers. If the data byte is either smaller in magnitude than the last clocked minimum or greater in magnitude than the last clocked maximum, a NEWMIN or NEWMAX signal is generated. The signal is routed through the Min/Max Clock Selector back to the clock input of the Min or Max Register (Min if it is a new minimum amplitude or Max if it is a new Maximum amplitude) and the new signal is clocked into the register. At the end of a Min/Max sample window, the data present at the output of the Min and Max Registers is clocked into the Swap Registers to be transferred to the Acquisition Memory.

When record sampling mode is selected, each waveform sample is successively clocked into the Min and Max Registers on alternate ODDCLK and /ODDCLK signals. When X-Y mode is selected, the Channel 1 and Channel 2 waveforms are sampled in a chopped manner, with samples of the two channel signals being taken with less time between the samples than in normal record sampling mode. Channel 1 data is clocked into the Min Register, and Channel 2 data is clocked into the Max Register.

Four eight-bit Swap Registers are used to reorder the Max and Min data obtained from each sample window. The Max Register data is clocked into two of the registers in parallel, and the Min Register data is clocked into the other two registers in parallel. The Min and Max data output from one of the Swap Registers in each set of two is applied to two busses going to the Acquisition Memory. If the Max and Min data is to be reversed to maintain the correct time order of the samples before being stored, the alternate swap register in each set of two is enabled, and the Max and Min data is applied to the opposite busses to memory.

Acquisition mode is controlled in part by the Microprocessor via data latched into the Acquisition Mode Register (see also, "Time Base Mode Register" in this section). These data bits select the channel or channels to be acquired, enable the XY mode, enable MIN/MAX acquisition, control the Swap function for reordering data, and select the Test function for diagnostics. Acquisition clock signals generated by the Acquisition Clock Decoder transfer the data from stage to stage in the digital acquisition circuitry in a pipe-line fashion.

A Diagnostics Code Generator is included as a troubleshooting aid. When in the Test mode, the A/D Buffer is disabled, and the Code Generator places its counter-output bytes on the input bus to the Max and Min Registers.

### Acquisition Memory

The Acquisition Memory is composed of two, 2-K by 8-bit random-access memory devices. One memory stores the Odd data bytes and the other stores the Even data bytes. The Odd and Even data can be swapped between the Swap Registers and the Acquisition Memory.

A programmable address counter is loaded with the number that is the amount of pretrigger data bytes needed to fill the pretrigger portion of the waveform acquisition. The PREFULL signal is sent to the Trigger Mux circuitry when the pretrigger count is full. That signal enables the Trigger Mux circuitry to accept a trigger signal. The remaining output bits from the Address Counter select the storage location for waveform data storage in the Acquisition Memory.

When waveform data is to be read out of the Acquisition Memory, the Address Counter is loaded with the address of the data for the waveform. The Microprocessor sequences through the addresses reading out the data bytes. Data transceivers allow data to be read from the memory to the bus or written from the bus to the memory.

Memory Address Registers place the address count on the bus along with bits that indicate the trigger status (TRIGD), the B trigger status (BTRIGD), the end-of-record status (ENDREC), and the byte-interrupt status (BYTEINT). These accompanying bits are used in establishing display attributes.

Memory writes, memory reads, and address counter load enabling and clocking are controlled by a quad, two-line-to-one-line multiplexer (Memory Control). Read and write signals from the Microprocessor control bus and write clocks are used to transfer the waveform data between the devices.

### Digital Time Base

An accurate frequency source for synchronizing the Microprocessor with the other digital devices on the bus is provided by a 40 MHz oscillator. That frequency is divided

<!-- page 3-6 -->

down by the Clock Generator to produce the various clocking rates. The Time Base Mode Register latches control data bits from the Microprocessor data bus to set the operating mode of the time base. These control bits switch the Trigger Mux circuit to either A or B Trigger, enable the trigger logic circuit, switch the clock multiplexer to change the clocking rate, start a storage acquisition, and enable interrupts to the Microprocessor. The programmable Time Base Divider, under control of the Microprocessor via the Time Base Divider Register, generates a sampling rate that corresponds to the front-panel SEC/DIV switch setting.

A Clock multiplexer at the end of the Time Base Divider chain selects the output of the Time Base Divider, the /WRITECLK, the /CONV clock, or an external clock signal to generate the SAVECLK signal.

The Digital Time Base Trigger Logic circuit looks at whether the pretrigger data portion of the record has been filled. If the pretrigger portion is full, then the A or B Gate generates the trigger. When a trigger is generated in Repetitive Storage mode, the Clock Delay Timer measures the time delay between the arrival of the trigger and the convert clock. The time difference value is used by the Microprocessor to accurately position the acquired data with respect to the actual trigger point.

The delay difference between the start of the acquisition and the occurrence of the B trigger is also measured. This value is only used in BOTH HORIZONTAL MODE when running the B Horizontal display in Triggerable After Delay to provide a readout of the time delay between the A Trigger and the B Trigger points.

Acquisitions are counted to determine when a full record of data has been stored (ENDREC) and to keep track of the beginning and ending memory locations of the record. The Record Counter is also programmable to provide for the different record lengths for one-channel or two-channel acquisitions, different Pretrigger selections, and either 4K-byte or 1K-byte record length.

### Digital Display

A custom IC handles the digital display generation. The Display Controller functions as an interface between the processor bus, display memory (RAM), and vector generators to form waveform and character displays on the crt. The controller reads a display list from the Display Memory and drives X- and Y-Vector Generators to create the waveform and readout displays. Z-Axis control signals are also generated to drive the crt Z-Axis Amplifier for Stored waveform and Readout intensity control. Control signals to the Microprocessor and Display Memory are generated in response to a processor read/write request.

Digital-to-analog converters take the digital data bytes supplied from the Display Memory via the Display Controller and change them to the X- and Y-Axis analog signals that drive the Horizontal and Vertical Vector Generators. The vector signals are applied to the Horizontal and Vertical Output Amplifiers to produce the STORE mode deflection signals and NON STORE mode character readout.

The Display Memory is six 16-K x 4-bit dynamic random access memories (RAM). Four of the RAMs provide the 8-bit data bytes of the stored waveform, and the remaining RAMs store each data-byte's intensity and Status attributes. A 4-bit word in each RAM is selected by latching a row address followed by a column address. Data is either stored or read out (as the operation in progress requires).

### Vector Generator

X- and Y-Axis analog signals from the Digital Display are converted by the Vector Generators into the vector signals used to drive the crt deflection plates. Vector signals are produced for the stored waveforms, the menu displays, and the readouts. The Vector Generator is switched to the dot-display mode for equivalent-time sampling waveforms and X-Y displays.

The X-Y Plotter driver circuit is included in this portion of the circuitry. When the X-Y Plotter is enabled, x-axis and y-axis signals are switched via the plot multiplexer to the x-axis and y-axis plot amplifiers. The /VECT SMPL signal is switched via the same multiplexer to drive the Pen-Down amplifier.

### Z-Axis

The Z-Axis Amplifier has input signals from multiple sources that control the crt intensity on a time-shared basis. Nonstore intensity signals are the level inputs from the A and B INTENSITY controls that are controlled by the Alternate Display switching and B Z-Axis Logic circuits. Additional Z-Axis drive current is supplied during the intensified portion of an A trace during the B Sweep when BOTH Horizontal display mode is selected. The remaining nonstore signals that have control of the display brightness are the EXT Z-AXIS INPUT signal, the CHOP mode blanking signal, and the /XY control signal. All of these sources are added to provide the time-shared nonstore displays.

For the Store waveform and the Menu and Readout character displays, an additional Z-Axis drive signal from the STORAGE/READOUT INTENSITY control is switched on and off by the Display Controller. The controller signals determine when the stored waveforms and the readout

<!-- page 3-7 -->

characters are turned on and if any portions of the display will be intensified more than the rest. Further amplification of the combined signal sources provides the amplitude levels required to drive the crt.

The Z-Axis signal is applied to the crt DC Restorer circuit where it is shifted to the large negative potential used by the crt. The potential controls the amount of current supplied by the electron beam to the crt phosphors.

### Power Supply

Operating potentials for the instrument are obtained from a power supply that consists of the Preregulator, Inverter and Transformer, and Rectifiers and Filters. Approximately +42 V is supplied by the Preregulator to drive the 20 kHz Inverter stage through the Transformer primary windings. The transformer secondary windings produce the various ac levels that are rectified and filtered to provide the supply voltages for the instrument's circuitry. A High Voltage Multiplier circuit produces the accelerating, focus, and cathode potentials used by the crt.

### Probe Adjust

A front-panel PROBE ADJUST output is provided for use in adjusting probe compensation. The voltage at the PROBE ADJUST connector is a negative-going square wave that has a peak-to-peak amplitude of approximately 0.5 V with a repetition rate of approximately 1 kHz.

### Communications Options

Options for this instrument provide a choice of either an IEEE-488 GPIB (General Purpose Interface Bus) or an RS-232-C serial output port. The options allow the transfer of stored waveforms and the control of certain instrument functions.

## DETAILED CIRCUIT DESCRIPTION

The detailed circuit description of the 2230 first describes the analog operating portion of the oscilloscope followed by the digital portion. During the description of the analog circuitry, references are made to circuitry that either provides information to the Microprocessor or is controlled by the instrument's storage circuitry.

The Attenuator circuit and switches (see Figure 3-2) provide control of the input coupling, the vertical deflection factor, and the variable volts/division gain. Vertical input signals for display on the crt or for acquisition by the storage circuitry may be connected to either or both the CH 1 OR X and the CH 2 OR Y input connectors. In the X-Y mode of operation, the signal applied to the CH 1 OR X connector provides horizontal (X-axis) deflection for the display, and the signal applied to the CH 2 OR Y connector provides the vertical (Y-axis) deflection for the display.

Switch contacts on the A14 CH 1 Logic board are read by the Microprocessor to find the CH 1 VOLTS/DIV switch and Input Coupling switch settings. A switch contact associated with CH 1 CAL control R43 (Variable Volts/Div) is also read to see whether that control is in or out of the calibrated (CAL) detent.

### ANALOG CIRCUITRY

The instrument has full conventional oscilloscope capabilities with all the associated analog circuitry. Signal pickoff points and signal insertion points connect the analog portion of the instrument to the digital operating system to acquire and display the stored waveforms. The digital circuitry enhances the analog display by providing crt readouts of the VOLTS/DIV, SEC/DIV, and Delay Time Position control settings.

### VERTICAL ATTENUATORS

The Channel 1 and Channel 2 Attenuator circuits, shown on Diagram 1, are identical with the exception of the additional Invert circuitry in the Channel 2 Paraphase Amplifier. Therefore, only the Channel 1 Attenuator is described, with the Invert circuitry of Channel 2 discussed separately.

### Input Coupling (AC-GND-DC)

A signal from the CH 1 OR X input connector may be ac or dc coupled to the High-Impedance Attenuator circuit or disconnected completely by the Input Coupling Switch. Signals from the CH 1 OR X input connector are routed through resistor R1 to Input Coupling switch S1. When S1 is set for dc coupling, the Channel 1 signal goes directly to the input of the High-Impedance Attenuator stage. When

<!-- page 3-8 -->

**[Figure 3-2. Block diagram of the Channel 1 Attenuator circuit — signal path: CH1 OR X -> R9100 -> Input Coupling Switch (S1, AC-GND-DC) -> High-Z Attenuator (÷1,÷10,÷100) -> Buffer Ampl Fast Path (Q13,Q18) / Buffer Ampl Slow Path (U10,Q15) [with Gain Feedback R47] -> Low-Z Attenuator (÷1,÷2,÷4,÷10, switched by S10 VOLTS/DIV) -> CH1 Paraphase Amplifier (U30) -> CH1- / CH1+ to CH1 Preamp; CH1 Logic Switch Contacts feed CH1 Status / CH1 Probe Code to the Status ADC circuit, gated by VAR GAIN R43/CAL. Drawing number 4999-02.]**

<!-- page 3-9 -->

ac coupled, the input signal must go through dc-blocking capacitor C2. The blocking capacitor stops the dc component of the input signal from reaching the Attenuator circuit. When switched into the signal path, attenuators AT1 and AT2 attenuate the input signal by factors of 100 and 10 respectively. When S1 is set to GND, the input of the Buffer Amplifier is connected to ground. This provides a ground reference for the analog display and the Microprocessor without removing the applied signal from the input connector. The coupling capacitor precharges through R2, R4, and R8 to prevent large trace shifts when switching from GND to AC.

A probe coding ring on the CH 1 OR X input connector is used to read the attenuation factor of the attached probe to automatically adjust the VOLTS/DIV scale factors in the readout. The default setting is for X1 attenuation when either coaxial cables or uncoded probes are connected to the vertical inputs.

### Buffer Amplifier and Low-Impedance Attenuator

The Buffer Amplifier presents a high-impedance, low-capacitance load to the signal from the High-Impedance Attenuator and a low output impedance to the Low-Impedance Attenuator. The dual-path buffer amplifier (slow path and fast path) combines good dc stability with high-speed performance.

The input signal goes to the gate of source-follower Q13 through R6 and C6, the fast path, and to the inverting input of operational amplifier U10 from the resistive voltage divider formed by R3 and R5, the slow path. Source-follower Q13 and emitter-follower Q18 have high-impedance inputs that isolate the applied signal from the loading effects of the Low-Impedance Attenuator. A voltage divider formed by R46, R47, and R48 at the emitter output of Q18 applies feedback to the noninverting input of slow-path amplifier U10. The two input voltages to amplifier U10 are compared, and the conductivity of current-source transistor Q15 is changed to correct for any frequency-gain error at the source of Q13. The bandwidth of U10 is limited by capacitor C10 so that the slow path responds only to frequencies below 100 kHz. Input offset voltage compensation for U10, provided by R10, eliminates trace shift between VOLTS/DIV switch settings. Gain in both paths is matched by adjusting MF/LF Gain Bal potentiometer R47. The path gains then remain matched by the corrective action of U10 and Q15 if gain differences in the two paths start to develop.

Low-Impedance Attenuator R19 divides down the Buffer Amplifier output signal for application to Paraphase Amplifier U30. The attenuator's output impedance is 75 ohms at all VOLTS/DIV switch settings. The VOLTS/DIV switch (S10) determines whether the Paraphase Amplifier receives a signal attenuated by a factor of 1 (no attenuation), 2, 4, or 10.

### Paraphase Amplifier

Paraphase Amplifier U30 converts the single-ended signal from the Low-Impedance Attenuator into a differential signal for the Vertical Preamplifier. Included in the circuitry is switching that provides extra gain for the 2 mV position of the VOLTS/DIV switch, adjustments for amplifier dc balance, and circuitry for the Variable Volts/Div function. Additionally, Channel 2 Paraphase Amplifier U80 contains circuitry to invert the Channel 2 display.

The signal from the Low-Impedance Attenuator goes to the base of one transistor in U30. The other input transistor is biased by the divider network formed by R30, R31, and R33 to a level that produces a null between the outputs of U30 (no trace shift on the crt screen) when the VOLTS/DIV control is switched between 5 mV and 2 mV. Emitter current for the two input transistors is supplied by R21, R22, R23, and VAR-BAL potentiometer R25. Resistor R29 is the gain-setting resistor between the two emitters. High-frequency compensation of the amplifier is provided by the series combination of R27 and C27 shunting R29. In the 2 mV position, amplifier gain is increased because contact 15 of S10 is closed to place 2 mV Gain potentiometer R26 and compensating capacitor C26 in parallel with R29.

The collector current from the two input transistors serves as emitter current for the two differential output transistor pairs. Base-bias voltages for the two output pairs are developed by the divider network formed by R39, R41, R42, and Variable Volts/Div potentiometer R43. The transistors of U30 have matched characteristics, so the ratio of currents in the two IC diodes connected to pin 11 determines the current ratios in the output transistor pairs. As Variable Volts/Div potentiometer R43 is rotated from calibrated to uncalibrated, the conduction level of the transistors connected to R35 increases. Since the transistor pairs are cross-connected, the increased conduction in one pair subtracts from the output current produced by the transistor pair connected to R38, and the overall gain of the amplifier decreases. VAR-BAL potentiometer R25 is adjusted to balance the amplifier for minimal dc trace shift as the Variable Volts/Div control is rotated.

Incorporated in the Channel 2 Paraphase Amplifier is circuitry that allows the user to invert the polarity of the Channel 2 signal. When INVERT switch S90 is out, the transistor pairs in U80 are biased as they are in U30, and CH 2 trace is not inverted. For the IN position of S90, connections to the bases of the output transistor pairs are

<!-- page 3-10 -->

reversed, reversing the polarity of the output signal to produce an inverted Channel 2 trace and Channel 2 storage acquisition signal. The inverted/noninverted state is read by the Microprocessor, and an indicator is displayed in the crt readout adjacent to the CH 2 VOLTS/DIV readout to indicate to the user when INVERT is in effect. Invert Bal potentiometer R75 is adjusted for minimal dc trace shift when the INVERT button is changed between IN and OUT.

### VERTICAL PREAMPLIFIERS

The Channel 1 and Channel 2 Vertical Preamplifiers, shown on Diagram 2, are identical in operation. Operation of the Channel 1 amplifier is described. Differential signal current from the Paraphase Amplifier is amplified to produce drive current to the Delay Line Driver and supply the Channel 1 signal to the Storage Acquisition circuitry. Internal trigger signals for the Trigger circuitry are picked off prior to the Vertical Preamplifier. The Channel Switch circuitry controls channel selection for the Nonstore crt display, STORE mode signal acquisition and display, and the selection of either STORE or NON STORE, is controlled by the Display Controller circuitry.

Common-base transistors Q102 and Q103, which complete the Paraphase Amplifier portion of the circuitry shown on Diagram 1, convert differential current from the Paraphase Amplifier into level-shifted voltages that drive the bases of the input transistors of Vertical Preamplifier U130. Differential internal trigger signals are picked off at this point from the collector signals of Q102 and Q103 before Vertical POSITION dc offset is added to the input signals.

The collector current of each input transistor of U130 is the emitter current for two of the differential output transistors. One of the collectors of each output pair supplies one side of the differential Nonstore signal to the Delay Line Driver, and the other collector in each pair supplies one side of the differential Channel signal to the Storage Acquisition circuitry. The base bias voltages of the output transistors are controlled by the Channel Switch Logic circuitry. The switching circuitry determines which channel is active (CH 1, CH 2 or both for ADD) in NON STORE, and which channel supplies the Storage Acquisition signal in STORE.

Vertical POSITION control R112 adds an offset voltage to the pair of differential transistors, Q114 and Q115, that supply the emitter current to the Preamplifier input transistors. Unequal collector currents from Q114 and Q115 go to the input transistors to introduce the vertical position offset to the Channel 1 NON STORE signal. Output signals from Q114 and Q115 are applied to a Storage Vertical Position conditioning circuit where dc offset adjustments provide tracking corrections between the vertical positions of the NON STORE and the STORE signals.

When Channel 1 is selected to drive the Delay Line Driver, the Q output (pin 5) of U540A is HI. That HI is switched through U7201 to the bases of the nonstore signal transistors (connected to pin 14 of U130). These transistors are then forward-biased, and the Channel 1 signal is conducted to the Channel Switch circuit. If Channel 1 is not selected, then the Q output of U540A is LO, and the nonstore signal transistors are reverse-biased to prevent the Channel 1 nonstore signal from being displayed. The gain of the Preamplifier is set by adjusting R145 to control the signal current that is shunted between the two differential outputs. Amplifier gain is reduced by the current shunted between the two halves of the Preamplifier.

### Channel Switch Logic

The Channel Switch Logic circuitry, shown on Diagram 2, utilizes the front-panel VERTICAL MODE and STORE/NON STORE mode switches to select the crt display format. See Figure 3-3 for a block diagram of the circuit.

When any display mode other than X-Y is selected, the XY line connected to S550 is at ground potential. VERTICAL MODE switches S545 and S550 control the connection between the XY control line and the Set and Reset inputs of flip-flop U540A for the nonstore display formats.

CHANNEL 1 DISPLAY ONLY. The CH 1 position of S550 grounds the Set input (pin 4) of U540A while the Reset input (pin 1) is held HI by pull-up resistor R539. This produces a HI and a LO on the Q and /Q outputs of U540A respectively. The levels are selected by multiplexer U7201, biasing on the Channel 1 nonstore output transistors in U130, allowing the Channel 1 input signal to drive the Delay Line Driver. The Channel 2 Preamplifier nonstore output transistors in U180 are biased off.

CHANNEL 2 DISPLAY ONLY. The CH 2 position of S550 holds the Reset input of U540A LO through CR538, and the Set input is held HI by pull-up resistor R538. The outputs of U540A are then Q LO and /Q HI biasing on the Channel 2 Preamplifier nonstore output transistors (in U180) and biasing off the Channel 1 Preamplifier nonstore output transistors (in U130). Channel 2 then supplies the signal to drive the Delay Line Driver.

To display the ADD, ALT, or CHOP formats, S550 must be in the BOTH position to ground the A, C, and F pins of S545.

<!-- page 3-11 -->

**[Figure 3-3. Store-Non Store Vertical Switching — block diagram showing CH1 Position (R112) -> Position Ampl (Q114,Q115) -> Position Signal Conditioning -> CH1 Position To Storage; CH1 signal from Paraphase Amplifier -> Common Base Ampl (Q102,Q103) -> POS+/POS-/CH1-/CH1+ and trigger signals to Internal Trigger Ampl -> CH1 Vertical Preamp (U130) -> PCH1 SIG/NCH1 SIG to Channel 1 Signal to Storage Acquisition; Vertical Channel Switching Logic and Chop Blanking (U537A,C,D / U540A,B / U7201 / U7202B), driven by Vertical Mode Controls and ENA STO from Display Controller, produces Chop Blank (to Z-Axis Ampl), CH1 ON/CH2 ON (to Trigger Logic), CH2 Enable, Acquisition Ampl Ena (x2), Store Gate Ena (to Horiz Diode Gate); Channel Switch (CR200,CR201,CR202,CR203,U7202A) combines Store Mode Vertical Signals (DV OUT, UV OUT) from Vector Generator and Channel 2 Vertical Signal into VERTP/VERTN feeding Delay-Line Driver (Q202,Q203,Q206,Q207) with Common Mode Feedback (U225,C225) to Vert Sig To Delay Line. Drawing number 4999-03.]**

<!-- page 3-12 -->

ADD DISPLAY. In the ADD position of S545, both the Set and Reset inputs of U540A are held LO by CR534 and CR537. The Q and /Q outputs of U540A are then both HI, and signal currents from the Channel 1 and Channel 2 Preamplifiers add together to drive the Delay Line Driver.

CHOP DISPLAY. In the CHOP position, the /CHOP ENABLE line is held LO, keeping the Q output of flip-flop U540B HI. This enables CHOP multivibrator U537D to begin switching. The switching rate is determined primarily by the component values of R544, R545, and C545. The output of U537C (the inverted output of the multivibrator circuit) drives U537A and supplies the CHOP clock to flip-flop U540A. The output of U537C also drives U537B, the CHOP Blanking Pulse Generator (see Diagram 9).

Coupling capacitor C547 and resistors R547 and R548 on pin 5 of U537B (see Diagram 9) form a differentiating circuit that produces short duration pulses during the switching of U540A. These pulses are inverted by U537B to generate the Chop Blank signal to the Z-Axis Amplifier. The pulses blank the crt during CHOP switching times.

The Alt Sync signal on pin 2 of U537A (see Diagram 2) is HI except during hold off. While pin 2 is HI, the output of U537C is inverted and passed by U537A to the clock input (pin 3) of U540A. Since the /Q output of U540A is connected back to the D input, and both the Set and Reset inputs are HI, the outputs of U540A switch (change states) with each clock input. The Delay Line Driver is then supplied alternately from the Channel 1 and Channel 2 Preamplifiers at the CHOP rate.

ALTERNATE DISPLAY. In ALT, the /CHOP ENABLE line is held HI, disabling CHOP multivibrator U537D. The output of U537C, the chop blanking signal, is HI. Input signals to U537A are the HI from U537C and ALT SYNC from the Hold-Off circuitry in the A Sweep Generator. The output of U537A is then the inverted ALT SYNC signal that clocks Channel Select flip-flop U540A. The /ALT SYNC clock toggles the outputs of U540A at the end of each sweep so that the Channel 1 and Channel 2 Preamplifiers alternately drive the Delay Line Driver.

STORE MODE DISPLAYS. Under direction from the Display Controller, multiplexer U7201 selects either nonstore or store signals to drive the Delay Line Driver. In NON STORE, the multiplexer switches the Q and /Q outputs of U540A to the Channel Switch to allow the switching sequences just described. However, when STORE is selected, the nonstore analog signal to the Channel Switch is turned off, and the store vertical deflection analog signals are applied to the Delay Line Driver input. The store waveform display is determined by the Display Controller.

The nonstore output transistors are biased off by setting pins 9 and 12 of U7201 LO. The forward bias is removed, and the nonstore path is disabled. Pin 7 of U7201 is switched LO in STORE mode. Inverter U7202B inverts the LO, supplying forward bias to the store output transistors in both Preamplifiers. Selection of either channel signal for digitizing is done by a channel switch IC in the Storage Acquisition circuit (Diagram 10).

The HI STORE ENABLE signal from U7202B also goes to the Sweep Sep circuit to disable that circuit during STORE mode and to Horizontal Diode Gate circuit (Diagram 7) to block the nonstore sweep signals from going to the Horizontal Output Amplifier. To complete the switching to STORE mode, Pin 4 of U7201 is switched HI and applied to Inverter U7202B. The LO output signal from U7202B (STORE) is applied to the Vertical Channel Switch circuit to pass the STORE mode vertical deflection signal to the Delay Line Driver. That same LO signal also goes to the Horizontal Mux to pass the STORE mode horizontal deflection signal to the Horizontal Output Amplifier.

A Z-Axis disabling signal /DIS Z applied to NAND-gate U537B (see Diagram 9) disables the Chop Blanking circuitry for STORE mode displays. (/DIS Z) holds the output of the Chop Blanking circuit HI to block the nonstore Z-axis signals from the Z-Axis Amplifier.

### VERTICAL OUTPUT AMPLIFIER

Vertical Output Amplifier circuitry, shown on Diagram 3, amplifies the vertical signal and drives the crt deflection plates. The Delay Line Driver converts the signal into a signal voltage to drive the Delay Line. Delay Line DL9210 delays the vertical signal so that the leading edge of the triggering signal can be viewed. The BW LIMIT switch reduces the bandwidth of the Amplifier when required by the application. The Vertical Output Amplifier drives the vertical deflection plates of the crt. The A/B Sweep Separation circuit vertically positions the Nonstore B trace with respect to the Nonstore A trace in Alt Horizontal mode displays.

### Delay Line Driver

The Delay Line Driver converts the signal current from the Vertical Preamplifiers or the Store mode Vector Generator circuitry into a signal voltage to drive the Delay Line. Transistors Q202, Q203, Q206, and Q207 form a differential shunt feedback amplifier with the gain controlled by feedback resistors R216 and R217. Amplifier compensation is provided by C210 and R210, and output common-mode dc stabilization is provided by U225. Should the dc voltage at the junction of R222 and R223 move off zero, U225 changes the base current supplied to

<!-- page 3-13 -->

Q202 and Q203 through R202 and R203 to return the output of the Delay Line Driver to an average dc voltage of zero.

Delay Line DL9210 adds about 90 ns of delay to the vertical signal. In that time, the Sweep Generator has sufficient time to start producing a sweep before the vertical signal that triggered the sweep reaches the crt. This permits viewing the leading edge of the triggering signal.

### Bandwidth Limit

BW LIMIT switch S226, C228, C229, and the diode bridge formed by CR226, CR227, CR228, and CR229 reduce the bandwidth of the amplifier when desired. With full 100 MHz bandwidth, R226 is grounded through BW LIMIT switch S226, and the nonconducting diode bridge isolates C228 and C229 from the vertical signal. With limited bandwidth on, R226 is connected to the +8.6 V supply, and the diode bridge is forward biased. The two bandwidth limiting capacitors are then in the vertical signal path, and high-frequency signals (above about 20 MHz) are attenuated.

### Vertical Output Amplifier

The Vertical Output Amplifier drives the vertical deflection plates of the crt. Signals from the Delay Line go to a differential amplifier formed by Q230 and Q231 with low- and high-frequency compensation provided by the RC networks between the emitters. Thermal compensation is provided by thermistor RT236, and overall circuit gain is set by R233. The output stage of the Amplifier is two totem-pole transistor pairs, Q254-Q256 and Q255-Q257, that convert the collector currents of Q230 and Q231 to proportional output voltages. Resistors R256, R258, R257, and R259 are feedback elements and bias voltage dividers. Biasing is set so each transistor in a pair develops one-half the final output voltage on a side. The amplifier output signals drive the Vertical crt deflection plates.

Beam Find is used to keep the vertical trace within the graticule area for locating off-screen and over-scanned traces. When the front-panel BEAM FIND switch opens the contacts of S390 (found on Diagram 9), the direct -8.6 V supply to R261 is removed, and emitter current goes through R261 and R262 in series. The added series resistance reduces the amount of available emitter current and limits the amplifier's dynamic range. In normal amplifier operation, S390 connects the -8.6 V supply directly to R261, and full emitter current is possible in the output transistors.

### A/B Sweep Separation Circuit

The circuit formed by Q283, Q284, Q285, and associated components acts to vertically position the Nonstore B trace with respect to the Nonstore A trace in BOTH Horizontal mode. In the B Sweep interval, the /SEP signal from the Alternate Display Switching circuit (Diagram 6) is LO, and Q283 is biased off. This puts A/B SWP SEP potentiometer R280 in the circuit where it can affect the bias level on one side of the differential current source formed by Q284 and Q285. Changing the bias adds a dc offset current to the Vertical Output Amplifier that moves the B trace vertically with respect to the A trace.

During the Nonstore A sweep interval, the /SEP signal is HI, and Q283 is turned on to isolate potentiometer R280 from the biasing circuit of Q284. The base voltages of Q284 and Q285 are then equal. With the same bias to both sides of the Vertical Output Amplifier, no offset is added to the A trace. In STORE mode, the HI STORE signal placed on the base of Q282 keeps Q283 off, and the A/B Sweep Sep circuit on.

### TRIGGERING

The Trigger Amplifiers, shown on Diagram 4, provide trigger signals to the Sweep Generators from either the Vertical Preamplifiers, the EXT INPUT connector, or the power line. The A&B INT switch selects either Channel 1 or Channel 2 as the trigger source, and the A SOURCE switch selects between internal, line, or external trigger sources. See Figure 3-4 for the block diagram of the trigger amplifiers and switching circuitry.

### Internal Trigger Pickoff

Signals from the Vertical Preamplifiers drive the CH1 and CH2 Internal Trigger Amplifiers with channel selection determined by the VERTICAL and HORIZONTAL MODE switches. Trigger signal pickoff from Channel 1 is done by Q302 and Q303, and Q327 and Q328 pick off the Channel 2 internal trigger signal. The circuitry associated with Channel 2 is the same as that for Channel 1 except for a trigger offset adjustment. Channel 1 trigger signal circuitry is described; equivalent components in Channel 2 perform identically.

Differential vertical signals from the Channel 1 Preamplifier go to Q302 and Q303. These emitter-follower transistors each drive one input transistor in trigger preamplifier IC U310. The collectors of the U310 input transistors in turn supply emitter current to a pair of two current-steering transistors. A compensation and biasing network is connected between the emitters of the input transistors. Trigger Offset potentiometer R309 in the emitter circuit adjusts the bias levels of the two input transistors of U310 to match the dc offsets of the Channel 1 and Channel 2 Trigger Amplifiers.

<!-- page 3-14 -->

**[Figure 3-4. Block diagram of Trigger Amplifiers and Switching — A&B INT switch (S555) and XY/CHOP ENABLE/CH1 ON/CH2 ON from Channel Switching Logic feed Trigger Switch (Q541-Q544, U555, U565) selecting CH1 TRIG / CH2 TRIG to CH1 Trigger Amplifier (Q302,Q303,U310) / CH2 Trigger Amplifier (Q327,Q328,U335), both feeding Internal Trigger Ampl (U350A,B,C,D,E) which outputs X-AXIS to Horiz Output and B Signal to B Trigger Level Comparator (also CH1 Selected to Alternate Display Logic, /CHOP to Status ADC); separately, EXT INPUT -> A Ext Trigger Ampl (Q382,Q384) -> EXT TRIGGER, LINE TRIGGER -> Q397, A SOURCE (S392), BW LIMIT (S226), HF REJ (S438/TBW) all feed Trigger Source Diode Switching (CR372,CR393,CR399,Q419,Q7362) along with A Internal Trigger Signal, producing A TRIGGER SIGNAL to A Trigger Level Comparator. Drawing number 4999-04.]**

<!-- page 3-15 -->

One transistor in each side of the output differential amplifier pairs of U310 has its base bias set to a fixed level by the divider network formed by R321 and R322. The bias voltage of the other transistor in each pair is controlled by the /CH1 TRIG signal from the Trigger Switch circuitry. When the /CH1 TRIG signal is HI, the transistors in each output pair with the collectors connected together (pin 6 and pin 14) are biased on, and the other transistors in the output pairs are off. The collector signal currents of the conducting transistors are equal in amount but of opposing polarity, so the signal is cancelled. When the /CH1 TRIG signal is LO, the other transistors in each pair are biased on, and a differential signal is developed across output load resistors R314 and R315 to drive the Internal Trigger Amplifier.

### Internal Trigger Amplifier

The Internal Trigger Amplifier converts the differential trigger signals from the Vertical Preamplifiers into a single-ended signal that drives the X-Axis Amplifier and the A and B Trigger Level Comparators.

Differential signal current is applied to the emitters of U350D and U350E. The collector current of U350D is changed to a voltage signal and inverted by U350C. The opposite-phase collector current of U350E produces a voltage drop across R359 which is in phase with and adds to the voltage across R360 at the collector of U350C. The summed voltages appear at the base of U350A. Feedback resistor R357 provides thermal bias stabilization for U350C.

Emitter-follower U350A buffers the signal and shifts the dc level back to 0 V. The emitter output signal of U350A drives the X-Axis Amplifier, the B Trigger Level Comparator, and the base of emitter-follower U350B. The emitter signal of U350B in turn supplies the A Internal Trigger signal. The circuit arrangement of U350A and U350B, with the common collector current path through R363, produces thermal bias stabilization of the two transistors.

### Trigger Switching Logic

Either Channel 1, Channel 2, or VERT MODE Internal Trigger signals may be selected by A&B INT switch S555 when A SOURCE switch S392 is set to INT. The INT position of the A SOURCE switch applies a voltage that reverse biases both CR393 and CR399 to stop the external trigger signal and the line trigger signal from reaching the A Trigger Level Comparator. The A Internal Trigger Signal from the emitter of U350B is passed to the A Trigger Level Comparator through forward-biased diode CR372.

CHANNEL 1. For triggering from Channel 1, the A&B INT switch is set to CH 1. The XY line connected to S555 is at ground potential, holding pin 4 of U555B LO. The output of U555B is then also LO, and the Channel 1 signal has a path through U310. At the same time, the Channel 2 signal path through U335 is shut off by the outputs of U555C and U565B both being HI.

CHANNEL 2. For triggering from Channel 2, the A&B INT switch is set to CH 2, and U555C pin 10 and U555D pin 12 are LO. The outputs of both AND-gates are then forced LO. A LO output from U555C enables the Channel 2 signal path through U335, and the HI outputs from U555B and U565C disable the Channel 1 path through U310.

VERT MODE. When the A&B INT switch is set to VERT MODE, the trigger source is selected by the two VERTICAL MODE switches. For all VERTICAL MODE switch combinations except BOTH-CHOP, the base of Q541 is HI. The inputs and outputs of U555B, U555C, and U555D are then all HI, and trigger signal selection is done by flip-flop U540A in the Channel Switch Logic circuit (Diagram 2) using the CH1 ON and CH2 ON control signals going to U565B and U565C.

With Channel 1 selected (VERTICAL MODE switch set to CH 1), both inputs to NAND-gate U565C are HI. The output of U565C is then LO, and U310 is biased on to select Channel 1 as the Internal Trigger signal source. The LO CH2 ON signal from the /Q output of U540A is applied to U565B, and the /CH2 TRIG line at the output of U565B is forced HI to shut off the Channel 2 Trigger signal path.

When Channel 2 is selected (VERTICAL MODE switch set to CH 2), the outputs of U540A, U565B, and U565C will be the reverse of the states described for Channel 1 selection. The Channel 2 signal is then selected as the Internal Trigger signal source, and the Channel 1 Trigger signal path through U310 is shut off.

With ALT VERTICAL MODE selected, the inputs of NAND-gates U565B and U565C toggle (change state) with each sweep. The outputs of the two gates also toggle, and U310 and U335 are alternately biased on to select the displayed channel signal as the Internal Trigger source.

In the ADD VERTICAL MODE position, both inputs to U565B and to U565C are HI, making the outputs of both gates LO. Both the Channel 1 and the Channel 2 signal path are turned on by biasing on U310 and U335 together. The output currents of both Trigger Preamplifiers are summed in the Internal Trigger Amplifier to produce the Internal Trigger signal.

<!-- page 3-16 -->

The CHOP VERTICAL MODE position grounds the base of Q541 and puts a LO on an input of both U555B and U555C. The outputs of these two gates are then LO, and the signal to the Internal Trigger Amplifier is the summed Channel 1 and Channel 2 trigger signals, the same as with ADD VERTICAL MODE.

### A External Trigger Amplifier

The A External Trigger Amplifier buffers signals from the EXT INPUT connector to drive the A Trigger Level Comparator. Input signal coupling is determined by A EXT COUPLING switch S380 which selects AC, DC, or DC ÷ 10 coupling.

When S380 is in the AC position, the input signal is ac-coupled through C376. In the DC position, the input signal is connected directly to the Amplifier. The DC ÷ 10 position attenuates the input signal by a factor of 10 through the compensated divider formed by R377, R378, C380, and C381.

### Line Trigger Amplifier

The Line Trigger Amplifier supplies a line-frequency trigger signal to the A Trigger Level Comparator when the A SOURCE switch is in the LINE position. Transformer T390 in the Power Supply (Diagram 8) provides the line-frequency trigger signal through R397 to Q397. Diode CR399 is forward biased when S392 is in the LINE position, and the emitter signal of Q397 drives the A Trigger Level Comparator.

### Trigger Signal BW Limit and HF REJ

The upper frequency of the trigger signal and the vertical channel bandpass are limited to 20 MHz when the front-panel BW LIMIT switch is pressed in. The BW Limit signal voltage forward biases Q419, and capacitor C419 shunts the higher trigger signal frequencies to ground through the transistor. With full 100 MHz bandwidth, Q419 is biased off to remove the shunting effect from the trigger signal line.

An additional bandwidth limiting circuit provides high-frequency rejection of the trigger signal. HF REJ is enabled when the center knob of the A TRIGGER LEVEL control is rotated clockwise. With HF REJ, Q7362 is biased on, and capacitor C7362 shunts trigger signal frequencies above about 50 kHz to ground through the transistor.

### P-P Auto Trigger Level

The P-P Auto Trigger Level circuit sets voltage levels at the ends of the A TRIGGER LEVEL potentiometer (R438) as a function of the A Trigger mode selection and the trigger signals selected by the A SOURCE switch.

In the P-P AUTO and TV FIELD Trigger modes, Q413 is biased off, and CR414 and CR415 are reverse biased. Trigger signals selected by the A SOURCE switch are sent to peak detector circuits formed by Q420-Q422 and Q421-Q423 via R420. These peak detectors track dc levels and have high voltage-transfer efficiency. The circuit arrangement of the transistors produces very low thermal drift and reduces the effect of differences in transistor characteristics.

The positive- and negative-peak signal levels are stored by hold capacitors C414 and C415. The charge on the capacitors is held near the peak voltage levels between trigger signal peaks by the long time constant discharge path through R426 and R427. Amplifiers U426A and U426B are voltage followers with feedback supplied by transistors Q428 and Q429. These feedback transistors compensate the P-P Auto Trigger Level circuit for any thermal drift of Q420 and Q421 and shift the output levels of the voltage followers back to the original dc levels of the input trigger signal peaks. The output of U426A is the positive peak voltage of the input trigger signal, and the output of U426B is the negative peak voltage. Auto Level Adjustment potentiometers R434 and R435 provide dc offset corrections to make certain that the output voltages applied to the ends of LEVEL potentiometer R438 remain at or just below the actual peaks of the input trigger signal. In this way, the range of the LEVEL control is held within the peak-to-peak limits of the applied trigger signal for ease in triggering the oscilloscope.

In NORM Trigger mode, +8.6 V is applied to the junction of R411 and R414. Diode CR414 is forward biased. Transistor Q413 is also turned on inverting the applied signal and forward biasing CR415. Input transistors Q420 and Q421 are then biased off, and no trigger signals reach the P-P Auto Trigger Level circuit. In this case, the inputs to U426A and U426B are fixed voltages, and the voltage levels applied to the ends of the LEVEL potentiometer are independent of trigger-signal amplitude. The user must then adjust the LEVEL control to the correct level to obtain triggering.

The Microprocessor is informed of the trigger mode by Q7440 and its associated biasing resistors. When the /P-P signal line is a LO at -8.3 V (indicating that the P-P AUTO Trigger mode is in effect), Q7440 is biased off, and its collector (and the PP signal line to the I/O circuit board) is pulled up to the +5 V supply via R7442. When the /P-P signal is a HI at +8.5 V for NORM Trigger mode, Q7440 is biased on, and the PP signal is pulled LO by the conducting transistor.

### A Trigger Level Comparator and Schmitt Trigger

The A Trigger Level Comparator compares the level of trigger signals selected by the A TRIGGER SOURCE

<!-- page 3-17 -->

switch to the voltage set by the A TRIGGER LEVEL control and produces an output trigger signal at the correct level. Rising or falling slope triggering is selected by the front-panel A TRIGGER SLOPE switch.

Integrated circuit U460, contains the A Trigger Level Comparator and Schmitt Trigger circuitry. The output voltage of the trigger amplifiers are applied to U460 pin 4. The other input to the comparator is the wiper voltage on the A Trigger LEVEL control, applied to pin 2 of U460. The resistor R452 and the voltage at pin 5 of U460 sets the emitter current for the comparator.

The Trigger Slope is determined by the relative voltages on U460 pins 7 and 8. If pin 8 is at a higher level than pin 7, the plus output of U460 will change to a HI state when a positive-going input signal crosses the threshold at pin 2 of U460. With pin 8 more negative than pin 7, the Schmitt fires on a negative-going input. The voltage at pin 7 is fixed, while that at pin 8 is selected by the A TRIGGER SLOPE switch S460 through R459, R461, and R462.

The sensitivity of the Schmitt Trigger is controlled by the current at pin 9. The setting of R471 determines the circuit hysteresis.

The outputs of the Schmitt Trigger are at pins 10 and 12 of U460. The outputs are at ECL levels and are from emitter followers internal to U460. Collector voltage to U460 is supplied through pins 11 and 14. When TV Field is not selected, the /SS line connected to CR476 and R473 is LO. Transistors Q473 and Q474 are biased off which also biases Q487 off. Resistor R477 biases CR467 and CR477 on and the +Out Trigger signal from pin 10 of U460 passes through the diodes to U506-6 of the A Sweep Generator.

### TV Trigger Circuit

When TV FIELD mode is selected the /SS line is HI. This disconnects the high-speed trigger path by reverse-biasing CR467 and CR477. Setting the A Trigger level threshold near the center of the horizontal-sync-pulse swing establishes the untriggered level. This in combination with the peak detectors makes the circuit insensitive to the video information. The A TRIGGER and LEVEL controls are set to provide a pulse-train corresponding to the sync pulses of the TV signal. This pulse train is filtered by R467, C467, R468, R469, C469, and R470, resulting in dc levels at the bases of Q473 and Q474. The untriggered level (horizontal pulses) turns Q474 on, which causes Q487 to conduct, providing a LO to the sweep generator. When the TV-Vertical-Sync block occurs the polarity reverses, turning Q487 off and providing a positive-going signal to U506 pin 6 to initiate a sweep.

## A SWEEP GENERATOR AND LOGIC

The A Sweep Generator and Logic circuitry, shown on Diagram 5, produces a linear voltage ramp that drives the Horizontal Preamplifier in the Nonstore mode. The Sweep Generator circuits also produce gate signals that time the crt unblanking and intensity levels for viewing the Nonstore displays. In STORE mode, the A Sweep Generator and Logic circuitry continues to produce timing gates used by the Storage circuitry for triggering the analog signal acquisitions. See Figure 3-5 for the block diagram of the A Sweep Generator and Logic circuitry.

The Sweep Logic circuitry controls the Nonstore hold-off time and generates gating signals that start the sweep when a trigger signal occurs and end the sweep at the proper level. When using P-P AUTO or TV FIELD triggering, the Sweep Logic circuitry causes the Sweep Generator to free run if a trigger signal is not received or does not come often enough.

### A Miller Sweep Generator and SEC/DIV Switching

The A Miller Sweep Generator is an integrator circuit that produces a linear voltage ramp to drive the Horizontal Amplifier for the Nonstore A Sweep deflection. It produces the ramp voltage by maintaining a constant current through timing capacitors, causing a linear voltage rise across them as they charge.

Field-effect transistors Q704A and Q704B are matched devices with Q704B acting as the current source for Q704A. Since the gate and source of Q704B are connected together with no voltage difference between them, the source current available to Q704A is just enough so that there is no voltage drop across the gate-source junction of Q704A.

When the sweep is not running, Q701 is biased on, holding the selected timing capacitors discharged. The low impedance of Q701 in the feedback path holds the A Miller Sweep output (A SWEEP) near ground potential. The voltage across Q701, in addition to the base-emitter voltage of Q706, prevents Q706 from becoming saturated.

A sweep ramp is started when Q576 is biased off. The /A GATE signal going to the base of Q701 from the Sweep Logic circuit turns Q701 off. The timing capacitors then begin charging at a rate set by timing resistors R701, R702, and the selected timing capacitors. Due to feedback from the circuit output through the timing capacitors, the integrator input voltage at the gate of Q704A remains fixed and sets a constant voltage across the timing resistors. This constant voltage produces a constant charging

<!-- page 3-18 -->

**[Figure 3-5. A Sweep Generator and Logic circuitry — SEC/DIV VAR (R721) -> HOR-REF -> A Timing Switch and RC (S701,R701,R702,C701,2,3); VAR HOLDOFF (R9521) -> Holdoff Timing (Q470,Q471,Q472,U504B); A TRIGGER MODE (S401) and A TRIGGER -> Auto Baseline (Q502,Q509,Q7420,U532D) [outputs /P-P TO TRIGGERS, NO TRIG] -> A Sweep Logic (Q511,Q521,Q522,Q523,Q576,Q578,U532A,B,C) [ALT SYNC in/out] -> A GATE to Z-Axis; A Sweep Logic and Timing Switch feed A Miller Sweep (Q527,Q701,Q706,U704A,B) which outputs A SWEEP to Horizontal and B Sweep Generator, gated by /A GATE; End of Sweep Comparator (Q525) feeds back into A Miller Sweep and A Sweep Logic. Drawing number 4999-05.]**

<!-- page 3-19 -->

current through the timing capacitors, which results in a linearly increasing voltage ramp as they charge. The ramp is the A SWEEP output signal at the collector of Q706.

Parallel timing capacitors C702 and C703 remain in the charging circuit for all SEC/DIV switch settings and are used mostly for high sweep speeds. Capacitors C701A and C701B are added in series at medium sweep speeds, and C701B alone is added to the charging path for slow sweep speeds.

When the ramp reaches approximately 12 V, the End-of-Sweep Comparator transistor (Q525) becomes forward biased. This action switches the /A GATE HI and starts the analog hold-off period. During hold off the A Sweep Generator is reset. The /A GATE signal going HI biases on Q701, and the timing capacitors are fully discharged before another sweep starts.

One end of timing resistor array R701 is connected to the HOR REF signal, and the other end is connected to the input of the Miller integrator by the SEC/DIV switch contacts. The voltage applied to the timing resistor array via the HOR REF signal varies with the setting of the front-panel Variable SEC/DIV control (R721, located on Diagram 7). The STORE mode time base is not affected by the variable potentiometer setting. In the CAL position of R721, a fixed reference level is applied to R701 to produce the calibrated Nonstore sweep speed ranges. Switch contacts actuated using the knob of R721 control the STORE mode 4K/1K Compress and the X10 MAG features. The X10 MAG feature works in both NONSTORE and STORE.

Coded analog signals developed by circuitry connected to the SEC/DIV switch contacts inform the Microprocessor of the A SEC/DIV switch setting. The Microprocessor then directs the Digital Time Base circuitry to set the correct STORE mode sampling rate.

### A Sweep Logic

The A Sweep Logic circuitry controls sweep generation, as a function of incoming trigger signals and the A Trigger mode selected.

Incoming trigger signals from the output of U460 clock U502, a one-shot multivibrator, and cause the Q output of U502 to go HI. If another trigger signal is not received by U502 within the time limit determined by R503 and C501, the Q output (U502 pin 3) will go LO. Whenever trigger signals are being received, the /Q output of U502 biases on Q509 to turn on DS518, the TRIG'D LED. The output of U502 is also used in the Auto Baseline circuit as described in the "P-P AUTO and TV FIELD" part of the discussion that follows.

NORM. When NORM Trigger mode is selected, input pin 12 of U532D is held HI by S401B, causing the gate output to also be HI. The output of U532C is then LO, and U506 pin 3 is not held HI. Input pin 4 of U532A is held HI by S401C, causing the output to be LO, placing a LO on input pin 7 of dual flip-flop U506. Trigger signals received at input pin 6 (a clock input) of U506 then clock this LO to the Q output (pin 2).

During the previous hold-off period, U506 pin 2 was set HI by U532B. This made the /Q output (pin 3) LO. The LO biased Q576 on, preventing the A Miller Sweep from running. Whenever U506 pin 6 is clocked by a trigger signal following hold off, the LO on the D input (pin 7) is transferred to the Q output (pin 2), and the /Q output (pin 3) goes HI. This biases Q576 off, and the A Miller Sweep generates the sweep ramp as described in the previous "A Miller Sweep Generator" discussion. When the ramp voltage reaches about 12 V, End-of-Sweep transistor Q525 is biased on. The output of U532B then changes from LO to HI, setting U506 pin 2 HI and biasing on /A GATE transistor Q576. This triggers Hold-off One-shot U504B to start the hold-off period, turning off Q525. Transistor Q701 in the A Miller Sweep generator is also biased on to discharge the timing capacitors during hold-off time.

With U504B triggered, output pin 10 changes from LO to HI, where it stays for a time set by the Hold-Off Timing circuitry and the A SEC/DIV switch position. VAR HOLD-OFF potentiometer R9521 sets the amount of current that is available to charge C518, C519, or C520 to the threshold voltage on pin 14. During the time pin 10 is HI, pin 5 (the set input) of U506 is held HI so that trigger pulses cannot start a new sweep. When pin 15 of U504B reaches the threshold level on pin 14, pin 10 goes LO to end hold off and release U506 from the set condition. The circuit is then reset to start another sweep on the next trigger pulse that appears at the clock input (pin 6) of U506. The holdoff capacitors are switched by transistors Q7470 and Q7471 according to the states of the timing switch. Q7472 serves as a dual diode to carry the discharge current. Logic signals AC-1 and AC-2 provide part of the timing switch information for the I/O board, where their states are read at an input port.

P-P AUTO and TV FIELD. When P-P Auto or TV Field trigger is in use, the Auto Baseline circuitry is active. Pin 12 of U532D is held LO by R569, and the output at pin 9 follows the signal provided by the Q output of U502.

<!-- page 3-20 -->

If trigger signals are being received, U502 remains set. As long as U502 is set, the output of U532D is HI, causing the output of U532C to be LO. Dual flip-flop U506 then responds to trigger signals at Clock input pin 6 as described in the "NORM" part of this discussion. If trigger signals are not being received by U502, its output and the output of U532D are both LO. With a LO on pin 10 of U532C, its output is the inverse of the input signal applied to pin 11. At the end of hold-off, that output goes HI, making U506 pin 2 LO and pin 3 HI. This automatically generates the A Gate and /A Gate signals, generating a sweep. The Auto Baseline continues holding NOR-gate U532C enabled so that new sweeps are generated at the end of hold-off as long as trigger signals are not received at U502.

SGL SWP. The following discussion presumes Nonstore mode. In Sgl Swp mode, both the P-P AUTO and NORM front-panel buttons are in their out position. This results in a LO at the output of U532C that does not permit flip-flop U506 pin 3 to be held HI. A LO is also on input pin 4 of U532A.

During hold-off, U532B makes U506 pin 14 HI and pin 15 LO, causing pin 7 (the D input) of U506 to be HI. After hold-off ends, clock signals (triggers) to U506 pin 6 keep U506 pin 3 LO, keeping the sweep generator held off. When the SGL SWP button is pushed in, pin 7 of U504A goes LO for a time period determined by the time constant of R504 and C504 and then returns HI. The HI clocks the HI on input pin 10 of U506 to output pin 15. Consequently the output of U532A goes LO, and CR514 is reverse biased to bias Q511 on, lighting the READY LED. The next trigger pulse applied to input pin 6 of U506 starts a sweep as described previously. At the end of the sweep, U506 pin 15 goes LO and pin 14 goes HI, causing the TRIG'D LED to go out and placing a HI on the input pin 7 of U506. A new sweep cannot be started until the SGL SWP button is again pressed, resetting the sweep.

In STORE mode, the major difference is that the STO-RDY line is not true until the processor recognizes that a trigger has occurred. This prevents the SGL SWP button from affecting the circuit directly. Instead, the processor determines the button was depressed, releases STO-RDY, causing the effect described above when a button is depressed in Nonstore mode.

X-Y. In the Nonstore X-Y mode, the /XY signal is LO and Q522 is biased on, pulling pin 7 of U532B LO. The output of U532B holds U506 pin 3 LO and pin 2 HI, and no sweeps can be started during X-Y mode. Nonstore X-Axis deflection (horizontal) is determined by the CH 1 OR X input signal. In STORE mode, the A Sweep Logic circuit must run to produce the gating required to synchronize the Storage signal acquisition. The Store signal forward biases CR7140 to override the /XY signal, and the A Sweep Logic circuitry operates as in Y-T Nonstore mode.

## B TIMING AND ALTERNATE B SWEEP

The Alternate B Sweep circuitry, shown on Diagram 6, produces a linear voltage ramp that drives the Horizontal Preamplifier for Nonstore B Sweeps. The Alternate B Sweep circuitry also produces the sweep-switching signals that control the display of the A and B Nonstore Sweeps and the gate signals used by the Intensity and Z-Axis circuits to set the crt unblanking and intensity levels for the Nonstore A Intensified and the B Sweep displays. The B Gate signal goes to the Digital Time Base circuitry and is the Storage trigger signal for B Delayed Horizontal Display mode.

The B Sweep ramp is started by the B Sweep Logic circuit either at the end of the set delay time (RUNS AFTER DELAY) or when the first trigger signal occurs after the delay time has elapsed (Trigger After Delay). This delay time is a function of the B Delay Time Position Comparator circuit and the A Sweep.

### B Miller Sweep Generator

The B Miller Sweep Generator is an integrator circuit formed by Q709, Q710A, Q710B, Q712, and associated timing components. This circuit produces the B Sweep signal and works the same as the A Miller Sweep Generator. See the "A Miller Sweep Generator" section for a description of circuitry operation. The output at the collector of Q712 drives the Horizontal Amplifier for Nonstore B Sweeps and is applied to the B end-of-sweep transistor, Q643.

### B Trigger Level Comparator and Schmitt Trigger

The B Trigger Level Comparator and Schmitt Trigger are contained in U605. This circuit determines both the trigger level and slope at which the B triggering signal is produced. It functions in the same manner as the A Trigger Level Comparator and Schmitt Trigger with the exclusion of the TV trigger circuitry. See the "A Trigger Level Comparator and Schmitt Trigger" section for a description of circuit operation. The +OUT terminal of U605 is directly connected to the clock input of U670A to initiate the B Sweep when the B Trigger is utilized.

### Run After Delay

The Run After Delay circuit lets the B Sweep Logic start a B Sweep without the need for a B Trigger signal. For the RUNS AFTER DELAY mode, B TRIGGER LEVEL

<!-- page 3-21 -->

control R602 is rotated fully clockwise. In this position of R602, transistor Q637 is biased off, and a LO is present at its collector. Inverter U660D then has a HI output at pin 8. Resistor R640 provides positive feedback to obtain rapid switching of the transistor. This HI output reverse biases CR626 so that the state of U670A is determined by the level at U660F pin 12.

If the B TRIGGER LEVEL control is not fully clockwise, Q637 is biased on, and the B Sweep is in the triggerable-after-delay mode. The output of U660D is then LO which keeps the S input of U670A LO, preventing the flip-flop from being set by the output of U660F.

Operation of the B Sweep Logic circuitry under both triggering modes is described in the "B Sweep Logic" part of the following discussion.

### Delay Time Position Comparator

The Delay Time Position Comparator circuit compares the amplitude of the A Sweep voltage ramp to the dc voltage level set by the position of B DELAY TIME POSITION potentiometer R9644. The output of the comparator enables the B Sweep Logic circuit to start the B Sweep after the end of the delay time.

The input voltages to Comparator U655 to be compared are the voltage from the wiper of B Delay Time Position potentiometer R9644 and the A Sweep voltage from the divider formed by R651, Delay Dial Gain potentiometer R652, and R653. Maximum and minimum input voltages are established by VR645 and R646 respectively for the noninverting input and by R652 for the inverting input. Delay Start potentiometer R646 is adjusted in conjunction with Delay End potentiometer R652 to set the B DELAY TIME POSITION crt readout calibration.

The comparator is controlled by the /A ONLY gate signal connected to pin 6. When the /A ONLY signal is HI, the comparator is able to make a comparison. While the A Sweep signal on pin 3 is below the wiper voltage on pin 2, the comparator output is at a HI level. When the A Sweep ramp reaches the comparison level, the output at pin 7 goes LO. If /A ONLY is LO, the comparator is switched to a high impedance output state. The comparator output level is then a HI that goes to pin 9 of NAND-gate latch U680C and U680D.

### B Sweep Logic

The B Sweep Logic circuitry utilizes signals from associated B Sweep circuitry to generate control signals for both the B Miller Sweep and the B Z-Axis Switching Logic circuits.

In the RUNS AFTER DELAY mode, the Run After Delay circuit holds the D input of flip-flop U670A LO via U660B. At the start of hold off when the A Sweep is reset, U680D pin 13 is strobed with an Alt Sync pulse negative transition. The output of the NAND-gate latch formed by U680C and U680D is latched HI, and the output of U660F goes LO. This places a LO on the S input of U670A and a HI on the R input causing the flip-flop to reset. The LO on pin 2 and a HI on pin 3 of U670A are converted to TTL levels by Q630 and Q631. The resulting HI on the collector of Q630 turns Q709 on. This discharges the B Miller Sweep timing capacitors to reset the B Sweep Generator and keeps a new B Sweep from starting. During the next A Sweep ramp when the voltage at U655 pin 3 exceeds the voltage at pin 2, the comparator output goes LO. The NAND-gate latch changes output states and causes the Set input of U670A to go HI. The LO on the Set input then controls the flip-flop, and the Q output of U670A goes LO. Shunting transistor Q709 shuts off, and the B Miller Sweep Generator runs to produce a sweep ramp.

When the ramp voltage reaches a level of about 12 V, B end-of-sweep transistor Q643 turns on and blanks the rest of the B Sweep trace by reverse biasing CR817 in the Z-Drive signal line (Diagram 9). The B Sweep Generator continues to run either until the ramp reaches about 13 V, at which time VR712 conducts to prevent the ramp voltage from increasing further, or until the A Sweep ends. In either case, the B Sweep generator is reset when the A Sweep ends.

The B Sweep Generator becomes reset when the ALT SYNC signal goes from HI to LO to switch the output state of the U680C-U680D latch. The Reset input of U670A then goes LO, causing the /Q output to switch HI and reset the Sweep Generator. Depending on the settings of the A and B SEC/DIV switches, the A Sweep may end before the B Sweep. In that case, the ALT SYNC signal going LO at the end of the A Sweep immediately resets the B Sweep Generator even if the sweep ramp has not reached its maximum amplitude. A new B Sweep starts the next time the B Delay Time Comparator goes LO.

When not in the Runs After Delay mode, the output of U660A is HI, and U670A has a HI on both the Set and the D input. The circuitry connected to the Reset input of U670A functions as described before. When the output of U660F goes HI, U670A is no longer held reset. In this case, the first B Trigger signal from the collector of Q630 after the end of the delay time clocks through the HI on the D input, setting flip-flop U670A. The /Q output of U670A is then LO, and a B Sweep is started by reverse biasing Q709 in the B Miller Sweep as before.

<!-- page 3-22 -->

### Alternate Display Switching Logic

The Alternate Display Switching Logic circuitry controls both the Nonstore Horizontal Amplifier sweep switching and the Nonstore Z-Axis Logic switching for A Inten and B Only traces. The B Sweep ramp and gates are produced for every A Sweep when the HORIZONTAL MODE is set to either ALT or B. In ALT, the intensified zone on the A Sweep trace is shown for one B Sweep interval, and during the next A Sweep interval, a B Sweep trace is displayed during the B Sweep interval. For B Only traces, the A Sweep must still run to produce the A gating signals used throughout the circuitry for timing, but it is not displayed.

HORIZONTAL MODE switch S648 selects the input logic levels that drive the display switching circuitry. In the A Horizontal mode, the Set input of U670B is LO, and the Reset input is HI. This holds U670B reset with the A DISP signal HI, passing only the A Sweep to the Horizontal Amplifier (by the A Sweep selection transistor, Q742, located on Diagram 7). In the B Horizontal mode, the set input of U670B is HI, and the reset input is LO. This holds U670B set with the B DISP signal HI, allowing only the B Sweep to reach the Horizontal Amplifier (via the B Sweep selection transistor, Q732).

With S648 set to ALT, and for all settings of the VERTICAL MODE switches except BOTH-ALT, the /VALT signal applied to U660E is HI and the Set and Reset inputs of U670B are both LO. The LO out of U660E causes the output of U680B to be HI. Each HI to LO transition of the ALT SYNC signal applied to pin 1 of U680A causes the NAND-gate output at pin 3 to change from LO to HI, clocking U670B. The Q and /Q outputs of U670B therefore toggle, and the A DISP and B DISP signals cause the sweep selection transistors (Diagram 7) to alternately pass the A and B Sweep signals to the Horizontal Amplifier.

When the CH 1-BOTH-CH 2 VERTICAL MODE switch (S550) is set to BOTH, the ADD-ALT-CHOP switch (S545) becomes active. In the ALT VERTICAL MODE position, the /VALT signal is LO, the HALT signal is HI, and the CH 1 SELECTED signal is a TTL square-wave signal that switches states at the end of the A Sweep. Input pin 4 of U680B is HI, and the gate output is the inverted CH 1 SELECTED signal. This output signal is combined with the ALT SYNC signal by NAND-gate U680A to clock U670B. Whenever the ALT SYNC signal goes LO at the end of a sweep and the CH 1 SELECTED signal (at U680B pin 5) switches from LO to HI, U670B is clocked. Since only positive transitions on the clock input causes the flip-flop to change output states, two A Sweeps must occur to cause the flip-flop output levels to switch. Switching this way, the crt first displays two A Intensified Sweeps, then two Alternate B Sweeps.

SWP SEP. Whenever the B Sweep is selected to drive the Horizontal Amplifier, the Q output of U670B is HI. This HI goes to U665C pin 10 through Q683 and Q687, and since pin 9 is also HI, the /SEP signal from U665C is LO to enable the A/B Sweep Separation circuitry (located on Diagram 3).

### B Z-Axis Logic

The B Z-Axis Logic circuitry switches signal current levels to drive the Z-Axis Amplifier for the Nonstore B Sweep and the A Intensified Sweep displays. The current supplied is summed with the other signal inputs on the Z-DRIVE line to set the Nonstore display intensity levels.

With the HORIZONTAL MODE switch in the ALT position, pin 5 of U665B is HI. Then, the Q and /Q outputs of U670B, the /B GATE signal from the output of U665D, and the B INTENSITY potentiometer, set the intensity levels of the Nonstore A Intensified and B Sweep traces. When the A Sweep trace is displayed, the /Q output of U670B is HI, and the Q output is LO. These output levels bias Q683 on and bias Q682 off. The collector voltage of Q683 reverse biases CR817 to stop Z-Axis drive current from flowing through the diode. With CR683 reverse biased, additional Z-Axis drive current to intensify the A Sweep is supplied whenever CR685 is biased off by the gating action of U665B. Since input pin 5 of U665B is HI, the gate output and therefore the conduction state of CR685 is set by the B GATE signal from U660C. While the B GATE is HI, the output of U665B is LO, and CR685 is biased off to add B INTENS current to the Z-DRIVE line via CR816. During periods that the B GATE is LO (B Sweep not running), the output of U665B is HI, and CR685 is biased on. Diode CR816 becomes reverse biased, and the extra current that was being supplied to the Z-DRIVE line to intensify the A Sweep is removed.

With the Q and /Q outputs of U670B switched to display the B Sweep (/Q LO and Q HI), Q683 is biased off, and Q682 is biased on. The collector voltage of Q682 reverse biases CR816 to block any Z-Axis drive current from being supplied through that diode. With CR687 off, the B Sweep is displayed if CR680 is reverse biased. During the B Sweep interval, the /B GATE output at pin 11 of U665D is LO. Diode CR680 is then reverse biased, and Z-Axis drive current from B INTENS flows through CR817. If the B Sweep is not running, the /B GATE output of U665D is HI. That HI forward biases CR680 and reverse biases CR817. No B Z-AXIS drive current flows through CR817.

## HORIZONTAL

The Horizontal Amplifier circuit, shown on Diagram 7, provides the signals that drive the horizontal deflection plates of the crt. Signals applied to the Horizontal Preamplifier may come from either the A or the B Miller Sweep Generator (for sweep deflection) or from the XY Amplifier (when Nonstore X-Y display mode is selected). A and B Sweep switching is controlled by signals from the Alternate Display Switching Logic circuit discussed earlier. Either the Nonstore sweeps or the Storage horizontal deflection signals are passed to the Horizontal Output Amplifier via a diode gating circuit. Signal selection by the Horizontal Mux circuit is controlled by the Channel Switch Logic output signals (located on Diagram 2). See Figure 3-6 for the block diagram of the Horizontal Amplifier.

The Horizontal POSITION control, X10 Magnifier circuitry, and the horizontal portion of the Beam Find circuitry are also part of the Horizontal Amplifier circuitry.

### Horizontal Preamplifier

The Horizontal Preamplifier switches the Nonstore horizontal drive signals and amplifies input signals for application to the Horizontal Output Amplifier.

The A and B Sweeps are applied to the emitters of Q732 and Q742, through Sweep Gain potentiometers R730 and R740. Switching of the A and B Sweeps is controlled with these transistors. Using the A DISP and B DISP signals obtained from the Alternate Display Switching Logic circuitry (Diagram 6), Q732 and Q742 are either biased into the active or cutoff regions via CR732 and CR742. The POSITION control (R726) horizontally adjusts the crt trace position by supplying a variable dc offset voltage, through pin 14, to the output of the preamplifier. The position offset voltage from the wiper of R726 also goes to the Vector Generator circuitry (Diagram 20) to horizontally position the STORE mode waveform displays. Readout displays are not affected by the Horizontal POSITION control. Preamplifier output bias current levels are set by R751 at pin 5, and frequency compensation for X-axis signals is provided by C751, connected to pin 13.

Nonstore horizontal X10 Gain is set by the resistor network between pins 3 and 6 of U760. When the X10 Magnifier is on, S721 is closed, and the amplifier gain increases by ten times. Magnified timing accuracy is adjusted using X10 Gain potentiometer R754. MAG REGIS potentiometer R749 is adjusted for no horizontal shift at the center of the graticule as X10 Magnifier is switched on

<!-- page 3-23 -->

and off. A second set of contacts on S721 informs the Microprocessor whether X10 Magnification is off or on. The SEC/DIV readout is automatically set to the correct scale factor, and STORE mode waveforms are digitally modified to reflect X10 magnification.

### X-Y Amplifier

The X-Y Amplifier amplifies the Nonstore Channel 1 signal (X-AXIS) from the Internal Trigger circuitry (Diagram 4) and passes it to the Horizontal Preamplifier.

When the Nonstore X-Y mode is selected, Q737 is biased on to place a HI on U760 pin 12 to internally disconnect the A and B Sweep and the HORIZ POS input pins. The /XY signal line is LO, biasing Q756 off to let the X-AXIS signal drive the noninverting input of U758. The output of U758 is a combination of the X-AXIS signal on pin 3 and the Horizontal POSITION voltage applied to pin 2 via R758. The X-Axis deflection accuracy is adjusted by X-GAIN potentiometer R760. The single-ended X-AXIS signal at pin 11 of U760 is changed to a differential signal at the preamplifier output pins. The differential signal is passed through the Horizontal Mux circuit to the Horizontal Output Amplifier for final amplification. When the X-Y mode is not selected, Q756 is biased on, and the X-AXIS signal is shunted to ground through the transistor.

### Horizontal Output Amplifier

The Horizontal Output Amplifier provides final amplification of the horizontal Nonstore sweep signals or the Store mode deflection signals to drive the horizontal crt deflection plates.

In NONSTORE mode, signals from the (+) and (-) SWP outputs of U760 drive the Horizontal Output Amplifier. In STORE mode, horizontal LH OUT or RH OUT deflection signals are passed through the diode gate to drive the amplifier. Drive signals for STORE mode and readout character displays are selected by the Display Controller. Either Nonstore sweeps or Store deflection signals are selected by the diode gating using signals from the Store/Nonstore Multiplexer (U7201 on Diagram 2) through inverters U7202A and U7202B.

The selected signals drive a differential shunt-feedback amplifier. Due to the feedback, the input impedance of the amplifier is low. The base voltages of Q770 and Q780 are biased at nearly the same dc level by forward-biased diodes CR765 and CR768 located between the two emitters.

<!-- page 3-24 -->

**[Figure 3-6. Horizontal Amplifier block diagram — X-AXIS and /XY -> XY Amplifier (Q756,U758); HORIZONTAL POSITION (R726), A SWEEP, B SWEEP, A DISP, B DISP, X10 PULL (S721), STORE / /STORE all feed Horizontal Preamp (Q732,Q737,Q742,U760) -> Horizontal Mux (CR7301-8, also fed by LH-OUT/RH-OUT) -> Horizontal Output Amplifier (Q770,Q775,Q779,Q780,Q785,Q789, gated by BEAM FIND S390) -> +/- HORIZ DEFL to CRT Deflection Plates. Drawing number 4999-06.]**

<!-- page 3-25 -->

Transistors Q770, Q775, and Q779, as one-half of the complementary differential circuit, form a cascode-feedback amplifier for driving the right crt horizontal deflection plate. Amplifier gain is set by R775, with C775 providing high-frequency compensation. For low-speed signals, Q779 serves as a current source for Q775. At high sweep rates, the deflection signal is coupled through C779 to the emitter of Q779 to provide added pull-up output current to drive the crt. The amplifier formed by Q780, Q785, and Q789 drives the left crt horizontal deflection plate in the same manner as described above, with zener diode VR782 shifting the collector signal level of Q780 to the correct level to drive the emitter Q785.

The BEAM FIND function is active when S390 is pushed in to disconnect the cathode of CR764 from the -8.6 V supply. The voltage on the cathode of VR764 goes positive, causing CR780 and CR770 to be forward biased. Current from R764 causes the output common-mode voltage of the two shunt-feedback amplifiers to be shifted negative to reduce the available voltage swing at the crt plates. This stops the trace from being deflected off-screen horizontally. The BEAM FIND voltage also goes to the Vertical Output Amplifier, and the vertical deflection is limited in that circuit when the voltage is removed.

A circuit formed by Q7501 and Q7502 supplies reference voltages for the 1 K and 4 K storage acquisitions and for the variable SEC/DIV control, R721. Transistor Q7502 provides a 0.6 V drop from the -8.6 V supply to generate a -8 V reference for the 1K REF and one end of potentiometer R721. The 4K REF is produced by Q7501 and is adjusted by using the RATIO ADJ potentiometer to set the correct ratio for the two reference voltages. This reference level also goes to the other end of R721. The wiper voltage of R721 is the HOR REF voltage for the A and B Sweep timing resistors in NONSTORE mode. In STORE mode, either the 1K REF or the 4K REF voltage level is applied to the A and B Sweep timing resistors. Switching between reference levels for the different modes is done by the Storage Panel ACQUISITION switches (located on Diagram 14).

### Probe Adjust

The Probe Adjust circuitry, shown on Diagram 7, is a square-wave generator and diode switching network that produces a negative-going square-wave signal at PROBE ADJUST connector J9900. Amplifier U985 forms a multivibrator that has an oscillation period set by the time constant of R987 and C987. When the output of the multivibrator is at the positive supply voltage, CR988 is forward biased. This reverse biases CR989, and the PROBE ADJUST connector signal is held at ground potential by R990. When the multivibrator output switches states, and is at the negative supply voltage level, CR988 is reverse biased. Diode CR989 becomes forward biased, and the circuit output level drops to approximately -0.5 V.

## MICROPROCESSOR AND STORE-PANEL CONTROLS

The Microprocessor, shown on Diagram 14, directs the operation of the Storage and digital circuitry in the oscilloscope by following firmware control instructions stored in the Microprocessor memory. The Store-Panel Controls are monitored by the Microprocessor to detect when a Storage operation is selected. The rest of the significant front-panel controls are monitored through the Front-Panel A-to-D converter and I/O interface circuitry. Circuit operation is then directed by the Microprocessor to perform the selected operation.

### Microprocessor, Clock, and Timer

Microprocessor U9111 is the center of control activities. It has an eight-bit combination bidirectional data bus for information transfer and addressing (AD0 through AD7) and a 12-bit address bus for selecting the source or destination of the data transfers (A8 through A19). Precise timing of instruction execution, addressing, and data transfer is provided by an external, crystal-controlled oscillator, shown on Diagram 18 and Clock Generator U9104.

A divide-by-three circuit in Clock Generator U9104 reduces the 20 MHz external input from the crystal oscillator circuit to 6.7 MHz for clocking the Microprocessor. An output from the 6.7 MHz clocking signal also drives the Display Controller (U9208 on Diagram 15) to time those devices. Another clock signal (PCLK) output, at one-half the Microprocessor clock frequency (3.3 MHz), is supplied to the input to U9108, a binary ripple counter that produces a lower frequency timing signal. The 6.7 MHz signal is also included in the Control Bus to provide a clocking signal for future options.

The RESET output of U9104 provides a power-on reset signal under normal operation or a manual reset using jumper connector P9104. The /RES voltage level at pin 11 is held below the switching threshold of an internal Schmitt Trigger circuit after the power is applied for a time period set by the RC time constant of R9107 and C9107. This holds the Microprocessor in the reset state until the power supply voltages are high enough to permit normal operation of the digital circuitry. The Microprocessor is held reset during the delay period. Manually moving jumper P9104 to the RESET position forces a reset of the Microprocessor and the Display Controller.

The only RAM available for general use is the Display RAM. It's access is mediated by the Display Controller and associated circuitry. To allow the Display Controller to have first priority access to the RAM, the RDY1 input to the clock generator is used to tell the Microprocessor to wait for access to the RAM.

<!-- page 3-26 -->

In addition, when one of the Communication Options is installed, the RDY1 input (U9104 pin 4) is used to synchronize the operation of the Microprocessor with the asynchronous activity of the GPIB (General Purpose Interface Bus) or RS-232-C Options for parallel or serial data transfer via the external communications port.

Resistor pack R9113 is a data bus pull-up. During normal operation, the resistor pack generates the interrupt vector pointer. During the hardware kernel test, the resistor pack generates the NOP instruction.

### Latch and Buffer

Addressing is done using dedicated address bus lines. Address latch U9112 demultiplexes the address bus (separates the address and data bytes). When an address is valid, the Microprocessor sets the address-latch enable (ALE) HI (U9111 pin 25). Both U9112 and U9114 are clocked to latch the address bits. The latched bits are held until the Microprocessor places a new address on the busses and again sets the ALE signal HI. Some bits passing through U9114 have status information multiplexed with the address, so U9114 also functions as a demultiplexer.

### Decoder

In addition to providing specific addresses to internal locations within memory devices, the addresses are decoded to provide enabling signals for blocks of addresses and to control the selection of I/O (Input/Output) devices. Table 3-1 shows the instrument's memory map.

In normal operation, address block decoder U9106 is always enabled. One-half of the dual 1-of-4 decoder looks at address bits A14 and A15. Latched address bits A18 and A19 from U9114 are looked at by the second half of the device.

I/O address decoding is performed by U9105. To perform its decoding, it must be enabled by the decoded output of U9106. The lower half of U9105 is controlled by a logic gating circuit formed by U9101D, U9102A, and U9102D. The lower half becomes enabled when either /RD or /WR is Low and BLOCK-0 and IO SEG are both LO. The upper half of U9105 is enabled only when address bits A12 and A13 are both HI, setting pin 9 of U9105 LO.

### ROM

The operating system firmware is contained in two 64K by 8-bit read-only memories (U9110 and U9109). Immediately after the power-up reset ends, the Microprocessor automatically fetches the first command from the reset vector (address 0FFFF0), and begins program execution. Other interrupts to the Microprocessor cause vectoring to addresses that start the interrupt handling routines. The NMI (non-maskable interrupt) vector is at 00008, and the Maskable Interrupt (INTR) is vectored to 03FC (both interrupt vectors are in RAM).

### Store Panel Controls and Buffer

The open or closed position of the Storage Panel Controls is passed to the Microprocessor via two octal bus drivers, U9301 and U9302. Each bus driver transfers eight individual data bits to the data bus when enabled. Enabling of the bus drivers is done by address line A2, which goes to both drivers, and decoded input/output enabling lines, going separately to each driver. Both enabling inputs must be LO on each IC to pass the input data bit to the data bus.

The Microprocessor communicates with the other devices on the data bus via Octal Bus Transceiver U9113. Two signals from the Microprocessor control enabling of the Transceiver and direction of the data flow. When the /DEN signal is LO U9113 is enabled for transfers, and the DT/R signal sets the direction of the transfer. IO/M qualifies the transfer to allow pull-ups to assert an interrupt number on the bus during interrupt cycles. While the address and data are available on the bus side of this transceiver, only the data time slot is used.

### Non-Storage Front-Panel Controls

There are many front-panel controls that do two things at the same time; control the real-time scope mode, and tell the Microprocessor what is being selected or modified. These controls include the vertical position controls, the vertical gain controls, the A and B time per division controls, the three major trigger mode controls, the vertical coupling controls, the sweep mode control, and the delay-time control. In addition, the probe-coding ring is read to determine true Volts per Division. In addition to acting as the user interface to the Microprocessor, the 1K/4K and STORE/NON STORE switches select the reference voltage applied the A and B timing resistors in the Sweep Generator circuitry.

## STATUS ADC AND BUS INTERFACE

Front-panel control settings and the operating status are passed to the Microprocessor via the Bus Interface. Digital signals that can be read directly as data bits are buffered onto the Data bus either via octal bus driver U6102 or U6103. Analog voltages are converted to digital data bytes by analog-to-digital converter U6105. The analog signals are multiplexed to a buffer amplifier either by

<!-- page 3-27 -->

**Table 3-1: Memory Space Allocation**

| Block Designation | Block Address (Hex) | Space Allocation Purpose |
|---|---|---|
| **RAM SEG** | **00000-3FFFF** | **Four images of Memory Segment 0** |
| RAM Primary | 00000-07FFF | 8-bit display RAM—waveforms, interrupt vectors, miscellaneous. |
| | 08000-0FFFF | 4 bits of display RAM for waveform attributes (LSB). |
| | 10000-3FFFF | RAM Images. |
| **IO SEG** | **40000-7FFFF** | **Four images of Memory Segment 1** |
| IO Main Image | 4067C (IO-2 A7,8) | Option Status Latch (in). |
| | 406BC (IO-2 A6,8) | Option Parameters Latch (in). |
| | 406F0 (IO-2 A3,8) | Option UART/GPIB chips (I/O). |
| | 406F1 (IO-2 A3,8) | Option UART/GPIB chips (I/O). |
| | 406F2 (IO-2 A3,8) | Option UART/GPIB chips (I/O). |
| | 406F3 (IO-2 A3,8) | Option UART/GPIB chips (I/O). |
| | 406F4 (IO-2 A3,8) | Option UART/GPIB chips (I/O). |
| | 406F5 (IO-2 A3,8) | Option UART/GPIB chips (I/O). |
| | 406F6 (IO-2 A3,8) | Option UART/GPIB chips (I/O). |
| | 406F7 (IO-2 A3,8) | Option UART/GPIB chips (I/O). |
| | 406F8 (IO-2 A2,8) | Option Interrupt Mask Latch (out). |
| | 407DE (IO-2 A5) | Time Base Mode Register U4119. |
| | 407EE (IO-2 A4) | Time Base Divider Register U4113. |
| | 41XXX (/INT-RST) | Display chip interrupt reset. |
| | 42XXX (/FRAME) | Display chip next frame. |
| | 4377E (IO-0 A7) | Acquisition Memory Address Buffer Low bits U3427. |
| | 437BE (IO-0 A6) | Acquisition Mode Register U3310. |
| | 437DE (IO-0 A5) | B Delay Timer U4123. |
| | 437EE (IO-0 A4) | Record Counter U4115 and U4116. |
| | 437F6 (IO-0 A3) | Front Panel A/D control U6104. |
| | 437FA (IO-0 A2) | Front Panel A/D data U6102. |
| | 4377F (IO-1 A7) | Acquisition Memory Address Buffer high bits U3428. |
| | 437DF (IO-1 A5) | B Delay Timer U4124. |
| | 437EF (IO-1 A4) | Record Counter U4117. |
| | 437F7 (IO-1 A3) | Clock Delay Timer U4231. |
| | 437FB (IO-1 A2) | Main Front Panel Input U6103. |
| | 43FFA (IO-0L A2) | Front Panel Buffer U9301. |
| | 43FFB (IO-1L A2) | Front Panel Buffer U9302. |
| | 48000-4BFFF | Acquisition Memory—Four images of Acquisition RAM U3418 and U3419. |
| IO Duplicate Images | 50000-7FFFF | |
| **COMM SEGMENT** | **80000-BFFFF** | **Two images of Memory Segment 2** |
| Option Main Image | 80000-87FFF | Half of Communication Options ROMs U1243 or U1343. |
| | 88000-8F7FF | Option nonvolatile RAM. |
| | 8F800-8FFFF | Nonvolatile RAM. |
| | 90000-97FFF | Half of Communication Options ROMs U1243 or U1343. |

<!-- page 3-28 -->

**Table 3-1 (cont): Memory Space Allocation**

| Block Designation | Block Address (Hex) | Space Allocation Purpose |
|---|---|---|
| Option Duplicate Image | 98000-BFFFF | |
| **ROM SEGMENT** | **C0000-FFFFF** | **Two images of Memory Segment 3** |
| ROM Duplicate Image | C0000-DFFFF | |
| ROM Main Image | E0000-E7FFF | System ROM 0—Low half of U9109. |
| | E8000-EFFFF | System ROM 1—Low half of U9110. |
| | F0000-F7FFF | System ROM 0—High half of U9109. |
| | F8000-FFFFF | System ROM 1—High half of U9110. |

U6106 for the Vertical status signals or by U6101 for the Horizontal Status signals. The multiplexers are controlled by the Microprocessor via the control bits latched into U6104. The buffer amplifier output drives the input to the ADC. The converted data from the ADC is buffered onto the data bus by U6102.

## STORAGE ACQUISITION

The Storage Signal Acquisition system, shown on Diagram 16, selects the channel or channels for digitizing, samples the signals at clock controlled intervals, and digitizes the samples. The circuitry consists of an analog Channel Switch, a Sample-and-Hold circuit, and the Analog-to-Digital Converter. A Strobe Generator drives the sampling circuitry diode bridge at the /ADCLK rate (20 MHz) for all acquisition modes.

### Channel Switch

With STORE mode selected, both channel signals are applied to analog Channel Switch U2101 where they may be selected for digitizing. Signals are selected by the /CHAN1 or the ADD signals from the Acquisition Memory, shown on Diagram 11. The /CHAN1 signal is derived from the delayed SAVECLK so channel switching takes place at the proper times for the A/D conversion. Both sides of the Channel Switch conduct in ADD Mode, summing the two input signals at the output. See "Channel Select" in the Acquisition Memory discussion for details on channel selection signals.

Differential channel signals are applied to the bases of a pair of transistors within the Channel Switch, at pins 2 and 15 for Channel 1 and pins 7 and 10 for Channel 2. Gain setting and compensation networks are connected between the emitters of both differential pairs in the emitter current source path. A gain setting potentiometer (R2118 for Channel 1 and R2108 for Channel 2) sets the acquisition gain for each channel. Thermistors RT2101 and RT2111 temperature compensate the gain of the circuit. Diodes CR2111 and CR2112 temperature compensate the gain of the circuit at high frequencies. Capacitors C2103 and C2113 set the high-frequency peaking.

The Channel 1 amplifier base biasing voltage is supplied via R2122 and the input termination resistors, R2121 and R2120. The termination resistors provide the proper impedance match between the signal lines from the Vertical Preamplifiers (Diagram 2) and the high impedance inputs of the Channel Switch. Corresponding resistors in the Channel 2 amplifier perform the same job for Channel 2. Selection of the channels is controlled by the inputs at pin 4 and pin 14. The /CHAN1 signal biases on the Channel 1 differential amplifier pair when LO and the Channel 2 differential amplifier pair when HI. The logic level of /CHAN1 is toggled at the proper rate to provide dual-channel operation. Diodes CR2103 and CR2104, and resistors R2128 and R2129 level shift /CHAN1 to the level required by U2101.

For ADD Mode, the /CHAN1 signal is held LO and the ADD signal applied to pin 14 is switched HI by the Microprocessor via Acquisition Mode Register U3310, shown on Diagram 17, biasing on both the Channel 1 and the Channel 2 amplifiers. The resulting output current is the sum of the input signals applied to Channel 1 and Channel 2. Diodes CR2101 and CR2102, and resistors R2126 and R2127 level shift ADD to the level required by U2101.

The differential output current from the Channel Switch (pins 12 and 13) is converted to a single-ended voltage for application to the sampling circuitry. An amplifier stage composed of Q2101, Q2102, Q2103, Q2104, Q2105, Q2106, and associated circuitry performs the conversion.

<!-- page 3-29 -->

Common-base transistors, Q2101 and Q2102 form a differential amplifier that presents a low-impedance load for the Channel Switch. Offset is adjusted (using potentiometer R2138) to match the store display with the non-store display. Thermistor RT2131 temperature compensates the offset. Output current from the collector of Q2102 is applied to the base of Q2103, a shunt-feedback inverting amplifier. The inverted output signal voltage is developed across R2146 in the collector circuit. The output signal of Q2101 is developed across R2147 in series with the signal at the collector of Q2103 to produce a single-ended replica of the differential input signal at the base of Q2105. Transistor Q2104 and its associated biasing resistors provide a constant-current bias source for Q2101, Q2102, and Q2103.

Emitter-followers Q2105 and Q2106 provide the necessary signal drive and impedance matching to the Sample-and-Hold diode bridge. Transistors Q2150 and Q2107 and associated circuitry clamp the signal level to -2.5 V and +1 V respectively at the sample and hold input.

### Sample-and-Hold

A sampling diode bridge formed by CR2203 is biased on by a strobe from the Strobe Generator. The bridge is biased off during the hold period while the Analog-to-Digital Converter (ADC) is converting the last sample. When the bridge is strobed on, Hold capacitor C2235 is charged to the new analog level present at the input to the bridge. The bridge becomes biased off when the strobe passes, and the voltage on the hold capacitor is held until the next sample is taken. Signal samples are buffered by a high-impedance input FET amplifier and coupled to the ADC via an emitter-follower amplifier that provides the input of the ADC with a low-impedance source. FET Q2209B, with its source and gate connected together, supplies source current to Q2209A. A constant-current load for the emitter-follower is provided by Q2211 and its associated biasing resistors.

### Strobe Generator

The ECL (Emitter-Coupled Logic) circuit formed by U2203A, B, and C produces two pairs of complementary control signals. One pair drives the sample strobe circuit to bias the sampling diode bridge on, and the other clocks the ADC. The 20 MHz /ADCLK clock from the Clock Generator circuit (Diagram 18, Digital Timebase) is shifted to ECL levels by the voltage divider formed by R2265, R2266, and R2267. Capacitor C2224 improves the high-frequency characteristics of the divider string, and R2268 limits the input current to U2203C. OR/NOR-gate U2203C produces the complementary ECL clocks to the ADC.

The sample-bridge strobe pulse is developed from the ADC clock signals by U2203B, U2203A, and the RC circuit composed of R2270 and C2225. The uninverted output of U2203C (/CLK A/D) is applied to pin 4 of OR/NOR-gate U2203A where its signal transitions are seen immediately. The inverted output of U2203C (CLK A/D) must charge C2225 (through R2270) to the switching threshold of U2203B before U2203B can switch state and change the state of input pin 5 of U2203A. When a HI-to-LO transition occurs on pin 4 of U2203A, the output at pin 3 goes HI and pin 2 goes LO to follow the input signal. A short time later, the charge on C2225 reaches the switching threshold of OR-gate U2203B, and the output of that gate goes HI. That HI switches the output at pin 3 of U2203A back LO and pin 2 back HI. The total duration of the pulse is approximately 10 ns. Pin 4 of U2203A switches from LO to HI on the next transition of /ADCLK, and after a short delay the output of U2203B goes LO again, readying the circuit for the next pulse.

The complementary sample strobes are applied to opposite bases of a current-mode switch formed by Q2208 and Q2207. The amplified output is coupled to the sampling diode bridge biasing circuit by T2201 and T2202, a common mode transformer. Transformer coupling prevents any dc offsets from entering the bridge via the biasing circuit by completely isolating the bias voltage from the signal voltage. The ECL output lines are terminated by R2278 and R2277 at the differential switch. The resistors match the characteristic impedance of the transmission path to prevent reflections that occur when the signals are not properly terminated. Common mode transformers T2202 and T2203 improve the symmetry of the strobe pulses so that, when the pulses are combined at CR2203, the pulses will cancel each other out and not show up as noise in the signal.

When the sample strobe is being amplified, the polarity of the pulse on pin 6 of T2201 is positive. The sampling strobe path is through C2229, C2230, and T2203 to forward bias CR2203. Signal current then flows through the forward biased diodes to charge Hold capacitor C2235. At the end of the strobe the polarity changes across pins 6 and 1 of T2201. The voltage on C2229 and C2230 increase the reverse bias on the bridge during the off time. R2281 and the duty cycle of the strobe determine the charge on C2229 and C2230.

### Analog-to-Digital Converter

Analog-to-Digital Converter U2204, converts analog input voltages in the range of 0 V to -2 V into 8-bit digital representations. The digital output code for 0 V is 11111111 and 00000000 for -2 V. Conversions are continually taking place at 20 Megasamples per second (the /ADCLK rate) regardless of the SAVECLK rate. The ADC is

<!-- page 3-30 -->

a high-speed ECL device having ECL compatible open-emitter outputs. Pull-down resistors to the -5 V supply are in resistor pack R2295. The ECL output levels are converted to TTL levels by U2205 and U2206 and placed on the C-DATA BUS.

An external voltage reference for the ADC is generated by a circuit composed of operational amplifier U2202B and Q2213. The +5 V reference voltage is converted to a current by R2259 and applied to the inverting input of U2202B. An extra current source is provided from the +8.6 V supply via R2260 to reduce loading on the +5 VREF. The closed-loop gain of the stage is -0.4 for an output voltage of -2 V at the emitter of Q2213.

## ACQUISITION MEMORY

The Acquisition Memory system, shown on Diagram 17, controls the movement of the digitized data from the A/D Converter to the Acquisition Memory. The acquisition mode controls the way the transfer occurs. Data may be transferred directly to memory through the MIN/MAX registers as either Odd and Even data for a single channel acquisition or Channel 1 and Channel 2 data for dual-channel acquisitions. In the Min-Max Mode, a certain number of data samples are compared for the highest and lowest amplitude during the comparison period. The maximum and minimum data values are transferred to the Acquisition Memory.

Data is transferred through the A/D Buffer, the MIN/MAX Registers, the Swap Registers, and finally into Acquisition Memory in a pipeline fashion. Waveforms are constantly sampled and digitized at the ADCLK rate, then the resulting data byte representing the value of each sample is latched into the A/D Buffer if the CONV clock is 20 MHz. Each succeeding sample clocked into the A/D Buffer follows the previous data sample through the digital devices of the acquisition system. Acquisition control clocks that are copies of the SAVECLK with various delays handle the data transfer timing.

### A/D Buffer

A data byte from the A/D Converter is latched into A/D Buffer U3229 on the rising edge of the convert (CONV) clock. The data is immediately available on the G data bus during normal operation because the buffer is enabled by a HI from NAND-gate U3426D.

For testing and diagnostics purposes, the /TEST signal on pin 12 of NAND-gate U3426D is made LO by the Microprocessor via the Acquisition Mode Register. That isolates the A/D Buffer from the bus and enables the Diagnostic Code Generator to place data on the G data bus to the MIN/MAX Registers.

### MIN/MAX Registers

Data is latched into the MIN/MAX Registers in four different ways depending on the acquisition mode. The MINCLK and MAXCLK clocking signals are selected by MIN/MAX Clock Selector multiplexer U3309. The mode selected determines the actual clock signals that latch data into the MIN/MAX Registers.

For Sampling mode, the data is latched by ODDCLK and /ODDCLK to place either odd and even data from a single channel or Channel 1 and Channel 2 data from both channels into the registers. The timing of the data bytes is evenly spaced in sampling mode (see Figure 3-7).

In X-Y mode for 20 microseconds per division and slower, both channels are chopped to obtain the horizontal and vertical deflection signals. The /EVENCLK signal clocks the MIN Register and /ODDCLK signal clocks the MAX Register. Selecting these clocks makes the time difference between the two samples 100 ns. The last possible sample in a Channel 1 SAVECLK period and the first possible Channel 2 sample are saved as a pair. X and Y data are then separated by one CONV clock period rather than the longer (possibly much longer) SAVECLK clock period.

Min-Max mode generates the last two clocking modes. The first is the Min-Max Initialization mode. For initialization, the first data sample in a SAVECLK period is latched into both the MIN and the MAX Registers at the same time. This is the sample with which the remaining samples taken during the SAVECLK period are compared. After storing the initial data sample, the MIN/MAX Clock Selector multiplexer (U3309) is switched. It then passes the NEWMIN and NEWMAX signals from the data Comparators, U3233 and U3235, to clock the MIN/MAX Registers.

COMPARATORS. Data bytes latched into the MIN/MAX Registers are compared with each new data byte latched in the A/D Buffer. If the data value is either lower than the present data in the MIN Register or higher than present data in the MAX Register, the appropriate Comparator output pin goes HI. The comparison takes some time after the clocking signals, so the MIN/MAX Clock Selector Multiplexer is disabled from passing the NEWMAX or NEWMIN until the CONV clock goes LO. By that time, the comparator outputs have stabilized. If a NEWMAX or NEWMIN has occurred, the new data byte is latched into the appropriate register one-half a CONV clock cycle after the data was latched into the A/D Buffer.

The Min-Max data comparisons for each saved data byte continue for the duration of the SAVECLK period. The minimum number of samples compared is 4 at 20 microseconds per division. This corresponds to the number of CONV clock periods possible at the fastest SAVECLK rate (a

<!-- page 3-31 -->

**[Figure 3-7. Sampling mode acquisition timing at 0.05 us per division (ADCLK=CONV=20 MHz) — timing diagram showing CONV/ADCLK (U4102B-10), SAVECLK (U4125B-9), EVENCLK (U3101B-9), ODDCLK (U3103B-9), BUFFERCLK (U3103A-5), C DATA BUS, CLK S/H, MINCLK CH1 (U3309-7), and MAXCLK CH2 (U3309-9) waveforms over 10 clock cycles, with arrows correlating C DATA BUS sample windows to the MINCLK/MAXCLK edges that latch them. Drawing number 4999-32.]**

<!-- page 3-32 -->

function of the SEC/DIV switch setting). As the SAVECLK period increases with slower SEC/DIV switch settings, the number of samples compared to find a min and a max per SAVECLK period also increases.

MIN-MAX OUTPUT. One CONV clock period before the end of the SAVECLK period, the ACQWRITE signal gives write control of the Acquisition Memory to the acquisition system (see Figure 3-8). If SWAPEN (U3320 pin 13) is HI, either SWAP (U3313A pin 3) or /SWAP (U3313B pin 6) becomes TRUE (depending on whether the last sample was a NEWMAX or a NEWMIN) at one-half a CONV clock period before the end of the SAVECLK to enable the output of one set of the Swap Registers onto the memory data buses. At the same time, BUFFERCLK (U3103A pin 5) goes HI to clock the last Min and Max data from the MIN/MAX Registers into and through the Swap Registers onto the memory data buses where the data is written into the Acquisition Memory. All 16 bits of the Min and Max data are transferred into memory in parallel. This 16-bit transfer also holds true for Odd and Even or Channel 1 and Channel 2 data bytes when those signals are being sampled.

### Acquisition Mode Register

The Acquisition Mode Register controls the manner in which data is transferred through the acquisition system from the A/D Buffer to the Acquisition Memory. Outputs of the MIN/MAX Clock Selector multiplexer, U3309, were discussed in the description of the MIN/MAX Registers. The control signals for switching the multiplexer and selecting which set of Swap Registers are enabled when transferring data to the Acquisition Memory are described in this part. The mode selection control of the MIN/MAX Clock Selector multiplexer is shown in Table 3-2.

**Table 3-2: MIN/MAX Clock Selector Multiplexer Switching**

| MODE | Control Input 0 | Control Input 1 | Input Selected |
|---|---|---|---|
| MIN/MAX INIT | 1 | 1 | 3 |
| MIN/MAX | 0 | 1 | 2 |
| SAMPLING | 1 | 0 | 1 |
| SAMPLING XY | 0 | 0 | 0 |

Multiplexer switching is controlled by the MIN/MAX and /XY signals from the Acquisition Mode Register U3310 (sent by the Microprocessor) and the state of the CONV clock. In MIN/MAX, the circuitry composed of U3306A and U3306B produces a 100 ns HI pulse at the beginning of each SAVECLK cycle to initialize the MIN/MAX Registers for making comparisons. Prior to entering the Min/Max mode, flip-flop U3306A is held in the Set state (reset is also LO, so both outputs of the flip flop are HI). Each rising CONV clock edge clocks the HI through flip-flop U3306B and pin 9 remains HI. With a LO MIN/MAX signal on control input 1 and a HI from flip-flop U3306B on control input 0, the multiplexer selects the sampling mode clocks (ODDCLK and /ODDCLK) to clock data into the MIN/MAX Registers.

When MIN/MAX (U3310 pin 14) goes HI, the set is removed from U3306A and the flip-flop becomes reset by the LO on pin 1. On the next rising edge of CONV, the LO is clocked through flip-flop U3306B, and the reset is removed from U3306A. On the next rising edge of EVENCLK, the fixed HI on the D input of U3306A is clocked through that flip-flop to the D input of U3306B. Then on the next rising edge of CONV, it is clocked to the Q output to make control input 0 of the multiplexer HI along with the MIN/MAX input on control input 1. The multiplexer will not yet pass the fixed HI inputs selected, because the outputs are not enabled. When CONV goes LO, AND-gate U4101C passes that LO to the enabling inputs of the multiplexer. The two input HI levels are then passed through the multiplexer to clock the same data byte into both MIN/MAX Registers. When CONV again goes HI, the multiplexer outputs become disabled, so the INIT clock to the MIN/MAX Registers last for only one-half of a CONV clock period.

When the HI was clocked to pin 9 of U3306B, pin 8 went LO, and U3306A became reset, placing a LO on its Q output. The next rising edge of the CONV clock clocks the LO through flip-flop U3306B, changing control input 0 of the multiplexer and removing the reset from flip-flop U3306A. The initialization pulse to control input 0 lasts for a period of one CONV clock; 100 ns in Min/Max mode. After initialization, the multiplexer switches to select the NEWMIN and NEWMAX outputs from the data comparators (U3233 and U3235) to clock the MIN/MAX Registers. The one-half CONV clock delay in enabling the multiplexer allows the outputs of the Comparators to settle when, on the next samples, the outputs of the comparators are used to clock the MIN/MAX Registers. A new initialization is started again on the next rising edge of EVENCLK (once for each SAVECLK).

The last Acquisition Mode is XY Sampling. The Microprocessor sets the MIN/MAX and /XY signals LO at the Acquisition Mode Register (U3310). That places a LO on control input 1 of the multiplexer and enables the outputs through AND-gate U4101C. With /XY LO, flip-flop U3306B is held reset, placing a LO on control input 0 of the Multiplexer. The MINCLK and MAXCLK are then the /EVENCLK and /ODDCLK signals respectively. These clocks produce the minimum possible time difference (100 ns) between the Channel 1 and Channel 2 data samples that are stored as a pair.

<!-- page 3-33 -->

**[Figure 3-8. MIN/MAX Acquisition timing at 20 us per division — timing diagram showing CONV (U4102B-10), /ADCLK (U4102A-6), SAVECLK (U4125B-9), EVENCLK (U3101B-9), BUFFERCLK (U3103A-5), MINCLK CH1 (U3309-7), MAXCLK CH2 (U3309-9), and INIT STROBE (U3309-14) waveforms, annotated "MIN MAX DATA is labeled by U3236-39", "Initialized Pulses for U3232 & U3234", "possible new MIN", "possible new MAX". Drawing number 4999-33.]**

<!-- page 3-34 -->

Another section of the circuitry is used in conjunction with the Min-Max Sampling mode to determine whether the last sample clocked into the MIN/MAX Registers was a NEWMIN or a NEWMAX. This knowledge is necessary in chopped Min/Max mode to place the Min and Max data samples into the Acquisition Memory in the correct order. Each Swap Register consists of two sets of two. The Min data is placed in both buffers of one set and the Max data in both buffers of the other set at the same time by the rising edge of BUFFERCLK. The outputs of one of the buffers in each set are connected to one of the memory data buses and the other half of the buffers are connected to the opposite memory data bus. Depending on which buffer in each set is enabled, the data is placed on the memory data buses by either the nonswapping buffers or the swapping buffers (controlled by enable signals SWAP and /SWAP).

When swapping is not enabled, as in sampling and X-Y modes, the SWAPEN signal from the Acquisition Mode Register (U3310) is LO, and flip-flop U3307B is held set. NAND-gates U3313B and U3313A have as one of their inputs the Q and /Q outputs of the flip-flop respectively. With the flip-flop held set, NAND-gate U3313B is enabled to pass the DATAEN enabling signal to the nonswapping buffers only. In chopped Min/Max mode, swapping is enabled to place the Min and Max data in memory in the correct order. The SWAPEN signal is set HI and the reset is removed from flip-flop U3307B so that the latch circuit on the D input controls the SWAP / /SWAP states.

At initialization in Min/Max mode, both MINCLK and MAXCLK (U3309) go HI for the first data byte. At the end of the initialization pulse, both inputs to the latch are removed by disabling the multiplexer outputs, and the output states of both NOR-gates (U3308C and U3308D) remain LO.

A MAXCLK or MINCLK signal going HI is accompanied by a LO on the opposite signal line. If the MINCLK signal goes HI, the accompanying LO on the MAXCLK line causes U3308C to change output state from LO to HI. That HI goes to pin 12 of U3308D, holding its output LO. No further switching of the latch occurs unless the MAXCLK signal goes HI. MINCLK going HI again will not cause any state changes in the latch. If MAXCLK goes HI, U3308D will change to a LO output state, and U3308D pin 13 will be latched HI. The state of pin 13 when the rising edge of BUFFERCLK occurs is clocked through flip-flop U3307B, enabling one of the NAND-gates that must pass the DATAEN enabling signal to the Swap Registers. If a NEWMAX (MAXCLK) occurred last, the Q output of U3307B will be HI and SWAP will be LO, enabling the nonswapping buffers, U3236 and U3239. If a NEWMIN (MINCLK) occurred last, pin 13 of U3308D will be LO.

When that LO is clocked through U3307B, NAND-gate U3313B goes LO, passing and inverting the DATAEN enabling signal. That makes SWAP LO, and the swapping buffers (U3237 and U3238) are enabled, placing the Max data into the Acquisition Memory that the processor looks at to find the data that occurred last.

### Acquisition Clock Decoder

The Acquisition Clock Decoder circuitry is composed of three parts. One part is a flip-flop delay chain that produces the transfer clocks. The output clocks from the chain are essentially copies of the SAVECLK delayed by successive CONV or /CONV clock periods. The second part controls acquisition writes by producing the ACQWRITE and DATAEN clocks. The outputs of this portion switch control of the Acquisition Memory to the acquisition memory system. This enables the data from the Swap Registers onto the memory data buses so it can be written into memory. The final section of the clock decoder circuitry drives the analog Channel Switch to select the vertical channel signal to be digitized.

DELAY CHAIN. The first four of five flip-flops in the delay chain (U4104B, U3101A, U3101B, and U3103B) are clocked by /CONV for delays through each of either 100 ns or 50 ns (10 MHz and 20 MHz CONV clock rates respectively). The various delays represented by the output clock lets data being transferred through each device in the acquisition pipeline settle at the outputs; and, in the case of Min-Max mode, be processed through the comparators before the next data byte is clocked in. The last flip-flop in the delay chain (U3103A) is clocked by CONV and produces a delay of one-half of a CONV-clock period between EVENCLK (U3101B) and BUFFERCLK (U3103A). Every rising edge of BUFFERCLK transfers both 8-bit data bytes from the MIN/MAX Registers into the Swap Registers and, in chopped Min-Max mode, clocks flip-flop U3307B in the Swap-Control circuitry. Flip-flop U3307B latches the last state of MINCLK and MAXCLK to determine which set of Swap Registers are enabled to pass data to the Acquisition Memory buses. See the "Acquisition Mode Control" description for additional information on Swap Register enabling.

ACQUISITION WRITE. Flip-flops U3105A and U3105B form a self-resetting circuit that produces the ACQWRITE signal once each SAVECLK period. The time duration of ACQWRITE is one WRITECLK period, either 100 ns or 200 ns (twice the CONV clock period) except at the fastest sampling rates when the SAVECLK is running at 10 MHz. In that case, once switched HI to write the first data into memory, ACQWRITE remains HI until /ENDREC goes LO (a full record). The logic gating of U3104A, B, C, and D controls the reset line to U3105B.

<!-- page 3-35 -->

Before the start of an acquisition, U3105A pin 1 is held LO by ACQENA. The LO keeps U3105A reset, putting a LO on U3105A pin 5 and U3104C pin 9. The LO on the input of NAND-gate U3104C causes the reset input of U3105B (pin 13) to be HI. This allows the next delayed SAVECLK to set U3105B. However before the start of an acquisition, SAVECLK is held LO, U3105B remains reset, the D input (pin 12) of U3105A is LO, and the WRITECLK signal continues clocking a LO to an already LO output of U3105A.

At the start of an acquisition, ACQENA goes HI on the reset input of U3105A. On the first rising edge of the delayed SAVECLK from U3101A pin 5, the fixed HI on the D input of U3105B is clocked through to place a HI on the D input of U3105A. On the next rising edge of WRITECLK, that HI is passed to the Q output of U3105A and pin 9 of NAND-gate U3104C. Assuming a HI is present on pin 10 of the NAND-gate, the output at pin 8 goes LO, resetting U3105B, and on the next rising edge of WRITECLK the LO from the Q output of U3105B is clocked through U3105A to end the ACQWRITE pulse. The ACQWRITE pulse also removes the reset from U3105B so that the next time it is clocked (by the next delayed SAVECLK), a new ACQWRITE pulse is produced for the next Acquisition Memory write.

The ACQWRITE signal goes to the Memory Control multiplexer (U3417) to switch Acquisition Memory write control to the acquisition system and is also applied to the D input of flip-flop U3307A. One-half of a CONV clock period later, the rising edge of CONV transfers the HI to the DATAEN clock line at the Q output of the flip-flop. DATAEN going HI enables NAND-gates U3313A and U3313B in the Swap-Control circuitry to pass the SWAP and /SWAP register enabling signals. That and BUFFERCLK going HI transfers the data from the MIN/MAX Registers onto the Acquisition Memory busses where it can be written into memory.

If the SEC/DIV setting is such that SAVECLK is running at 10 MHz, RNGA and RNGB will both be HI at the inputs to NAND-gate U3104D. That makes the output of U3104A also a H/I. ENDREC goes LO only when an acquisition is completed with a full record. The output of U3104B is therefore LO, and U3104C is disabled, preventing a reset from being passed to flip-flop U3105B. When /ENDREC does go LO, NAND-gate U3104C is enabled, and the reset is passed to U3105B. On the next rising edge of WRITECLK, ACQWRITE is clocked LO, switching memory write control away from the acquisition system. When operating at the fastest SAVECLK rates, a pair of Swap Registers are enabled for the entire acquisition period to immediately transfer data clocked in by BUFFERCLK to the memory data buses.

CHANNEL SELECT. When only Channel 1 or Channel 2 is selected, the Microprocessor controls the choice via the Acquisition Mode Register. For Channel 1 only, the Microprocessor sets the /CH1 line LO, which sets U3102A and holds the /CHAN1 line L/O. CHAN1 switches the analog Channel Switch (U2101 on Diagram 16) to select and apply the Channel 1 signal to the Sample-and-Hold circuitry. Conversely, Channel 2 is selected when the Microprocessor sets the /CH2 line LO, which resets U3102A and holds the /CHAN1 line HI. When the signals from both channels are to be added for ADD Mode, the /CHAN1 signal line is held LO, and the ADD signal is held HI. This turns on both sides of the analog Channel Switch to sum the input signals.

For dual-channel acquisitions, both the set and reset input to flip-flop U3102A are HI, and channel switching is controlled by ADCLK and the logic circuitry driving the D input of the flip-flop. Channel switching is then determined by the acquisition mode and the range setting of the SEC/DIV switch. The channel switching is timed to place the switching point between ADCLK positive transitions (between sampling points) at the correct time for starting waveform data into the acquisition system pipeline.

Multiplexer U4103 (Diagram 18) is switched by the RNGA and RNGB signals from the Timebase Mode Register. For SEC/DIV settings of 0.05 microseconds to 10 microseconds, CONV clock and ADCLK run at 20 MHz and are in phase. In that case, the SAVECLK signal phase is also correct for driving the analog Channel Switch. For the remaining SEC/DIV switch settings, the CONV clock runs at one-half the ADCLK clock rate, and the control clocks developed by the delay chain are delayed by 100 ns through each flip-flop rather than by 50 ns as at the faster SEC/DIV settings. Since this changes the delays of data going through the pipeline, a delayed SAVECLK is required to switch channels at the proper time. The 100 ns delayed SAVECLK from the Q output of U4104B is delayed another 25 ns, by the rising edge of ADCLK, before reaching the output of flip-flop U3106A (Diagram 17).

Either the delayed SAVECLK from U3106A or SAVECLK is selected by the multiplexer and applied to the clock input of U3102B and to one input of NAND-gate U3112 (pin 2). When Min-Max mode is selected, flip-flop U3102B divides the selected clock by two. The channel is switched only once for each SAVECLK so that the samples compared for min and max during a SAVECLK cycle are all from the same channel.

When ACQENA on the reset input of U3102 is HI, the flip-flop is enabled to toggle on each rising clock edge. If Min-Max mode is also HI, NAND-gate U3313 is enabled to pass the signal from the Q output of the flip-flop. NOR-gate U3308, connected as an inverter, places a LO on

<!-- page 3-36 -->

pin 1 of U3112A, and NAND-gate U3112 is disabled from passing the selected clock signal. U3112A puts a HI on pin 13 of U3112D, enabling U3112D to pass the divided clock signal to the D input of flip-flop U3102A. Rising edges of /ADCLK transfer the inverted state of the signal at the D input of U3102A to the /CHAN1 signal line, switching the Analog-Channel Switch at one-half the SAVECLK frequency. In Sampling and XY Sampling Modes, MIN/MAX is LO. This disables U3313D, stopping the divided clock, and enables U3112 to pass the selected clock to the D input of U3102A. Then, the selected clock and /CHAN1 are the same frequency. Another 50 ns of delay is added when clocking through U3102A. The delay is present for either selected clock.

MEMORY CONTROL. Memory Control multiplexer U3417 selects the enabling and read-write signals that control the Acquisition Memory. When the ACQWRITE clock goes HI (see Figure 3-9), the multiplexer turns the memory over to the Acquisition System (1 inputs) to perform a write to memory. From the inverting multiplexer, the /E enabling signal (pin 7) is a fixed LO that selects the Acquisition Memory devices for access. The /G enabling signal (pin 4) is a fixed HI that disables the memory devices for outputting data. Writing to memory is controlled by the WRITECLK signal from the Clock Generator (Diagram 18). It becomes the /W (write enable) on pin 9 and the ADDRCLK (memory address clock) on pin 12 of multiplexer (U3417). When the ACQWRITE signal switches the multiplexer, one-half a CONV clock period later, the Swap Registers are enabled onto the memory buses, transferring from the MIN/MAX Registers the samples that are to be stored. In another one-half CONV clock period, the data bytes have settled, and the memories are enabled for an acquisition write by the LO state of the second half period of WRITECLK. The WRITECLK falling transition increments the Address Counters to the address of the next location to be written to in memory.

For a memory read or memory write by the Microprocessor, the Memory Control multiplexer is switched to the /1 input signals. /RD and /WR (read and write control signals) from the Microprocessor control bus, determine if a read or write is to be done. Loading the Address Counter (U3423, U3424, and U3425), enabling the Microprocessor Data Transceivers (U3421 and U3422), and gating the control logic is done by the ACQSEL signal. The signal is the OR of the IO-SEG and BLCK2 signals in the processor section. Both address selection signals must be LO to access the Acquisition Memory from the Microprocessor. The ADDRCLK signal from pin 12 of the multiplexer is a fixed HI that disables the Address Counters from counting while the Microprocessor is either reading from or writing to memory. The /RD signal is inverted to pin 2 of the multiplexer by U3416A, and is again inverted to pin 4 by the multiplexer. When the

memory is enabled for reading stored data, pin 4 is LO (/RD). The ACQSEL signal is inverted by U3416B and applied to pin 5 of the multiplexer. It is again inverted through the multiplexer to a LO, enabling the memory outputs onto the memory data buses. The WR signal is also HI to enable the memory for a read.

The Microprocessor writes to the memory only for diagnostics. /WR and ACQSEL must both be LO at the inputs of U3420C to cause pin 9 of the multiplexer to be LO, enabling a memory write. The Address Counters are enabled for a parallel load of the selected memory address. Only one memory device at a time is read from or written to by the Microprocessor, because the microprocessor data transceivers that buffer data to and from the memory devices are never both enabled at the same time. The enabling signals are gated by U3420A (/ODDEN) and U3420D and U3426A (/EVENEN). Address bit A0 selects the data transceiver. When A0 is HI, transceiver (U3422) is enabled; when LO, transceiver (U3421) is enabled. The /RD signal from the microprocessor control bus selects the direction of transfer through the transceivers. When it is LO, the transfer is from the memory bus to the microprocessor data bus (read); when HI, the transfer is from the microprocessor data bus to the memory bus (write).

### Acquisition Memory and Microprocessor Access

The Acquisition Memory stores the acquired waveform data that will be read out for the stored waveform display. In the normal operation, the Acquisition System controls writing the acquired data bytes, and the Microprocessor controls reading the data out for display. For diagnostic purposes, the Microprocessor also has a limited ability to write to the memory.

The Acquisition Memory is composed of two, 2K by 8-bit static random-access memories (U3418 and U3419) for a total of 4K bytes of memory. The memory space is divided into Odd and Even halves. Single channel data is stored as odd and even data byte pairs. Dual-channel operation requires that the Channel 1 data and Channel 2 data be stored in the opposite memory halves for a record length of 2k bytes each channel. In Min-Max mode, the minimum and maximum data points of each data pair are stored in opposite halves of the memory. When both channels are being acquired (CHOP) in Min-Max mode, min data points and max data points for each channel are alternately stored in opposite halves of the memory.

Both memories are enabled at the same time for either reading or writing in parallel. When reading from or writing to the memories from the Microprocessor, the microprocessor data transceivers (U3421 and U3422) are enabled on opposite states of A0, the least-significant address bit, to select the half of memory placed on the data bus for

<!-- page 3-37 -->

**[Figure 3-9. Acquisition Memory timing — timing diagram showing CONV, BUFFERCLK, SWAP or /SWAP, ACQWRITE, WRITECLK, /G, /E, and /W waveforms across roughly 7 clock cycles, illustrating the write-enable/address-clock relationship described in the Memory Control text. Drawing number 4999-34.]**

<!-- page 3-38 -->

access by the Microprocessor. The memory address to be written to or read from is controlled by the Address Counter.

### Acquisition Memory Address Counter

The Address Counter contains three, 4-bit binary counters (U3423, U3424, and U3425). They are presettable and cascaded to obtain a maximum count of 2048. The last bit count from the last counter (U3425) is the PREFULL signal, and when it goes HI the pretrigger portion of the record has been completed. When a triggered acquisition mode is in effect, PREFULL qualifies the next trigger received as a valid trigger point. For triggered operation of the acquisition system, the counters are preloaded with a count that causes the last bit to become a 1 when the pretrigger portion of the memory is full. The following data point pairs of a record are then stored starting at location 0 and continue up to the end of the record. The end (ENDREC) is determined by the Record Counter in the Digital Time Base circuitry (shown on Diagram 18).

While waiting for a trigger after the pretrigger part of the record is filled, data pairs are continually written into the essentially circular memory space to keep the stored waveform data (pretrigger data) current. When the acquisition becomes triggered, the Record Counter (Diagram 18) starts counting the post trigger data pairs. At the end-of-record count, ENDREC goes HI and the acquisition is stopped. The Microprocessor then reads the address of the last data byte pair that was stored. Using that address and the known length of record for the type of acquisition being done, the Microprocessor calculates the beginning address for the record.

When a read of the memory is done, the Address Counter is enabled for a parallel load of the location to be read by the ACQSEL signal from the processor. The beginning address of the record is the first address loaded from the Microprocessor Address Bus, bits A1 through A12. The least significant address bit (A0) is reserved for selecting which of the memories is to be read. The Microprocessor sequences through the addresses reading out the data bytes. In ROLL and SCAN even though there is a continual updating of the waveform seen on the crt, the Microprocessor and Acquisition System are not required to run in step at all times. Instead, the Microprocessor is allowed to carry out other processes as the data pairs are being stored in memory. When a read is started, the current address count is read and stored away. The Microprocessor then loads the address of the next unread data pair to begin reading data. Memory locations are then read and transferred to the display RAM (Diagram 15). At the end of the read, the address count is reset to the previously stored address to resume storing more data pairs into the Acquisition Memory.

### Acquisition Memory Address Registers

These registers pass the address count onto the Microprocessor data bus when enabled. Registers U3427 and U3428 are enabled during different I/O periods. The lower seven bits of the address count and the SAVECLK are buffered by U3427; the upper four bits of the address count and four status bits (BTRIGD, TRIGD, BYTEINT, and /ENDREC) are buffered by U3428. SAVECLK is checked because both sample data pairs are transferred in parallel from the MIN/MAX Registers into the Acquisition Memory, losing the trigger-point reference. However, the samples stored in one half period of SAVECLK are stored in the opposite memory half from the samples stored in the other half period. The memory half that the trigger must be associated with is determined by the state of SAVECLK at the end of the acquisition.

The two address registers are read by the Microprocessor, as the result of an interrupt, to determine the cause of an interrupt. If the /ENDREC bit is LO, the address of the end of the waveform record is stable because the acquisition stopped. In that case, the Microprocessor must read the address and store it. To do a memory read, the Microprocessor must change the count of the Address Counters. After a BYTEINT read has been done to transfer more waveform data to the display RAM to update the display, the stored address count is restored to the Address Counter to allow the acquisition to continue.

## DIGITAL TIME BASE

### Clock Generator

Accurate clock signals are needed to transfer the data and to control the timing of each operation. The main clocking signals are produced by an oscillator and clock generator circuit. A 40 MHz signal is produced by crystal oscillator Y4100. The 40 MHz signal clocks all the flip-flops in the Clock Generator, setting the clock edge timing of all the other clocks. In the following description, refer to the clock timing diagram, Figure 3-10.

Flip-flop U4102A divides the 40 MHz input clock by two. The 20 MHz Q output goes to the Microprocessor clock divider for timing the processor operations. The 20 MHz /Q (Converter) and is one input to the Clock state machine (formed by the logic gates of U3112B, C, U3113C, U4101B, and flip-flops U4118A, U4102B and U4104A). Use of the state machine allows the choice of a CONV clock rate of either 20 MHz (the same as the ADCLK rate) or 10 MHz (one-half the ADCLK rate).

The final flip-flop circuit (U4104A) in the Clock Generator produces the WRITECLK and /WRITECLK signals at one-half the selected CONV clock rate. The flip-flop is held

<!-- page 3-39 -->

**[Figure 3-10. Clock timing — two timing-diagram groups. Top group (".05us to 10us/DIV"): 40MHz CLOCK (Y4100-8), ADCLK 20MHz (U4102A-6), CONV CLK 20MHz (U4102B-10), WRITECLK 10MHz (U4104A-6), and RNGB (U4103-2, held LO) over 6 cycles. Bottom group ("20us/DIV to .5s/DIV"): the same 4 signals but CONV CLK at 10MHz and WRITECLK at 5MHz (half-rate), RNGB held HI, showing how RNGB selects the divide ratio. Drawing number 4999-35.]**

<!-- page 3-40 -->

reset when the ACQENA signal is LO. ACQENA is clocked HI by the /CONV clock going HI (one-half CONV clock cycle after CONV goes HI). Therefore WRITECLK, at the Q output of U4104A, starts off LO at the beginning of an acquisition period.

The gating circuit of the Clock Generator looks at the states of ADCLK, CONV, and RNGB to set the active LO K input of U4102B and U4104A. The J and K inputs of U4104A have complemented signals applied from the logic gating (J from NAND-gate U3112C and K from AND-gate U4101B). When clocked, the flip-flop toggles for one state of the applied J and K signals (J HI and K LO) and has no change for the other (J LO and K HI). The WRITECLK and /WRITECLK outputs of the flip-flop are therefore at one-half the CONV clock rate. The K signal from AND-gate U4101B also goes to the K input of U4102B to set up U4104A to either divide the ADCLK by two or just clock ADCLK through. The CONV clock switches from 20 MHz to 10 MHz when the SEC/DIV switch is switched from 10 microseconds to 20 microseconds while the ADCLK remains at 20 MHz for all SEC/DIV switch settings.

### Time Base Mode Register

The Microprocessor controls the Digital Time Base via the Time Base Mode Register, U4119. Control bits are latched into the register from the Data bus by the rising edge of the signal on pin 11 of OR-gate U4114D. The output of U4114D pin 11 is normally HI, but when /IO 2 and address bit A5 are both made LO by the Microprocessor, U4114D pin 11 goes LO. The data on the AD0 through AD7 bus lines then becomes valid. Either /IO 2 or A5 going HI then causes the signal on pin 11 to also go HI, latching the data that is on the bus into the register. The outputs are permanently enabled by the fixed LO on pin 1 of the register.

### Time Base Divider and Divider Register

The Time Base Divider is formed by a chain of six programmable counters (U4107-U4112). The Microprocessor loads the counters to produce an output from the divider that is a function of the SEC/DIV switch setting from 20 microseconds to 5 s per division. Alternate sources of the SAVECLK are selected at the fast sampling rates used for SEC/DIV switch settings of 10 microseconds to 0.05 microseconds (see Table 3-3).

The Microprocessor writes the preloaded counts to the Time Base Divider via time base Divider Register U4113 (see Table 3-4). A data byte is loaded into the counters of the Time Base Divider chain by placing the data on the Microprocessor Data Bus during I/O time segment /IO 2. After the data settles, the /IO 2 signal goes HI. The rising transition is gated through OR-gate U4114C to clock the

data into the register. The data bits loaded determine the number of times the /CONV 10 MHz clock is divided to produce the SAVECLK frequency. Flip-flop U4125A divides the output of the divider chain by two.

An external signal may be used to clock the digital acquisition system. TTL level signals up to 1 kHz may be applied to the EXT CLK INPUT connector on the instrument side panel. The external signal is applied to the D input of flip-flop U4126A where it is clocked through to the Q output on the rising edge of the /WRITE clock. That Q output is applied to the D input of flip-flop U4126B and also clocked through by the rising edge of the /WRITE clock. The external clock is therefore delayed by two /WRITE clock periods and synchronized with the rising edge of /WRITE. The Q output of U4126B is applied to the SAVECLK multiplexer where it is selected when the A SEC/DIV switch is set to EXT CLK. External clock symmetry is not critical, but each amplitude must remain stable for at least 100 microseconds to acquire the waveform sample. One sample of a sample pair is acquired on each half cycle of the SAVECLK. As with the other clocking frequencies, flip-flop U4125A divides the signal by two to produce the SAVECLK frequency.

### Record Counter

The Record Counter (U4115-U4117) determines when the total number of data samples have been acquired to fill the acquisition memory for triggered acquisitions. Depending on the record length for the acquisition and the amount of pretrigger, the Record Counters are preloaded with a count that will cause full count (ENDREC) to be generated when the record is full. When the acquisition starts, the Acquisition Memory Address Counters count up to PRE-FULL. At that point, the Trigger Mux is enabled. After a trigger arrives, the Clock Delay Timer generates TRIGD at the next CONV clock, enabling the Record Counter. The Record Counter counts RECCLK clocks until ENDREC goes HI, stopping the acquisition (because the entire record has been acquired).

### Interrupt Logic

When selectively enabled by the Microprocessor, interrupts (/INTR) are generated after a full record is acquired, after a byte pair is acquired, or when a trigger occurs. After the interrupt is generated, the Microprocessor polls U3428 to find out what caused the interrupt.

RECORD INTERRUPT. Record interrupts are generated each time a full record has been acquired in a triggered acquisition mode. When Record Counter U4115-U4117 overflows and stops, the end of record signal ENDREC is generated HI at U4105B pin 9. If the Microprocessor has

<!-- page 3-41 -->

**Table 3-3: Time Base Clock Frequencies**

| SEC/DIV | CONV | SAVECLK | RECCLK | SAVECLK SOURCE | RANGE A | RANGE B |
|---|---|---|---|---|---|---|
| 0.05 us | 20 MHz | 10 MHz | 20 MHz | /CONV/2 | 1 | 1 |
| 0.1 us | 20 MHz | 10 MHz | 20 MHz | /CONV/2 | 1 | 1 |
| 0.2 us | 20 MHz | 10 MHz | 20 MHz | /CONV/2 | 1 | 1 |
| 0.5 us | 20 MHz | 10 MHz | 20 MHz | /CONV/2 | 1 | 1 |
| 1 us | 20 MHz | 10 MHz | 20 MHz | /CONV/2 | 1 | 1 |
| 2 us | 20 MHz | 10 MHz | 20 MHz | /CONV/2 | 1 | 1 |
| 5 us | 20 MHz | 10 MHz | 20 MHz | /CONV/2 | 1 | 1 |
| 10 us | 20 MHz | 5 MHz | 10 MHz | WRITECLK/2 | 0 | 1 |
| 20 us | 10 MHz | 2.5 MHz | 5 MHz | DIVIDER/2 | 1 | 0 |
| 50 us | 10 MHz | 1 MHz | 2 MHz | DIVIDER/2 | 1 | 0 |
| 0.1 ms | 10 MHz | 0.5 MHz | 1 MHz | DIVIDER/2 | 1 | 0 |
| 0.2 ms | 10 MHz | 0.25 MHz | 0.5 MHz | DIVIDER/2 | 1 | 0 |
| 0.5 ms | 10 MHz | 0.1 MHz | 0.2 MHz | DIVIDER/2 | 1 | 0 |
| 1 ms | 10 MHz | 50 kHz | 100 kHz | DIVIDER/2 | 1 | 0 |
| 2 ms | 10 MHz | 25 kHz | 50 kHz | DIVIDER/2 | 1 | 0 |
| 5 ms | 10 MHz | 10 kHz | 20 kHz | DIVIDER/2 | 1 | 0 |
| 10 ms | 10 MHz | 5 kHz | 10 kHz | DIVIDER/2 | 1 | 0 |
| 20 ms | 10 MHz | 2.5 kHz | 5 kHz | DIVIDER/2 | 1 | 0 |
| 50 ms | 10 MHz | 1 kHz | 2 kHz | DIVIDER/2 | 1 | 0 |
| 0.1 s | 10 MHz | 0.5 kHz | 1 kHz | DIVIDER/2 | 1 | 0 |
| 0.2 s | 10 MHz | 0.25 kHz | 0.5 kHz | DIVIDER/2 | 1 | 0 |
| 0.5 s | 10 MHz | 0.1 kHz | 0.2 kHz | DIVIDER/2 | 1 | 0 |
| 1 s | 10 MHz | 50 Hz | 100 Hz | DIVIDER/2 | 1 | 0 |
| 2 s | 10 MHz | 25 Hz | 50 Hz | DIVIDER/2 | 1 | 0 |
| 5 s | 10 MHz | 10 Hz | 20 Hz | DIVIDER/2 | 1 | 0 |
| EXT | 10 MHz | EXT/2 | EXT | EXTCLK/2 | 0 | 0 |

<!-- page 3-42 -->

**Table 3-4: Time Base Divider Preload Bits**

| SEC/DIV Setting | SAVE CLOCK Frequency | DIVIDER Output | Divider DD Bits (7 6 5 4 3 2 1 0) | Divide Ratio |
|---|---|---|---|---|
| 20 us | 2.5 MHz | 5 MHz | 1 1 1 1 1 1 0 0 | 2 |
| 50 us | 1 MHz | 2 MHz | 1 1 1 1 1 0 1 1 | 5 |
| 0.1 ms | 0.5 MHz | 1 MHz | 1 1 1 1 1 0 0 0 | 10 |
| 0.2 ms | 0.25 MHz | 0.5 MHz | 1 1 1 1 0 1 0 0 | 20 |
| 0.5 ms | 0.1 MHz | 0.2 MHz | 1 1 1 1 0 0 1 1 | 50 |
| 1 ms | 50 kHz | 100 kHz | 1 1 1 1 0 0 0 0 | 100 |
| 2 ms | 25 kHz | 50 kHz | 1 1 1 0 0 1 0 0 | 200 |
| 5 ms | 10 kHz | 20 kHz | 1 1 1 0 0 0 1 1 | 500 |
| 10 ms | 5 kHz | 10 kHz | 1 1 1 0 0 0 0 0 | 1,000 |
| 20 ms | 2.5 kHz | 5 kHz | 1 1 0 0 0 1 0 0 | 2,000 |
| 50 ms | 1 kHz | 2 kHz | 1 1 0 0 0 0 1 1 | 5,000 |
| 0.1 s | 0.5 kHz | 1 kHz | 1 1 0 0 0 0 0 0 | 10,000 |
| 0.2 s | 0.25 kHz | 0.5 kHz | 1 0 0 0 0 1 0 0 | 20,000 |
| 0.5 s | 0.1 kHz | 0.2 kHz | 1 0 0 0 0 0 1 1 | 50,000 |
| 1 s | 50 Hz | 100 Hz | 1 0 0 0 0 0 0 0 | 100,000 |
| 2 s | 25 Hz | 50 Hz | 0 0 0 0 0 1 0 0 | 200,000 |
| 5 s | 10 Hz | 20 Hz | 0 0 0 0 0 0 1 1 | 500,000 |

set RECINTEN (U4119 pin 12) HI, ENDREC and the enable are combined at U4120D, making /INTR LO generating a maskable interrupt. To clear the interrupt, the Microprocessor makes ACQENA (U4118A pin 5) LO via U4119. This makes ENDREC (U4105B) LO and /INTR (U4120D) HI, removing the interrupt.

BYTE INTERRUPT. Byte interrupts are generated each time a byte pair is acquired in the byte modes of ROLL and SCAN. To start the acquisition of a byte pair, the Microprocessor sets BYTEINTEN (U4119 pin 13) HI. After the acquisition of two bytes, SAVECLK (U4125B pin 9) goes HI setting U4118B. A HI at pin 9 of U4118B is inverted by U4120B, generating a L/O INTR, the maskable interrupt, at U4120B pin 4. To clear the interrupt, the Microprocessor makes TBMODE (U4114D pin 11) LO. This resets U4118B, removing the interrupt.

TRIGGERED INTERRUPT. Triggered interrupts are generated when triggers occur after first being enabled by the Microprocessor in a triggered mode with triggers allowed. The Microprocessor enables the interrupt by setting TRIGINTEN (U4119 pin 14) HI. When a trigger occurs, TRIGD (U4226B pin 9) goes HI. The HI TRIGD and TRIGINTEN are combined at U4120C, making /INTR LO. To clear the interrupt, the Microprocessor makes TRIGINTEN (U4119 pin 14) LO, removing the interrupt.

### Trigger Mux

Multiplexer U4227 is driven by the B/A TRIG and CALTIMER signals. The multiplexer selects either the A GATE, B GATE, or /CONV signal to drive the Clock Delay Timer circuit. The /CONV clock is used by the CALTIMER to determine the maximum and minimum counts from the Clock Delay Timer circuit. See Table 3-5 for the switching logic of the multiplexer. The additional state of the /TEST signal is necessary to determine if a maximum or a minimum count is to be measured by the Clock Delay Timer for calibration.

### Clock Delay Timer

The circuitry forming the Clock Delay Timer is used only during equivalent-time sampling (20 microseconds per division to 0.05 microseconds per division). The purpose of the timer is to determine the time interval between the trigger event and the next rising edge of the /CONV clock. The Microprocessor

<!-- page 3-43 -->

**Table 3-5: Trigger Logic Multiplexer Switching**

| CALTIMER | /A/BTRIG | /TEST | TRIGGER MODE | SELECTED SIGNAL |
|---|---|---|---|---|
| 0 | 0 | 1 | A TRIG | A GATE |
| 0 | 1 | 1 | B TRIG | B GATE |
| 1 | 0 | 0 | MIN COUNT | /CONV |
| 1 | 1 | 1 | MAX COUNT | /CONV |

must know the information to place the data samples into the correct locations in Display Memory. Since the trigger is asynchronous to the /CONV clock (and therefore to the SAVECLK that stores data byte pairs into the Acquisition Memory), no fixed timing relationship exist between the trigger and the data samples taken as a result of the trigger. Therefore the relationship must be determined for each trigger in equivalent-time sampling.

The timer is formed by a dual-slope capacitor charging circuit. A fast-charging current source composed of Q4203 and Q4204 charges capacitor C4201 when FET Q4207 is turned off, removing its shunting effect (short) from the capacitor. This happens for every A GATE or B GATE (depending on which trigger it is looking for) regardless of whether a STORE mode trigger is enabled or not. If a STORE mode trigger was not enabled, the capacitor is immediately discharged when the gate signal passes. If a STORE mode trigger is enabled (PREFULL generated from the acquisition memory Address Counter), Q4207 is held off to allow C4201 to continue to charge. The fast-charging current source through Q4204 is then shut off by the second rising edge of the /CONV signal clocking a LO onto the /Q output of flip-flop U4226B. The LO turns Q4203 on and shuts off the fast-charging current source, Q4204. The complementary HI on the Q output of U4226B also removes the reset from the Clock-Delay-Timer counter, U4230, enabling the counter to count.

A slow-charging current source (Q4205 and associated resistors) then begins discharging C4201 towards the -8.6 V supply through Q4205 and R4212. This discharge path has a long time constant so that the discharge time is much longer than the capacitor's charge time. The voltage on C4201 is applied to the inverting input of comparator U4229. A comparison voltage with a threshold of about 0.6 V is on the noninverting input of the comparator.

When the capacitor's voltage drops to the comparison voltage, the output of the comparator goes HI. That HI is applied to NAND-gate U4106, the Set input of flip-flop U4232A. The flip-flop has been toggling on the CONV

clock, so depending on the state of the Q output when the comparator changes state, the flip-flop will either be set immediately (if the Q state is HI) or as soon as the logic state of the Q output of U4232A goes HI. The action of the NAND-gate ensures that the flip-flop becomes set within +/-1 CONV clock period of the actual comparator output level change. As soon as U4232A becomes set, a LO is placed on the D input of flip-flop U4232B. On the next rising edge of the /CONV clock, the LO is clocked to a HI on the /Q output of U4232B, stopping the Clock-Delay-Timer counter. The count now held in the counter is a measure of the time between the trigger point and the next rising edge of the /CONV clock. In I/O period IO-1, address line A3 is made LO by the Microprocessor, enabling the count onto the data bus so the count can be read. The count is used by the Microprocessor to place the equivalent-time data samples into the correct (in relation to the trigger) display memory locations.

In order for the Microprocessor to place the data samples into the correct display locations, the Microprocessor needs to know the maximum and minimum counts produced by the Clock Delay Timer. A calibration routine in the Diagnostics determines the maximum and minimum counts and calculates the calibration constant used by the equivalent-time sampling firmware.

To determine the maximum count, /CONV is selected as the trigger source (U4227 pins 12 and 13). The trigger source, through U4228A, U4127, and Q4207 starts the ramp on C4201. The /CONV trigger also propagates through U4228A, U4228B, U4127C, and U4226B to Q4203, stopping the current source for the ramp and removing the clear on counter U4230 pin 10. Counter U4230 starts counting and contains the maximum count when stopped by /CONV through U4232.

To determine the minimum count, /CONV is also selected as the trigger source. The trigger, through U4228A, starts the ramp on C4201. The calibration routine sets /TEST LO. With /TEST LO (on U4228B pin 10), /CONV bypasses U4228B, stopping the current source for the ramp, starting counter U4230 50 ns sooner.

<!-- page 3-44 -->

Resistor R4213 and C4202 adjust the counter's gain and offset. Nominal counts are 300 for maximum and 100 for minimum. The difference of the two counts represents the 50 ns /CONV clock period.

### B-Delay Timer

The B-Delay Timer determines the starting address of the B Display in memory. The length of the record is determined by the setting of the SEC/DIV switch and the acquisition-mode information (i.e. is it CHOP, single trace, or a 1K or a 4K acquisition). The BTRIGD signal (U4121A pin 5) goes HI on the first falling edge of RECCLK after the B-GATE signal goes HI. BTRIGD going HI causes U4123 and U4124 to latch the value of the Record Counter. The microprocessor then reads the starting address from U4123 and U4124, which are enabled by IO 2, IO 1, and A5 (Address Decode).

## DIGITAL DISPLAY

A custom LSI integrated circuit controls the stored waveform and readout displays. Six 16K x 4-bit random-access memories (RAM) make up the Display Memory. Four of the RAM chips provide 32K x 8-bit waveform data, and two RAMs hold the 32K x 4-bit waveform-attribute data. Waveform data may be stored in the RAM from data on the Microprocessor bus or data may be read from the RAM and transferred to a Communication Option. For waveform displays, data is read from the RAM (display memory) by the display controller. The display controller then processes the data, and then drives the Vertical (Y) and Horizontal (X) digital-to-analog converters (DAC) where the data is converted to analog voltages used to drive the X- and Y-Axis vector generators.

### Data Transceivers

Communication between the Microprocessor and the display memory is via two bus transceivers, U9206 and U9207. Waveform data from the Acquisition Memory is transferred to the display memory where the data is always available to the Display Controller for refreshing the display. The data transceivers are enabled by logic gating in U9211 that decodes the PA15 and PA14 signals from the Microprocessor and the /PROCEN signal from the Display Controller to determine when a transfer is possible. The direction of transfer is controlled by the WR (write) signal from the Microprocessor. The WR signal also enables U9211 to allow either a read from memory (for outputting data) or a write to memory (for transferring in the data from the Acquisition Memory). Bus transceiver U9206 is enabled for 8-bit data transfers and transceiver U9207 is enabled for 4-bit transfers.

### Address Decoder

To access a byte in RAM, a row address followed by a column address is required. Row and column memory addresses are written together as one address word from the Microprocessor. Address Decoders U9204 and U9205 are switched by the ROW/COL signal from the Display Controller to select either the row address or the column address from the Microprocessor address bus. The /RAS and /CAS signals enable the address latches, internal to each display RAM, to latch the selected row and column addresses. Column addresses are decoded from the middle six bits of the 8-bit address by address decoders in each RAM. Row addresses require all eight bits. The Display Controller has direct access to addresses in the RAM using the RA bus.

### RAM

Six 16K by 4-bit memories make up the display RAM. The 8-bit waveform bytes are stored with the lower four bits in U9203 and U9233 and the higher four bits in U9202 and U9232. The remaining RAMs (U9201 and U9231) store attribute bits that are used to define the waveform print intensity and mark the end of the record. The memories are arranged in a 256 X 64 row and column format to allow eight addressing lines to access the 16K of 4-bit memory addresses (64K-bits of memory).

Memory refreshing is satisfied whenever the 256 Row addresses are accessed. Refreshing occurs when the Display Controller does a memory read for display purposes. While the Microprocessor is controlling the Display Memory, it must also perform memory refreshing by activating all the memory Row addresses. To maintain the dynamic memory, a refresh must be done at least every eight milliseconds.

DATA TYPES. The data stored in the Display Memory is either readout characters or waveforms. The microprocessor also uses the display memory for operational data storage. In either case a 9-byte field-attribute preamble is read first. The preamble defines the data type and sets up the display attributes. Readout information is displayed using short vector X-Y displays positioned to specified fields on the crt.

### Display Controller

The Display Controller runs the display system for the STORE waveform and STORE and NON STORE readout displays. It takes control of the RAM to read the waveform or readout data. Besides the waveform data, the Display Controller runs the Store Z-Axis, selects the type of display (vector, dots, or X-Y plotter output), and drives the horizontal and vertical channel switches.

<!-- page 3-45 -->

When reading data out of the RAM, the Display Controller has direct access to the memory address bus (RA). RAM row and column addresses to be read from are sequenced through in order. When a display data read is taking place, the dynamic memory is refreshed by the Display Controller.

When the Display Controller has completed a display frame, it signals the Microprocessor (using the INTR signal) that the last field is finished and awaiting the next frame request. After the interrupt is received, the Microprocessor can request the next frame (/FRAME), then the Display Controller resumes control of the RAM for the next frame of data. When PROC RQ (U9208 pin 3) is HI, the Display Controller is in the middle of a display cycle and the Microprocessor is denied access to the display RAM. The Microprocessor can request access to the Display RAM using the PROC RQ (RAM SEG) signal line to either write in new waveform data or read out data for the Communication Option. The Display Controller allows the Microprocessor to access the display RAM by setting the /PROC EN (U9208 pin 5) signal line LO. A L/O PROC EN signal enables the circuitry that allows the WR, PA14, and PA15 signals, from the Microprocessor, to control the display RAM. Even though the memory addresses are under control of the Microprocessor, the /RAS and /CAS signals are generated by the Display Controller.

### YDAC and XDAC

Data from display controller U9208 is applied to X- and Y-axis DACs U9210 and U9220. These DACs are biased to provide output currents (approximately 0 to 2 mA) proportional to the digital data. R9214 and R9224 are adjustments to align the storage signals on the crt. The DAC currents are applied to the Vector Generator along with various control signals from U9208 via W6100.

## VECTOR GENERATOR

### Vector Generators

Vector Generator circuitry is shown on Diagram 20. U6303 and U6304 convert the DAC currents into bipolar voltages (approximately -2.5 V to +2.5 V) which are applied to sample and hold circuits U6305 and U6306. Outputs of the sample and hold circuits are applied to integrator stages U6307 and U6308 through electronic switches in U6301A and C. The integrator output signals are continuously fed back to the sample and hold inputs, causing these input voltages to be equal to the difference between the drive inputs and the integrator outputs. When the vector sample (/VECT-SMPL) control line (via U6301B) is actuated, the outputs of the sample and hold circuits store these difference signals. Since the integrator output slopes are proportional to these signals, the net result is to effectively "connect the dots" which are equivalent to the digital data values.

These circuits also have a "dot" mode available so that the integrator outputs are stepped (dots) rather than continuous (vectors). When the VECT/DOT signal is LO, U6301A and C switch the integrator inputs directly to the difference signals while also disconnecting the integration capacitors C6315 and C6314. The feedback loops are thus closed continuously, resulting in normal amplifier action.

Although the Vertical and Horizontal vector generators operate the same, there are some differences between the circuits and between their signal characteristics. To end up with the proper signal polarities at the crt, X DAC U9210 (Horizontal) current is from 2 mA to 0 mA, while Y DAC U9220 (Vertical) current is from 0 mA to 2 mA. Also, the vertical integrator output is -2 V to +2 V while the horizontal integrator output is -2.5 V to +2.5 V. The reduced vertical dynamic range allows proper interface to the main deflection system. Since the vertical signal eventually passes through the vertical delay line before reaching the crt, it is necessary to delay the horizontal signal as well. This is done in the vector mode by delaying slightly the vector sample signal applied to U6306 via R6320 and C6312. In the dot mode the crt beam is blanked during the transitions so the dots are only displayed after the signals have arrived and settled.

VECTOR INTEGRATOR. The Y-axis (vertical) current from the D/A Converter goes to the inverting input of operational amplifier U6303. The amplifier is biased to produce a bipolar output voltage, from -2.5 V to +2.5 V, that is proportional to the input current. Negative feedback from the parallel combination of R6303 and C6311 stabilizes the amplifier.

Biasing of the non-inverting input of both the X-axis and the Y-axis amplifiers is identical and supplied by a resistive divider formed by R6304 and R6305 between ground and the +5 V reference. Both resistors are equal valued to produce a bias voltage of +2.5 V. Resistor R6308 provides a summing node for the input vector current and the feedback current and develops the voltage on the inverting input of U6303. Full current range of the vector signal is from 0 to 2 mA. With no vector current in, the feedback current supplies the full current through R6308, and the output voltage of U6303 goes to -2.5 V. At maximum vector current input, the sum of the current through R3608 must remain the same as with no vector current; therefore the feedback current is reduced by the amount of the vector current, and the output voltage goes to +2.5 V.

SAMPLE-AND-HOLD. The voltage output of U6303 is applied via R6309 to sample-and-hold circuit U6305. Sample-and-Hold (S/H) switching is controlled by the /VECT SMPL signal from the Display Controller applied to

<!-- page 3-46 -->

U6305 pin 14. That signal in turn is controlled by the /PLT-EN signal (U6301B pin 9) that switches section B of multiplexer U6301. When displaying storage waveforms and readout characters, the /PLT-EN signal is not active, and the VECT SMPL signal is switched to control the S/H circuit. For producing X-Y Plots, U6301C is activated, and the VECT SMPL signal drives the X-Y Plotter Pen-Down circuit (shown on Diagram 22).

SAMPLE INTEGRATOR. During digital storage waveform displays, the S/H circuit and the Y-Integrating circuit formed by U6307 and associated components produce either vectors or dots. When U6301C connects pin 13 to pin 14, U6307 integrates each step output of the S/H circuit into a smooth ramp signal. This integrated signal is the vertical deflection signal (still single-ended) that connects the data points of the stored waveform display. When the user selects either dot displays or X-Y Mode, multiplexer U6301C connects pin 12 to pin 14. The long time constant integrating function of U6308 is switched out, and U6307 acts as an S/H amplifier only for the voltage being held by the S/H circuit, causing the crt display to be dots. For readout character displays both during STORE and NONSTORE modes, the S/H and integrator work only in the vector mode because readout characters are vector displays.

The integrator output is subtracted from the input voltage at all times. When /VECT SMPL goes LO, the difference value is sampled and held by S/H U6305. The held voltage value sets the slope of the integrator and effectively "connects the dots" since the slope of the output vector is proportional to the difference between the input voltage and the output voltage of the integrator.

Diode clamps CR6301, CR6303, CR6305, and CR6307 prevent voltage transients that could cause U6301C latch up.

### Vector Amplifiers

The integrator outputs are applied to vector amplifiers U6401 and U6402, which are differential voltage-to-current converters. Their outputs are differential currents which are sent to the main deflection multiplex circuitry via J6410 and the I/O wiring harness. Vertical positioning information is processed by display controller U9208, but horizontal position information is not. Therefore the horizontal position voltage is applied to U6402D to affect horizontal position control of stored waveforms. At times when readout characters are being drawn, this position signal is shunted by transistor U6403A to reduce the positioning effect on the characters. This action is controlled by the HPOS-DIS signal from the display controller.

### Plot Drive

When plot mode is on, the display controller activates the /PLT-EN signal, causing U6301B to apply the VECT-SMPL signal to the PEN-DN line via U6404A and U6402E, and the display controller internal modes change so that VECT-SMPL provides the pen down control function. The PEN-DN signal is sent via J6420 to the Z-axis section and to the X-Y board or communication option board (if installed). When U6301B activates plot mode, Q6301 pulls the sample control lines of U6305 and U6306 LO putting them in tracking mode. This closes the vector generator feedback loops regardless of vector/dot mode selection. The PLT-EN signal also turns on operational transconductance amplifiers U6404A, B, and C via transistor U6403E. Normally, their outputs are off, the plotter signals are zero (held at ground by R6433, R6434). In plot mode they turn on and act as voltage followers for the vector signals (Y POINT, X POINT, and /PEN). The "Y" amplifier input is connected ahead of the Y vector generator to preserve the +/-2.5 V range and correct polarity. The X-PLOT and Y-PLOT signals are sent via J6420 to the X-Y board or communication option board (if installed).

### Readout Off Detector

To detect when the Storage/Readout Intensity knob is at its counterclockwise end, U6405A (Diagram 20) monitors the readout (RO) voltage from J6410. Since RO voltage is normally negative, but goes slightly positive at the end of its rotation, U6405A output will go positive, turning on transistor U6403B, causing the /NO-RO line to be LO. This signal is sent to the I/O board as status information.

### Signal Conditioning

The signals ARES1, A-RES2, B-RES, and B-CAPS on J6420 come from the Sweep Interface board. They are encoded analog currents which contain most of the information about the positions of the A and B Timing switches. Since the sum of the possible changes in these currents is larger than U6302 (5V REF) can accommodate, U6405B (Diagram 21) is used to buffer the 5V reference to supply the termination resistors (Diagram 20). As these currents change, the resulting voltages are measured by the Status A/D (Diagram 19) so that the Microprocessor can determine the state of the timing switch.

### I/O and Vector Generator Board Power Distribution

+/-15 VOLT POWER SUPPLYS. U6305 and U6306 operate from +/-15 Volt supplies. These are generated by flyback converters (see Diagram 21) consisting of U6202A, U6202B, Q6202, Q6203, and associated circuitry. The comparators in U6202 form oscillators which drive the switch transistors to alternately store and unload energy in

<!-- page 3-47 -->

their respective chokes. Feedback is applied to the comparators causing duty cycle and frequency modulation, which adjusts output power accordingly.

+5 VOLT POWER SUPPLY. Logic power (+5 V) for all I/O board and Vector Generator board circuitry is generated from the +8.6 V supply by U6201.

+5 VOLT REFERENCE. The 5 Volt Reference is generated by U6302. It is used by the vector generator circuits, status A/D circuit, display DAC circuit, and acquisition system. Associated with each of these circuits is a local pull-up resistor from the +8.6 V supply to the 5V reference line to supply nominal load current so that U6302 does not have to supply the total load current. This also greatly reduces the reference line current which could cause excessive voltage drops at the far ends of its travel.

### Status ADC and Bus Interface

I/O PORTS. The system data bus and associated control signals are sent to the I/O board via J6100 (see Diagram 19). Input ports U6102 and U6103 transfer logic signals representing instrument status. U6103 operates as a simple port for eight of the status lines. U6102 has 15 input signals. It serves as a data buffer for the Status A/D converter U6105, when required. During part of the status scanning cycle, U6105 data outputs are tri-stated, and seven additional status signals are applied via 22 kOhm resistors (R6121 through R6126). The Microprocessor then reads these status lines through U6102. When U6105 is active, its outputs dominate the data lines and the 22 kOhm resistors act as high impedance loads. The Microprocessor can then read the data from U6105 via U6102. Output port U6104 is used to control the operation of U6105 to perform the A/D conversion function. U6104 is also the multiplexer selection register, driving U6106 and U6108, which select the analog status signals to be measured. The port address selection is made by combinations of control lines /IO-0, and /IO-1, and address lines A2 and A3. U6101A and B provide the selection logic for U6104.

STATUS A/D. U6105 is a 10-bit A/D converter which allows measurement of analog status signals. After each conversion it produces an interrupt which is gated by U6101D and applied to Q6201 via R6218. This produces a processor interrupt to indicate completion of its task. This interrupt is maskable by U6104. U6107A serves as a buffer amplifier to drive the input resistance of U6105 while maintaining fairly high load impedance for U6106 and U6108. U6107B and U6107C are differential amplifiers which convert the differential vertical position signals to single voltage levels within the range of the measuring system.

## POWER INPUT, PREREGULATOR AND INVERTER

The Power Supply (see Diagram 8 and Diagram 9) changes the ac power-line voltage into the voltages needed for instrument operation. It consists of the Power Input, Preregulator, and Inverter circuits (which drive the primary of the power transformer) and secondary circuits (which produce the necessary supply voltages for the instrument).

### Power Input

The Power Input circuit changes the ac power-line voltage to filtered dc for use by the Preregulator.

POWER switch S901 connects the ac power line through fuse F9001 to the bridge rectifier formed by CR901, CR902, CR903, and CR904. The full-wave bridge rectifies the source voltage, and the output is filtered by C906. Input surge current at instrument power-on is limited by thermistor RT901. The thermistor resistance is moderately high when the power is first turned on, but decreases as the input current warms the device. The instrument is protected from large voltage transients by suppressor VR901. Conducted interference originating within the power supply is attenuated by common-mode transformer T901, differential-mode transformer T903, line filter FL9001, and capacitors C900, C902, and C903.

### Preregulator

The Preregulator provides a regulated dc output voltage for use by the Inverter circuitry.

When the instrument is turned on, the voltage developed across C906 charges C925 through R926. When the voltage across C925 has risen to a level high enough that Pulse-Width Modulator U930 can reliably drive Q9070, U930 receives operating supply voltage through Q930. This voltage level is set by zener diode VR925 in the emitter of Q928 and by the voltage divider formed by R925 and R927. The zener diode keeps Q928 biased off until the base voltage reaches approximately 6.9 V. At that point, Q928 is biased into conduction, and the resulting collector current causes a voltage drop across R929 that biases on Q930. The positive feedback through R930 reinforces the turn-on of Q928, which quickly drives both Q928 and Q930 into saturation. Once Q930 is on, the Pulse-Width Modulator begins to function.

Pulse-Width Modulator U930 controls the output voltage of the Preregulator by regulating the duty cycle of the pulse going to the gate of Q9070. The modulator has an

<!-- page 3-48 -->

oscillator that operates at a frequency set by R919 and C919 (approximately 60 kHz). A sawtooth voltage produced at pin 5 of U930 is compared internally with the output voltage produced by the two internal error amplifiers. Whenever the sawtooth voltage is greater than the error amplifier output voltage, Q9070 is biased on to supply current to the remaining portions of the switching circuitry and charge C940. The two error amplifiers maintain a constant output voltage and monitor the output current of the Preregulator. One input of each amplifier is connected through a divider network to the IC internal +5 V reference. The output voltage of the Preregulator is monitored by the voltage divider at pin 2. The voltage drop across R907, produced by the Preregulator output current, is applied to the internal current-limit amplifier at pin 16.

When the instrument is first turned on, the current-limit amplifier controls the conduction time of Q9070. While Q9070 is conducting, the output current increases until a voltage large enough to permit the current-limit circuitry to function is developed across R907. The current-limit amplifier then holds the output current below the limiting threshold of approximately 1 A. When the voltage across C940 reaches approximately 43 V, the internal voltage amplifier starts controlling the duty cycle of Q9070, and the Preregulator will not limit current unless there is excessive current demand.

With Q9070 off, C907 charges to the output voltage of the Power Input circuit. When Q9070 turns on, current through the FET comes from the winding connected to pins 1 and 2 of T906 and from C907. Current to C907 is supplied by the winding connected to pins 4 and 5 of T906. When U930 shuts off Q9070, the collapsing magnetic field raises the voltage at the anode of CR907. This diode then becomes forward biased and passes the currents supplied by C907 and the winding connected to pins 4 and 5 of T906. For this part of the cycle, current to C907 is supplied by the winding connected to pins 1 and 2 of T906. This process continues for each period of the oscillator, and the duty cycle controlling the conduction period of Q9070 is altered as necessary to maintain 43 V across C940. During each oscillator period, Q908 is used to discharge the gate-drain capacitance of Q9070. At the shutoff point, Pin 10 of U930 goes LO to reverse bias CR908 and turn on Q908 to switch off the FET.

Once the supply is running, power to U930 is supplied from the winding connected to pins 6 and 7 of T906. Diode CR920 half-wave rectifies the voltage across pins 6 and 7 to keep filter capacitor C925 charged and to maintain supply voltage to U930 through Q930.

Instrument protection from excessive output voltage is supplied by silicon-controlled rectifier Q935. Should the Preregulator output voltage exceed 51 V, zener diode VR935 conducts, causing Q935 to also conduct. The Preregulator output current is then shunted through Q935, and the output voltage quickly drops to zero. With the supply voltage of U930 no longer being provided by the winding connected to pins 6 and 7 of T906, the Preregulator shuts down, and Q935 becomes reset. The supply then attempts to power up, but it will shut down again if the overvoltage condition reoccurs. This sequence continues until the overvoltage condition is corrected. A thermal shutdown circuit is included to protect the instrument from damage in case of fan failure or air flow restriction at high ambient temperatures. Overheating causes the resistance of RT950 to increase, eventually firing SCR Q950, which reduces voltage on VR943. This causes all outputs to drop to very low values, thus reducing total power dissipation. To reset the circuit, the power must be shut off momentarily.

### Inverter

The Inverter circuit changes the dc voltage from the Preregulator to ac for use by the supplies that are connected to the secondaries of T948.

The output of the Preregulator circuit is applied to the center tap of T948. Power-switching transistors Q946 and Q947 alternate conducting current from the Preregulator output through the primary windings of T948. The transistor switching action is controlled by T944, a saturating base-drive transformer.

When the instrument is first turned on, one or the other of the switching transistors starts to conduct. As the collector voltage of the conducting transistor drops toward the common voltage level, a positive voltage is induced from T944 to the base of the conducting transistor that reinforces conduction. Eventually T944 saturates; and, as the voltage across T944 (and T948) begins to reverse, the conducting transistor is cut off by the drop in base drive. The other transistor does not start conduction until the voltage on the leads of T944 reverse enough to bias it on. The saturation time of T944 plus the transistor-switching time determine the frequency of Inverter operation (typically about 20 kHz). After the initial Inverter start up, the switching transistors do not saturate; they remain in the active region during switching.

Diodes CR946 and CR947 serve as a negative-peak detector to generate a voltage for controlling the output of the error amplifier. Capacitor C943 charges to a voltage equal to the negative peak voltage at the collectors of Q946 and Q947, referenced to the Preregulator input voltage. This voltage level is applied to the divider formed by R937, R938, and R939. The error amplifier, formed by Q938 and Q939, is a differential amplifier that compares the reference voltage of VR943 with the wiper voltage of potentiometer R938. The current through Q939 sets the

<!-- page 3-49 -->

base drive of Q944 and, thereby, controls the voltage on C944. This voltage biases Q946 and Q947 to a level that maintains the peak-to-peak input voltage of T948. The amplitude of the voltage across the transformer primary winding, and thus that of the secondary voltages of T948, is set by adjusting -8.6-V-ADJ potentiometer R938.

At turn-on, Q938 is biased off, and Q939 is biased on. All the current of the error amplifier then goes through Q939 to bias on Q944. The current through Q944 controls the base drive for Q946 and Q947. Base current provided by base-drive transformer T944 charges C944 negative with respect to the Inverter circuit floating ground (common) level.

## POWER SUPPLY SECONDARIES, Z-AXIS AND CRT

### XFMER and LV Power Supplies

The Low-Voltage supplies use center-tapped secondary windings of T948 (XFMER). The +100 V supply is rectified by CR954 and CR955 and filtered by C954. Diodes CR956 and CR957 rectify ac from taps on the 100 V winding, and C956 filters the output to produce +30 V dc. The full-wave diode bridge formed by CR960, CR961, CR962, and CR963 produces the +8.6 V and -8.6 V supplies. Filtering of the +8.6 V is done by C960, L960, and C962. Filtering of the -8.6 V is done by C961, L961, and C963. Ac voltage from the +/-8.6 V primary is rectified by CR965 and CR967, and then filtered by C965 and R965 to provide the fan power source. The +5 V supply is produced by CR970, C968, L968, C958 and C970. The -5 V supply is produced by CR980, CR981, C964, L962, and C959.

### Unblanking Logic, Intensity, and Z-Axis Ampl

The Z-Axis Amplifier, shown on Diagram 9, controls the crt intensity level via several input-signal sources. The effect of these input signals is either to increase or decrease trace intensity or to completely blank portions of the display. The Nonstore Z-Axis drive signal currents, as set by the A and B Z-Axis switching logic and the input current from the EXT Z AXIS INPUT connector (if in use), are summed at the emitter of common-base amplifier Q825. The total sets the collector current of the stage. The common-base amplifier provides a low-impedance termination for the input signals and isolates the signal sources from the rest of the Z-Axis Amplifier.

For the Nonstore Z-Axis signals, common-base transistor Q829 passes a constant current through R832. This current is divided between Q825 and Q829, with the portion through Q829 driving the shunt-feedback output amplifier formed by Q835, Q840, and Q845. The bias level of Q825 therefore controls the emitter current available to Q829. Feedback-resistor R841 sets the transresistance

gain for changing the input current to a proportional output voltage. Emitter-follower Q835 is dc coupled to Q840, and for low-speed signals, Q845 acts as a current source. Fast transitions couple through C845, providing added current gain through Q845 for fast voltage swings at the output of the Amplifier.

Store Z-Axis signals, controlled by the Display Controller, are applied to the Z-Axis amplifier at the emitter of Q829. The Nonstore Z-Axis signals are shunted away from Q829 by CR824, which is forward biased from the CHOP Blanking circuit (Diagram 2) during STORE mode displays. The overall store waveform and readout character intensity level is set by the STORAGE/READOUT INTENSITY control (see Diagram 13). The level setting of that control sets the Z-Axis drive current supplied to the Z-Axis Amplifier by Q829 during digitally controlled displays. When the Display Controller turns off Q7203, Q7202, or Q7201, the current normally shunted away from the emitter of Q829 is added via the forward biased diode connected to the emitter of the cutoff transistor. With more current available from Q7204, more current flows in Q829 to intensify the crt display.

The intensity of the Nonstore crt display in the A, B, and Alt Horizontal modes is set by the INTENSITY controls and associated circuitry. The A INTENSITY potentiometer controls the base voltage of Q804 to set the amount of emitter current that flows through that transistor and, therefore, the level of the Z-Axis signal. Likewise the B INTENSITY potentiometer controls the base voltage of Q814 and the intensity of the B and Alt Sweep displays.

When only the Nonstore A Sweep is displayed, Q586 and Q583 are biased off. The current through R818, as set by the A INTENSITY potentiometer, flows through CR818 and Q825 to fix the voltage level at the Z-Axis Amplifier output. For a B-Only display, Q586 is biased on to reverse bias CR818 and prevent A-Intensity current from reaching Q825. Current set by the base voltage of Q814 flows through CR817 to Q825 and sets the B Sweep intensity. For an alternating A and B display, Q586 is biased off when the A Sweep is displayed. During the portion of the A Sweep in which the B Sweep runs, current from R816 is passed through CR816 by the Alternate Display Switching and the Unblanking Logic circuitry to produce an intensified zone on the A Sweep trace.

When CHOP VERTICAL MODE is selected, the Chop Blanking signal is sent to the collector of Q825 through U537B and CR824 during the Nonstore display-switching time. Signal current is shunted away from CR825, and the forward bias of Q829 rises to the blanking level. When blanked, the output of the Z-Axis Amplifier drops to reduce the crt beam current below viewing intensity.

<!-- page 3-50 -->

For a Nonstore X-Y display, CR818, CR817, and CR816 are reverse biased. The XY signal is LO to reverse bias CR551 and allow current in R820 to flow through CR820. The crt intensity is then controlled by the A INTENSITY potentiometer which sets the current in R820 through Q804.

During Nonstore operation, any applied External Z-Axis input voltages drive proportional input currents through R822 and R823 to the Z-Axis Amplifier. Sensitivity to external signals is determined by the transresistance gain of the shunt-feedback amplifier. Diode CR823 protects the Z-Axis Amplifier if excessive voltage levels are applied to the EXT Z AXIS INPUT connector. External Z-Axis modulation does not function for STORE MODE displays.

BEAM FIND switch S390 controls the base bias voltages of Q825 and Q829. When the BEAM FIND button is out, -8.6 V is supplied to the normal base-biasing network. When the button is held in, the -8.6 V supply is removed, and the voltage at the anode of VR828 rises to about -5.6 V. This voltage level turns off the current supply from Q829. The Z-Axis amplifier output voltage is then fixed by R835 and the voltage at the BEAM FIND switch, as set by other parts of the Beam Find circuitry. The output voltage of Q835 is set to a level that displays either a bright trace or dot (depending on whether the sweep is

triggered or not), and the INTENSITY controls and the Z-Axis drive signals have no control over the crt intensity.

### Hv Multiplier, Dc Restorer, and Crt

The Dc Restorer circuit sets the crt control-grid bias and couples the ac and dc components of the Z-Axis Amplifier output to the crt control grid. Direct coupling of the Z-Axis Amplifier output to the crt control grid is not employed due to the high potential differences involved. Refer to Figure 3-11 during the following discussion.

Ac drive to the Dc Restorer circuit is obtained from pin 16 of T948. The drive voltage has a peak amplitude of about +/-100 V at a frequency of about 20 kHz and is coupled into the Dc Restorer circuit through C853 and R853. The cathode of CR851 is biased by the wiper voltage of Grid Bias potentiometer R851, and the ac-drive voltage is clamped whenever the positive peaks reach a level that forward biases CR851.

The Z-Axis Amplifier output voltage, which varies between +10 V and +75 V, is applied to the Dc Restorer at the anode of CR853. The ac-drive voltage holds CR853 reverse biased until the voltage falls below the Z-Axis Amplifier output voltage level. At that point, CR853 becomes forward biased and clamps the junction of

**[Figure 3-11. Simplified diagram of the Dc Restorer circuitry — schematic showing Z-AXIS OUTPUT (a step from +10V to +10V-to-+75V) coupled via C855/CR853/CR855 network, Grid Bias potentiometer R851 (from +100V) feeding CR851/C851/R852, AC DRIVE VOLTAGE FROM T940 PIN 16 (a +/-100V 20kHz waveform) coupled via C853/R853, R854/C854/CR854 network, and CATHODE VOLTAGE SUPPLY (~-2kV) through R858/R860(6.2M)/C979 driving neon lamps DS856/DS858 to the CONTROL GRID and CATHODE outputs. Drawing number 4206-14.]**

<!-- page 3-51 -->

CR851, CR853, and R854 to the Z-Axis output level. Thus, the ac-drive voltage is clamped at two levels to produce a square-wave signal with a positive dc-offset level.

The Dc Restorer is referenced to the -2 kV crt cathode voltage through R858 and CR854. Initially, both C855 and C854 charge up to a level determined by the difference between the Z-Axis output voltage and the crt cathode voltage. Capacitor C855 charges from the Z-Axis output through R858, CR854, and CR855, to the crt cathode. Capacitor C854 charges through R858, CR854, R854, and CR853 to the crt cathode.

During the positive transitions of the ac drive, from the lower clamped level toward the higher clamped level, the charge on C854 increases due to the rising voltage. The voltage increase across C854 is equal to the amplitude of the positive transition. The negative transition is coupled through C854 to reverse bias CR854 and to forward bias CR855. The increased charge of C854 is then transferred to C855 as C854 discharges toward the Z-Axis output level. Successive cycles of the ac input to the Dc Restorer charge C855 to a voltage equal to the initial level plus the amplitude of the clamped square-wave input.

The charge held by C855 sets the control-grid bias voltage. If more charge is added to that already present on C855, the control grid becomes more negative, and less crt writing-beam current flows. Conversely, if less charge is added, the control-grid voltage level becomes closer to the cathode-voltage level, and more crt writing-beam current flows.

During periods that C854 is charging, the crt control-grid voltage is held constant by the long time-constant discharge path of C855 through R860.

Fast-rise and fast-fall transitions of the Z-Axis output signal are coupled to the crt control grid through C855 to start the crt writing-beam current toward the new intensity level. The Dc Restorer output level then follows the Z-Axis output-voltage level to set the new bias voltage for the crt control grid.

Neon lamps DS858 and DS856 protect the crt from excessive grid-to-cathode voltage if the potential on either the control grid or the cathode is lost for any reason.

High-voltage multiplier U975 uses the 2-kV winding of T948 to generate 12 kV to drive the crt anode. An internal half-wave rectifier diode in the multiplier produces -2 kV for the crt cathode. The -2 kV supply is filtered by a low-pass filter formed by C975, C976, R976, R978, and C979.

Neon lamp DS870 protects against excessive voltage between the crt heater and crt cathode by conducting if the voltage exceeds approximately 75 V.

Focus voltage is also developed from the -2 kV supply by a voltage divider formed by R894, R892, FOCUS potentiometer R893, R891, R890, R889, R888, and R886.

## X-Y PLOTTER

The X-Y plotter circuitry (see Diagram 22) drives the internal circuitry for the external clock, and an external XY Plotter, if connected.

### External Clock

The TTL compatible (active LO) /EXT CLK signal, accessed through the AUXILIARY CONNECTOR (J1011 pin 1), drives the external clock circuitry (active HI) of the oscilloscope through internal connector J4110 pin 1.

Operational amplifier U1001A, PNP transistor Q1011, and associated components buffer and invert the external clock signal /EXT CLK. Input bias resistors R1011, R1014, and R1015 condition the /EXT CLK input signal. The same three resistors protect the external clock circuitry from over-voltage and reverse-voltage inputs. Resistor R1016 provides hysteresis.

Operational amplifier U1001A serves as a buffer and amplifier. Even though /EXT CLK only swings from 0 V to +5 V maximum, the input bias resistors produce plus and minus voltage swings of <=2 V at non-inverting input U1001A pin 3. The amplifier output U1001A pin 1 has a plus and minus 7 V range which, through current limit resistor R1017, overdrives the base of Q1011. This base current overdrive assures a fast clean rise and fall time of the EXT CLK output signal (J4110 pin 1) required by the oscilloscopes external clock circuit input.

The emitter of Q1011 goes to +5 Vk and the collector goes to both the EXT CLK output and to level-shift resistor R1012. Level-shift resistor R1012 makes the EXT CLK output a valid TTL LO when Q1011 is shut off. The EXT CLK output is an active HI TTL drive.

### Shield Ground

The SHIELD GND connection (J1011 pin 4) is the chassis ground connection for cable shield connections.

<!-- page 3-52 -->

### Signal Ground

The AUXILIARY CONNECTOR SIG GND connection (J1011 pin 9) is the ground point for all signal path ground returns.

### Pen-Down Circuit

The Pen-Down circuitry controls the pen mechanism of an external X-Y plotter or the motor drive of a Y-T strip chart recorder.

The Pen-Down circuit is comprised of operational amplifier U1001B, transistor Q1012, relay K1001, and related components. The /PEN DWN signal (J6423 pin 1) drives the non-inverting input of the operational amplifier (U1001B pin 5). The inverting input of the operational amplifier (U1001B pin 6) is tied to ground. The operational amplifier output, U1001B pin 7, goes to the base of PNP relay-drive transistor Q1012, through current limiting resistor R1005. This amplifier has no negative feedback resistor and operates in an open-loop gain configuration. Small input signals therefore drive the output near one rail or the other. The output signal resembles a square wave, regardless of the input waveform.

Transistor Q1012 inverts the signal and drives relay K1001. Diode CR1016 protects the transistor from inductive kick-back voltages generated by the relay's collapsing magnetic field as the transistor turns off. Fuse F1001, in the RELAY COMM signal path, provides over-current protection for all relay contact configurations.

When the /PEN DWN signal on U1001B pin 5 goes negative, the output on pin 7 of the operational amplifier also goes negative, turning on transistor Q1012 and energizing the relay coil. When the relay is energized, the relay common to normally closed connection opens and the relay common to normally open connection closes. When /PEN DWN returns to a positive level, the transistor shuts off. The relay's coil discharges its kick-back current through diode CR1016, and the relay common returns to its normally closed position.

In order to drive both an X-Y plotter and a Y-T strip chart recorder, the Pen-Down circuitry does double duty. With an X-Y plotter, the circuitry simply lowers the plotter pen. with a Y-T strip chart recorder, the pen-down circuitry is actually a motor drive control circuit. This double duty is accomplished by providing the Pen-Down signal to the operational amplifier about 1 s prior to the signals being provided to X & Y plot output circuitry. This allows the motor to have time to start up before signals are applied to the Y plot output circuit. The circuit can not differentiate between X-Y plotters and Y-T strip chart recorders, therefore the time delay from /PEN DWN to X and Y channel information output is the same in each case.

### X and Y Amplifiers

The X and Y amplifiers drive the X and Y outputs. Because both amplifiers operate the same, only the X-PLOT amplifier is discussed in detail.

Input signal X PLOT goes to the non-inverting input of unity gain amplifier U1001C pin 10. The output of the operational amplifier is fed to auxiliary connector J1011 pin 3 through resistor R1002. The resistor limits the output current and is part of the amplifier's protection network. The X-PLOT protection network consists of diodes CR1003, CR1011, R1002, VR1012, and VR1011. If the X output goes above 5.8 V peak, VR1011 and CR1011 turn on, clipping U1001C pin 8 to about +6 V. If output goes below -5.8 V peak, VR1012 and CR1003 turn on, clipping U1001C pin 8 to about -6 V. The Y-PLOT protection components are CR1001, CR1002, R1001, VR1012, and VR1011.

### Power Supplies

The filters for all supplies are pi filters, consisting of two filter caps to ground, one on each side of a series choke.

Each filter circuit for the three supplies filter in both directions. The filters reduce noise on the power supply lines generated elsewhere in the instrument, and they also reduce noise generated by the X-Y plotter board as the noise goes back out to the supplies in the rest of the instrument. Capacitors C1003, C1004, and C1005 decouple and by-pass the supplies.

The +4.2 V output makes interfacing to various X-Y and Y-T devices easier. The +5 Vg goes to the anode of reverse voltage protection diode CR1014. The diode drops the voltage to +4.2 V. The +4.2 V goes through current limit resistor R1013 to the auxiliary connector output (J1011 pin 6).
