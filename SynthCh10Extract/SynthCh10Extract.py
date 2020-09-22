'''
SynthCh10Extract - A program to extract data fields from synthetically generated
    Chapter 10 data.

Created on Wed Sep 10 10:30:00 2020

@author: Bob Baggerman
'''

# pip3 install progress
# from progress.bar import Bar

import sys
#import ctypes

import Py106
import Py106.Packet
import Py106.Status
import Py106.Time
import Py106.MsgDecode1553

import pandas as pd
import pyarrow as pa
import pyarrow.parquet as pq

# ---------------------------------------------------------------------------

# Synthetic IRIG 106 Ch 10 - Format 1
# Channel IDs
CH_ID_TIME          =  1
CH_ID_VIDEO_HUD     = 10
CH_ID_VIDEO_COCKPIT = 11
CH_ID_VIDEO_CHASE   = 12
CH_ID_PCM           = 20
CH_ID_1553_NAV      = 30

# 1553 RTs
RT_NAV              =  6

# 1553 Messages
NAV_MSG_RT          = RT_NAV
NAV_MSG_TR          =  1
NAV_MSG_SA          = 29


# ---------------------------------------------------------------------------
# Nav Data
# ---------------------------------------------------------------------------

class AircraftNav:
    def __init__(self):
        self.nav_time  = 0
        self.latitude  = 0
        self.longitude = 0
        self.altitude  = 0
        self.roll      = 0
        self.pitch     = 0
        self.heading   = 0

        self.nav_time_list  = []
        self.latitude_list  = []
        self.longitude_list = []
        self.altitude_list  = []
        self.roll_list      = []
        self.pitch_list     = []
        self.heading_list   = []
        
    def append_to_list(self):
        self.nav_time_list.append(self.nav_time)
        self.latitude_list.append(self.latitude)
        self.longitude_list.append(self.longitude)
        self.altitude_list.append(self.altitude)
        self.roll_list.append(self.roll)
        self.pitch_list.append(self.pitch)
        self.heading_list.append(self.heading)
        
    def make_data_frame(self):
        data_cols = {"Time"     :self.nav_time_list,    \
                     "Latitude" :self.latitude_list,    \
                     "Longitude":self.longitude_list,   \
                     "Altitude" :self.altitude_list,    \
                     "Roll"     :self.roll_list,        \
                     "Pitch"    :self.pitch_list,       \
                     "Heading"  :self.heading_list}
        self.data_frame = pd.DataFrame(data_cols)

    def make_arrow(self):
        self.arrow_table = pa.Table.from_pandas(self.data_frame)
        
    def print(self):
        print("{},{:.3f},{:.3f},{:5.0f},{:5.1f},{:5.1f},{:3.0f}".format(    \
            self.nav_time,          \
            self.latitude,          \
            self.longitude,         \
            self.altitude,          \
            self.roll,              \
            self.pitch,             \
            self.heading))

# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

# Normalize angle from -180 to +180
def angle_180(angle):
    while (angle > 180.0):
        angle -= 360.0
    while (angle < -180.0):
        angle += 360.0
    return angle


# Normalize angle from 0 to 360
def angle_360(angle):
    while (angle > 360.0):
        angle -= 360.0
    while (angle < 0.0):
        angle += 360.0
    return angle


# ---------------------------------------------------------------------------

print()
print("Extract data from a synthetically generated Chapter 10 file")
print()

# Make IRIG 106 library classes
pkt_io      = Py106.Packet.IO()
time_utils  = Py106.Time.Time(pkt_io)
decode_1553 = Py106.MsgDecode1553.Decode1553F1(pkt_io)

# First command line argument is file name
if len(sys.argv) > 1 :
    ret_status = pkt_io.open(sys.argv[1], Py106.Packet.FileMode.READ)
    if ret_status != Py106.Status.OK :
        print ("Error opening data file %s" % (sys.argv[1]))
        sys.exit(1)
else :
    print ("Usage : SynthCh10Extract.py <filename>")
    sys.exit(1)

# Synchronize the RTC to clock time
ret_status = time_utils.SyncTime(False, 0)
if ret_status != Py106.Status.OK:
    print ("Sync Status = %s" % Py106.Status.Message(ret_status))
    sys.exit(1)

# Initialize some variables
TR = ("R", "T")
packet_count = 0
dec_count    = 0
aircraft_nav = AircraftNav()


# Read all the packets
for packet_header in pkt_io.packet_headers():
#   print("Ch ID {}  Type {}".format(packet_header.ChID, packet_header.DataType))
    packet_count += 1
    
    # Handle 1553 Nav messages
    if (packet_header.ChID     == CH_ID_1553_NAV                     ) and \
       (packet_header.DataType == Py106.Packet.DataType.MIL1553_FMT_1):

        # Read the 1553 packet data
        pkt_io.read_data()
           
        # Read the individual 1553 messages
        msg_count = 0
        for msg_1553 in decode_1553.msgs():
#                TimeUtils.RelInt2IrigTime()

            # Handle the high rate RT to RT nav message
            if (msg_1553.p1553Hdr.contents.Field.BlockStatus.RT2RT == 0         ) and \
               (msg_1553.pCmdWord1.contents.Field.RTAddr           == NAV_MSG_RT) and \
               (msg_1553.pCmdWord1.contents.Field.TR               == NAV_MSG_TR) and \
               (msg_1553.pCmdWord1.contents.Field.SubAddr          == NAV_MSG_SA) :
            
                word_cnt = decode_1553.word_cnt(msg_1553.pCmdWord1.contents.Value)
                msg_time = time_utils.RelInt2IrigTime(msg_1553.p1553Hdr.contents.Field.PktTime)

                if (msg_1553.p1553Hdr.contents.Field.BlockStatus.MsgError == 0) and (word_cnt >= 25):
                    aircraft_nav.nav_time  = msg_time.time
                    aircraft_nav.latitude  = angle_180(((msg_1553.pData.contents[20] << 16) | msg_1553.pData.contents[21]) * 180.0 / 2**31)
                    aircraft_nav.longitude = angle_180(((msg_1553.pData.contents[22] << 16) | msg_1553.pData.contents[23]) * 180.0 / 2**31)
                    aircraft_nav.altitude  = msg_1553.pData.contents[24] * 4.0;
                    aircraft_nav.roll      = angle_180(msg_1553.pData.contents[9]  * 180.0 / 2**15);
                    aircraft_nav.pitch     = angle_180(msg_1553.pData.contents[10] * 180.0 / 2**15);
                    aircraft_nav.heading   = angle_360(msg_1553.pData.contents[11] * 180.0 / 2**15);
                    aircraft_nav.append_to_list()
                    if (dec_count == 0):
                        aircraft_nav.print()
                        dec_count = 10
                    else:
                        dec_count -= 1

# Make a pandas DataFrame from the aircraft nav data
print("Make pandas DataFrame")
aircraft_nav.make_data_frame()

# Make an pyarrow Table from the pandas DataFrame
print("Make Apache Arrow table")
aircraft_nav.make_arrow()

# Write the pyarrow Table to an Apache Parquet file
print("Make Arrow table to Parquet file")
pq.write_table(aircraft_nav.arrow_table, "example.parquet")

pkt_io.close()

#print ("1553 Packets = %d" % packet_count)
print("Success!")
