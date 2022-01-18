# -*- coding: utf-8 -*-
"""
make_nasa_ac_pcm_data.py - A program to make a CSV file with aircraft
performance data suitable for generating interesting PCM.

Created on Tue May 26

@author: BBaggerman
"""

import sys
import os

import nasa_mat

# ---------------------------------------------------------------------------
# Main routine
# -----------------------------------------------------------------------------

# make_nasa_pcm_data.py <data dir> <filename.mat>
#
#   <data dir>     - Matlab data directory to process.
#   <filename.mat> - Specific Matlab file to process. If not specified
#                    then all file in <data dir> are processed.

if __name__=='__main__':

    # Make our NASA Matlab class    
    nm = nasa_mat.NasaMat()
    
    # Setup directories and file names
    root_data_dir  = "./"
    dataset_dir    = None
    data_file_list = None
 
    # If no command line parameters then use these
    if len(sys.argv) < 2:
        dataset_dir = "Tail_652_1/"
        data_file_list = ("652200101120916.mat",)   # File OK
#        dataset_dir = "Tail_652_2/"
#        data_file_list = ("652200108031352.mat",)   # File Broken

    # Get the working directory name
    if len(sys.argv) >= 2:
        dataset_dir = sys.argv[1] + "/"
        
    matlab_data_dir = root_data_dir + "Matlab/" + dataset_dir
    csv_data_dir    = root_data_dir + "CSV/"    + dataset_dir
    
    # Get the file names list
    if len(sys.argv) >= 3:
        data_file_list = { sys.argv[2] }

    # If there is no file names list then make one
    if data_file_list == None:
        data_file_list = os.listdir(matlab_data_dir)

    # Set the output filename extension. Necessary to check if output file
    # has already been generated so we can skip in that case. Don't forget to
    # choose the appropriate output routine further down.
    output_filename_ext = ".csv"
#   output_filename_ext = ".parquet"
#   output_filename_ext = ".h5"

    # SynthCh10Gen uses these fields for PCM
    write_cols = None
    # These are the same as the nav data output
    write_cols = [ "LATP", "LONP", "ALT", "TAS", "TH", "MH", "PTCH", "ROLL", "AOAC", "VRTG", "GS", "IVV", "FPAC" ]
    # Add these additional data columns for PCM
    write_cols.extend(["PLA_1", "PLA_2", "EGT_1", "EGT_2", "OIT_1", "OIT_2", "FF_1", "FF_2", "N1_1", "N1_2", "N2_1", "N2_2", 
                       "VIB_1", "VIB_2", "OIP_1", "OIP_2", "AOA1", "AOA2", "WOW", "LGDN", "LGUP", 
                       "AIL_1", "AIL_2", "ELEV_1", "ELEV_2", "RUDD", 
                       "CWPC", "CWPF", "CCPC", "CCPF", "RUDP", "FLAP"])

    # Iterate over the list of files to (maybe) process
    file_num = 1
    for data_filename in data_file_list:

        # Split the file name into various components
        (data_filename_base, data_filename_ext) = os.path.splitext(data_filename)
        input_data_filename  = matlab_data_dir + data_filename
        output_data_filename = csv_data_dir + data_filename_base + "_pcm.csv"
 
        # Only process if the file is a ".mat"
        if data_filename.endswith(".mat"):

            print("File {0} - {1}".format(file_num, matlab_data_dir + data_filename), end = '')
          # print("File {0} - {1}".format(file_num, matlab_data_dir + data_filename))
            file_num += 1

            # Only process if output file does not exist yet
            if not os.path.isfile(output_data_filename):

                # Read the matlab data and put it into a dictionary
                nm.read_nasa_matlab(input_data_filename, var_names=write_cols)

                # Make a pandas dataframe of flight test data
                nm.make_flight_dataframe()
            
                # If no error making flight data then write it out
                if nm.nasa_frame is not None:
    
                    # First upsample to 100 Hz
                    nm.nasa_frame = nm.nasa_frame.resample("10ms").asfreq()

                    # Fill in missing values
                    interp_keys = set(write_cols) - {'WOW','LGDN','LGUP'}
                    nm.fill_flight_dataframe(interp_keys)
                    
#                    if False:
                        # nm.nasa_frame['LATP'  ].interpolate(inplace=True)
                        # nm.nasa_frame['LONP'  ].interpolate(inplace=True)
                        # nm.nasa_frame['ALT'   ].interpolate(inplace=True)
                        # nm.nasa_frame['PTCH'  ].interpolate(inplace=True)
                        # nm.nasa_frame['ROLL'  ].interpolate(inplace=True)
                        # nm.nasa_frame['TAS'   ].interpolate(inplace=True)
                        # nm.nasa_frame['TH'    ].interpolate(inplace=True)
                        # nm.nasa_frame['MH'    ].interpolate(inplace=True)
                        # nm.nasa_frame['VRTG'  ].interpolate(inplace=True)
                        # nm.nasa_frame['AOAC'  ].interpolate(inplace=True)
                        # nm.nasa_frame['GS'    ].interpolate(inplace=True)
                        # nm.nasa_frame['IVV'   ].interpolate(inplace=True)
                        # nm.nasa_frame['FPAC'  ].interpolate(inplace=True)
    
                        # nm.nasa_frame['EGT_1' ].interpolate(inplace=True)
                        # nm.nasa_frame['EGT_2' ].interpolate(inplace=True)
                        # nm.nasa_frame['OIT_1' ].interpolate(inplace=True)
                        # nm.nasa_frame['OIT_2' ].interpolate(inplace=True)
                        # nm.nasa_frame['FF_1'  ].interpolate(inplace=True)
                        # nm.nasa_frame['FF_2'  ].interpolate(inplace=True)
                        # nm.nasa_frame['N1_1'  ].interpolate(inplace=True)
                        # nm.nasa_frame['N1_2'  ].interpolate(inplace=True)
                        # nm.nasa_frame['VIB_1' ].interpolate(inplace=True)
                        # nm.nasa_frame['VIB_2' ].interpolate(inplace=True)
                        # nm.nasa_frame['OIP_1' ].interpolate(inplace=True)
                        # nm.nasa_frame['OIP_2' ].interpolate(inplace=True)
                        # nm.nasa_frame['AOA1'  ].interpolate(inplace=True)
                        # nm.nasa_frame['AOA2'  ].interpolate(inplace=True)
                        # nm.nasa_frame['WOW'   ].ffill(inplace=True)
                        # nm.nasa_frame['LGDN'  ].ffill(inplace=True)
                        # nm.nasa_frame['LGUP'  ].ffill(inplace=True)
                        # nm.nasa_frame['AIL_1' ].interpolate(inplace=True)
                        # nm.nasa_frame['AIL_2' ].interpolate(inplace=True)
                        # nm.nasa_frame['ELEV_1'].interpolate(inplace=True)
                        # nm.nasa_frame['ELEV_2'].interpolate(inplace=True)
                        # nm.nasa_frame['RUDD'  ].interpolate(inplace=True)
                        # nm.nasa_frame['CWPC'  ].interpolate(inplace=True)
                        # nm.nasa_frame['CWPF'  ].interpolate(inplace=True)
                        # nm.nasa_frame['CCPC'  ].interpolate(inplace=True)
                        # nm.nasa_frame['CCPF'  ].interpolate(inplace=True)
                        # nm.nasa_frame['RUDP'  ].interpolate(inplace=True)
                        # nm.nasa_frame['FLAP'  ].interpolate(inplace=True)
    
                        # Now downsample to 25 Hz
    #                    nm.nasa_frame = nm.nasa_frame.resample("40ms").asfreq()

                    # Make sure destination directory exists
                    if not os.path.isdir(csv_data_dir):
                        os.mkdir(csv_data_dir)
                    
                    # Rearrange the columns and write it out
                    print(" - write {0}".format(output_data_filename))
                    output_frame = nm.nasa_frame[write_cols]
                    output_frame.to_csv(output_data_filename,index_label="DATE_TIME")
#                    nm.nasa_frame.to_csv(output_data_filename,index_label="DATE_TIME", columns=write_cols)
#                   nm.nasa_frame.to_parquet(data_filename_root+output_filename_ext)
#                   nm.nasa_frame.to_hdf(data_filename_root+output_filename_ext, "data_"+data_filename_base, mode="w", complevel=1)
            
                # There was an error converting
                else:
                    print(" - error making dataframe")
                
            # Input file skipped
            else:
                print(" - skipped")

    print("Done!")