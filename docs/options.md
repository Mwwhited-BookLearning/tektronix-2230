# Tektronix 2230 Service Manual — Section 7: Options

OCR transcription of the service manual's Options section (46 pages),
rendered from `Tektronix_2230_Service_Manual-7-Options.pdf` (not in
the repo — see the user's own manual copy). Primary source material —
transcribed as printed, not summarized. Figures/diagrams/photos are
noted with placeholders, not redrawn.

**Signal names with a printed overbar** (indicating an active-low/
inverted signal, e.g. a line over `RESET`) are written with a leading
`/` instead (`/RESET`) — the standard plain-text convention for a
signal that can't be typeset with a real overline, and one that stays
grep-able (searching `RESET` still finds `/RESET`).

<!-- page 7-1 -->

## OPTIONS

### INTRODUCTION

This part contains a general description of instrument options available at the time of publication of this manual. Additional information about instrument options and option availability can be obtained either by consulting the current Tektronix Product Catalog or by contacting your local Tektronix Field Office or representative.

### POWER CORD OPTIONS

Instruments are shipped with the detachable power-cord configuration ordered by the customer. Descriptive information about the international power-cord options is provided in "Preparation for Use" in Section 2. The following list identifies the Tektronix part numbers for the available power cords.

| Power cord | Part number |
|---|---|
| Standard (United States) | 161-0104-00 |
| Option A1 (Universal Euro) | 161-0104-06 |
| Option A2 (United Kingdom) | 161-0104-07 |
| Option A3 (Australian) | 161-0104-05 |
| Option A4 (North American) | 161-0104-08 |
| Option A5 (Switzerland) | 161-0167-00 |

### OPTION 33

Option 33, the **Travel Line** option, provides impact protection needed for rough industrial and service environments. When the instrument is ordered with Option 33, the instrument comes equipped with the Accessory Pouch and the Front Panel Cover, front and rear mounted shock absorbing rubber guards, an easy-to-use power cord wrap, and a carrying strap.

### OPTION 10 AND OPTION 12

#### INTRODUCTION

Option 10 provides a communications interface and additional memory for the instrument. The interface implemented conforms to the specifications contained in IEEE Standard Digital Interface for Programmable Instrumentation (ANSI/IEEE Std 488-1978), commonly referred to as the General Purpose Interface Bus (GPIB). It also complies with a Tektronix Standard relating to GPIB Codes, Formats, Conventions and Features.

Option 12 provides a communications interface and additional memory for the instrument. The interface implemented conforms to RS-232-C specifications. It also complies with a subset of the Tektronix Codes, Formats, Conventions and Features standard.

<!-- page 7-2 -->

Three indicators, displayed on the crt and labeled on the bezel tag, display the condition of the options. A battery backed-up CMOS memory and its battery are also included in the options. Option commands allow saving additional SAVE REF waveforms in the memory.

**WARNING**

*The battery used in this device contains lithium. Do not expose to heat. Do not short terminals. See service information for complete instructions.*

The communication options allow remote control of oscilloscope functions. This remote control is accomplished by messages sent to the instrument via either the GPIB (IEEE-488 Standard Bus) or the RS-232-C interface. Messages used are defined either in ANSI/IEEE-488-1978 or in the Tektronix standard on Codes, Formats, Conventions, and Features. Messages to the option can have one of three purposes:

1. Query the state of the oscilloscope.

2. Query the results of measurements made.

Set the instrument operation mode.

The main purpose of the communication options is to allow digitized waveform data to be sent and received by the instrument.

### STANDARD FUNCTIONS, FORMATS, AND FEATURES

The interface-function repertoire of a GPIB instrument, in terms of interface-function subsets, is identified in ANSI/IEEE Std 488-1978. The status of subsets applicable to this instrument with Option 10 are listed in Table 7-1.

Both the GPIB interface and the RS-232-C interface conform to a Tektronix standard on Codes, Formats, Conventions, and Features of messages sent over the bus to communicate with other instruments equipped with a like interface. Specific features implemented in this instrument are listed in Table 7-2, and specific formats implemented are shown in Table 7-3.

**Table 7-1**
**Function Subsets Implemented**

| Function Subset | Capability | States Omitted | Other Requirements | Other Subsets Required |
|---|---|---|---|---|
| SH1 (Source Handshake) | Complete Capability | None | None | T6 |
| AH1 (Acceptor Handshake) | Complete Capability | None | None | None |
| T6 (Talker) | Basic Talker, Serial Poll, Talker Only, Unaddress if MLA | None | Include [MLA (ACDS)] | SH1 and L3 |
| L3 (Listener) | Basic Listener, Listen Only, Unaddress if MTA | None | Include [MTA (ACDS)] | AH1 and T6 |
| SR1 (Service Request) | Complete Capability | None | None | T6 |
| RL2 (Remote/Local) | No Local Lock Out | LWLS and RWLS | None | L3 |
| PP0 (Parallel Poll) | No Capability | All | None | None |
| DC1 (Device Clear) | Complete Capability (Selective Device Clear) | None | None | L3 |
| DT0 (Device Trigger) | No Capability | All | None | None |
| C0 (Controller) | No Capability | All | None | None |
| E2 (Drivers) | Three-state | | | |

<!-- page 7-3 -->

**Table 7-2**
**Specific Format Choices**

| Format Parameter | Choice Made |
|---|---|
| Format Characters | Not transmitted; ignored on reception. |
| Message Terminator | Either EOI or LF modes can be selected for implementation. |
| Measurement Terminator | Follows program message-unit syntax. |
| Link Data (Arguments) | Used in Listen and Talk. |
| Multiple Event Reporting | Not implemented. |
| Instrument Identification Query | Descriptors added for all options, including GPIB. |
| Set Query | Extended by using other commands. |
| Device Trigger (DT) | Not implemented. |
| Init Command | Causes the instrument to return to a power-on condition. All operating modes will then agree with front-panel settings. |
| Time/Date Commands | Not implemented. |
| Stored Setting Commands | Not implemented. |
| Waveform Transmission | Implemented. |
| Return to Local (rtl) | Asserted when any front-panel control attempts to change a GPIB-controllable function. |
| IEEE 728 | Compliance not intended. |

### PERFORMANCE CONDITIONS

The specifications for the GPIB Option, RS-232-C Option, and the Memory Option are listed in Table 7-4. All other specifications for the instrument (including the performance conditions) are identical to those specified in "Specification" in Section 1 of this manual.

### OPTIONS SIDE PANEL

The instrument is supplied with one of three possible side panels. The standard side panel (Figure 3-8) includes one AUXILIARY connector. The Option 10 side panel (Figure 7-1A) includes one AUXILIARY connector, one GPIB (IEEE 488-1978) interface port, and one PARAMETERS switch. The side panel for Option 12 instruments (Figure 7-1B) includes one AUXILIARY connector, one RS-232-C interface port (includes one DTE and one DCE connector), and one PARAMETERS switch. The Controls, Connectors, and Indicators part of this manual contains information on the use of the AUXILIARY Connector. Refer to Figure 7-1 for location of items 46 through 51.

**Table 7-3**
**Implementation of Specific Features**

| Feature | Choice Made | Comments |
|---|---|---|
| Secondary Addressing | Not implemented. | |
| Indicators | ADDR (addressed), SRQ (service request), and PLOT (acquisitions locked out) indicators are included. | |
| Parameter Selection | 10 position switch. Instrument reinitializes to power-up state with exception of issuing power-on service request. | To retain the instrument's preinitialization setup, the controller should store the response to a SET query before a change is made; then return the settings afterwards. |

46. **AUXILIARY Connector**—Provides connections for an X-Y Plotter and an External Clock input (see Controls, Connectors, and Indicators).

47. **GPIB Connector**—Provides the ANSI/IEEE Std 488-1978 compatible electrical and mechanical connection to the GPIB. The connector is only on instruments with Option 10. The function of each pin of the connector is shown in Table 7-5.

<!-- page 7-4 -->

**Table 7-4**
**Option Electrical Characteristics**

| Characteristics | Performance Requirements |
|---|---|
| **EXTENDED MEMORY** | |
| Power-Down | |
| &nbsp;&nbsp;Battery Voltage | Memory retained for battery voltages greater than 2.3 V.ᵃ |
| &nbsp;&nbsp;Data Retention | Memory maintained at least 6 months without instrument power.ᵃ |
| &nbsp;&nbsp;Battery Life | Power-down data retention specification shall be maintained for 3 years without battery change.ᵃ |
| Power-Down Detection | |
| &nbsp;&nbsp;Threshold | Fail asserted for supply drop to less than 4.75 V.ᵃ Reset held until supply is greater than 5.0 V.ᵃ |
| &nbsp;&nbsp;Reset Delay | Power-down interrupt to reset delay ≥ 1 ms.ᵃ |
| **GPIB OPTION** | |
| GPIB Requirements | Complies with ANSI/IEEE Standard 488-1978.ᵃ |
| **RS-232-C OPTION** | |
| RS-232-C Requirements | Complies with EIA Standard RS-232-C.ᵃ |
| Baud Rates | |
| &nbsp;&nbsp;Available Rates | 110, 300, 600, 1200, 1800, and 2400 baud. |
| &nbsp;&nbsp;Accuracy | < 1% error.ᵃ |

ᵃ Performance Requirement not checked in manual.

**[Figure 7-1. Option side panels — (A) GPIB SIDE PANEL: AUXILIARY CONNECTOR (RELAY N.O./+4.2 VDC/RELAY COMM/SIG GND/EXT CLK/RELAY N.C./X/Y/SHIELD GND, item 46), PARAMETERS 10-position switch (ADDR bits 1/2/4/8/16, EDI, LF OR EDI, LON, TON, AUX 1, AUX 2, item 48), CAUTION ≤25Vpk AND <100mA ABS. MAX APPLIED TO ANY CONNECTOR, IEEE STD 488 PORT (SH1,AH1,T5,L3,SR1,RL2,PPØ,DC1,DTØ,CØ,E2, item 47). (B) RS-232-C SIDE PANEL: AUXILIARY CONNECTOR (item 46), PARAMETERS 10-position switch (item 49), RS-232-C PORT with RS232 DTE connector (item 50) and RS232 DCE connector (item 51), same CAUTION label. Drawing number 4999-101.]**

Figure 7-1. Option side panels.

<!-- page 7-5 -->

**Table 7-5**
**GPIB Connector**

| Pin | Line Name | Description |
|---|---|---|
| 1 | DIO1 | IEEE-488 Data I/O |
| 2 | DIO2 | IEEE-488 Data I/O |
| 3 | DIO3 | IEEE-488 Data I/O |
| 4 | DIO4 | IEEE-488 Data I/O |
| 5 | EOI | IEEE-488 END or Identify |
| 6 | DAV | IEEE-488 Handshake |
| 7 | NRFD | IEEE-488 Handshake |
| 8 | NDAC | IEEE-488 Handshake |
| 9 | IFC | IEEE-488 Input |
| 10 | SRQ | IEEE-488 Output |
| 11 | ATN | IEEE-488 Input |
| 12 | SHIELD | System Ground (Chassis) |
| 13 | DIO5 | IEEE-488 Data I/O |
| 14 | DIO6 | IEEE-488 Data I/O |
| 15 | DIO7 | IEEE-488 Data I/O |
| 16 | DIO8 | IEEE-488 Data I/O |
| 17 | REN | IEEE-488 Input |
| 18 | GND | Digital Ground (DAV) |
| 19 | GND | Digital Ground (NRFD) |
| 20 | GND | Digital Ground (NDAC) |
| 21 | GND | Digital Ground (IFC) |
| 22 | GND | Digital Ground (SRQ) |
| 23 | GND | Digital Ground (ATN) |
| 24 | GND | Digital Ground (LOGIC) |

**Table 7-6**
**GPIB PARAMETERS Switch**

| Switch Section | Switch Position | Function |
|---|---|---|
| 1 | 0 / 1 | Address selection: 0 / 1 |
| 2 | 0 / 1 | Address selection: 0 / 2 |
| 3 | 0 / 1 | Address selection: 0 / 4 |
| 4 | 0 / 1 | Address selection: 0 / 8 |
| 5 | 0 / 1 | Address selection: 0 / 16 |
| 6 | 0 / 1 | Terminator selection: EOI / LF or EOI |
| 7 | 0 / 1 | No function / LON |
| 8 | 0 / 1 | No function / TON |
| 9 | | Printer/plotter selectionᵃ |
| 10 | | Printer/plotter selectionᵃ |

ᵃSwitches 9 and 10 select printer/plotter devices at power-up. The devices may be changed after power-up using Option commands, or by using the MENU. Two EPSON(tm) formats are selectable. EPS7 uses seven print wires per head pass, and is usually slower. It is the chr$(27) "L" mode. EPS8 uses eight print wires per head pass, and is usually the faster print-head speed. It is the chr$(27) "Y" mode. In this mode most Epson and Epson-compatible printers will not strike any print wire more often than every second pixel. EPS8 is selected when parity is disabled. Devices are selected with the following switch positions:

| Switch 9 | Switch 10 | Device Selected |
|---|---|---|
| 0 | 0 | HP-GL® plotter |
| 1 | 0 | [EPS7] or EPS8 |
| 0 | 1 | ThinkJet® printer |

® HP-GL and ThinkJet are trademarks of Hewlett - Packard Company. Epson is a trademark of Epson Corporation.

48. **GPIB PARAMETER Switch**—Allows the selection of setup options for the GPIB interface. The switch is read at power-up and when interface clear messages are received. Five sections of the switch select the GPIB address, one selects the terminator, two select talk/listen modes, and two are used for printer/plotter selection. The function of each switch section is shown in Table 7-6.

49. **RS-232-C PARAMETER Switch**—Allows the selection of setup options for the RS-232-C interface. The switches are read at power-up and when interface clear messages are received. Four sections of the switch select the baud rate, three select parity, one selects the terminator, and two are for printer/plotter selection. The function of each switch section is shown in Table 7-7.

<!-- page 7-6 -->

**Table 7-7**
**RS-232-C PARAMETERS Switch**

| Switch Section | Switch Position | Function |
|---|---|---|
| 1 | — | Baud rateᵃ |
| 2 | — | Baud rateᵃ |
| 3 | — | Baud rateᵃ |
| 4 | — | Baud rateᵃ |
| 5 | 0 | Parity enable: Parity error will NOT cause SRQ (also selects 8-bit character length) |
| 5 | 1 | Parity error WILL cause SRQ (also selects 7-bit character length) |
| 6 | | Parity selectᵇ |
| 7 | | Parity selectᵇ |
| 8 | 0 | Line terminator selection: Lines are terminated with carriage return (CR) |
| 8 | 1 | Lines are terminated with carriage return-line feed (CR-LF) |
| 9 | | Printer/plotter selectionᶜ |
| 10 | | Printer/plotter selectionᶜ |

ᵃSee Table 7-8

ᵇSee Table 7-9

ᶜSwitches 9 and 10 select printer/plotter devices at power-up. The devices may be changed after power-up using Option commands, or by using the MENU. Two EPSON® formats are selectable. EPS7 uses seven print wires per head pass, and is usually slower. It is the chr$(27) "L" mode. EPS8 uses eight print wires per head pass, and is usually the faster print-head speed. It is the chr$(27) "Y" mode. In this mode most Epson and Epson-compatible printers will not strike any print wire more often than every second pixel. EPS8 is selected when parity is disabled. Devices are selected with the following switch positions:

| Switch 9 | Switch 10 | Device Selected |
|---|---|---|
| 0 | 0 | HP-GL® plotter |
| 1 | 0 | [EPS7] or EPS8 |
| 0 | 1 | ThinkJet® printer |

® HP-GL and ThinkJet are trademarks of Hewlett - Packard Company. Epson is a trademark of Epson Corporation.

**Table 7-8**
**Baud Rate**

| Index | Switch Position (4 3 2 1) | Baud Rate |
|---|---|---|
| 0 | 0 0 0 0 | 50 |
| 1 | 0 0 0 1 | 75 |
| 2 | 0 0 1 0 | 110 |
| 3 | 0 0 1 1 | 134.5 |
| 4 | 0 1 0 0 | 150 |
| 5 | 0 1 0 1 | 300 |
| 6 | 0 1 1 0 | 600 |
| 7 | 0 1 1 1 | 1200 |
| 8 | 1 0 0 0 | 1800 |
| 9 | 1 0 0 1 | 2000 |
| 10 | 1 0 1 0 | 2400 |
| 11 | 1 0 1 1 | 3600 |
| 12 | 1 1 0 0 | 4800 |
| 13 | 1 1 0 1 | 7200 |
| 14 | 1 1 1 0 | 9600 |
| 15 | 1 1 1 1 | Off Line |

50. **RS-232-C DTE Connector**—Provides connection meeting the EIA RS-232-C standard for data terminal equipment. The connector is shown in Figure 7-1B. Table 7-10 lists the function of each pin of the connector. The connector is only on Option 12 instruments.

**NOTE**

*Some controllers use nonstandard connectors and pin assignments. Consult your controller operators manual for specific interfacing information.*

51. **RS-232-C DCE Connector**—Provides connection meeting the EIA RS-232-2 standard for data communications equipment. The connector is shown in Figure 7-1B. Table 7-11 lists the function of each pin of the connector. The connector is only on Option 12 instruments.

**NOTE**

*Some controllers use nonstandard connectors and pin assignments. Consult your controller operators manual for specific information.*

<!-- page 7-7 -->

**Table 7-9**
**Parity Selectionᵃ**

| Index | Switch Position (6 7) | Parity Type | Comment |
|---|---|---|---|
| 0 | 0 0 | ODD | The most significant bit (MSB) is set or cleared so that the number of 1s per byte is ODD. |
| 1 | 0 1 | EVEN | The MSB is set or cleared so that the number of 1s per byte is even. |
| 2 | 1 0 | MARK | The MSB is set. |
| 3 | 1 1 | SPACE | The MSB is cleared. |

ᵃCharacters are always accepted if possible. An SRQ is sent if the received parity doesn't match the parity selected. Parity must be disabled (switch position 5 set to 0) for binary transfers to take place.

**Table 7-10**
**RS-232-C DTE Connector**

| Pin | Signal Name (Internal) | Signal Name (External) | Function |
|---|---|---|---|
| 1 | CHAS GND | CHAS GND | Chassis ground |
| 2 | ITXD | TXD | Transmitted data |
| 3 | IRXD | RXD | Received data |
| 4 | IRTS | RTS | Request to send |
| 5 | ICTS | CTS | Clear to send |
| 6 | IDSR | DSR | Data set ready |
| 7 | SIG GND | SIG GND | Signal ground |
| 8 | IRLSD2 | RLSD | Received line signal detect |
| 20 | IDTR | DTR | Data terminal ready |

**Table 7-11**
**RS-232-C DCE Connector**

| Pin | Signal Name (Internal) | Signal Name (External) | Function |
|---|---|---|---|
| 1 | CHAS GND | CHAS GND | Chassis ground |
| 2 | IRXD | TXD | Transmitted data |
| 3 | ITXD | RXD | Received data |
| 4 | ICTS | RTS | Request to send |
| 5 | IRTS | CTS | Clear to send |
| 6 | IDTR | DSR | Data set ready |
| 7 | SIG GND | SIG GND | Signal ground |
| 8 | IRLSD1 | RLSD | Received line signal detect |
| 20 | IDSR | DTR | Data terminal ready |

<!-- page 7-8 -->

### INTERFACE STATUS INDICATORS

Three indicators appear in the crt readout to indicate the status of the communications options. The indicators are labeled SRQ, ADDR, and PLOT on the crt bezel, and appear as intensified lines in the crt under the labels. Refer to Figure 7-2 for the location of items 52 through 54.

52. **SRQ Indicator**—Indicates the communications option requires service by the controller. Service requests are cleared when the instrument has been polled for its status and no further warning or error conditions are pending. The communication options assert Service Request (SRQ) when powered up.

53. **ADDR Indicator**—Indicates the instrument is addressed to talk or listen on the GPIB option. Indicates carrier detect on the RS-232-C option.

54. **PLOT Indicator**—Indicates the communication option is currently sending waveform data over its interface and acquisitions are inhibited.

### MENU SELECTED FUNCTIONS

The following functions are available as part of the ADVANCED FUNCTIONS Menu on instruments containing the GPIB or RS-232-C options.

REFERENCE—Allows a SAVE REF memory to be Erased or Copied.

ERASE—Selects and erases a nonvolatile SAVE REF memory.

COPY—Selects and copies one nonvolatile SAVE REF memory to another SAVE REF memory.

COMM—Allows the selection of parameters for optional communications options, when they are present.

DATA—Selects the data-coding format, source or destination of the data, and channel selection for data transmissions.

STOP BITS—Selects the number of stop bits for RS-232-C data transmissions.

FLOW—Sends the waveform data to a listen only device.

**[Figure 7-2. Interface status indicators — photo of the Tektronix 2230 front panel/crt bezel labeled "SRQ", "ADDR", "PLOT" above the graticule, with intensified marker lines on the crt at items 52 (SRQ), 53 (ADDR), 54 (PLOT), and a partial view of the right-side control panel (WAVEFORM REFERENCE/MEMORY DISPLAY buttons, MENU SELECT, INTENSITY A-B knob, TRACE ROTATION, BEAM FIND, STORAGE/READOUT INTENSITY, FOCUS, POWER ON/OFF switch). Drawing number 4999-102.]**

Figure 7-2. Interface status indicators.

<!-- page 7-9 -->

Menus are displayed with as much of the selection path visible as possible. This method displays the current location in the menu as well as the available alternatives and messages on how to make a selection.

The COMM Menu:

The COMM menu resides under the ADVANCED FUNCTIONS menu:

```
ADVANCED FUNCTIONS
    REFERENCE
    COMM
    ACQ MODE SETUP TREE
    DIAGNOSTICS
```

Once COMM is selected, its submenus appear:

```
COMM
    DATA
    STOP BITS (Option 12 only)
    FLOW (Option 12 only)
```

If DATA is selected, its functions appear:

```
DATA
    ENCDG
    SOURCE
    TARGET
    CHANNEL
```

The ENCDG function selects waveform encoding for transmission and expected encoding for waveform reception. At power-up, the default encoding is binary. Make one of three choices from the menu:

```
ENCDG
    ASCII
    BINARY
    HEX
```

The SOURCE function selects whether one of the Reference Memories or the current acquisition is the source for waveform transfers. If REF is selected, use the Cursor knob to select the actual reference. REF4 is an explicit 4K reference:

```
SOURCE
    REF
    ACQ
```

The TARGET function is nearly identical to the SOURCE function. The only difference is that ACQ is not a valid TARGET. The TARGET reference is the destination for all waveforms sent to the instrument:

```
TARGET
    REF
```

Like SOURCE, waveform TARGET references are selected with the Cursor knob.

The CHANNEL function selects the channel whose waveform is sent. With the exception of XY waveforms, only data from one channel is sent at a time, even if both channels were acquired in ALT or CHOP Vertical Mode:

```
CHANNEL
    CH1
    CH2
```

The STOP BITS function, available ONLY on Option 12 (RS-232-C), sets the number of stop bits. Use the Cursor knob to select.

The FLOW function, available ONLY on Option 12 (RS-232-C), enables or disables Control-S/Control-Q handshaking. FLOW must be OFF during binary waveform transfers.

```
FLOW
    ON
    OFF
```

### GPIB PARAMETER SELECTION

Selection of GPIB parameters (primary address, message terminator, and talk/listen mode) can be made at any time using the GPIB PARAMETERS switch and Table 7-6.

#### Primary Address

The selected GPIB address establishes both the primary talk and listen addresses for the oscilloscope. It can be set to any value between 0 and 31, inclusive.

**NOTE**

*This instrument has no provisions for secondary addressing as defined by ANSI/IEEE Std 488-1978.*

With an address of 31, the instrument still presents an active load but does not respond to nor interfere with any bus traffic. This is useful for changing the instrument's status without turning off the oscilloscope's power.

#### Input End-of-Message Terminator

The end-of-message terminator can be selected to be either the End-or-Identify (EOI) interface signal or the Line-Feed (LF) character.

<!-- page 7-10 -->

When EOI (normal mode) is selected as the terminator, the instrument will:

- Accept only EOI as the end-of-message terminator.
- Assert EOI concurrently with the last byte of a message.

When LF is selected as the terminator, the instrument will:

- Accept either LF or EOI as the end-of-message terminator.
- Send Carriage Return (CR) followed by LF at the end of every message, with EOI asserted concurrently with the LF.

#### Talk/Listen Mode

Three talk/listen modes are selectable:

- TALK ONLY mode allows the instrument to send data over the GPIB.
- LISTEN ONLY mode permits the instrument to receive data over the GPIB.
- TALK/LISTEN mode (both TON and LON modes selected) allows the instrument to both send and receive data over the GPIB.

The default mode is TALK/LISTEN.

To select or change the talk/listen mode, select TON and/or LON using the GPIB PARAMETERS switch and Table 7-6.

### RS-232-C PARAMETER SELECTION

Selection of RS-232-C parameters (baud rate, parity, and line terminator) can be made at any time using the RS-232-C PARAMETER switch and Table 7-7 through Table 7-9.

#### Baud Rate

The selected RS-232-C baud rate establishes the baud rate used by the instrument for both sending and receiving data. Baud rates selectable are listed in Table 7-9.

When OFF LINE is selected as the baud rate, the instrument still presents an active load but does not respond to nor interfere with any bus traffic. This is useful for changing the instrument's status without turning off the oscilloscope's power.

Use Table 7-7, Table 7-8 and the PARAMETERS switch to select the desired baud rate.

#### Parity

The parity parameters selected determine the instrument response to received parity errors and the parity of data sent by the instrument.

Section 5 of the PARAMETERS switch determines whether or not received parity errors will cause an SRQ (see Table 7-7).

Sections 6 and 7 of the PARAMETERS switch determine the parity used when transmitting data over the bus. ODD, EVEN, MARK, or SPACE are selectable (see Table 7-9).

#### Line Terminator

The line terminator can be selected to be either the carriage return (CR) or the CR and Line-Feed (LF) characters.

When CR (normal mode) is selected as the terminator, the instrument will:

- Accept only CR as the line terminator.
- Send CR as the last byte of a message.

When CR LF is selected as the terminator, the instrument will:

- Accept either CR or LF as the line terminator.
- Send Carriage Return (CR) followed by LF at the end of every message.

Section 8 of the PARAMETERS switch determines the line terminator. Select the desired line terminator using the PARAMETERS switch and Table 7-7.

### MESSAGES AND COMMUNICATION PROTOCOL

Option commands can set the instrument operating mode, query the results of measurements made, or query the state of the oscilloscope. The commands are specified

<!-- page 7-11 -->

in mnemonics that are related to the functions implemented. For example, the command INIt initializes instrument settings to states that would exist if the instrument's power was cycled. To further facilitate programming, command mnemonics are similar to front-panel control names.

**NOTE**

*All measurement results returned by the options have the same accuracy as the main instrument.*

#### Commands

Commands for this instrument, like those for other Tektronix instruments, follow the conventions established in a Tektronix Codes and Formats Standard. The command words were chosen to be as understandable as possible, while still allowing a familiar user to shorten them as much as necessary, as long as the result is not ambiguous. Syntax is also standardized to make the commands easier to learn.

In the command lists (Tables 7-13 through 7-24), headers and arguments are listed in a combination of uppercase and lowercase characters. The instrument accepts any abbreviated header or argument containing at least the characters shown in uppercase. Any characters added to the abbreviated (uppercase) version must be those shown in lowercase. For a query, the question mark must immediately follow the header. For example, any of the following formats are acceptable:

```
VMO?
VMOd?
VMOde?
```

#### Headers

A command consists of at least a header. Each command has a unique header, which may be all that is needed to invoke a command; e.g.,

```
INIt
OPC
```

#### Arguments

Some commands require the addition of arguments to their headers to describe exactly what is to be done. If there is more to the command than just the header (including the question mark if it is a query), then the header must be followed by at least one space.

In some cases, the argument is a single word; e.g.,

```
REFF REF4
PLOt STArt
```

In other cases, the argument itself requires another argument. When a second argument is required, a colon must separate the two arguments; e.g.,

```
ACQuisition REPetitive:SAMple
WFMpre XINcr:1.0E-3
```

Where a header has multiple arguments, the arguments (or argument pairs, if the argument has its own argument) must be separated by commas; e.g.,

```
DATa ENCdg:BINary,CHAnnel:CH2
VMOde? CH1,CH2,ADD
```

#### Default Arguments

Arguments shown within brackets ([argument]) are defaults. In any command that has a default, omitting the default argument selects the default. Do not confuse default arguments with power-up default conditions; the power-up defaults may differ from the argument default in the same function. The default argument may be sent in any command. Do not send the brackets as part of the default argument. All commands that do not have a default must always include a argument, where one or more exists.

#### Command Separator

It is possible to put multiple commands into one message by separating the individual commands with a semicolon; e.g.,

```
DATa ENCdg:BINary,CHAnnel:CH2;WFMpre XINcr:1.0E-3
```

#### Command Formatting

Commands sent to the oscilloscope must have the proper format (syntax) to be understood; however, this format is flexible in that many variations are acceptable. The following paragraphs describe this format and the acceptable variations.

The oscilloscope expects all commands to be encoded as either uppercase or lowercase ASCII characters. All data output is in uppercase.

Spaces, Carriage Returns, and Line-Feed characters are all formatting characters that can be used to enhance the readability of command sequences. As a general rule, these characters can be placed either after commas and semicolons or after the space that follows a header.

<!-- page 7-12 -->

#### Message Terminator

As previously explained, GPIB messages may be terminated with either EOI or LF. Some controllers assert EOI concurrently with the last data byte; others use only the LF character as a terminator. The GPIB interface can be set to accept either terminator. With EOI selected, the instrument interprets a data byte received with EOI asserted as the end of the input message; it also asserts EOI concurrently with the last byte of an output message. With the LF setting, the instrument interprets the LF character without EOI asserted (or any data byte received with EOI asserted) as the end of an input message; it transmits a Carriage Return character followed by Line Feed (LF with EOI asserted) to terminate output messages.

RS-232-C messages may be terminated with either carriage return (CR) or the CR and Line-Feed (LF) characters. The RS-232-C Option can be set to accept either terminator. With CR selected, the instrument interprets a line ending in CR as the end of the input message; it also sends CR as the last byte of an output message. With the CR and LF setting, the instrument interprets either the CR character or the LF character as the end of an input message; it transmits a Carriage return character followed by a Line Feed to terminate output messages.

#### Numeric Arguments

Many commands have numeric arguments. The numeric arguments are shown in either `<NR1>`, `<NR2>`, or `<NR3>` notation. These symbols refer to the format of the numeric argument. All values must be decimal (base 10).

Table 7-12 depicts the number formats for numeric arguments in the command set. As shown in the table, both signed and unsigned numbers are accepted; but unsigned numbers are interpreted to be positive. Any command or query that has an `<NR2>` argument may have that argument sent to the the instrument in either `<NR2>` or `<NR1>` format. Likewise, an `<NR3>` argument may be sent in `<NR3>`, `<NR2>` or `<NR1>` format.

### COMMAND LISTS

Tables 7-13 through 7-24 describe all commands available in the instrument equipped with either the GPIB or RS232 Option. Query and Response examples are shown in Table 7-25. The first column lists the name (or header) of the command. The capitalized letters must be present to identify the command, while those shown in lowercase are optional. The second column lists arguments that can

**Table 7-12**
**Numeric Argument Format for Commands**

| Numeric Argument Symbol | Number Format | Examples |
|---|---|---|
| `<NR1>` | Integers | +1, 2, -1, -10 |
| `<NR2>` | Explicit decimal point (floating point) | -3.2, +5.1, 1.2 |
| `<NR3>` | Floating point in scientific notation | +1.E-2, 1.0E+2, 1.E-2, 0.02E+3 |

be associated with the command. The third column lists arguments associated with the first argument. Finally, descriptions of each command and its arguments are contained in the last column.

One or more arguments, separated by commas, may be given in a query to request only the information wanted. For example: CH1? VOLts,COUpling. However, some headers in the command tables are Query only, that is, they may only be sent as queries; never as commands. The queries are listed in the same general format as command/query headers. The arguments returned by the instrument are shown in smaller type. Do not send these arguments as part of the query; they are returnable only. For example, AC, DC, or GND are returned in response to a CH1? COU.

Instrument commands are presented in tables divided into the following functional groups:

| Table | Command Group | Page |
|---|---|---|
| 7-13 | Vertical Commands | 7-13 |
| 7-14 | Horizontal Commands | 7-14 |
| 7-15 | Trigger Commands | 7-15 |
| 7-16 | Cursors Commands | 7-16 |
| 7-17 | Display Commands | 7-17 |
| 7-18 | Acquisition Commands | 7-18 |
| 7-19 | Save and Recall References Commands | 7-20 |
| 7-20 | Waveforms Commands | 7-22 |
| 7-21 | Waveform Preamble Fields | 7-23 |
| 7-22 | Service Request Group Commands | 7-25 |
| 7-23 | Miscellaneous Commands | 7-26 |
| 7-24 | RS-232-C Specific Commands | 7-26 |

<!-- page 7-13 -->

**Table 7-13**
**Vertical Commands**

| Header | Argument | Link Argument | Description |
|---|---|---|---|
| CH1? | | | Query only. Returns all current CH1 settings: `CH1 VOL:<NR3>, COU:string;`, where `<NR3>` is the volts/div setting and string is either AC, DC, or GND. |
| | VOLts | `<NR3>` | Query only. Returns Channel 1 volts/div reading including probe attenuation. For example: 5.0E0 is returned when the CH1 VOLTS/DIV switch is set to 50 mV and a 100X probe is attached. A warning SRQ is generated if the CH1 Variable knob is not in the calibrated position. |
| | COUpling | AC, DC, GND | Query only. Returns the current position of the CH1 INPUT COUPLING switch: `CH1 COU:string;`, where string is either AC, DC, or GND. |
| CH2? | | | Query only. Like CH1?, except includes an INVert query response. |
| | INVert | ON, OFF | Query only. Returns status of CH2 INVERT switch: `CH2 INV:string;`, where string is either ON or OFF. |
| VMOde? | CH1, CH2, ADD, CHOp, ALT, XY | | Query only. Returns current state of the vertical display: `VMO string;`, where string is either CH1, CH2, ADD, CHOp, ALT, or XY. |
| PROBe? | CH1, CH2 | `<NR1>` | Query only. Returns the probe attenuation coding: `CHn PROB:<NR1>;`, where n is either 1 or 2 and `<NR1>` is either 1000, 100, 10, 1, -1, or -2. PROBe returns -2 for unknown encoding, -1 for identify, and positive values for proper probe encoding. |

<!-- page 7-14 -->

**Table 7-14**
**Horizontal Commands**

| Header | Argument | Link Argument | Description |
|---|---|---|---|
| HORizontal? | | | Query only. Returns all current Horizontal settings in the form: `HOR MOD:string, ASE:<NR3>, BSE:<NR3>, EXT:string;` where the MODe string is either ASWeep, AINtb, or BSWeep. The EXTclk string is either ON or OFF. |
| | MODe | ASWeep, AINtb, BSWeep | Query only. Returns the current Horizontal Mode setting in the form: `HOR MOD:string;` where string is either ASWeep, AINtb, or BSWeep. |
| | ASEcdiv | `<NR3>` | Query only. Returns the current A SEC/DIV setting. The `<NR3>` value returned is zero when the knob is set to EXT CLK. |
| | BSEcdiv | `<NR3>` | Query only. Returns an `<NR3>` value representing the current B SEC/DIV setting. |
| | HMAg | ON, OFF | Query only. Returns status of Horizontal Magnifier (X10 PULL) in the form: `HOR HMA:string`, where string is either ON or OFF. |
| | EXTclk | ON, OFF | Query only. Returns status of EXTclk in the form: `HOR EXT:string;`, where string is either ON or OFF. |
| DELAy? | | | Query only. Returns current Horizontal delay settings in the form: `DELA VAL:<NR3>, UNI:string;`. |
| | VALue | `<NR3>` | Query only. Returns the current DELay VALue setting in the form `DELA VAL:<NR3>;`, where `<NR3>` is the delay value returned in units indicated by the UNIts query. |
| | UNIts | S, DIVs | Query only. Returns the current DELAy UNIts in the form: `DELA UNI:string;` where string is either S (seconds) or DIVs. |

<!-- page 7-15 -->

**Table 7-15**
**Trigger Commands**

| Header | Argument | Link Argument | Description |
|---|---|---|---|
| ATRigger? | | | Query only. Returns current A Trigger status: `ATR MOD:string`, where the MODe string is either NORmal, PPAuto, or SGLswp. |
| | MODe | NORmal, PPAuto, SGLswp | Query only. Returns current A Trigger Mode setting in the form `ATR MOD:string;`, where string is either NORmal, PPAuto, or SGLswp. PPAuto is returned for both P-P AUTO and TV FIELD modes. |
| SGLswp | ARM, DONe | | As a query, SGLswp returns the status of the SGLswp trigger mode: `SGL string;`, where string is either ARM or DONe. ARM indicates that the sweep is armed or running. DONe indicates that a sweep is complete. An execution error SRQ is generated if SGL SWP is not ON. As a command, only `SGLswp ARM;` is legal. ARM re-arms a completed sweep. An execution warning SRQ is generated if SGL SWP is not ON or if ARM is active. |
| TRIggered? | ON, OFF | | Query only. Returns the status of the TRIG'D indicator, either `TRI ON;` or `TRI OFF;`. |

<!-- page 7-16 -->

**Table 7-16**
**Cursor Commands**

| Header | Argument | Link Argument | Description |
|---|---|---|---|
| CURSor | SELect | CURS1, CURS2 | Selects the cursor to be positioned. |
| | TARget | ACQuisition, REF1, REF2, REF3, REF4 | Selects the waveform on which cursors appear. Although the TARget waveform can be selected with either CURS1 or CURS2, both cursors will be on the last selected TARget. REF4 is the 4k reference location. |
| | CHAnnel | CH1, CH2 | Selects active cursor channel. CHAnnel determines which channel's DELTAV or DELTAT values are returned. Cursor positioning is independent of channel. |
| | POSition | `<NR1>` | Selects the cursor screen position in the range of 0-1023 for 1024 point waveforms and 0-4095 for 4096 point waveforms. If the value is outside the defined range, the value is limited and a warning SRQ is generated. |
| DELTAV? | VALue | `<NR3>` | Query only. Returns the voltage difference between cursors: `DELTAV VAL:<NR3>;`. An SRQ is sent if the voltage cannot be measured. VALue is returned in PERcent if the VAR knob is uncalibrated, otherwise Volts are returned. |
| | UNIts | V, PERcent | Query only. Indicates whether DELTAV VALue is returned in Volts or PERcent. |
| DELTAT? | VALue | `<NR3>` | Query only. Returns the time difference between cursors: `DELTAT VAL:<NR3>;`. An SRQ is sent if the time cannot be measured. VALue is returned in DIVs when in EXTCLK. |
| | UNIts | S, DIVs | Query only. Indicates whether DELTAT value is returned in S (seconds) or DIVs. |

<!-- page 7-17 -->

**Table 7-17**
**Display Commands**

| Header | Argument | Link Argument | Description |
|---|---|---|---|
| READout | ON, OFF | | Turns CRT readout ON or OFF. |
| MESsage | `<NR1>` | "string" | Command only. Writes text strings on row `<NR1>` of the screen. Legal values for `<NR1>` are 0 through 16; 1 writes to the bottom row, 16 writes to the top row, and 0 clears all messages and restores the default displays. The "string" must always be within quote marks and is displayed left justified. Long strings are truncated to approximately 40 characters. (Characters have proportional spacing.) Displaying multiple simultaneous messages may cause display flicker and may exceed display memory capacity. |
| PLOt | STArt | | Initiates a plot via the GPIB (Option 10) or RS-232-C (Option 12) interface port, or the XY Plotter Port. While the plot is in progress all commands or queries are ignored except for PLOt ABOrt, which terminates the plot. If enabled, an OPC SRQ is sent when the plot completes. |
| | ABOrt | | Terminates a plot in progress and returns the instrument to its previous mode. PLOt ABOrt is the only command or query the instrument responds to during a plot. |
| | AUTo | **[ON]**, OFF | Turns AUTo mode ON or OFF. If AUTo is ON, each waveform is plotted after it is acquired, however, the graticule will only be plotted once, if GRAt is ON. |
| | GRAt | **[ON]**, OFF | Determines if a plot will include a graticule image. |
| | FORmat | **[XY]**, HPGl, EPS7, EPS8, TJEt | Defines plot format and output port. FORmat reverts to XY if port is not configured for plotting. HPGl formats for HP-GL® compatible plotters. EPS7 and EPS8 format for 7 bit (low-speed, double density) and 8 bit (high-speed, double density) Epson® format printers, respectively. TJEt formats for the Hewlett-Packard ThinkJet® printer. A GPIB Controller In Charge may issue PLOt STArt to the oscilloscope, My Listen Address (MLA) to the printer or plotter, then My Talk Address (MTA) to the oscilloscope to produce a plot. |
| | SPEed | `<NR1>` | SPEed changes the analog plotter pen speed. `<NR1>` must be an integer from 1 through 10. Units are roughly in divisions per second. |

Epson is a trademark of Epson Corporation. HP-GL and ThinkJet are trademarks of Hewlett-Packard Company.

<!-- page 7-18 -->

**Table 7-18**
**Acquisition Commands**

| Header | Argument | Link Argument | Description |
|---|---|---|---|
| STORe? | ON, OFF | | Query only. Returns the operating mode of the instrument; either `STOR ON;` for digital storage mode, or `STOR OFF;` for analog mode. |
| ACQuisition | REPetitive | SAMple, ACCpeak, **[AVErage]** | Selects the acquisition algorithm for 0.05 us/div to 2 μs/div. |
| | HSRec | SAMple, **[ACCpeak]**, AVErage | Selects the acquisition algorithm for 5 μs/div and 10 μs/div. |
| | LSRec | SAMple, ACCpeak, AVErage, **[PEAkdet]** | Selects the acquisition algorithm for 0.02 ms/div to 50 ms/div. |
| | SCAn | SAMple, ACCpeak, AVErage, **[PEAdet]** | Selects the acquisition algorithm for 0.1 sec/div to 5 sec/div, when in SCAN Display mode. |
| | ROLl | SAMple, **[PEAkdet]** | Selects the acquisition algorithm for 0.1 sec/div to 5 sec/div, when in ROLL Display mode. |
| | CURRent | SAMple, AVErage, PEAkdet, ACCpeak, DEFault | Without the second argument, this command selects the default algorithm for the acquisition parameters that are currently active. With an argument, the command selects the specified algorithm. An SRQ is generated if the argument is not legal for the acquisition parameters that are active. |
| | RESet | | Sets sampling modes at all sweep speeds to their default conditions. |
| | SMOoth | ON, OFF | Applies the smoothing algorithm, when ON. |
| | WEIght | `<NR1>` | Sets the number of weighted acquisitions included in an AVErage display. The value of `<NR1>` must be either 1, 2, 4, 8, 16, 32, 64, 128, or 256. |
| | NUMsweeps | `<NR1>` | Sets the number of sweeps done before halting. 0 implies continuous mode (don't halt). |
| | TRIGCount | `<NR1>` | Sets the number of points before the trigger point in an acquisition. For 1k acquisitions, TRIGCount may range between 4 and 512 when in post-trigger, and 512 through 1020 when in pre-trigger. For 4k acquisitions, TRIGCount may range between 16 and 2048 when in post-trigger, and 2048 through 4080 when in pre-trigger. Resolution of `<NR1>` is 4. |
| | VECtors | ON, OFF | Turns Vector Mode ON or OFF. |

<!-- page 7-19 -->

**Table 7-18 (cont)**
**Acquisition Commands (cont)**

| Header | Argument | Link Argument | Description |
|---|---|---|---|
| ACQuisition? | SWPcount | `<NR1>` | Query only. Returns the number of sweeps completed, in the form: `ACQ SWP:<NR1>;`. |
| | POInts | `<NR1>` | Query only. Returns the number of data points in the acquisition, either 1024 or 4096, in the form: `ACQ POI:<NR1>;`. |
| | TRIGMode | PRE, POSt | Query only. Returns the current trigger mode in the form: `ACQ TRIGM:string;`, where string is either PRE or POSt. |
| | SAVE | ON, OFF | Query only. Returns the current state of the acquisition system in the form: `ACQ SAVE:string;`, where string is ON when the acquisition system has halted or is in the process of halting, or OFF. |
| | DISplay | ROLl, SCAn | Query only. Returns the current Acquisition Display mode in the form: `ACQ DIS:string;`, where string is either ROLl or SCAn. |

<!-- page 7-20 -->

**Table 7-19**
**Save and Recall Reference Commands**

| Header | Argument | Link Argument | Description |
|---|---|---|---|
| REFFrom | **[ACQ]**, REF1, REF2, REF3, REF4, REFA, ..., REFZ | | Selects the waveform memory source for SAVeref commands. |
| SAVeref | REF1, REF2, REF3, REF4, REFA, ..., REFZ | | Saves the waveform selected by REFFrom in the named reference. REF1, REF2, and REF3 are used for 1024 point reference waveform storage and REF4 is for 4096 point references. 4096 point references from ACQ or REF4 may be saved as 1024 point references in REF1 through REF3. The portion of the 4096 points reference saved is determined by the position of the active cursor. 4096 point references from REFA through REFZ may NOT be saved as 1024 point references in REF1 through REF3. 1024 point references are saved as either 1024 bytes, or 2048 bytes for AVEraged waveforms. |
| REFDisp | REF1, REF2, REF3, REF4 | ON, OFF, EMPty | Controls the display of the named reference. EMPty causes the contents of the reference to be deleted and its display turned OFF. REF1, REF2, and REF3, are 1024 point references, and REF4 is the 4096 point reference. |
| | REFA, ..., REFZ | EMPty | The non-volatile references may not be displayed, only EMPtied. To display the non-volatile references, first transfer them to a numbered reference. |
| REFProt | REFA, ..., REFZ | LOCked, PERM, UNLocked | Controls the write protection of non-volatile reference memories, REFA through REFZ. LOCked and PERM disable further storage into the named reference; UNLocked enables storage. PERM cannot be overwritten via front panel controls. |
| REFStat? | FILl | `<string>` | Query only. Returns a 30 character string with each reference memory's fill status indicated by a single character. `<string>` is ordered REF1 through REF4 followed by REFA through REFZ. Each string character is either 0, 1, 2, 4, or 8, which represents the waveform data in kilobytes. |
| | PROTect | `<string>` | Query only. Returns a 30 character string with each reference memory's protection status indicated by a single character. The order is identical to the FILl query. The characters which may make up the string are U, L, and P, which correspond to UNLocked, LOCked, and PERManently locked. |
| | FREe | `<NR1>` | Query only. Returns number of free kilobytes in the non-volatile reference memory. |

<!-- page 7-21 -->

**Table 7-19 (cont)**
**Save and Recall Reference Commands (cont)**

| Header | Argument | Link Argument | Description |
|---|---|---|---|
| REFOrmat | TARget | REF1, REF2, REF3, REF4 | Selects the reference to REFOrmat. |
| | CHAnnel | **[CH1]**, CH2 | Selects channel to REFOrmat. If there is no waveform for the channel (empty reference), an SRQ error is sent. If an XY waveform is selected, either channel may be selected. |
| | VGAin | `<NR3>` | Changes the vertical gain of the waveform pointed to by REFOrmat TARget. Maximum change is ±3 detents (in a 1,2,5 sequence) from the vertical gain setting of the original waveform acquisition. Cannot be used on XY waveforms. |
| | BASegain | `<NR3>` | Query only. Returns acquired vertical gain setting. |
| | VPOsition | `<NR3>` | Adjusts vertical position, relative to the original acquisition, in divisions. Valid range is ±10 divisions. Resolution is one displayed bit. |
| | HMAg | ON, OFF | When ON, increases the horizontal gain of the waveform pointed to by REFOrmat TARget. Affects both vertical channels. Cannot be used on XY waveforms. |
| | MODe | CH1, CH2, ADD, CHOP, ALT, XY | Query only. Returns originally acquired vertical mode. |

<!-- page 7-22 -->

**Table 7-20**
**Waveform Commands**

| Header | Argument | Link Argument | Description |
|---|---|---|---|
| WAVfrm? | `<string>` | | Query only. Response is a waveform from the oscilloscope, in the form: `WFMpre <ascii string>; CURVe <string>;`, which is a concatenation of the WFMpre and CURVe queries. The waveform pointed to by the DATa SOUrce and DATa CHAnnel pointers are sent in the current DATa ENCdg format. |
| CURVe | `<Wfm Data>` | | The CURVe command or query is used to send or receive waveform data from the oscilloscope. The DATa SOUrce or DATa TARget pointers show where to get or put data, respectively. The DATa ENCdg pointer shows which format, HEX, BINary, or ASCii data is sent or expected. The DATa CHAnnel pointer selects either CH1 or CH2. `<Wfm Data>` is in the form: `CURVE <Data>;` where `<Data>` is either `%<Byte Count><Binary Data><Checksum>` for BINary, `#H<Byte Count><Hexadecimal Data><Checksum>` for HEX, or `<ASCII Data>` for ASCii ENCdg. For ASCii ENCdg, each data value is separated by a comma. |
| DATa | | | Sets data parameters for data transmission and reception. |
| | SOUrce | REF1, REF2, REF3, REF4, **[ACQ]** | Selects which reference memory is source for the next WFMpre? or CURVe? query sent to the instrument. The default at power-up is ACQuisition. |
| | TARget | REF1, REF2, REF3, REF4 | Selects which reference memory receives the next WFMpre or CURVe command sent to the instrument. The default at power-up is REF1. |
| | CHAnnel | **[CH1]**, CH2 | Points to the waveform that a CURVe? or WAVfrm? query will return. If there is no waveform for the CHAnnel and SOUrce selected (empty reference), an SRQ error is sent when the waveform is requested. Power-up default is CH1. |
| | ENCdg | ASCii, **[BINary]**, HEX | Sets the data encoding/decoding format. The default at power-up is BINary. All ENCdg formats represent an unsigned integer. |

<!-- page 7-23 -->

**Table 7-21**
**Waveform Preamble Fields**

| Header | Argument | Link Argument | Description |
|---|---|---|---|
| WFMpre | WFId | "ascii str" | The WFId field includes labeling information to help you remember key features about the waveform. The information includes Vertical Mode, Coupling, Volts/Div, Time/Div, and Acquisition Mode. The scaling information is the same as in the corresponding preamble fields, but is labeled in the appropriate units. There is no command form of this argument. If received as a command, it is ignored. The fields and their possible values for the WFId section of the preamble are: Source (ACQ, REF1, ..., REF4), Chan (CH1, CH2, XY), Cplng (DC, AC, GND, Unknown), Vert (0.2MV ... 5V DIVS), Horiz (50ns ... 5s CLKS), Acq-Mode (SMPL, AVG, PKDET, PKDET-SMOOTH, ACCPK, ACCPK-SMOOTH). |
| | ENCdg | ASCii, **[BINary]**, HEX | Determines waveform encoding for waveform transmission or reception. WFMpre ENCdg and DATa ENCdg operate identically. Power-up default is BINary. All ENCdg formats represent unsigned integers. |
| | NR.Pts | `<NR1>` | Number of points in waveform. Each point can be a single Y value (T implied), an X-Y pair, or a Max-Min pair. Although digitized record length is either 1024 or 4096 points, NR.Pts may be 256, 512, 1024, 2048, or 4096, depending on number of acquired channels, acquisition mode, whether or not smoothing is enabled. NR.pts-to-RECLEN ratio table: (NumChn, AcquireMode, SMOOTH) -> ratio: (1,SMPL,N/A)->RECLEN/1; (1,AVG,N/A)->RECLEN/1; (1,PKDET,ON)->RECLEN/1; (1,ACCPK,ON)->RECLEN/1; (2,SMPL,N/A)->RECLEN/2; (2,AVG,N/A)->RECLEN/2; (2,PKDET,ON)->RECLEN/2; (2,ACCPK,ON)->RECLEN/2; (1,PKDET,OFF)->RECLEN/2; (1,ACCPK,OFF)->RECLEN/2; (2,PKDET,OFF)->RECLEN/4; (2,ACCPK,OFF)->RECLEN/4. |

<!-- page 7-24 -->

**Table 7-21 (cont)**
**Waveform Preamble Fields (cont)**

| Header | Argument | Link Argument | Description |
|---|---|---|---|
| WFMpre (cont) | PT.Off | `<NR1>` | Point offset identifies the trigger position relative to the first point of the waveform. For a 1024 point record PT.Off normally varies between 4 and 1024 in increments of 4. Normal range with 4096 point records is between 4 and 4096. NOTE: PT.Off returns a negative value if the trigger occurred before the first point of the waveform. Since a 1024 record portion of a 4096 point record can be transferred, legal values for PT.Off range from -3076 to +4096. If the value is unknown, -10000 is returned. |
| | PT.Fmt | Y, XY, ENV | Point format defines how to interpret the curve data. Y format means that X information is implicit and that data points are Y values. XY format means that data points are XY pairs, with X first. ENV: format used for envelope waveforms. The data is sent in the form: ..., y1max, y1min, y2max, y2min,... ENV is valid for PEAkdet and ACCpeak when SMOoth is OFF. |
| | XUNits | S, CLKs | If the argument is S, the XINcr value is in seconds. If it is CLKs, the scaling is unknown (EXTCLK). |
| | XINcr | `<NR3>` | Value gives the time interval between points (sampling rate). If `<NR3>` does not correspond to a legitimate time/div setting, the nearest legitimate setting is substituted and a warning SRQ is issued if EXW is ON. For a query response with an unknown time/div (i.e. EXTCLK), `<NR3>` is set to 1. |
| | YUNits | V, DIV | Indicates the units associated with YMUlt. |
| | YMUlt | `<NR3>` | This value gives the vertical "step" size of the digitizer (volts between points). If `<NR3>` does not correspond to a legitimate volts/div setting it is treated as a "variable" setting and a warning SRQ is sent. On a query response, an unknown vertical scaling (i.e. variable) sets `<NR3>` to 0.04 (25 pts/div). |
| | YOFf | `<NR1>` | YOFf is the Y coordinate of ground. If ground is unknown, -10000 is returned. |
| | XMUlt, XOFf | | XMUlt and XOFf are analogous to YMUlt and YOFf. They are used when an XY waveform is indicated. For all XY waveforms, the YUNits indicator is valid for both X and Y data. The XUNits value references sampling rate. |
| | BN.Fmt | RP | Binary format is always a right-justified, positive binary integer, also known as an unsigned binary integer. |
| | BYT/nr | `<NR1>` | Each data value is contained in 2 bytes for ACQuisition AVEerage or 1 byte otherwise. If 2 bytes are sent, the most significant byte is sent first. In HEX format, each data byte is represented by 2 ASCII encoded hex characters. |
| | BIT/nr | `<NR1>` | The data consists of 8 or 16 bits. NOTE: The least significant bits of a 16 bit waveform may not be valid, depending on the number of waveforms averaged. |
| | CRVchk | CHKsm0 | CHKsm0 indicates that the last byte of a binary curve is a checksum. It is the 2's complement of the modulo 256 sum of the binary count and curve data bytes. It does not include the "CURVE %" that precedes the binary count. |

<!-- page 7-25 -->

**Table 7-22**
**Service Request Group Commands**

| Header | Argument | Link Argument | Description |
|---|---|---|---|
| RQS | **[ON]**, OFF | | When enabled, the instrument asserts SRQ when it has an event to report. When disabled, the events are still accumulated and can be retrieved with an EVEnt? query. Default is ON with no argument and at power-up. |
| OPC | **[ON]**, OFF | | When enabled, the instrument asserts SRQ upon completion of certain commands. Commands that assert OPC service requests include REFTo, PLOt complete, and Self-test complete. Power-up default is OFF. |
| EVEnt? | `<NR1>` | | Query only. Returns: `EVE <NR1>;`, where `<NR1>` is the oldest SRQ event held by the instrument, when multiple SRQs exist. If no error is pending, 0 is returned. |

<!-- page 7-26 -->

**Table 7-23**
**Miscellaneous Commands**

| Header | Argument | Link Argument | Description |
|---|---|---|---|
| ID? | `<string>` | | Query only. Returns: `ID <string>;` where `<string>` is TEK/2230, V81.1, VERS:xx. "xx" is the firmware revision number of the instrument. |
| HELp? | `<string>` | | Query only. Returns a list of all valid command headers available in the instrument. |
| INIt | | | Command only. Causes the instrument to go to an initialized state equivalent to power-on. |
| LONg | **[ON]**, OFF | | When LONg is ON, all queries respond with the full length versions of commands. When LONg is OFF, the shortest acceptable version of commands are used in query responses. Default argument is ON. At power-up, LONg is OFF. |
| SET? | `<string>` | | Query only. Returns an ASCII string that reflects the current instrument state. The returned string can be sent to the instrument to recreate that state. In order to comply with Codes and Formats, SET? does not respond with its header. **NOTE**: This query has very limited capability because only settable values are returned in response to the SET? query. The status of LONg affects the length of the response to the SET? query. |

**Table 7-24**
**RS-232-C Specific Commands**

| Header | Argument | Link Argument | Description |
|---|---|---|---|
| REMote | **[ON]**, OFF | | REMote must be ON in order to change the state of the instrument. REMote is similar to the GPIB Remote Enable (REN) and Go To Local (GTL) messages. |
| STOp | 1, 2 | | Selects the number of stop bits. |
| FLOw | **[ON]**, OFF | | Enables and disables DC1/DC3 flow control. When FLOw is ON, BINary data transfers cannot be made. Omitting the argument turns FLOw ON. Power-on default is OFF. |
| STAtus? | `<NR1>` | | Query only. Returns the current status byte in the same manner as a GPIB Serial Poll. |

<!-- page 7-27 -->

**Table 7-25**
**Query and Response Examples**

**Vertical Query Examples**

| Query | Response |
|---|---|
| CH1? VOL | `CH1 VOL:0.5E0;` |
| CH2? | `CH2 VOL:10.0E-3,COU:AC,INV:OFF;` |
| VMO? | `VMO ADD;` |
| CH1? VOL | `CH1 VOL:5.0E-3;` |
| PROB? | `PROB CH1:10,CH2:1;` |

**Horizontal Query Examples**

| Query | Response |
|---|---|
| HOR? | `HOR MOD:ASW,ASE:2.0E-6,BSE:5.0E-9,HMA:OFF,EXT:OFF;` |
| DELA? | `DELA VAL:2.45E-3,UNI:S;` |

**Trigger Query Examples**

| Query | Response |
|---|---|
| ATR? | `ATR MOD:PPA;` |
| ATR? MOD | `ATR MOD:NOR;` |
| SGL? | `SGL DON;` |
| TRI? | `TRI ON;` |

**Cursor Query Examples**

| Query | Response |
|---|---|
| CURS? TAR | `CURS TAR:REF2;` |
| CURS? | `CURS SEL:CURS1,TAR:REF2,CHA:CH1,POS:765;` |
| DELTAT? | `DELTAT VAL:11.5E-6,UNI:S;` |

**Acquisition Query Examples**

| Query | Response |
|---|---|
| ACQ? HSR | `ACQ HSR:AVE;` |
| ACQ? | `ACQ REP:AVE,HSR:SAM,LSR:PEA,SCA:SAM,ROL:PEA,SMO:ON,WEI:8,SWP:6,NUM:0,POI:4096,TRIGM:PRE,TRIGC:320,SAVE:OFF,DIS:SCA,VEC:ON;` |

**Save and Recall Reference Query Examples**

| Query | Response |
|---|---|
| REFO? VGA | `REFO VGA:10.0E-3;` |
| REFO? | `REFO TAR:REF1,CHA:CH2,VGA:10.0E-3,VPO:0.0,HMA:ON,MODE:CHOP;` |

<!-- page 7-28 -->

**Table 7-25 (cont)**
**Query and Response Examples**

**Waveform Query Examples**

| Query | Response |
|---|---|
| WFM? WFI | `WFM WFI:"REF1,CH1,10.0MV,DC,50.0MS,SAMPLE-SMOOTH,CRV# 4";` |
| WFM? PT.F | `WFM PT.F:ENV;` |
| WFM? ENC | `WFM ENC:ASC;` |

### WAVEFORM TRANSFERS

The instrument can transmit and receive waveforms. It can transfer these waveforms, in binary, hexadecimal, or ASCii format. When sending waveforms to the instrument, the target is a reference memory. Waveforms transferred from the oscilloscope to the controller are selected from the same reference memories or the current acquisition. The data source and data target are selected independently.

#### Waveform Preamble

The waveform preamble indicates the waveform attributes, such as number of points per waveform, scale factors, offset, horizontal increment, scaling units, and data encoding. The preamble information is sent as an ASCII string. The length of the string depends on the characteristics of the waveform.

A typical response to the preamble query WFMpre? for a Y (time implied) acquisition is:

```
Query      Response
WFMpre?    WFM WFI:"ACQ, CH1, 0.5V, DC, 0.2MS,
           SAMPLE - SMOOTH, CRV# 2",
           NR.P:4096, PT.O:122, PT.F:Y,
           XMU:0.0E0, XOF:0, XUN:S, XIN:2.0E-6,
           YMU:20.0E-3, YOF:-20, YUN:V,
           ENC:HEX, BN.F:RP, BYT:1, BIT:8,
           CRV:CHK;
```

A typical response to the preamble query WFMpre? for an XY acquisition is:

```
Query      Response
WFMpre?    WFM WFI:"ACQ, XY, 0.2V, DC, 50.0MV,
           DC, 1.0US, SAMPLE, CRV# 19",
           NR.P:2048, PT.O:216, PT.F:XY,
           XMU:8.0E-3, XOF:0, XUN:S, XIN:20.0E-9,
           YMU:2.0E-3, YOF:0, YUN:V, ENC:BIN,
           BN.F:RP, BIT:8, BYT:1, CRV:CHK;
```

In these examples, the instrument response is shown on multiple lines. WFMpre? responses, as well as all other query responses, are sent as a single line of data ending with a carriage return line feed. With the GPIB interface, EOI is also sent if that message terminator mode is selected.

#### Transferring Waveforms

The oscilloscope can respond with either the Preamble only, Curve only, or both Preamble and Curve together:

| Query | Response |
|---|---|
| CURVe? | Curve Data Only |
| WFMpre? | Preamble Only |
| WAVfrm? | Preamble and Curve data |

When responding to the WAVfrm? query, the preamble is separated from the curve data with a ";".

The instrument digitizes data internally as an 8-bit, unsigned integer. Before data is sent over the GPIB or RS-232-C Option, it is changed into one of three formats,

<!-- page 7-29 -->

BINary, HEXADECIMAL, or ASCii. The resolution of data points sent over the bus may be either 8 or 16 bits. Waveform record length is 1024 or 4096, but the number of data points per record depends on several variables. See the description of NR.Pts in the Command Tables for more information.

#### Binary Encoding

BINary data is transferred as an unsigned binary integer. Each record is 8 bits, or 16 bits when averaged.

In BINary format, the waveform curve data is in the form of: `CURVE <space> % <Binary Count MSB> <Binary Count LSB> <Data> <Checksum> <Terminator>`

Where:

- `%` is used as a header character to show the start of a binary block.
- `<Binary Count MSB>` is the most significant byte of the two-byte Binary Count. Binary Count is the length of the waveform, in bytes, plus the one byte Checksum.
- `<Binary Count LSB>` is the least significant byte of the Binary Count.
- `<Data>` is made up of 256, 512, 1024, 2048 or 4096 data points. Each data point is either a 1 byte (8-bit) or 2 byte (16-bit) representation of each digitized value.
- `<Checksum>` is the two's-complement of the modulo 256 sum of the preceding data bytes and the binary count. The Checksum is used by the controller to verify that all data values have been received correctly.

Table 7-26 shows an example of data sent over the interface during a 4096 point, 8-bit BINary waveform transfer. The actual waveform point (Pt.) values will vary depending upon the signal acquired.

Table 7-27 shows an example of data sent over the interface during a 4096 point, 16-bit BINary waveform transfer.

**Table 7-26**
**Typical 8-Bit Binary Waveform Data**

| Byte | Contents | Decimal | GPIB EOI (1=Asserted) |
|---|---|---|---|
| 1 | C | 67 | 0 |
| 2 | U | 85 | 0 |
| 3 | R | 82 | 0 |
| 4 | V | 86 | 0 |
| 5 | E | 69 | 0 |
| 6 | `<SP>` | 32 | 0 |
| 7 | % | 37 | 0 |
| 8 | `<Bin Count MSB>` | 16ᵃ | 0 |
| 9 | `<Bin Count LSB>` | 01ᵃ | 0 |
| 10 | 1st Pt | d₁ | 0 |
| 11 | 2nd Pt | d₂ | 0 |
| ... | ... | ... | 0 |
| 4105 | 4096th Pt | d₄₀₉₆ | 0 |
| 4106 | `<Checksum>` | chk | 1 When TERM=EOI |
| 4107ᵇ | `<CR>` | 13 | 0 |
| 4108ᶜ | `<LF>` | 10 | 1 |

ᵃ(1001₁₆ or 4097₁₀)
ᵇAll RS-232-C or GPIB with TERM = LF/EOI.
ᶜRS-232-C with TERM = CR-LF.

#### Hexadecimal Encoding

In HEXadecimal waveform encoding, characters representing an 8-bit or 16-bit data point are sent in a fixed ASCII hexadecimal format. There are no delimiters between data points. Data format is very similar to BINary format, with the following exceptions:

1. The curve header is "CURVE #H" instead of "CURVE %".

2. Each data point is 2 ASCII hexadecimal characters for 8-bit and 4 ASCII hexadecimal characters for 16-bit transfers.

<!-- page 7-30 -->

3. The byte count is sent as four successive ASCII hexadecimal characters, but the value of the byte count is identical to a comparable BINary transfer.

4. The checksum is sent as two successive ASCII hexadecimal characters.

Table 7-28 and Table 7-29 show 8-bit and 16-bit HEXadecimal waveform CURVe structures.

**Table 7-27**
**Typical 16-Bit Binary Waveform Data**

| Byte | Contents | Decimal | GPIB EOI (1=Asserted) |
|---|---|---|---|
| 1 | C | 67 | 0 |
| 2 | U | 85 | 0 |
| 3 | R | 82 | 0 |
| 4 | V | 86 | 0 |
| 5 | E | 69 | 0 |
| 6 | `<SP>` | 32 | 0 |
| 7 | % | 37 | 0 |
| 8 | `<Bin Count MSB>` | 32ᵃ | 0 |
| 9 | `<Bin Count LSB>` | 01ᵃ | 0 |
| 10 | 1st Pt MSB | d₁H | 0 |
| 11 | 1st Pt LSB | d₁L | 0 |
| 12 | 2nd Pt MSB | d₂H | 0 |
| 13 | 2nd Pt LSB | d₂L | 0 |
| ... | ... | ... | 0 |
| 8200 | 4096th Pt MSB | d₄₀₉₆H | 0 |
| 8201 | 4096th Pt LSB | d₄₀₉₆L | 0 |
| 8202 | `<Checksum>` | chk | 1 When TERM=EOI |
| 8203ᵇ | `<CR>` | 13 | 0 |
| 8204ᶜ | `<LF>` | 10 | 1 |

ᵃ(1001₁₆ or 4097₁₀)
ᵇAll RS-232-C or GPIB with TERM = LF/EOI.
ᶜRS-232-C with TERM = CR-LF.

**Table 7-28**
**Typical 8-Bit Hexadecimal Waveform Data**

| Byte | Contents | Decimal | GPIB EOI (1=Asserted) |
|---|---|---|---|
| 1 | C | 67 | 0 |
| 2 | U | 85 | 0 |
| 3 | R | 82 | 0 |
| 4 | V | 86 | 0 |
| 5 | E | 69 | 0 |
| 6 | `<SP>` | 32 | 0 |
| 7 | # | 35 | 0 |
| 8 | H | 72 | 0 |
| 9 | `<Bin Count MS 4 bits>` | 49 | 0 |
| 10 | . | 48 | 0 |
| 11 | . | 48 | 0 |
| 12 | `<Bin Count LS 4 bits>` | 49 | 0 |
| 13 | 1st Pt MS 4 bits | d₁H | 0 |
| 14 | 1st Pt LS 4 bits | d₁L | 0 |
| 15 | 2nd Pt MS 4 bits | d₂H | 0 |
| 16 | 2nd Pt LS 4 bits | d₂L | 0 |
| ... | ... | ... | 0 |
| 203 | 4096th Pt MS 4 bits | d₄₀₉₆H | 0 |
| 204 | 4096th Pt LS 4 bits | d₄₀₉₆L | 0 |
| 205 | `<Checksum MS 4 bits>` | chkH | 0 |
| 206 | `<Checksum LS 4 bits>` | chkL | 1 When TERM=EOI |
| 207ᵃ | `<CR>` | 13 (if term=LF/EOI) | 0 |
| 208ᵇ | `<LF>` | 10 (if term=CR-LF) | 1 |

ᵃAll RS-232-C or GPIB with TERM = LF/EOI.
ᵇRS-232-C with TERM = CR-LF.

#### ASCII Encoding

In ASCii encoding, ASCII characters representing the binary value of each data point are sent in variable length format, separated by commas.

<!-- page 7-31 -->

**Table 7-29**
**Typical 16-Bit Hexadecimal Waveform Data**

| Byte | Contents | Decimal | GPIB EOI (1=Asserted) |
|---|---|---|---|
| 1 | C | 67 | 0 |
| 2 | U | 85 | 0 |
| 3 | R | 82 | 0 |
| 4 | V | 86 | 0 |
| 5 | E | 69 | 0 |
| 6 | `<SP>` | 32 | 0 |
| 7 | # | 35 | 0 |
| 8 | H | 72 | 0 |
| 9 | `<Bin Count MS 4 bits>` | 50 | 0 |
| 10 | . | 48 | 0 |
| 11 | . | 48 | 0 |
| 12 | `<Bin Count LS 4 bits>` | 49 | 0 |
| 13-16 | 1st Pt MS/LS 4 bits (×4) | d₁H..d₁L | 0 |
| 17-20 | 2nd Pt MS/LS 4 bits (×4) | d₂H..d₂L | 0 |
| ... | ... | ... | 0 |
| 6393-6396 | 4096th Pt MS/LS 4 bits (×4) | d₄₀₉₆H..d₄₀₉₆L | 0 |
| 6397 | `<Checksum MS 4 bits>` | chkH | 0 |
| 6398 | `<Checksum LS 4 bits>` | chkL | 1 When TERM=EOI |
| 6399ᵃ | `<CR>` | 13 (if term=LF/EOI) | 0 |
| 6400ᵇ | `<LF>` | 10 (if term=LF/EOI) | 1 |

ᵃAll RS-232-C or GPIB with TERM = LF/EOI.
ᵇRS-232-C with TERM = CR-LF.

In ASCII format, the curve data transfer is represented as:

```
CURVE<space>data,data,data,...,data<terminator>
```

Table 7-30 shows an example of an 8-bit ASCii waveform CURVe transfer. Transmission length depends on specific data values, record length, acquisition mode and smoothing, and whether the acquisition was 1 or 2 channels.

### REMOTE-LOCAL OPERATING STATES

The following paragraphs describe the two operating states of the instrument: Local and Remote.

**Table 7-30**
**Typical ASCII Waveform Data**

| Byte | Contents | Decimal | GPIB EOI (1=Asserted) |
|---|---|---|---|
| 1 | C | 67 | 0 |
| 2 | U | 85 | 0 |
| 3 | R | 82 | 0 |
| 4 | V | 86 | 0 |
| 5 | E | 69 | 0 |
| 6 | `<SP>` | 32 | 0 |
| 7 | Pt100₁* | d100₁ | 0 |
| 8 | Pt10₁* | d10₁ | 0 |
| 9 | Pt1₁* | d1₁ | 0 |
| 10 | , | 44 | 0 |
| ... | ... | ... | 0 |
| XXX | Pt100₄₀₉₆* | d100₄₀₉₆ | 0 |
| XXX | Pt10₄₀₉₆ | d10₄₀₉₆ | 0 |
| XXX | Pt1₄₀₉₆* | d1₄₀₉₆ | 0 |
| XXXᵃ | `<CR>` | 13 | 0 |
| XXXᵇ | `<LF>` | 10 | 1 |

\* Pt100 and Pt10 values are NOT sent when 0, so each Pt may be 1, 2, or 3 digits.
ᵃAll RS-232-C or GPIB with TERM = LF/EOI.
ᵇRS-232-C with TERM = CR-LF.

<!-- page 7-32 -->

#### Local State (LOCS)

In LOCS, instrument parameters are both set and changed manually by operator manipulation of the front- and side-panel controls. Only option interface messages can be received and executed. Device-dependent commands (without REN asserted) will cause SRQ errors since their functions are under front-panel control while in LOCS.

#### Remote State (REMS)

In this state, the oscilloscope executes all commands addressed to it over the communication options bus. Front-panel indicators and crt readouts are updated as applicable when commands are executed. Manually changing any option-controllable front-panel control causes the instrument to return to the Local State. If a waveform is being transmitted over the bus, the PLOT indicator is lit and acquisitions are prevented until the transmission is complete.

### INSTRUMENT RESPONSE TO INTERFACE MESSAGES

The following explains effects on the oscilloscope of standard interface messages received from a remote controller. Message abbreviations used are from ANSI/IEEE Std 488-1978.

#### Local Lockout (LLO)

Local Lockout is not supported by the instrument. In response to a LLO message via the GPIB, the option generates an SRQ error.

**NOTE**

*The RS-232-C Option uses Option Interface Commands to implement the following GPIB (hardware) messages.*

#### Remote Enable (REN)

When Remote Enable is asserted and the instrument receives its listen address, the oscilloscope is placed in the Remote State (REMS). When in the Remote State, the oscilloscope's Addressed (ADDR) indicator is lit.

Disasserting REN causes a transition to LOCS; the instrument remains in LOCS as long as REN is false. The transition may occur after processing of a different message has begun. In this case, execution of the message being processed is not interrupted by the transition.

#### Go To Local (GTL)

Instruments that are already listen-addressed respond to GTL by assuming a local state. Remote-to-local transitions caused by GTL do not affect the execution of any message being processed when GTL was received.

#### My Listen and My Talk Addresses (MLA and MTA)

The primary Talk/Listen address is established as previously explained in this section.

#### Unlisten (UNL) and Untalk (UNT)

When the UNL message is received, the oscilloscope's listen function is placed in an idle (unaddressed state). In the idle state, the instrument will not accept commands over the bus.

The talk function is placed in an idle state when the oscilloscope receives the UNT message. In this state, the instrument cannot transmit data via the interface bus.

#### Interface Clear (IFC)

When IFC is asserted, both the Talk and Listen functions are placed in an idle state and the crt ADDR indicator is turned off. This produces the same effect as receiving both the UNL and the UNT messages.

#### Device Clear (DCL)

The DCL message reinitializes communication between the instrument and the controller. In response to DCL, the instrument clears any input and output messages as well as any unexecuted control settings. Also cleared are any errors and events waiting to be reported (except the power-on event). If the SRQ line is asserted for any reason (other than power-on), it becomes unasserted when the DCL message is received.

#### Selected Device Clear (SDC)

This message performs the same function as DCL; however, only instruments that have been listen-addressed respond to SDC.

#### Serial Poll Enable and Disable (SPE and SPD)

The Serial Poll Enable (SPE) message causes the instrument to transmit its serial-poll status byte when it is talk-addressed. The Serial Poll Disable (SPD) message switches the instrument back to its normal operation.

<!-- page 7-33 -->

### GPIB PROGRAMMING

Programming considerations are provided in this part to assist in developing programs for interfacing to the oscilloscope via the GPIB. For additional information see the "Instrument Interfacing Guide". Before a program can be used for controlling the oscilloscope, the GPIB parameters (primary address, message terminator, and talk/listen mode) must be set. These parameters are selected and set at the oscilloscope using the GPIB PARAMETERS switch.

Programs are usually composed of two main parts (or routines), which can be generally categorized as a command handler and a service-request handler.

#### Command Handler

Basically, a command handler should establish communication between the controller and oscilloscope, send commands and queries to the oscilloscope, receive responses from the oscilloscope, and display responses as required. The following outline indicates the general sequence of functions that the command-handling routine should perform to accommodate communications between the controller and oscilloscope over the GPIB.

1. Initialize the controller.
2. Disable the service-request handler until the program is ready to handle them.
3. Get the GPIB address of the oscilloscope.
4. Enable the service-request handler.
5. Get the command to send to the oscilloscope.
6. Send the command to the oscilloscope.
7. Check for a response from the oscilloscope.
8. If there is a response, perform the desired function.
9. You are ready for a new command. Repeat the functions in statements 5 through 9 as many times as desired.

#### Service-Request Handler

The typical service-request handler routine contains the necessary instructions to permit proper processing of interrupts. For example, whenever power-on occurs, the oscilloscope asserts an SRQ interrupt. If a GPIB program is operating on the controller when a power-on SRQ is received, the program should be able to determine that the oscilloscope's power was interrupted at some time during program operation. This event could cause improper program execution, unless the program was written to adequately handle the possibility of a power-on SRQ occurring.

Other interrupts (or events) for which the oscilloscope asserts SRQ are identified in Table 7-32.

While some controllers have the capability of ignoring service requests, others require that all SRQs be managed. The programmer should understand the controller being used. If service requests are to be handled in the program, the interrupts must first be enabled.

A service-request handler routine can be developed to service interrupts when they occur during program operation. It basically should consist of an interrupt-enabling statement (ON SRQ) near the beginning of the program and a serial-poll subroutine somewhere in the program. The ON SRQ statement directs program control to the serial-poll subroutine whenever an SRQ interrupt occurs. For each interrupt received by the controller, the program should perform a serial-poll subroutine.

The following general steps are required to handle service requests from the oscilloscope:

1. Perform a serial poll.
2. Send an EVENT? query to the oscilloscope requesting service.
3. If the EVENT? query response is not zero, then perform the desired response to the event.
4. Return to the main program.

### RS-232-C PROGRAMMING

Programming considerations are provided in this part to assist in developing programs for interfacing to the oscilloscope via the RS-232-C. For additional information see the

<!-- page 7-34 -->

"Instrument Interfacing Guide". Before a program can be used for controlling the oscilloscope, the RS-232-C parameters (baud rate, line terminator, and parity) must be set. These parameters are selected and set at the oscilloscope using the RS-232-C PARAMETERS switch.

Programs are usually composed of two main parts (or routines), which can be generally categorized as a command handler and a service-request handler.

#### Command Handler

Basically, a command handler should establish communication between the controller and oscilloscope, send commands and queries to the oscilloscope, receive responses from the oscilloscope, and display responses as required. The following outline indicates the general sequence of functions that the command-handling routine should perform to accommodate communications between the controller and oscilloscope.

1. Initialize the controller.
2. Check for a service request from the oscilloscope (by sending an EVEnt query); if not zero, service the request.
3. Get the command to send to the oscilloscope.
4. Send the command to the oscilloscope.
5. Check for a response from the oscilloscope.
6. If there is a response, perform the desired function. If there is also an error response, perform step 2.
7. You are ready for a new command. Repeat the functions in statements 2 through 7 as many time as desired.

#### Service-Request Handler

The typical service-request handler routine contains the necessary instructions to permit proper processing of service requests. For example, whenever power-on occurs, the oscilloscope sends an SRQ. If a GPIB program is operating on the controller when a power-on SRQ is generated, the program should be able to determine that the oscilloscope's power was interrupted at some time during program operation. This event could cause improper program execution, unless the program was written to adequately handle the possibility of a power-on SRQ occurring. Other events for which the oscilloscope generates SRQ are identified in Table 7-32.

The following general steps are required to handle service requests from the oscilloscope:

1. Send an EVENT? query to the oscilloscope requesting service.
2. If the EVENT? query response is not zero, then perform the desired response to the event.
3. Return to the main program.

### RESET UNDER COMMUNICATION OPTIONS CONTROL

The oscilloscope may be set to its power-up state by sending the INIt command via the communication option. This command always initiates the power-up self tests. On completion of power-up tests, SRQ code 65 (operation complete) is generated, and the oscilloscope enters the normal operating state. If there is a self-test error, the option also generates SRQ code 65 and does not shift the instrument to the normal operating state. Invoking the INIt command can simplify a program. When using INIt, fewer commands will usually be needed to set the instrument state, since all front-panel settings may not need to be individually specified.

### STATUS AND ERROR REPORTING

The status and error reporting system used by the Communication Options interrupts the bus controller. On the GPIB Option, the bus controller is interrupted by asserting the Service Request (SRQ) line on the bus. This SRQ provides the means of indicating that an event (either a change in status or an error) has occurred. To service a request, the GPIB controller performs a Serial Poll; in response, the instrument returns a Status Byte (STB), which indicates the type of event that occurred. On the RS-232-C Option, as soon as a change of status or an error occurs, the instrument returns a Status Byte (STB), which indicates the type of event that occurred. Bit 4 of the Status Byte is used to indicate that the command processor is active. This bit is set when the command processor is executing a command, and reset when it is not. The Status Byte, therefore, provides a limited amount of information about the specific cause of the SRQ. The various status events and errors that can occur are divided into several categories as defined in Table 7-31.

Each time the GPIB controller performs a serial poll, it can cause a second SRQ if more than one error exists. The most serious error at the time of the serial poll is the error reported. An EVEnt? query returns a number indicating the specific type of error that occurred. Table 7-32 lists the EVEnt? codes generated by the communication options.

<!-- NOTE: printed page 7-35 (which should contain Table 7-31, the
     Status Byte bit-field format defining the event categories
     referenced above) does not appear in the rendered page sequence -
     page_34.png is confirmed printed page 7-34 and page_35.png is
     confirmed printed page 7-36, with no intervening page rendered.
     Likely a scanning/rendering gap in the source PDF rather than a
     missing page in the real manual; flagged rather than guessed at. -->

<!-- page 7-36 -->

**Table 7-32**
**Event Codes**

| EVEnt? Code | Instrument Status |
|---|---|
| 000 | No status to report |

**Command Errors**

| EVEnt? Code | Instrument Status |
|---|---|
| 101 | Command header error. |
| 102 | Header delimiter error. |
| 103 | Command argument error. |
| 104 | Argument delimiter error. |
| 105 | Non-numeric argument, numeric expected. |
| 106 | Missing argument. |
| 107 | Invalid message-unit delimiter. |
| 108 | Checksum error. |
| 109 | Byte-count error. |
| 151 | The argument is too large. |
| 152 | Illegal hex character. |
| 153 | Non-binary argument; binary or hex expected. |
| 154 | Invalid numeric input. |
| 155 | Unrecognized argument type. |

**Execution Errors**

| EVEnt? Code | Instrument Status |
|---|---|
| 201 | Command cannot be executed when in LOCAL. |
| 203 | I/O buffers full, output dumped. |
| 205 | Argument out of range, command ignored. |
| 206 | Group execute trigger ignored. |
| 251 | Illegal command. |
| 252 | Integer overflow. |
| 253 | Input buffer overflow. |
| 254 | Invalid waveform preamble. |
| 255 | Invalid instrument state. |
| 256 | GPIB (Option 10) Command not allowed. |
| 258 | Command not allowed on a 2220. |
| 259 | Command not allowed on a 2230. |
| 260 | Cannot execute command with RQS OFF. |
| 261 | Reference memory busy with local (front-panel) command. |
| 262 | Reference memory non-existent or specified as different size than selected waveform. |
| 263 | Plot active; only PLOT ABORT allowed while plotting. |

**Internal Errors**

| EVEnt? Code | Instrument Status |
|---|---|
| 351 | Firmware failure. Contact your nearest Tektronix Service Center for assistance. |

<!-- page 7-37 -->

**Table 7-32 (cont)**

**System Events**

| EVEnt? Code | Instrument Status |
|---|---|
| 401 | Power on. |
| 451 | Parity error. |
| 452 | Framing error. |
| 453 | Carrier lost. |
| 454 | End of acquisition OPC. |
| 455 | End of plot OPC. |
| 456 | Diagnostics test complete OPC. |

**Execution Warnings**

| EVEnt? Code | Instrument Status |
|---|---|
| 551 | Single sweep is already armed. |
| 552 | No ground-dot measurement available. |
| 553 | Invalid probe code or identify. |
| 554 | Query not valid for current instrument state. |
| 555 | Requested setting is out of detent (uncalibrated). |
| 556 | MESsage display buffer is full. |
| 557 | Waveform preamble incorrect, has been corrected. |
| 558 | Waveform transfer ended abnormally. |

<!-- page 7-38 -->

**Table 7-33**
**Readout/MESage Command Character Set**

A 7-bit ASCII character table (bits B1-B7, 16 rows x 8 columns) mapping bit patterns `0000`-`1111` (B4 B3 B2 B1) crossed with column group `B7 B6 B5` (000=CONTROL, 001=CONTROL, 010=SYMBOLS, 011=SYMBOLS, 100=UPPERCASE, 101=UPPERCASE, 110=LOWERCASE, 111=LOWERCASE) to character/code pairs. Notable entries: control codes 0-31 include special glyphs (2=`BWL` mode?, 3=a boxed-X symbol, 4=Δ, 5=dash, 6=≡, 7=ground symbol, 14=Hz, 15=¼Δ, 16=µ, 17=~V, 18=an up-arrow-like glyph, 19=a raised dot). Symbols 32-63 are standard ASCII space/punctuation/digits (SP,!,",#,$,%,&,...,/,0-9,:,;,<,=,>,?). Uppercase 64-95 and lowercase 96-127 are standard ASCII letters (with `@`/backtick rendered distinctively). This is the character set used by the READout and MESsage commands for on-screen text.

<!-- page 7-39 -->

**Table 7-34**
**ASCII Code Chart**

A standard 7-bit ASCII/GPIB code chart (bits B7 B6 B5 across columns 000-111, bits B4 B3 B2 B1 down rows 0000-1111), with each cell showing: GPIB mnemonic (top), octal code (top-left corner), ASCII character/mnemonic (large, center), and decimal code (bottom-right corner). Columns are grouped: CONTROL (000, 001 - includes GPIB addressed commands GTL/LLO/DCL/SDC/PPC/PPU and universal commands DCL/DC1-DC4/NAK/SYN/ETB/CAN/EM/SUB/ESC/FS/GS/RS/US alongside standard ASCII control codes NUL/SOH/STX/ETX/EOT/ENQ/ACK/BEL/BS/HT/LF/VT/FF/CR/SO/SI), NUMBERS/SYMBOLS (010, 011 - SP through `?`, includes GPIB listen addresses 40-137 octal region), UPPER CASE (100, 101 - `@` through `_`, doubling as GPIB talk addresses), LOWER CASE (110, 111 - `` ` `` through DEL/RUBOUT, doubling as GPIB secondary addresses/commands PPE/PPD). Row/column headers along the bottom identify the GPIB address-class bands: ADDRESSED COMMANDS, UNIVERSAL COMMANDS, LISTEN ADDRESSES, TALK ADDRESSES, SECONDARY ADDRESSES OR COMMANDS (PPE)/(PPD). A KEY legend shows the cell layout: octal 25/GPIB code PPU, hex 15/decimal 21, ASCII character NAK. Drawing number 4999-103.

### OPTION 10 THEORY OF OPERATION

The General Purpose Interface Bus (GPIB) option (see Diagram 24) provides a general purpose interface for the exchange of waveform data and instrument-state information. It retains the XY Plotter function of the base instrument, and provides a means of adding non-volatile waveform memory.

The XY Plotter circuitry is unchanged from the standard instrument. The circuit descriptions covering the standard XY Plotter still apply, and are not repeated here. The following discussion refers only to the GPIB portion of the board.

The board contains 64K bytes of ROM, 2K bytes of RAM, and an interface to the GPIB port. Supporting the GPIB port are two 8-bit input ports for status signals and parameter switches, and a 1-bit output port used for diagnostics. The remainder of the circuitry provides signal buffering and address decoding.

The microprocessor bus extends to this option through W8101. The address bus, the data bus, the bus control signals, and several address decode lines which are generated on the storage board are included. Power supplies are also brought in through this connector, and J9301 in the XY Plotter portion of the board is not used.

#### Bus Buffers

The address lines are buffered by U1341 and U1333. The buffers are always enabled. Bidirectional data bus buffer U1331 isolates the circuitry from the storage board and provides improved signal drive capability. Also buffered are the /RD, /WR, 6.7MHZCLK, and RESET signals.

#### Address Decoding

The GPIB occupies all of the addresses in the COM-SEG range (80000H to BFFFFH). Its I/O occupies several addresses in the I/O-SEG range (40000H to 7FFFFH). Table 3-1 lists the actual addresses used.

Primary address decoding is accomplished by U1345. It provides a one-of-eight, active-LO signal when BA12, BA13, /IO SEG, and /BLK0, are all LO. Three address lines, BA3, BA6, and BA7, are decoded to produce the eight strobes. Four of the strobes enable the GPIB controller U1351, Parameter buffer U1322, Status buffer U1323, and Diagnostic latch U1335. Also generated by U1345 is a signal that is LO whenever one of the strobes is enabled and BA8 is LO. This signal is gated with /COM SEG and /DEN in U1332 to produce an enable for data buffer U1331 via U1344C.

Half of U1332 generates the ROM enable signal. The ROM is enabled whenever /COM SEG is LO and either /BLK0 or /BLK1 is LO. This enable drives the output enable (pin 22) of U1343 and not its chip enable (pin 20) which is driven by /A18.

The other half of U1332 generates the /DATEN enable for the data bus buffer. When /DEN is LO and either /IO 2OPT or /COM SEG is LO, pin 8 of U1332 goes HI. U1344 inverts this signal, producing /DATEN. The data bus buffer is enabled only for references in COM SEG or to I/O ports used by this option.

The RAM enable signal /RAMEN (U1334A pin 3) is produced by U1334A and U1334C. RAM enable /RAMEN is LO only if RAM DIS, /BLK3, and /COM SEG are all LO. RAM DIS disables U1342 if the non-volatile RAM is present.

#### RAM and ROM

Temporary storage for the option is provided by RAM U1342. Option operating system firmware is contained in ROM U1343.

#### GPIB Controller

The GPIB controller, U1351, is a TMS9914A which handles much of the protocol required to interface to the IEEE STANDARD 488 bus. The controller has eight internal registers decoded by RS0, RS1, and RS2. Under certain conditions it generates an interrupt to the microprocessor which appears as a LO /INT (U1351 pin 9). This pin is an open drain output connected to the microprocessor's maskable interrupt.

Data bus lines are reversed, BD0 for BD7, to accommodate the internal convention of the GPIB controller.

Trigger signal TR, U1351 pin 39, is used only for diagnostics and is read by the microprocessor via U1322 pin 2.

#### Line Drivers

Bus buffers U1324 and U1325 provide the drive characteristics required by IEEE 488 bus standards. They also control characteristics of the drive circuitry during bus operation.

<!-- page 7-40 -->

All of the signal lines that are at GPIB levels are protected by diode arrays CR1321, CR1322, and zener diode VR1321. These networks clip voltage transients greater than +6.8 volts or less than -0.6 volts.

Connector J1314 is a standard GPIB interface connector.

#### Clock Divider and Diagnostic Latch

U1335 is a dual J-K flip-flop that performs two independent functions. U1335A divides the 6.7 MHz clock by two for GPIB controller U1351. U1335B provides a one-bit latch for diagnostic use. When its enable (clock), U1335B pin 12, is strobed LO, the data on BD0 is latched.

#### Parameter Buffer

Parameter buffer U1322 provides an eight-bit input port for selecting parameters associated with the GPIB option such as address and terminator. It consists of U1322, S1321, and part of resistor pack R1322. The switch is sensed by enabling buffer U1322 which gates its inputs onto the data bus. Bit 7 is used to sense TR, U1351 pin 39, for diagnostic use.

#### Status Buffer

Status buffer U1323 is used to sense three of the GPIB PARAMETER switch positions as well as miscellaneous other signals. Buffer circuitry consists of U1323, S1321, R1321, and part of resistor pack R1322. Status buffer functions are shown in Table 7-35.

**Table 7-35**
**GPIB Status Buffer Functions**

| BIT | Signal Name | Function |
|---|---|---|
| Bit 0 | /PWR INT | Power going down interrupt |
| Bit 1 | +5Vₚ | Logic HI |
| Bit 2 | TRIG | GPIB chip diagnostic |
| Bit 3 | | PARAMETER SWITCH position 8 |
| Bit 4 | | PARAMETER SWITCH position 10 |
| Bit 5 | | PARAMETER SWITCH position 9 |
| Bit 6 | +5Vₚ | Logic HI |
| Bit 7 | DIAG | Diagnostic latch |

### OPTION 12 THEORY OF OPERATION

The RS-232-C communication option (see Diagram 23) provides a general-purpose interface for the exchange of waveforms and instrument-state information. It replaces the XY Plotter board of the standard instrument but includes the XY Plotter circuitry. The following discussion refers only to the RS-232-C portion of the board.

The option includes 64K bytes of ROM, 2K bytes of RAM, and an RS-232-C interface. Supporting the RS232 port are two 8-bit input ports for status signals and parameter switches, and a 4-bit output port used mainly for interrupt masking. The remaining circuitry either decodes addresses or buffers signals.

Microprocessor bus signals are extended to this board through W8101. The address bus, data bus, bus control signals, several address decode lines, and power supplies all pass through this connector.

#### Bus Buffers

The address lines are buffered by U1241 and U1233. These buffers are always enabled. Data bus buffer U1231 is bidirectional. It isolates the option from the storage board and improves signal driving capabilities. Also buffered are the /RD (U1233), /WR (U1234D), and RESET (U1244E) signals.

#### Address Decoding

All addresses in the COM-SEG range (80000H to BFFFFH) are used by the option. Several addresses in the I/O-SEG range (40000H to 7FFFFH) are used by option I/O circuitry. Table 3-1 lists the actual addresses used.

Primary address decoding is accomplished by U1245. It provides a one-of-eight, low-asserting signal when BA12, BA13, /IO SEG, and /BLK0, are all LO. Address lines BA3, BA6, and BA7 are decoded to produce eight strobes. Three of the strobes are used to enable UART U1251, parameter buffer U1222, and Status buffer U1223. A fourth strobe is gated with /BWR at U1234A to produce a write strobe for the interrupt mask latch (U1236). Also generated by U1245 is a signal that is LO whenever one of the strobes is enabled and BA8 is LO. This signal is gated with /COM SEG and /DEN in U1232A to produce an enable for the data bus buffer (U1231).

The ROM and RAM enable signals are generated by U1235. One half of U1235 is enabled by /COM SEG. It decodes /BLK0 and /BLK1 into four strobes, two are wire-ANDed together to produce the /ROMEN enable for the

<!-- page 7-41 -->

ROM chip (U1243). The resultant function is to enable the ROM whenever /COM SEG is LO and either /BLK0 or /BLK1 is LO. This enable drives the output enable pin of U1243 and not its chip enable pin which is driven by /A18.

The other half of U1235 provides a similar function for U1242, the RAM chip. It generates a LO-going strobe when /COM SEG, RAM DIS, and /BLK3 are LO and /BLK0 and /BLK1 are HI. RAM DIS disables U1242 if the non-volatile RAM is present. Although the RAM has images throughout the 88000 to 8FFFF address range, only the highest image is used.

Half of U1232 and inverter U1244C generate the /DATEN signal for the bidirectional data bus buffer U1231. /DATEN is LO for any reference in COM-SEG and for references to the option I/O ports. It is LO when /DEN, the data enable from the processor, is LO and either /COM SEG or /IO 2OPT (U1245 pin 3) is LO.

#### RAM and ROM

Temporary storage for the option is provided by RAM U1242. Option operating system firmware is contained in ROM U1243.

#### UART

The UART U1251 communicates with the Microprocessor, providing serial-to-parallel conversion and handling some of the RS232 protocol. Also included is an internal baud rate generator. Crystal Y1251 provides a time base which is divided by software selectable ratios to provide the required bit transfer speeds. Three interrupt lines, INTR, TBRE, and DR, inform the Microprocessor that intervention is required.

#### Line Drivers

Driver U1225 translates from TTL logic levels to the levels required by the EIA RS-232-C standard. It requires positive and negative supplies which are derived by diodes isolation (CR1224 and CR1223) on the +8.6 V and -8.6 V supplies. Diode isolation protects the instrument from transients or faults coupled through the RS-232-C connectors. The RLSD signal is generated by Interrupt Mask Latch U1236.

The RS-232-C receiver is U1224. It translates from RS-232-C levels to TTL logic levels and also has a protected supply. Its +5 V supply is generated by dropping the +8.6 V supply through zener diode VR1232. The IRSLD2 signal goes to Status Buffer U1223.

All of the RS-232-C signals are protected by diode arrays CR1221 and CR1222, and zener diodes VR1221 through VR1224. Any transients that exceed a ±25 V range are clipped by the networks.

Two connectors, J1212 and J1214, are provided to make interfacing easier. The male DB-25 connector conforms to the DTE (data terminal equipment) specifications of RS-232-C, and the female DB-25 connector conforms to the DCE (data communications equipment) specification. Only one of the connectors may be used at one time.

#### Interrupt Circuitry

Two interrupt lines from the UART, INTR and DR, are combined via OR gate U1234B, generating the DR+INTR interrupt line. That signal is then routed to U1232A, an AND-OR-INVERT gate, where it is gated with /DR+INTR MASK, which comes from the Interrupt Mask Latch (U1236). When /DR+INTR MASK is LO, DR+INTR can not propagate through to the output. TBRE is similarly masked by TBRE MASK, then they are ORed together and inverted within the AND-OR-INVERT gate. Inverter U1244D inverts the signal and applies it to the base of Q1221. Transistor Q1221 inverts the signal to /INTR, driving the Microprocessor maskable interrupt.

#### Interrupt Mask Latch

Interrupt Mask Latch U1236 provides four signals that are directly controlled by the Microprocessor. It is enabled when the Microprocessor writes to the addresses decoded as /LATCH. This latch uses BA0 and BA1 to select either 0D, 1D, 2D, or 3D, and latches the data present on U1235 pin 13 into the selected output when enabled. Two of the outputs are used for interrupt masking, one for the RS-232-C port, and one for diagnostics. The outputs are forced LO by the /BRST line to insure that interrupts are masked when the Microprocessor powers up.

#### Parameter Buffer

This circuit is an eight-bit input port for selecting parameters associated with the option such as baud rate and parity. It consists of buffer U1222, switch S1221, and resistor pack R1222. The switch is sensed by enabling the buffer which gates the buffer inputs onto the data bus. Bit 7 is used to sense serial data out (SDO) from U1251 for diagnostic use.

#### Status Buffer

Status buffer U1223 is used to sense three positions of Parameter switch S1221 as well as miscellaneous other signals. Functions of the Status buffer are shown in Table 7-36.

<!-- page 7-42 -->

**Table 7-36**
**RS-232-C Status Buffer Functions**

| Bit | Signal Name | Function |
|---|---|---|
| Bit 0 | /PWR INT | Power-going-down interrupt |
| Bit 1 | /DR+INTR | UART interrupt request |
| Bit 2 | TBRE | UART interrupt request |
| Bit 3 | | Parameter switch position 8 |
| Bit 4 | | Parameter switch position 10 |
| Bit 5 | | Parameter switch position 9 |
| Bit 6 | DIAG | Interrupt mask latch D3 |
| Bit 7 | /DCD2 | Data carrier detect |

### OPTION MEMORY

Option Memory (see Diagram 25) contains 32K-bytes of non-volatile memory, a lithium battery, and power failure sensing and control circuitry. When the board is installed, the option RAM is disabled.

#### Address Decoding

Addresses are decoded by U1162. All addresses in the COM-SEG range (88000-8FFFF) are used. Four active LO strobes, one for each RAM, are generated, /DECODE 0 (U1162 pin 4), /DECODE 1 (U1162 pin 5), /DECODE 2 (U1162 pin 12), and /DECODE 3 (U1162 pin 11).

#### RAM

Four 8K-byte RAMs make up the 32K-byte non-volatile memory. When instrument power is turned off, /STANDBY goes LO, placing the memories in a low current standby state. In the standby state the lithium battery (BT1101) supplies the memories standby current needs.

Each RAM is selected by its Decode signal (pin 20) when the memories are not in standby. Data is read onto the data bus, BD1-BD7, from the memory location selected by BA0-BA12 when /BRD goes LO. Data on the data bus, BD0-BD7, is written to the memory location selected by BA0-BA12 when /BWR goes LO.

#### Power Sense

Power to the RAM array is supplied by the Power Sense circuitry. The Power Sense circuit supplies power to the RAM either from the instrument power supply or from the lithium battery.

**WARNING**

*The battery used in this device may present a fire or chemical burn hazard if mistreated. Do not recharge, rapidly discharge, disassemble, heat, or short terminals. See service information for complete instructions.*

#### Lithium Battery

If instrument power is available, 5 V from the instrument forward biases CR1102 and reverse biases CR1104, disconnecting lithium battery BT1101 from the circuit. Because CR1102 is forward biased, the instrument supplies power (+Vₛ) to the RAM through CR1102.

If instrument power is not available, the lithium battery forward biases CR1104 and reverse biases CR1102, supplying power (+Vₛ) to the RAM through CR1104. If there is a circuit failure, lithium battery current is limited to safe levels by ceramic current limiter RT1102.

#### Voltage Comparator

U1122 compares the instrument voltage to an internal reference to determine if the power is going down. If power is going down, an interrupt is generated to tell the Microprocessor that the power is failing. Also, the RAM is put in standby.

Comparator U1122 compares its internal reference to the voltage on pin 3. The voltage at U1122 pin 3 is set by the instrument power supply and the voltage divider made up of R1112, R1114, and R1116.

If power is up, the voltage at pin 3 is about 1.2 V, and FAIL at U1122 pin 4 is LO. /FAIL is inverted and delayed by U1132B, C, D, and associated circuitry, making /STANDBY normal operating mode. Also, FAIL is inverted and delayed by U1132B and associated circuitry, generating /FAIL HI. Comparators U1142A and U1142B look at both the instrument supply voltage and /FAIL. If /FAIL goes HI (power is coming up), interrupts are not generated, but U1132D pulses the IRST signal HI to reset the microprocessor systems.

If power is going down or is down, the voltage at U1122 pin 3 drops below the internal reference voltage, causing FAIL, U1122 pin 4 to go HI. FAIL is inverted and delayed by U1132B and associated circuitry, generating /FAIL LO. Comparators U1142A and U1142B look at both the instrument supply voltage and /FAIL. Since /FAIL is LO

<!-- page 7-43 -->

(power failing), interrupts are generated to tell the Microprocessor that instrument power is going down. A LO /FAIL is also delayed by U1132C, D, and associated circuitry, making /STANDBY LO. This places the RAM in the low current standby operating mode.

### PERFORMANCE CHECK PROCEDURE

#### Introduction

This part of Section 7 contains the GPIB Option and RS-232-C portion of the instrument's performance check procedures. The "Performance Check Procedure" is used to check the GPIB Option performance against the requirements listed in Table 7-4. It is not necessary to remove the instrument cover to accomplish any of the performance checks.

The Option performance check intervals are identical to the basic instrument as indicated in "Performance Check Interval" in the "Performance Check Procedure" Section 4 of this manual.

#### Limits and Tolerances

The limits and tolerances stated in this procedure are GPIB and RS-232-C specifications only if they are listed in the "Performance Requirements" column of Table 7-4. The tolerances given in this procedure are valid for an instrument that is operating in and has been previously calibrated in an ambient temperature between +20°C and +30°C. The instrument also must have had at least a 20-minute warm-up period. Refer to Table 7-4 for tolerances applicable to an instrument that is operating outside this temperature range. All tolerances specified are for the instrument only and do not include test-equipment error. When performing either the GPIB or the RS-232 checks, it is assumed that the standard instrument meets all of its "Performance Requirements" as stated in the "Specification" (Section 1) of the Service manual.

#### Test Equipment Required

Test equipment listed in Table 7-37 is required to perform this procedure. Test equipment specifications described in Table 7-37 are the minimum necessary to provide accurate results. Therefore, equipment used must meet or exceed the listed specifications. Detail operating instructions for test equipment are not given in this procedure.

When equipment other than that recommended is used, control settings of the test setup may need to be altered. If the exact item of equipment given as an example in Table 7-37 is not available, check the "Minimum Specification" column to determine if any other available test equipment might suffice for the performance check procedure.

**1. GPIB Performance Check**

a. Set the RS-232-C Parameter switch to match the requirements of your controller, GPIB Address 1.

b. Set the oscilloscope's front panel controls to obtain a baseline trace.

c. Set the oscilloscope's POWER button to OFF and then to ON.

d. CHECK—The SRQ indicator is on when the power-up sequence is finished.

e. Connect the Controller via GPIB cable to the IEEE STD 488 PORT connector.

**Table 7-37**
**Test Equipment Required**

| Item and Description | Minimum Specification | Purpose | Example of Suitable Test Equipment |
|---|---|---|---|
| 1. Controller | IEEE-488-1978 compatible. | Signal source. | TEKTRONIX 4041 System Controller. |
| 2. GPIB Cable | IEEE-488-1978 compatible. | Signal interconnection. | Tektronix Part Number 012-0630-00. |
| 3. RS-232 Cable | Connectors, Male-to-female, 2 meter, 25 wires, general purpose. | Signal interconnection. | Tektronix Part Number 012-0815-00. |

<!-- page 7-44 -->

f. Enter the following program to the Controller.

```
100  Init
110  ! Initialize gpib
120  Gpib_adr = 1
130  Open #1:"gpib0(pri="&str$(gpib_adr)&",
     EOM=<0>):"
140  ! Poll the instrument
150  Poll srq_stat, srq_addr; gpib_adr
160  ! Get its EVENT code
170  Print #1: "EVENT?"
180  Input #1: eve_code
190  ! Print responses
200  Print "SRQ : ";srq_stat
210  Print " EVENT : ";eve_code
220  Close all
230  end
```

g. Run the program entered in Part f.

h. CHECK—The SRQ indicator is turned off.

i. CHECK—The controller for SRQ: 65.0 and EVEN: 401.0.

j. Disconnect the test equipment from the instrument.

**2. RS-232-C Performance Check**

a. Set the RS-232-C Parameter switch to match the requirements of your controller.

b. Set the oscilloscope's front panel controls to obtain a baseline trace.

c. Set the oscilloscope's POWER button to OFF and then to ON.

d. CHECK—The SRQ indicator is on when the power-up sequence is finished.

e. Connect the Controller via RS-232 cable to the RS232 DCE connector.

f. Enter the message "ID?;" from the controller to the RS-232.

g. CHECK—The response to the controller from the RS-232 is "TEK/2230,V81.1.VERS:XX", where "XX" is the ROM's firmware version number in the instrument.

h. CHECK—The SRQ indicator is turned off.

i. Disconnect the test equipment from the instrument.

### ADJUSTMENT PROCEDURE

There are no adjustment procedures for the GPIB and RS-232-C Options.

### OPTION MAINTENANCE INFORMATION

**WARNING**

*The battery used in this device may present a fire or chemical burn hazard if mistreated. Do not recharge, rapidly discharge, disassemble, heat above 100°C (212°F), or incinerate.*

*Replace battery with part number listed in replaceable parts section only. Use of another battery may present a risk of fire or explosion.*

*Dispose of used battery promptly. Small quantities of used batteries may be disposed of in normal refuse. Keep away from children. Do not disassemble and do not dispose of in fire.*

Maintenance information contained in the Maintenance Section of the manual also applies to these options. Additional information for the Options is contained in this part of the manual.

#### Diagnostics

Additional diagnostics are added to the instrument when Option 10 or Option 12 are added to the instrument. This discussion describes each diagnostic separately.

**COMM_RAM.** This test checks the Option RAM and its microprocessor interface. This test is performed during Power-Up. The RAM is filled with a checkerboard pattern of AA55 and 55AA and checked to see if the values are correct.

<!-- page 7-45 -->

**NOTE**

*The displayed address is offset from 0x80000 and is a 4 digit hexadecimal number between F800 and FFFF.*

If an error is found, the address of the error, the actual data found at the address, and the data expected at the address are displayed on the crt:

```
COMM_RAM : @ <address> <actual data> <>
<expected data>
```

**CMOS_RAM.** This test checks the nonvolatile CMOS memory. It is performed during Power-Up. Each stored waveform is analyzed to determine if they contain errors. If errors are found, the diagnostic either repairs or removes the waveform. If seven or more errors are found, the entire CMOS memory is reformatted, erasing all stored waveforms. If errors are found, the result of the recovery attempt displayed on the crt:

```
CMOS : reformatted
or
CMOS : recovered
```

If errors are reported, the instrument should be turned off and then powered up again. An error should be ignored unless it is repeatable.

**ROM_MATCH.** This test checks to see that all ROMs are the correct version number ROMs. Each ROM is checked during Power-Up. If an error is found, the version numbers found are displayed on the crt:

```
ROMS:mismatch,nn,mm,oo
```

where nn, mm, and oo are the version numbers of the ROMs in the instrument.

**COMM_READBACK.** Bit paths within the Option are checked by COMM_READBACK. GPIB circuitry checked includes U1335B and U1323. RS-232-C circuitry checked includes U1236 and U1223. Data is first written to the Option. Registers are then read and checked for the correct data. If the data read back is in error, the actual data read back is displayed on the crt:

```
COMM_RB : rb(1) = x₂x₁ & rb(0) = y₂y₁
```

where:

rb is the data written to the Option (U1236 pin 7 or U1335 pin 10).

x₁ = y₁ = data read back from the Option (U1223 pin 3 or U1323 pin 3).

x₂ = y₂ = data read back from the Option (U1223 pin 2 or U1323 pin 2).

**COMM_LOOPBACK.** This test checks the GPIB controller U1321 and associated circuitry by commanding the controller to change its TR output and then checking the TR output. If an error is found it is displayed on the crt:

```
COMM_LB : FGET NOT SET
or
COMM_LB : FGET NOT CLEAR
```

**INPUT_PORTS.** Two additional ports are added to the INPUT_PORTS diagnostic. Option 10 adds U1322 and U1323. Option 12 adds U1222 and U1223. They are labeled on the crt display as COMM_STAT U1x23 and COMM_PARAM U1x22.

**OUT_PORTS.** Two output ports are added the the OUT_PORTS diagnostic by the Options. OUT_PORTS is run at power-up only. Option 10 adds U1335B. The pattern seen on U1315B pin 10 is about an eight second square wave. Option 12 adds U1236. The pattern seen on U1236 is the same type of shift pattern as for the PRC test.

#### Removal and Replacement Instructions

The exploded view drawings in the "Replaceable Mechanical Parts" list (Section 9) may be helpful during the removal and reinstallation of the GPIB and RS-232-C assembly and its circuit boards from the instrument. Circuit board and component locations are shown in the "Diagrams" section.

**CABINET.** To remove either the GPIB or the RS-232-C Assembly from the instrument, perform the "Cabinet" removal procedure in the "Removal and Replacement Instructions" of Section 6. In step 4 of the procedure, remove two screws and two post spacers and washers from the GPIB side panel or two screws and four post spacers and washers from the RS-232-C side panel.

**MEMORY CIRCUIT BOARD.** The Memory circuit board can be removed and reinstalled as follows:

1. Remove the four flat-head screws that secures the insulation and the Memory circuit board to the Option Assembly. Remove the insulation from the Memory circuit board.

<!-- page 7-46 -->

2. Remove the Memory circuit board from GPIB Assembly by carefully pulling the connectors P1251 and P1222 on the Memory circuit board from the pins of J1251 and J1222 on either the GPIB or the RS-232-C circuit board. The connectors are located on the inside and at each end of the Memory circuit board. Disconnect P1152 from the rear of the Memory circuit board as it being removed from the GPIB Assembly.

To reinstall the Memory circuit board, perform the reverse of the preceding steps.

**GPIB AND RS-232-C ASSEMBLIES.** The Option assembly can be removed and reinstalled as follows:

**NOTE**

*The field-installed GPIB Option and RS-232-C Option have one more connector to be removed than the factory installed Options.*

1. Disconnect the following connectors from the Option Assembly and the instrument.

a. P4110, a two-wire connector located at the rear of the Option Assembly.

b. P6423, a four-wire connector located at the rear of the Option Assembly.

c. P9301, a five-wire connector located at the rear of the Option Assembly.

d. P8100, a ribbon cable from the Storage circuit board.

**NOTE**

*Instruments with factory-installed GPIB and RS-232-C, proceed to step 3. For field-installed GPIB and RS-232-C, proceed with step 2.*

2. Disconnect either P1316 (GPIB) or P1216 (RS-232-C) from the front of the Option assembly circuit board.

3. Stand the instrument on its side (Option Assembly up) and remove two screws from the extreme edge of the bottom chassis frame underneath the delay line cable.

4. Lay the instrument down and remove the two screws from the top of the chassis frame (located inside the two cutouts on the Storage circuit board). Note the position of the ground clip when removing the screw from the chassis frame.

5. Remove the Option Assembly out from between the top and bottom chassis frames.

6. Slide the Option Assembly forward until the ribbon cable clears the Storage circuit board.

7. Remove the Option Assembly from the instrument by tilting the bottom of the assembly out first.

To reinstall the Option Assembly, perform the reverse of the preceding steps.
