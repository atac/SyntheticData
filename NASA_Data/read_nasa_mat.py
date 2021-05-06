# -*- coding: utf-8 -*-
"""
Created on Wed Sep  2 20:18:23 2020

@author: Bob Baggerman
"""

import sys
import os

#import numexpr
from scipy.io import loadmat
import pandas as pd

# -----------------------------------------------------------------------------

def read_nasa_matlab(matlab_filename):
    """
    Read NASA data in matlab format and return it in a python dictionary.
    The layout of the data and various attributes are a bit involved so use
    the var_*() functions below for easy access.
    """

    # Load the Matlab data file
    nasa_mat = loadmat(matlab_filename)

    # Get rid of some unused data columns
    del nasa_mat['__header__']
    del nasa_mat['__version__']
    del nasa_mat['__globals__']
    del nasa_mat['VAR_1107']    # 1107    SYNC WORD FOR SUBFRAME 1
    del nasa_mat['VAR_2670']    # 2670    SYNC WORD FOR SUBFRAME 2
    del nasa_mat['VAR_5107']    # 5107    SYNC WORD FOR SUBFRAME 3
    del nasa_mat['VAR_6670']    # 6670    SYNC WORD FOR SUBFRAME 4
#   del nasa_mat['DVER_1']      # DVER.1  DATABASE ID VERSION CHAR 1
#   del nasa_mat['DVER_2']      # DVER.2  DATABASE ID VERSION CHAR 2
    return nasa_mat

# -----------------------------------------------------------------------------

# Matlab variable access routines

def var_name(nasa_var):
    return nasa_var['Alpha'][0][0][0]

def var_desc(nasa_var):
    return nasa_var['Description'][0][0][0]

def var_units(nasa_var):
    if (len(nasa_var['Units'][0][0]) > 0) :
        return nasa_var['Units'][0][0][0]
    else :
        return ""

def var_rate(nasa_var):
    return nasa_var['Rate'][0][0][0][0]

def var_value(nasa_var, index):
    return nasa_var['data'][0][0][index][0]
    
def var_data_array_np(nasa_var):
    return nasa_var['data'][0][0][:,0]

def print_point(nasa_var, data_point):
    data_value = nasa_mat[nasa_var]['data'][0][0][data_point][0]
    data_rate  = nasa_mat[nasa_var]['Rate'][0][0][0][0]
    if (len(nasa_mat[nasa_var]['Units'][0][0]) > 0) :
        data_units = nasa_mat[nasa_var]['Units'][0][0][0]
    else :
        data_units = ""

    data_desc  = nasa_mat[nasa_var]['Description'][0][0][0]
    data_name  = nasa_mat[nasa_var]['Alpha'][0][0][0]

    print("'{0}' - {1} - {2} Hz".format(data_name, data_desc, data_rate))
    print("{0} {1}".format(data_value, data_units))

# -----------------------------------------------------------------------------

def make_time_series(nasa_var):
    """ Make a pandas time series of data from the matlab data variable passed in """
    
    # Get the starting date/time
    starting_date_time = "{0}-{1:0>2}-{2:0>2} {3:0>2}:{4:0>2}:{5:0>2}".format( \
            var_value(nasa_mat['DATE_YEAR'],0), var_value(nasa_mat['DATE_MONTH'],0), var_value(nasa_mat['DATE_DAY'],0), \
            var_value(nasa_mat['GMT_HOUR'],0),  var_value(nasa_mat['GMT_MINUTE'],0), var_value(nasa_mat['GMT_SEC'],0))
    
    data_period = "{0}L".format(1000 / var_rate(nasa_var))
        
    data_array   = var_data_array_np(nasa_var)
    data_idx     = pd.date_range(start=starting_date_time, periods=data_array.size, freq=data_period)
    data_series  = pd.Series(data_array, index=data_idx)
    
    # If the sample rate is higher than 4 Hz resample down
    # Name  Rate Description
    # BLAC   16  BODY LONGITUDINAL ACCELERATION
    # CTAC   16  CROSS TRACK ACCELERATION
    # FPAC   16  FLIGHT PATH ACCELERATION
    # IVV    16  INERTIAL VERTICAL SPEED LSP
    # PTCH    8   PITCH ANGLE LSP
    # RALT    8   RADIO ALTITUDE LSP
    # ROLL    8   ROLL ANGLE LSP
    # VRTG    8   VERTICAL ACCELERATION
    if var_rate(nasa_var) > 4:
        data_series = data_series.resample('250ms').mean()
        
    return data_series


# -----------------------------------------------------------------------------

def data_row_valid(data_frame):
    """ 
    Return a pandas series of bools that indicate validity of a row of data 
    from the pandas data frame passed in.
    """
    
    # These are some heuristic santity checks. These are often bad with bad data.
    valid_series = \
        (data_frame['GMT_HOUR']   <  24) & (data_frame['GMT_HOUR']   >=    0) & \
        (data_frame['GMT_MINUTE'] <  60) & (data_frame['GMT_MINUTE'] >=    0) & \
        (data_frame['GMT_SEC']    <  60) & (data_frame['GMT_SEC']    >=    0) & \
        (data_frame['LATP']       <  60) & (data_frame['LATP']       >    20) & \
        (data_frame['LONP']       < -20) & (data_frame['LONP']       >  -120)
        
    return valid_series

        
# -----------------------------------------------------------------------------

def make_flight_dataframe(nasa_mat, trimmed=True):
    """ Take a matlab object of NASA data and return a pandas dataframe object
        of the same data. If trimmed is true only a range of validated data
        is returned.
    """

    # Any error will return a null frame object
    try:

        # Make a Series for each column of NASA data and put them in a list
        nasa_series = {}
        for key in nasa_mat:
    #        print(key, " ", end = '')
            nasa_series[key] = make_time_series(nasa_mat[key])
    #    print()
        
        # Turn the list of Series into a DataFrame
        nasa_frame = pd.DataFrame(nasa_series)
    
        # Find the limits of DataFrame rows that don't pass the sniff test
        valid_series = data_row_valid(nasa_frame)
        first_valid_data_timestamp = nasa_frame[valid_series].first_valid_index()
        last_valid_data_timestamp  = nasa_frame[valid_series].last_valid_index()
    
        # Fill in missing data between the first and last valid data points. 
        # Lat and Lon are interpolated. Everything else is forward fill.
        nasa_frame['LATP'] = nasa_frame.loc[first_valid_data_timestamp:last_valid_data_timestamp,'LATP'].interpolate()
        nasa_frame['LONP'] = nasa_frame.loc[first_valid_data_timestamp:last_valid_data_timestamp,'LONP'].interpolate()
        nasa_frame.ffill(inplace=True)
    
        if (trimmed):
            return nasa_frame[first_valid_data_timestamp:last_valid_data_timestamp]
        else:
            return nasa_frame

    # Error so return a null object
    except:
        return None
    
# ---------------------------------------------------------------------------
# Main routine
# -----------------------------------------------------------------------------

# read_nasa_mat.py <data dir> <filename.mat>
#
#   <data dir>     - Matlab data directory to process.
#   <filename.mat> - Specific Matlab file to process. If not specified
#                    then all file in <data dir> are processed.

if __name__=='__main__':

    # Setup directories and file names
    root_data_dir  = "./"
    dataset_dir    = None
    data_file_list = None
 
    # If no command line parameters then use these
    if len(sys.argv) < 2:
        dataset_dir = "Tail_652_1/"
        data_file_list = ("652200101120916.mat",)   # File OK
#        data_file_list = ("652200108031352.mat",)   # File Broken

    # Get the working directory name
    if len(sys.argv) >= 2:
        dataset_dir = sys.argv[1] + "/"
        
    matlab_data_dir = root_data_dir + "Matlab/" + dataset_dir
    csv_data_dir    = root_data_dir + "CSV/"    + dataset_dir
    
    # Get the file names list
    if len(sys.argv) >= 3:
        data_file_list = sys.argv[2]

    # If there is no file names list then make one
    if data_file_list == None:
        data_file_list = os.listdir(matlab_data_dir)

    # Set the output filename extension. Necessary to check if output file
    # has already been generated so we can skip in that case. Don't forget to
    # choose the appropriate output routine further down.
#   output_filename_ext = ".csv"
#   output_filename_ext = ".parquet"
#   output_filename_ext = ".h5"

    # Iterate over the list of files to (maybe) process
    file_num = 1
    for data_filename in data_file_list:

        # Split the file name into various components
        (data_filename_base, data_filename_ext) = os.path.splitext(data_filename)
#        data_filename_root = matlab_data_dir + data_filename_base
        input_data_filename  = matlab_data_dir + data_filename
        output_data_filename = csv_data_dir + data_filename_base + ".csv"
 
        # Only process if the file is a ".mat"
        if data_filename.endswith(".mat"):

            print("File {0} - {1}".format(file_num, matlab_data_dir + data_filename), end = '')
            file_num += 1

            # Only process if output file does not exist yet
            if not os.path.isfile(output_data_filename):

                # Read the matlab data and put it into a dictionary
                nasa_mat = read_nasa_matlab(input_data_filename)
    
                # Make a pandas dataframe of flight test data
                nasa_frame = make_flight_dataframe(nasa_mat)
                
                # If no error making flight data then write it out
                if nasa_frame is not None:
    
                    # Plot the flight path
#                   nasa_frame.plot(x='LONP', y='LATP')
            
                    # Make sure destination directory exists
                    if not os.path.isdir(csv_data_dir):
                        os.mkdir(csv_data_dir)
                    
                    # Write it out
                    print(" - write {0}".format(output_data_filename))
                    nasa_frame.to_csv(output_data_filename,index_label="DATE_TIME")
#                   nasa_frame.to_parquet(data_filename_root+output_filename_ext)
#                   nasa_frame.to_hdf(data_filename_root+output_filename_ext, "data_"+data_filename_base, mode="w", complevel=1)
                    
                # There was an error converting
                else:
                    print(" - error making dataframe")
                
            # Input file skipped
            else:
                print(" - skipped")

    print("Done!")