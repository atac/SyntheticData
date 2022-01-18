@echo off

REM Make_Ch10.cmd <Dataset Dir> <Data filename>
REM where
REM   <Dataset Dir>   - Name of the directory of a set of NASA data files. E.g. Tail_652_1
REM   <Data filename> - Name of an individual NASA Matlab datafile. E.g. 652200101120916
REM
REM Example - Make_Ch10 Tail_652_1 652200101120916

echo %1  %2

REM Convert NASA Matlab file into CSV data file
python make_nasa_pcm_data.py %1 %2.mat

REM Convert CSV file into Ch 10 file
..\Release\SynthCh10Gen-NASA -v -N CSV\%1\%2_pcm.csv Ch10\%1\%2.ch10



