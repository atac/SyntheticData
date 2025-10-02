# IRIG 106 Synthetic Data Generation Tools

These software tools are used to generate IRIG 106 Chapter 10 flight test data files from synthesized flight and sensor data. 
Generating synthetic Ch 10 data is a Multi-Step process. COTS, GOTS, and Custom Applications are used. This repository contains the custom
software tools which, when used with the described GOTS and COTS sofware, will generate 


Current Chapter 10 Data Types Generated
- Time
- PCM
- ARINC-429
- MIL-STD-1553
- Video (Cockpit, HUD, Chase view)


|  Software          | License      | Description  |
| ------------------ | ------------ | ------------ |
|  BlueMax7          | GOTS         | Aircraft Flight Dynamics, Flight Path Generator, Maneuver, Mission and Aero-Performance Evaluation Model. Used to generate simulated flight path data. |
|  BM6oDB            | Custom       | Convert BlueMax7 output to SQLite3 flight database. |
|  BM6toXPlaneVideo  | Custom       | Used to add MPEG video data to SQLite3 flight database |
|  csv-parser        | Open Source  | Library to easily read and parse CSV files |
|  ffmpeg            | Open Source  | Library for MPEG video encoding |
|  XPlane 12         | COTS         | Flight simulator, used to generate realistic flight imagery for video generation |
|  SQLite3           | Open Source  | SQL database library. Generates standalone SQL database files. |
|  Irig106lib        | Open Source  | Library to aid reading and writing IRIG 106 Chapter 10 data files |
|  SynthCh10Gen      | Custom       | Convert SQLite3 DB or text-based flight data to IRIG 106 Chapter 10 data file. |


## BM6toDB
- Reads standard BlueMax7 output
- Writes flight data to SQLite3 flight data database file
- Required external libraries
  - sqlite3
  - csv-parser

```
C:\SynthData\Release>BM6toDB.exe
BM6toDB  Oct  2 2025 17:50:51
Write BlueMax7 text output to database file
Usage: BM6toDB <input filename> <output filename>
   <input filename>   Input BlueMax7 text file name
   <output filename>  Output database file name
```

## BM6toXPlaneVideo
- Reads SQLite3 flight data database file
- Sends data to XPlane 12
  - Position and Attitude
  - Throttle
  - Landing Gear Position
- Captures XPlane screens
- Creates and writes video MPEG packets back to SQLite3 database file
- Required external libraries
  - csv-parser
  - ffmpeg
  - sqlite3

```
C:\SynthData\Release>BM6toXPlaneVideo.exe
BM6toXPlaneVideo  Oct  2 2025 17:51:50
Drive XPlane with BlueMax nav data and record a video
Usage: BM6toXPlaneVideo [flags]
   -v           Verbose output (disables console progress bar)
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
  - csv-parser
  - sqlite3
  - irig106lib

```
C:\SynthData\Release>SynthCh10Gen.exe
usage: SynthCh10Gen [options] <config>

  <config>:
    Path to a configuration file that defines data sources and output format.
    See "Configuration Specification.md" for details.

  [options]:
    --help          Show this help string
    --validate      Show configuration validation information
```

# Build directory structure

The Visual Studio solution is configured to expect software components in the following directory structure.

```
\3rdParty\atac
\3rdParty\csv-parser
\3rdParty\ffmpeg
\3rdParty\irig106lib
\3rdParty\json
\3rdParty\sqlite
\BM6toDB
\BM6toXPlaneVideo
\SynthCh10Extract
\SynthCh10Gen
\XPlaneControl
``` 



