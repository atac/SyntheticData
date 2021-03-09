# IRIG 106 Synthetic Data Generation Tools

These software tools are used to generate IRIG 106 Chapter 10 flight test data files from synthesized flight and sensor data. 
Generating synthetic Ch 10 data is a Multi-Step process. COTS, GOTS, and Custom Applications are used. This repository contains the custom
software tools which, when used with the described GOTS and COTS sofware, will generate 


Current Ch 10 Data Types Generated
- Time
- Video
- Cockpit View
- HUD View
- Chase Plane View
- 1553 Navigation Message


|  Software          | License      | Description  |
| ------------------ | ------------ | ------------ |
|  BlueMax6          | GOTS         | Aircraft Flight Dynamics, Flight Path Generator, Maneuver, Mission and Aero-Performance Evaluation Model. Used to generate simulated flight path data. |
|  BMtoDB            | Custom       | Convert BlueMax6 output to SQLite3 flight database. |
|  BM6toXPlaneVideo  | Custom       | Used to add MPEG video data to SQLite3 flight database |
|  csv-parser        | Open Source  | Library to easily read and parse CSV files |
|  ffmpeg            | Open Source  | Library for MPEG video encoding |
|  XPlane 11         | COTS         | Flight simulator, used to generate realistic flight imagery for video generation |
|  SQLite3           | Open Source  | SQL database library. Generates standalone SQL database files. |
|  Irig106lib        | Open Source  | Library to aid reading and writing IRIG 106 Chapter 10 data files |
|  SynthCh10Gen      | Custom       | Convert SQLite3 flight database to IRIG 106 Chapter 10 data file. Simulate various events and derived data based on time and aircraft dynamics. |
|                    | Open Source  |  |


## BMtoDB
- Reads standard BlueMax6 output
- Writes flight data to SQLite3 flight data database file
- Required external libraries
  - sqlite3

```
C:\SynthData\Release>BM6toDB.exe
BM6toDB  Sep 21 2020 11:13:35
Write BlueMax6 text output to database file
Usage: BM6toDB <input filename> <output filename>
    <input filename>   Input BlueMax6 text file name
    <output filename>  Output database file name
```

## BM6toXPlaneVideo
- Reads SQLite3 flight data database file
- Sends data to XPlane 11
  - Position and Attitude
  - Throttle
  - Landing Gear Position
- Captures XPlane screens
- Creates and writes video MPEG packets back to SQLite3 database file
- Required external libraries
  - ffmpeg
  - sqlite3

```
C:\SynthData\Release> BM6toXPlaneVideo.exe
BM6toXPlaneVideo  Sep 21 2020 11:46:53
Drive XPlane with BlueMax nav data and record a video
Usage: BM6toXPlaneVideo [flags]
   -d filename  Input database file name
   -t filename  Input text file name
   -D filename  Output database file name
   -T tablename Output database table name
   -M filename  Output MPEG file name
```


## SynthCh10Gen
- Reads SQLite3 flight database file
  - Simulated flight dynamics
  - Generated MPEG video packets
- Writes simulated navigation and sensor data to IRIG 106 Ch 10 data file
- Required external libraries
  - sqlite3
  - irig106lib

```
C:\SynthData\Release> SynthCh10Gen.exe
SynthCh10Gen  Sep 21 2020 11:47:15
Convert a Bluemax simulation file to a Ch 10 1553 nav message file
Usage: SynthCh10Gen  [flags] <output file>
   -v              Verbose
   -p              Data set name
   -s m-d-y-h-m-s  Data start time
   -d filename     Input database file name
   -b filename     Input BlueMax database file name
   -B filename     Input BlueMax text data file name
   -N filename     Input NASA CSV data file name
   <output file>   Output Ch 10 file name
```

# Build directory structure

The Visual Studio solution is configured to expect software components in the following directory structure.

```
\3rdParty\ffmpeg-<version>-win32-dev
\3rdParty\csv-parser
\3rdParty\SQLite
\3rdParty\irig106lib
\BM6toDB
\BM6toXPlaneVideo
\SynthCh10Extract
\SynthCh10Gen
\XPlaneControl
``` 



