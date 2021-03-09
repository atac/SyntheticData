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
---

## Individual IRIG Data Channel Layouts



### IRIG 1553 Subaddress Data Layouts

Below are definitions of various 1553 messages. Each message layout section defines up to
32 16-bit words associated with one 1553 subaddress.

#### IRIG 1553 Subaddress Data Layout B100 - Synthetic GPS/INS Navigation

##### Word 1 - Status

| 15 | 14 | 13 | 12 | 11 | 10 |  9 |  8 |  7 |  6 |  5 |  4 |  3 |  3 |  1 |  0 |
|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|
|    |    |    |    |    |    |    |    |    | AL | VP | VV | VA | TH | MH | PR |

Bit 6 – Position data valid  
: 0 = Not Valid  
: 1 = Valid

Bit 5 – Altitude data valid  
	0 = Not Valid  
	1 = Valid

Bit 4 – Velocity data valid
	0 = Not Valid
	1 = Valid
Bit 3 – Acceleration data valid
	0 = Not Valid
	1 = Valid
Bit 2 – True Heading data valid
	0 = Not Valid
	1 = Valid
Bit 1 – Magnetic Heading data valid
	0 = Not Valid
	1 = Valid
Bit 0 – Pitch and Roll data valid
	0 = Not Valid
	1 = Valid
