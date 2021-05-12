# -*- coding: utf-8 -*-
"""
A module to handle reading and interpreting NASA Matlab flight data
recordings. More details from NASA here...

https://c3.nasa.gov/dashlink/projects/85/

Created on Mon May 10 14:58:44 2021

@author: BBaggerman
"""

#import sys
#import os
import datetime

#import numexpr
from scipy.io import loadmat
import pandas as pd


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
    
def var_array_len(nasa_var):
    return nasa_var['data'][0][0][:,0].size
    
def var_data_array_np(nasa_var):
    return nasa_var['data'][0][0][:,0]
    

# -----------------------------------------------------------------------------
# NasaMat
# -----------------------------------------------------------------------------

class NasaMat:
    """Read and manipulate NASA data files"""

    def __init__(self):
        self.nasa_mat        = None
        self.nasa_frame      = None
        self.start_date_time = None

# -----------------------------------------------------------------------------

    def var_name(self, key):
        return var_name(self.nasa_mat[key])
    
    def var_desc(self, key):
        return var_desc(self.nasa_mat[key])
    
    def var_units(self, key):
        return var_units(self.nasa_mat[key])
    
    def var_rate(self, key):
        return var_rate(self.nasa_mat[key])
    
    def var_value(self, key, index):
        return var_value(self.nasa_mat[key], index)
        
    def var_array_len(self, key):
        return var_array_len(self.nasa_mat[key])
    
    def var_data_array_np(self, key):
        return var_data_array_np(self.nasa_mat[key])
        
# -----------------------------------------------------------------------------

    def read_nasa_matlab(self, matlab_filename, var_names=None):
        """
        Read NASA data in matlab format and return it in a python dictionary.
        The layout of the data and various attributes are a bit involved so use
        the var_*() functions below for easy access.
        """
        # If we are not getting all Matlab variables then make a list of
        # what to get.
        var_names_read = None
        if var_names != None:
            var_names_read = {'GMT_HOUR', 'GMT_MINUTE', 'GMT_SEC',
                              'DATE_YEAR', 'DATE_MONTH', 'DATE_DAY',
                              'LATP', 'LONP'}
            var_names_read.update(var_names)
            
       # Load the Matlab data file
        self.nasa_mat = loadmat(matlab_filename, variable_names=var_names_read)
    
        # Get rid of some unused data columns
        if '__header__' in self.nasa_mat:
            del self.nasa_mat['__header__']
        if '__version__' in self.nasa_mat:
            del self.nasa_mat['__version__']
        if '__globals__' in self.nasa_mat:
            del self.nasa_mat['__globals__']
        if 'VAR_1107' in self.nasa_mat:
            del self.nasa_mat['VAR_1107']    # 1107    SYNC WORD FOR SUBFRAME 1
        if 'VAR_2670' in self.nasa_mat:
            del self.nasa_mat['VAR_2670']    # 2670    SYNC WORD FOR SUBFRAME 2
        if 'VAR_5107' in self.nasa_mat:
            del self.nasa_mat['VAR_5107']    # 5107    SYNC WORD FOR SUBFRAME 3
        if 'VAR_6670' in self.nasa_mat:
            del self.nasa_mat['VAR_6670']    # 6670    SYNC WORD FOR SUBFRAME 4
    
        # Find the start time of the data
        self.start_date_time = self.find_start_time()
        
# -----------------------------------------------------------------------------

    def print_point(self, key, index):
        data_value = self.nasa_mat[key]['data'][0][0][index][0]
        data_rate  = self.nasa_mat[key]['Rate'][0][0][0][0]
        if (len(self.nasa_mat[key]['Units'][0][0]) > 0) :
            data_units = self.nasa_mat[key]['Units'][0][0][0]
        else :
            data_units = ""
    
        data_desc  = self.nasa_mat[key]['Description'][0][0][0]
        data_name  = self.nasa_mat[key]['Alpha'][0][0][0]
    
        print("'{0}' - {1} - {2} Hz".format(data_name, data_desc, data_rate))
        print("{0} {1}".format(data_value, data_units))

# -----------------------------------------------------------------------------

    def make_time(self, time_index):
        """ Make a time string from the time fields """
        return "{0:0>2}:{1:0>2}:{2:0>2}".format(               \
            var_value(self.nasa_mat['GMT_HOUR'],  time_index), \
            var_value(self.nasa_mat['GMT_MINUTE'],time_index), \
            var_value(self.nasa_mat['GMT_SEC'],   time_index))

# -----------------------------------------------------------------------------

    def make_date(self, date_index):
        """ Make a date string from the data date fields """
        return "{0}-{1:0>2}-{2:0>2}".format(                   \
            var_value(self.nasa_mat['DATE_YEAR'], date_index), \
            var_value(self.nasa_mat['DATE_MONTH'],date_index), \
            var_value(self.nasa_mat['DATE_DAY'],  date_index))

# -----------------------------------------------------------------------------

    def make_date_time(self, date_index):
        """ Make a date/time string from the data date and time fields """

        # Time is recorded at a rate 8 times the date rate
        time_index = date_index * 8
        
        return self.make_date(date_index) + " " + self.make_time(time_index)
       
# -----------------------------------------------------------------------------

    def find_start_time(self):
        """ Sometimes time is jacked up at the beginning. Find the first reasonable
            time and then figure out what the data start time must have been. """
            
        # The original way
#        return self.make_date_time(0)
            
        # For now just get time out of the middle of the data and hope for the best
        middle_date_index = int(self.var_array_len('DATE_YEAR') / 2)
        middle_time       = datetime.datetime.fromisoformat(self.make_date_time(middle_date_index))
        start_time_offset = datetime.timedelta(seconds = middle_date_index * 4)
        start_time        = middle_time - start_time_offset
        return  start_time.isoformat(sep=" ")
        
# -----------------------------------------------------------------------------

    def make_time_series(self, key):
        """ Make a pandas time series of data from the matlab data variable passed in """
        
        data_period = "{0}L".format(1000 / var_rate(self.nasa_mat[key]))

        data_array   = var_data_array_np(self.nasa_mat[key])
        data_idx     = pd.date_range(start=self.start_date_time, periods=data_array.size, freq=data_period)
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
        if var_rate(self.nasa_mat[key]) > 4:
            data_series = data_series.resample('250ms').mean()

        return data_series
    

# -----------------------------------------------------------------------------

    def data_row_valid(self, data_frame):
        """ 
        Return a pandas series of bools that indicate validity of a row of data 
        from the pandas data frame passed in.
        """
        
        # These are some heuristic santity checks. These are often bad with bad data.
        valid_series = \
            (data_frame['GMT_HOUR']   <   24) & (data_frame['GMT_HOUR']   >=    0) & \
            (data_frame['GMT_MINUTE'] <   60) & (data_frame['GMT_MINUTE'] >=    0) & \
            (data_frame['GMT_SEC']    <   60) & (data_frame['GMT_SEC']    >=    0) & \
            (data_frame['DATE_YEAR']  < 2020) & (data_frame['DATE_YEAR']  >= 2000) & \
            (data_frame['LATP']       <   60) & (data_frame['LATP']       >    20) & \
            (data_frame['LONP']       <  -20) & (data_frame['LONP']       >  -120)
            
        return valid_series

        
# -----------------------------------------------------------------------------

    def make_flight_dataframe(self, trimmed=True):
        """ Take the matlab object of NASA data and turn it into a pandas dataframe 
            object of the same data. If trimmed is true only a range of validated 
            data is returned.
        """
    
        # Any error will return a null frame object
        try:

            # Make a Series for each column of NASA data and put them in a list
            nasa_series = {}
            for key in self.nasa_mat:
                nasa_series[key] = self.make_time_series(key)

            # Turn the list of Series into a DataFrame
            self.nasa_frame = pd.DataFrame(nasa_series)
        
            # Find the limits of DataFrame rows that don't pass the sniff test
            valid_series = self.data_row_valid(self.nasa_frame)
            first_valid_data_timestamp = self.nasa_frame[valid_series].first_valid_index()
            last_valid_data_timestamp  = self.nasa_frame[valid_series].last_valid_index()

            # Fill in missing data between the first and last valid data points. 
            # Lat and Lon are interpolated. Everything else is forward fill.
            self.nasa_frame['LATP'] = self.nasa_frame.loc[first_valid_data_timestamp:last_valid_data_timestamp,'LATP'].interpolate()
            self.nasa_frame['LONP'] = self.nasa_frame.loc[first_valid_data_timestamp:last_valid_data_timestamp,'LONP'].interpolate()
            self.nasa_frame.ffill(inplace=True)

            if (trimmed):
                self.nasa_frame = self.nasa_frame[first_valid_data_timestamp:last_valid_data_timestamp]
    
        # Error so return a null object
        except:
             self.nasa_frame = None
    