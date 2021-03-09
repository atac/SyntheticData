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
compliance with IRIG 106. The data in the synthetic data file contains one or more 
IRIG 106 data channels. Each synthetic data channel present is formed and formatted 
in one of the “Channel Layouts” defined below.
A synthetic data file will be formatted in accordance with a specific “IRIG File Layout” as defined in the following sections. For example, IRIG File Layout 1 defined in Section 3.1 contains (or may contain) specific TMATS, Time, Video, PCM, and MIL-STD-1553 data channels. Each IRIG data channel, such as the MIL-STD-1553 channel, contains data of the specified type and formats as defined in Section 4. In this way different IRIG File Layout definitions can be defined in a “mix and match” fashion from defined data channel types.

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

| 1553 Cmd Word | Description |
| :-----------: | ----------- |
| 6-T-29-32     | 1553 Message Layout B100 - Synthetic GPS/INS Navigation at 25 Hz rate |

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

##### Word 3,4 - X Velocity

| Word | Bit Position | Description |
| :--: | :----------: | ----------- |
|   3  | 15 - 0       | X Velocity MSW |
|   4  | 15 - 0       | X Velocity LSW |

32 bit two’s-complement signed integer representing aircraft inertial velocity in the 
local X (North) direction in feet per second.  
LSB = 4 feet per second

##### Word 5,6 - Y Velocity

| Word | Bit Position | Description |
| :--: | :----------: | ----------- |
|   5  | 15 - 0       | Y Velocity MSW |
|   6  | 15 - 0       | Y Velocity LSW |

32 bit two’s-complement signed integer representing aircraft inertial velocity in the 
local Y (East) direction in feet per second.  
LSB = 4 feet per second

##### Word 7,8 - Z Velocity

| Word | Bit Position | Description |
| :--: | :----------: | ----------- |
|   7  | 15 - 0       | Z Velocity MSW |
|   8  | 15 - 0       | Z Velocity LSW |

32 bit two’s-complement signed integer representing aircraft inertial velocity in the 
local Z (Down) direction in feet per second.  
LSB = 4 feet per second

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

##### Word 12 - True Heading

| Word | Bit Position | Description |
| :--: | :----------: | ----------- |
|  12  | 15 - 0       | True Heading |

16 bit two’s-complement signed integer aircraft heading from True North in degrees  
MSB = 180 degrees

##### Word 13 - Magnetic Heading

| Word | Bit Position | Description |
| :--: | :----------: | ----------- |
|  13  | 15 - 0       | Magnetic Heading |

16 bit two’s-complement signed integer aircraft heading from Magnetic North in degrees  
MSB = 180 degrees

##### Word 14 - X Acceleration

| Word | Bit Position | Description |
| :--: | :----------: | ----------- |
|  14  | 15 - 5       | X Acceleration |
|  14  | 4 - 0        | Not used       |

Bits 5 - 15 – 11 bit two’s complement signed integer representing aircraft acceleration local X (North) direction in feet / second\^2.  
LSB = 1 foot / second\^2

##### Word 15 - Y Acceleration

| Word | Bit Position | Description |
| :--: | :----------: | ----------- |
|  15  | 15 - 5       | Y Acceleration |
|  15  | 4 - 0        | Not used       |

Bits 5 - 15 – 11 bit two’s complement signed integer representing aircraft acceleration local Y (East) direction in feet / second\^2.  
LSB = 1 foot / second\^2

##### Word 16 - Z Acceleration

| Word | Bit Position | Description |
| :--: | :----------: | ----------- |
|  16  | 15 - 5       | Z Acceleration |
|  16  | 4 - 0        | Not used       |

Bits 5 - 15 – 11 bit two’s complement signed integer representing aircraft acceleration local Z (Down) direction in feet / second\^2.  
LSB = 1 foot / second\^2

##### Word 17 to 20 - Not Used

##### Word 21,22 - Latitude

| Word | Bit Position | Description |
| :--: | :----------: | ----------- |
|  21  | 15 - 0       | Latitude MSW |
|  22  | 15 - 0       | Latitude LSW |

32 bit two’s-complement signed integer representing aircraft latitude in semi-circles.  
MSB = 180 degrees

##### Word 23,24 - Longitude

| Word | Bit Position | Description |
| :--: | :----------: | ----------- |
|  23  | 15 - 0       | Longitude MSW |
|  24  | 15 - 0       | Longitude LSW |

32 bit two’s-complement signed integer representing aircraft longitude in semi-circles.  
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



