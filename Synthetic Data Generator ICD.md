# Synthetic Data Generator  
# Interface Control Document

## Overview

Synthetic IRIG 106 data files are data files that contain data IRIG 106 format data
that is similar to actual recorded flight test test. Most actual flight test data has
restricted distribution and so isn't available for many developers. Synthetic data
solves this problem by providing data files that are very similar to what might be
expected from an actual flight data recorder recording an actual aircraft mission.

Normally an Interface Control Document (ICD) describes a single data file format.
It is anticipated, though, that synthetic data files may be similar to each other but
may be tailored to demonstrate particular data types or values.

A synthetic IRIG 106 data file is a complete and properly formed data file in 
compliance with IRIG 106. A synthetic data file will be of a particular **Data File Layout** as
described in the sections below. 

A particular synthetic data file contains one or more 
IRIG 106 data channels. Each synthetic data channel present is formed and formatted 
in one of the **Data Channel Layouts** defined below.

A synthetic data file will be formatted in accordance with a specific “IRIG File Layout” as defined 
in the following sections. For example, IRIG Data File Layout 1 defined below contains (or may contain) 
specific TMATS, Time, Video, PCM, and MIL-STD-1553 data channels. Each IRIG data channel, such as the 
MIL-STD-1553 channel, contains data of the specified type and formats as defined later in this document. 
In this way different IRIG File Layout definitions can be defined in a “mix and match” fashion from 
defined data channel types.

---

## IRIG Synthetic Data File Layouts

### Synthetic Data File Layout 1

| Ch ID | IRIG Type | Description |
| :---: | --------- | ----------- |
|   0   | "Computer Generated Data Packet Format 1 TMATS (0x01)” | |
|   1   | “Time Data Format 1 (0x11)”    | |
|  10   | “Video Data Format 2 (0x42)”   | This channel is a video channel of the unobstructed view out the front of the aircraft with a HUD display superimposed. |
|  11   | “Video Data Format 2 (0x42)”   | This channel is a video channel of the view of the interior of the cockpit as the pilot would see it. |
|  12   | “Video Data Format 2 (0x42)”   | This channel is a video channel of the view of the aircraft as it would be seen by a chase plane. |
|  20   | “PCM Format 1 (0x09)”          | See section **File Layout 1 Channel ID 20 Message Layout** below for details. |
|  30   | “MIL-STD-1553 Format 1 (0x19)” | See section **File Layout 1 Channel ID 30 Message Layout** below for details |
|  40   | “ARINC-429 Format 0 (0x38)”    | Left engine parameters. See section **File Layout 1 Channel ID 40 Message Layout** below for details |
|  41   | “ARINC-429 Format 0 (0x38)”    | Right engine parameters. See section **File Layout 1 Channel ID 41 Message Layout** below for details |


#### File Layout 1 Channel ID 20 Message Layout

File Layout 1 Channel ID 20 is a PCM data channel. The PCM frame data layout is described in **IRIG PCM Frame Data Layout P100** later in this document. 
 Complete PCM frames occur at a 25 Hz rate.

#### File Layout 1 Channel ID 30 Message Layout

File Layout 1 Channel ID 30 is a MIL-STD-1553 channel. It may have various 1553 message
as described below.


| RT Number | RT Description |
| :-------: | -------------- |
|     6     | Nav device such as an INS / GPS |

Note: The 1553 Command Word is depicted as "RT Num - T/R Bit - Subaddress - Word Count" in the table below.

| 1553 Cmd Word |  Rate       | Description |
| :-----------: | :---------: | ----------- |
| 6-T-29-32     | 25 Hz       | 1553 Message Layout B100 - Synthetic GPS/INS Navigation Position |

#### File Layout 1 Channel ID 40 Message Layout

File Layout 1 Channel ID 40 data represent engine performance parameters for the left aircraft engine
in ARINC-429 messages. This Ch 10 channel will have engine data from multiple ARINC-429 buses as described
in **ARINC-429 Engine Data Layout AR100** later in this document. All messages occur at a 100 Hz rate.

#### File Layout 1 Channel ID 41 Message Layout

File Layout 1 Channel ID 41 data represent engine performance parameters for the right aircraft engine
in ARINC-429 messages. This Ch 10 channel will have engine data from multiple ARINC-429 buses as described
in **ARINC-429 Message Data Layout AR100** later in this document. All messages occur at a 100 Hz rate.

---

## Individual IRIG Data Channel Layouts


### IRIG 1553 Subaddress Data Layouts

Below are definitions of various 1553 messages. Each message layout section defines up to
32 16-bit words associated with one 1553 subaddress. For the purposes of this ICD 1553 bit
positions are numbered as shown below. Note that this is opposite of the bit order shown
in MIL-STD-1553. Also note that Word 1 is the first 1553 word. There is no Word 0.

| 15 | 14 | 13 | 12 | 11 | 10 |  9 |  8 |  7 |  6 |  5 |  4 |  3 |  3 |  1 |  0 |
|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|


#### IRIG 1553 Subaddress Data Layout B100 - Synthetic GPS/INS Navigation

##### Word 1 - Status


| Word | Bit Position | Description |
| :--: | :----------: | ----------- |
|   1  | 15 - 7       | Not Used    |
|   1  | 6            | Position data valid, 0 = Not Valid, 1 = Valid |
|   1  | 5            | Altitude data valid, 0 = Not Valid, 1 = Valid |
|   1  | 4            | Velocity data valid, 0 = Not Valid, 1 = Valid |
|   1  | 3            | Acceleration data valid, 0 = Not Valid, 1 = Valid |
|   1  | 2            | True Heading data valid, 0 = Not Valid, 1 = Valid |
|   1  | 1            | Magnetic Heading data valid, 0 = Not Valid, 1 = Valid |
|   1  | 0            | Pitch and Roll data valid, 0 = Not Valid, 1 = Valid |

##### Word 2 - Not Used

##### Words 3 to 8 - Velocity

| Word | Bit Position | Description |
| :--: | :----------: | ----------- |
|   3  | 15 - 0       | X Velocity MSW |
|   4  | 15 - 0       | X Velocity LSW |
|   5  | 15 - 0       | Y Velocity MSW |
|   6  | 15 - 0       | Y Velocity LSW |
|   7  | 15 - 0       | Z Velocity MSW |
|   8  | 15 - 0       | Z Velocity LSW |

32 bit two’s-complement signed integer representing aircraft inertial velocity in the 
local X (North), Y (East), and Z (down) direction in feet per second.  
LSB = 1 / 262,144 (3.814697E-6) feet per second

##### Word 9 - Azimuth

| Word | Bit Position | Description |
| :--: | :----------: | ----------- |
|   9  | 15 - 0       | Aircraft Azimuth |

16 bit two’s-complement signed integer aircraft azimuth from True North in degrees  
MSB = 180 degrees

##### Word 10 - Roll

| Word | Bit Position | Description |
| :--: | :----------: | ----------- |
|  10  | 15 - 0       | Aircraft Roll |

16 bit two’s-complement signed integer representing aircraft roll from local level in degrees  
MSB = 180 degrees  
Positive = Left wing up

##### Word 11 - Pitch

| Word | Bit Position | Description |
| :--: | :----------: | ----------- |
|  11  | 15 - 0       | Aircraft Pitch |

16 bit two’s-complement signed integer representing aircraft pitch from local level in degrees  
MSB = 180 degrees  
Positive = Nose up

##### Word 12 to 13 - Heading

| Word | Bit Position | Description |
| :--: | :----------: | ----------- |
|  12  | 15 - 0       | True Heading |
|  13  | 15 - 0       | Magnetic Heading |

16 bit two’s-complement signed integer aircraft heading from North in degrees  
MSB = 180 degrees

##### Words 14 to 16 - Acceleration

| Word | Bit Position | Description |
| :--: | :----------: | ----------- |
|  14  | 15 - 0       | X Acceleration |
|  15  | 15 - 0       | Y Acceleration |
|  16  | 15 - 0       | Z Acceleration |

Bits 0 - 15 – Two’s complement signed integer representing aircraft acceleration local X (North), Y (North), Z (Down) direction in feet / second\^2.  
LSB = 1/32 (0.03125) foot / second\^2

##### Word 17 to 20 - Not Used

##### Word 21 to 24 - Latitude and Longitude

| Word | Bit Position | Description |
| :--: | :----------: | ----------- |
|  21  | 15 - 0       | Latitude MSW |
|  22  | 15 - 0       | Latitude LSW |
|  23  | 15 - 0       | Longitude MSW |
|  24  | 15 - 0       | Longitude LSW |

32 bit two’s-complement signed integer representing aircraft latitude / longitude in semi-circles.  
MSB = 180 degrees

##### Word 25 - Altitude

| Word | Bit Position | Description |
| :--: | :----------: | ----------- |
|  25  | 15 - 0       | Altitude    |

16 bit two’s-complement signed integer representing aircraft MSL altitude in feet.  
LSB = 4 feet

##### Word 26 to 32 – Not used

---

### IRIG PCM Frame Data Layouts

#### IRIG PCM Frame Data Layout P100

| PCM Word  | Data Format | Units   | Description |
| :-------: | ----------- | ------- | ----------- |
|  0        | uint32      |         | uFrameSync |
|  1,2      | uint32      | SemiCir | LATITUDE POSITION |
|  3,4      | uint32      | SemiCir | LONGITUDE POSITION |
|  5        | uint16      | FEET    | PRESSURE ALTITUDE + 1000 FEET |
|  6        | uint16      | KNOTS   | TRUE AIRSPEED |
|  7        | uint16      | SemiCir | TRUE HEADING (+/-180, 0 = north) |
|  8        | uint16      | SemiCir | MAGNETIC HEADING |
|  9        | int16       | SemiCir | PITCH ANGLE |
| 10        | int16       | SemiCir | ROLL ANGLE (+ = left wing high) |
| 11        | int16       | SemiCir | CORRECTED ANGLE OF ATTACK |
| 12        | int16       | G       | VERTICAL ACCELERATION (+ = down, nominal = +1) |
| 13        | uint16      | KNOTS   | GROUND SPEED |
| 14        | int16       | FT/MIN  | INERTIAL VERTICAL SPEED |
| 15        | int16       | G       | FLIGHT PATH ACCELERATION (X,Y component, + = forward) |
| 16        | int16       | SemiCir | POWER LEVER ANGLE 1 (roughly 0 to 90) |
| 17        | int16       | SemiCir | POWER LEVER ANGLE 1 (roughly 0 to 90) |
| 18        | int16       | DEG     | EXHAUST GAS TEMPERATURE 1 |
| 19        | int16       | DEG     | EXHAUST GAS TEMPERATURE 2 |
| 20        | int16       | DEG     | OIL TEMPERATURE 1 |
| 21        | int16       | DEG     | OIL TEMPERATURE 2 |
| 22        | uint16      | LBS/HR  | FUEL FLOW 1 |
| 23        | uint16      | LBS/HR  | FUEL FLOW 2 |
| 24        | int16       | %RPM    | FAN SPEED 1 |
| 25        | int16       | %RPM    | FAN SPEED 2 |
| 26        | int16       | %RPM    | CORE SPEED 1 |
| 27        | int16       | %RPM    | CORE SPEED 2 |
| 28        | int16       | IN/SEC  | ENGINE VIBRATION 1 |
| 29        | int16       | IN/SEC  | ENGINE VIBRATION 2 |
| 30        | int16       | PSI     | OIL PRESSURE 1 |
| 31        | int16       | PSI     | OIL PRESSURE 2 |
| 32        | int16       | SemiCir | ANGLE OF ATTACK 1 |
| 33        | int16       | SemiCir | ANGLE OF ATTACK 2 |
| 34        | bit         |         | WEIGHT ON WHEELS (0 = true) |
|           | bit         |         | GEARS L&R DOWN LOCKED (0 = true) |
|           | bit         |         | GEARS L&R UP LOCKED (0 = true) |
|           | unused      |         | |
| 35        | int16       | SemiCir | AILERON POSITION LH |
| 36        | int16       | SemiCir | AILERON POSITION RH |
| 37        | int16       | SemiCir | ELEVATOR POSITION LEFT |
| 38        | int16       | SemiCir | ELEVATOR POSITION RIGHT |
| 39        | int16       | SemiCir | RUDDER POSITION |
| 40        | uint16      | COUNTS  | CONTROL WHEEL POSITION CAPT |
| 41        | uint16      | COUNTS  | CONTROL WHEEL POSITION F/O |
| 42        | uint16      | COUNTS  | CONTROL COLUMN POSITION CAPT (750 - 4000) |
| 43        | uint16      | COUNTS  | CONTROL COLUMN POSITION F/O |
| 44        | uint16      | COUNTS  | RUDDER PEDAL POSITION (1000 - 3000) |
| 45        | uint16      | COUNTS  | T.E. FLAP POSITION |
| 46-48     | uint16      |         | Filler

| Data Format | Description |
| ----------- | ----------- |
| SemiCir     | MSB = 180 degress, 2nd MSB = 90 degrees, etc. |
| uint16      | 16 bit unsigned integer |
| uint32      | 32 bit unsigned integer |
| float       | 32 bit IEEE-754 floating point |
| bit         | Single bit value |
|             | Note: All multibyte data is stored in little endian byte order |

---

### ARINC-429 Message Data Layouts

The ARINC-429 data word format contains an 8 bit **Label** field. The Label field bit order is
reversed, with the most significant bit being right-most in the 32 bit 429 data word. Besides
being reversed, the value of the label field is often displayed in 3-bit octal notation. So
for example, an ARINC-429 Octal Label = 41 is 00 100 001 in binary. Reversing the bits this
label value is stored in the lable field as 1000 0100 in binary (or 0x84 in hex).

For all ARINC-429 Message Data Layout messages defined below **SDI** and **SSM** fields have the
following definition.

| SDI Code          | 10  | 9   |
| ----------------  | :-: | :-: |
| (0) Channel A     |  0  | 0   |
| (1) Channel B     |  0  | 1   |
| (2) Channel C     |  1  | 0   |
| (3) Not Used      |  1  | 1   |

| SSM Code [BNR]    | 31  | 30  |
| ----------------  | :-: | :-: |
| Failure Warning   |  0  |  0  |
| No Computed Data  |  0  |  1  |
| Functional Test   |  1  |  0  |
| Normal Operation  |  1  |  1  |

#### ARINC-429 Message Data Layout AR100

This set of ARINC-429 messages primarily represent engine performance paramaters. Different engine
parameters are stored in different ARINC-429 buses within one Ch 10 channel. There is only one message
type and format per ARINC-429 bus.

| ARINC-429 Bus | Octal Label | Hex Value | Description |
| :-----------: | :---------: | :-------: | ----------- |
|  0            |  Label 41   | 0x84      | Engine Fan RPM (N1 Actual)    |
|  0            |  Label 42   | 0x44      | Engine Fan RPM (N1 Demand)    |
|  0            |  Label 43   | 0xC4      | Engine Oil Pressure           |
|  0            |  Label 44   | 0x24      | Engine Turbine RPM (N2)       |
|  0            |  Label 45   | 0xA4      | Exhaust Gas Temperature (EGT) |
|  0            |  Label 46   | 0x64      | Engine Oil Temperature        |
|  0            |  Label 47   | 0xE4      | Fuel Flow                     |

Bit positions marked RESERVED shall contain a '0'.

##### ARINC-429 Message Data Layout AR100 Bus 0

_Engine Fan RPM (N1 Actual)_

| Bit | Description |
| --- | ----------- |
| 1-8 | Octal Label = 41 (0x84) |
| 9   | SDI Code LSB |
| 10  | SDI Code MSB |
| 11  | RESERVED |
| ... ||
| 17  | RESERVED |
| 18  | N1 RPM LSB = MSB/2^10 RPM |
| ... ||
| 28  | N1 RPM MSB = 64% RPM |
| 29  | RESERVED |
| 30  | SSM Code |
| 31  | SSM Code |
| 32  | Parity (Odd) |

Engine RPM N1 Actual value is in unsigned integer format.

Range
- Max Value: 110
- Min Value: 0

##### ARINC-429 Message Data Layout AR100 Bus 1

_Engine Fan RPM (N1 Demand)_

| Bit | Description |
| --- | ----------- |
| 1-8 |  Octal Label = 42 (0x44) |
| 9   |  SDI Code LSB |
| 10  |  SDI Code MSB |
| 11  |  RESERVED |
| ... ||
| 17  |  RESERVED |
| 18  |  PMC Demand (LSB = MSB/2^10 RPM) |
| ... ||
| 28  |  PMC Demand (MSB) (64% RPM) |
| 29  |  RESERVED |
| 30  |  SSM Code |
| 31  |  SSM Code |
| 32  |  Parity (Odd) |

Engine RPM N1 Demand value is in unsigned integer format.

Range
- Max Value: 110 
- Min Value: 0

##### ARINC-429 Message Data Layout AR100 Bus 2

_Engine Oil Pressure_

| Bit | Description |
| --- | ----------- |
| 1-8 |  Octal Label = 43 (0xC4) |
| 9   |  SDI Code LSB |
| 10  |  SDI Code MSB |
| 11  |  Sensor Data Status: (0 = Raw Sensor Data, 1 = Calibrated Data)
| 12  |  Engine Oil Pressure LSB = MSB/ 2^16
| ... ||
| 28  |  Engine Oil Pressure MSB = 64 psi
| 29  |  Sign     |
| 30  |  SSM Code |
| 31  |  SSM Code |
| 32  |  Parity (Odd) |

Engine Oil Pressure value is in two's complement format.

##### ARINC-429 Message Data Layout AR100 Bus 3

_Engine Turbine RPM (N2)_

| Bit | Description |
| --- | ----------- |
| 1-8 |  Octal Label = 44 (0x24) |
| 9   |  SDI Code LSB |
| 10  |  SDI Code MSB |
| 11  |  RESERVED |
| ... ||
| 17  |  RESERVED |
| 18  |  N2 RPM LSB= MSB/2^10 RPM
| ... ||
| 28  |  N2 RPM MSB = 64% RPM
| 29  |  RESERVED |
| 30  |  SSM Code |
| 31  |  SSM Code |
| 32  |  Parity (Odd) |

Engine Turbine RPM N2 value is in unsigned integer format.

Range
- Max Value: 110
- Min Value: 0

##### ARINC-429 Message Data Layout AR100 Bus 4

_Exhaust Gas Temperature (EGT)_

| Bit | Description |
| --- | ----------- |
| 1-8 |  Octal Label = 45 (0xA4) |
| 9   |  SDI Code LSB |
| 10  |  SDI Code MSB |
| 11  |  RESERVED |
| ... ||
| 17  |  RESERVED |
| 18  |  EGT Value LSB = MSB/2^10 |
| ... ||
| 28  |  EGT Value MSB = 1024 °C |
| 29  |  RESERVED |
| 30  |  SSM Code |
| 31  |  SSM Code |
| 32  |  Parity (Odd) |

Exhaust Gas Temperature value is in unsigned integer format.

Range
- Max Value: 1100
- Min Value: 0

##### ARINC-429 Message Data Layout AR100 Bus 5

_Engine Oil Temperature_

| Bit | Description |
| --- | ----------- |
| 1-8 |  Octal Label = 46 (0x64) |
| 9   |  SDI Code LSB |
| 10  |  SDI Code MSB |
| 11  |  RESERVED |
| ... ||
| 19  |  RESERVED |
| 20  |  Engine Oil Temperature LSB = MSB/2^8 |
| ... ||
| 28  |  Engine Oil Temperature MSB = 128 °C |
| 29  |  Sign     |
| 30  |  SSM Code |
| 31  |  SSM Code |
| 32  |  Parity (Odd) |

Engine Oil Temperature value is in two's complement format.

Range
- Max Value: 200
- Min Value: -50

##### ARINC-429 Message Data Layout AR100 Bus 6

_Fuel Flow_

| Bit | Description |
| --- | ----------- |
| 1-8 |  Octal Label = 47 (0xE4) |
| 9   |  SDI Code LSB |
| 10  |  SDI Code MSB |
| 11  |  RESERVED |
| ... ||
| 14  |  RESERVED |
| 15  |  Fuel Flow LSB = MSB/2^13 |
| ... ||
| 28  |  Fuel Flow MSB = 8192 PPH |
| 29  |  RESERVED |
| 30  |  SSM Code |
| 31  |  SSM Code |
| 32  |  Parity (Odd) |

Fuel Flow value is in unsigned integer format.

Range
- Max Value: 15000
- Min Value: 0

---
