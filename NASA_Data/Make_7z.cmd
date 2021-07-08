REM @echo off

REM NASA CSV files take up a lot of space but are very compressable. This command
REM runs one of the python scripts to generate CSV files which are later used by
REM SynthCh10Gen to make Chapter 10 files. The first script parameter is the working
REM directory of NASA Matlab data. All files in the directory are processed. After the 
REM CSV files are generated they are compressed with 7-Zip to save space.
REM
REM This command script can be run by itself to make CSV data for one directory. It 
REM is also run from "To_CSV.cmd" to make data for multiple directories.

echo %1

REM This python script makes a CSV file with enough data to make 1553 Nav data.
python make_nasa_ac_nav.py %1

REM This python script makes a much larger CSV file which includes enough data 
REM to make both 1553 Nav data and PCM frames.
REM python make_nasa_pcm_data.py %1

cd CSV
"\Program Files\7-Zip\7z.exe" a -sdel %1 %1
cd ..
