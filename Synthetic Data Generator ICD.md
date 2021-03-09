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
|  30   | “MIL-STD-1553 Format 1 (0x19)” | |

#### Channel ID 30 1553 Message Layout

| 1553 Cmd Word | Description |
| :-----------: | ----------- |
| 7-T-29-32           | RT->BC Synthetic GPS/INS Navigation Message Type 1 at 1 Hz rate |
| 27-R-26-32 / 7-T-29-32 | RT->RT Synthetic GPS/INS Navigation Message Type 1 at 25 Hz rate |


## IRIG Data Channel Layouts

### IRIG Data Channel Layout 1