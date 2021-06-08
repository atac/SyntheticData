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
|   1   | “Time Data Format 1 (0x11)” | |
|  10   | “Video Data Format 2 (0x42)” | This channel is a video channel of the unobstructed view out the front of the aircraft with a HUD display superimposed. |
|  11   | “Video Data Format 2 (0x42)” | This channel is a video channel of the view of the interior of the cockpit as the pilot would see it. |
|  12   | “Video Data Format 2 (0x42)” | This channel is a video channel of the view of the aircraft as it would be seen by a chase plane. |
|  20   | “PCM Format 1 (0x09)”        | This channel contains PCM data. The PCM data conforms to the Synthetic PCM Source Type 1 format described in Section 4.3. Complete PCM frames occur at a 25 Hz rate. |
|  30   | “MIL-STD-1553 Format 1 (0x19)” | See section "File Layout 1 Channel ID 30 Message Layout" below for details |

#### File Layout 1 Channel ID 20 Message Layout

File Layout 1 Channel ID 20 is a PCM data channel. (More to come)

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
local X (North) direction in feet per second.  
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

#### IRIG PCM Frame Data Layout 1

| Start Bit | End Bit | Data Format | Units   | Description |
| :-------: | :-----: | ----------- | ------- | ----------- |
| 1         | 32      | uint32      |         | uFrameSync |
| 33        | 64      | uint32      | SemiCir | LATITUDE POSITION |
| 65        | 96      | uint32      | SemiCir | LONGITUDE POSITION |
| 97        | 112     | uint16      | FEET    | PRESSURE ALTITUDE + 1000 FEET |
| 113       | 144     | float       | KNOTS   | TRUE AIRSPEED |
| 145       | 176     | float       | DEG     | TRUE HEADING (+/-180, 0 = north) |
| 177       | 208     | float       | DEG     | MAGNETIC HEADING |
| 209       | 240     | float       | DEG     | PITCH ANGLE |
| 241       | 272     | float       | DEG     | ROLL ANGLE (+ = left wing high) |
| 273       | 304     | float       | DEG     | CORRECTED ANGLE OF ATTACK |
| 305       | 336     | float       | G       | VERTICAL ACCELERATION (+ = down, nominal = +1) |
| 337       | 368     | float       | KNOTS   | GROUND SPEED |
| 369       | 400     | float       | FT/MIN  | INERTIAL VERTICAL SPEED |
| 401       | 432     | float       | G       | FLIGHT PATH ACCELERATION (X,Y component, + = forward) |
| 433       | 464     | float       | DEG     | POWER LEVER ANGLE 1 (roughly 0 to 90) |
| 465       | 496     | float       | DEG     | POWER LEVER ANGLE 1 (roughly 0 to 90) |
| 497       | 528     | float       | DEG     | EXHAUST GAS TEMPERATURE 1 |
| 529       | 560     | float       | DEG     | EXHAUST GAS TEMPERATURE 2 |
| 561       | 592     | float       | DEG     | OIL TEMPERATURE 1 |
| 593       | 624     | float       | DEG     | OIL TEMPERATURE 2 |
| 625       | 656     | float       | LBS/HR  | FUEL FLOW 1 |
| 657       | 688     | float       | LBS/HR  | FUEL FLOW 2 |
| 689       | 720     | float       | %RPM    | FAN SPEED 1 |
| 721       | 752     | float       | %RPM    | FAN SPEED 2 |
| 753       | 784     | float       | %RPM    | CORE SPEED 1 |
| 785       | 816     | float       | %RPM    | CORE SPEED 2 |
| 817       | 848     | float       | IN/SEC  | ENGINE VIBRATION 1 |
| 849       | 880     | float       | IN/SEC  | ENGINE VIBRATION 2 |
| 881       | 912     | float       | PSI     | OIL PRESSURE 1 |
| 913       | 944     | float       | PSI     | OIL PRESSURE 2 |
| 945       | 976     | float       | DEG     | ANGLE OF ATTACK 1 |
| 977       | 1008    | float       | DEG     | ANGLE OF ATTACK 2 |
| 1009      | 1009    | bit         |         | WEIGHT ON WHEELS (0 = true) |
| 1010      | 1010    | bit         |         | GEARS L&R DOWN LOCKED (0 = true) |
| 1011      | 1011    | bit         |         | GEARS L&R UP LOCKED (0 = true) |
| 1012      | 1024    | unused      |         | |
| 1025      | 1056    | float       | DEG     | AILERON POSITION LH |
| 1057      | 1088    | float       | DEG     | AILERON POSITION RH |
| 1089      | 1120    | float       | DEG     | ELEVATOR POSITION LEFT |
| 1121      | 1152    | float       | DEG     | ELEVATOR POSITION RIGHT |
| 1153      | 1184    | float       | DEG     | RUDDER POSITION |
| 1185      | 1200    | uint16      | COUNTS  | CONTROL WHEEL POSITION CAPT |
| 1201      | 1216    | uint16      | COUNTS  | CONTROL WHEEL POSITION F/O |
| 1217      | 1232    | uint16      | COUNTS  | CONTROL COLUMN POSITION CAPT (750 - 4000) |
| 1233      | 1248    | uint16      | COUNTS  | CONTROL COLUMN POSITION F/O |
| 1249      | 1264    | uint16      | COUNTS  | RUDDER PEDAL POSITION (1000 - 3000) |
| 1265      | 1280    | uint16      | COUNTS  | T.E. FLAP POSITION |

| Data Format | Description |
| ----------- | ----------- |
| uint16      | 16 bit unsigned integer |
| uint32      | 32 bit unsigned integer |
| float       | 32 bit IEEE-754 floating point |
| bit         | Single bit value |
|             | Note: All multibyte data is stored in little endian byte order |

