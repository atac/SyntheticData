# -*- coding: utf-8 -*-
"""
Created on Wed Apr  7 16:03:22 2021

@author: BBaggerman
"""

import os
import sys
import math
import csv
import datetime
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


# ---------------------------------------------------------------------------
# Utility Routines
# ---------------------------------------------------------------------------

# Convert a TimeUTC format string into milliseconds since midnight
def make_utc_from_str(utc_str):
    (utc_hours, utc_minutes, utc_seconds) = utc_str.split(":")
    utc_msec = (int(utc_hours) * 3600 + int(utc_minutes) * 60 + int(utc_seconds)) * 1000
    return utc_msec



# ---------------------------------------------------------------------------
# NASA CSV Data Routines
# ---------------------------------------------------------------------------

def read_nasa_csv(nasa_filename):
    df = pd.read_csv(nasa_filename, delimiter=',')

    # Probably need to convert DATETIME to a pandas index

    return df


def read_nasa_csv2(nasa_filename):

    # Read the CSV file
    # -----------------

    nasa_data_array = []
    with open(nasa_filename, newline='') as nasa_data:
        nasa_reader = csv.DictReader(nasa_data)
        nasa_reader.__next__()
        try:
            for nasa_row in nasa_reader:
                
                # Convert strings to numbers
                # if convert_v2_row(v2_row) == False:
                #     print("Format error in {}, line {}".format(v2_filename, v2_reader.line_num))
                #     continue
                
                # Dont' store if weight on wheels is false
                # if nasa_row["vnGPSFix"] == "0":
                #     continue

                # We got to here so store the data                
                nasa_data_array.append(nasa_row)
        
        # Catch any other read errors
        except csv.Error as e:
            sys.exit('file {}, line {}: {}'.format(nasa_filename, nasa_reader.line_num, e))

    # Make a time index value for each row
    # ------------------------------------

   # Make a UTC Time value to use as an index
    array_idx = 0
    index_time = []
    while array_idx < len(nasa_data_array):
        # Make values for the data timestamp and UTC time
        # data_timestamp = int(v2_data_array[array_idx]["timeStamp"])

        # Calculate and store a time index value which is milliseconds since midnight
        data_time_utc  = mid_time_utc + (data_timestamp - mid_timestamp)
        data_time_utc  = round(float(data_time_utc) / 20.0) * 20
        index_time.append(data_time_utc)

        array_idx += 1

    # Make a pandas dataframe of flight test data
    # -------------------------------------------
    nasa_dataframe = pd.DataFrame(nasa_data_array, index_time)
    
    return nasa_dataframe



# ---------------------------------------------------------------------------

def make_city_pair_kml(city_pairs):

    kml_file = open("NASA_City_Pairs.kml","w")

    # Top part of KML
    kml_file.write(
        '<kml xmlns="http://www.opengis.net/kml/2.2"\n'
        '     xmlns:gx="http://www.google.com/kml/ext/2.2">\n'
        '  <Document>\n'
        '    <Style id="OrangeLineOrangePoly">\n'
        '      <LineStyle>\n'
        '        <color>ff00a5ff</color>\n'
        '        <width>2</width>\n'
        '      </LineStyle>\n'
        '      <PolyStyle>\n'
        '        <color>ff008cff</color>\n'
        '      </PolyStyle>\n'
        '    </Style>\n')

    # City pair string KML
    for city_pair in city_pairs:
        (filename, start_lat, start_lon, stop_lat, stop_lon) = city_pair
        # print("{1},{2}\n{3},{4}\n".format(start_lon, start_lat, stop_lon, stop_lat))
        kml_file.write(
            '    <Placemark>\n'
            '      <name>{0}</name>\n'
            '      <styleUrl>#OrangeLineOrangePoly</styleUrl>\n'
            '      <LineString>\n'
            '        <altitudeMode>clampToGround</altitudeMode>\n'
            '        <coordinates>\n'
            '          {1},{2}\n'
            '          {3},{4}\n'
            '        </coordinates>\n'
            '      </LineString>\n'
            '    </Placemark>\n'.format(filename, start_lon, start_lat, stop_lon, stop_lat))

    # End part of KML
    kml_file.write(
        '  </Document>\n'
        '</kml>')
            
    kml_file.close()
    
    return


# =============================================================================
# Main routine
# =============================================================================

if __name__=='__main__':

    # Load the NASA CSV data file(s)

    root_data_dir   = "./"
    dataset_dir     = "Tail_652_1/"
    csv_data_dir    = root_data_dir + "CSV/"    + dataset_dir

#    data_file_list = ("652200101120916.csv",)
    data_file_list = os.listdir(csv_data_dir)

    # Iterate over the list of files to (maybe) process
    file_num = 1
    city_pairs = []
    for data_filename in data_file_list:

        # Only process if the file is a ".csv"
        if data_filename.endswith(".csv"):

            print("File {0} - {1}".format(file_num, csv_data_dir + data_filename), end = '')
            file_num += 1

            # Read the CSV file
            nasa_df = read_nasa_csv(csv_data_dir + data_filename)
    
            # Make a pandas dataframe of flight test data
            # nasa_frame = make_flight_dataframe(nasa_mat)

            # Find start / stop points
            nasa_df_wow = nasa_df[nasa_df.WOW == 1.0]
            if nasa_df_wow.size > 0:
                start_lat = nasa_df_wow["LATP"].array[0]
                start_lon = nasa_df_wow["LONP"].array[0]
                stop_lat  = nasa_df_wow["LATP"].array[-1]
                stop_lon  = nasa_df_wow["LONP"].array[-1]
                print(" - Done")

                (filename, ext) = data_filename.split('.')
                city_pairs.append((filename, start_lat, start_lon, stop_lat, stop_lon))
            else:
                print(" - None")
                
            # Input file skipped
        # else:
        #     print(" - Skipped")

        # if file_num > 10:
        #     break

    make_city_pair_kml(city_pairs)
    
    print("Done!")