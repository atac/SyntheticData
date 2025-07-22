// BM6toXPlaneVideo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// -t ..\BlueMax6\Output\A-10_China_Lake_ILS__UDO.txt
// -t ..\BlueMax6\Output\A-10__China_Lake__Echo_Range___UDO.txt
// -d ..\BMtoDB\A-10_test.sql
// -M A-10_China_Lake_ILS_External_HUD.mpg
// -M A-10_China_Lake_ILS_External_HUD.mpg
// -d ..\OutputData\A-10_China_Lake_ILS.sql -D ..\OutputData\A-10_China_Lake_ILS_video.sql -T Video_HUD


#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include <iostream>
#include <cstdio>
#include <cassert>
#include <ctime>
#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream

#if defined(__GNUC__)
#define SOCKET            int
#define INVALID_SOCKET    -1
#define SOCKET_ERROR      -1
#define SOCKADDR          struct sockaddr
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#endif

#include <sqlite3.h>

#include "XPlaneControl.h"
#include "DXGIManager.hpp"
#include "MpegEncoder.h"

#include "SimState.h"
#include "Source_BMNavTxt.h"
#include "Source_BMNavDB.h"
#include "SimTimer.h"

/*
 * Macros and definitions
 * ----------------------
 */

#define MAJOR_VERSION  "01"
#define MINOR_VERSION  "00"

#if !defined(__GNUC__)
#define M_PI        3.14159265358979323846
#define M_PI_2      1.57079632679489661923
#define M_PI_4      0.785398163397448309616
#endif

#define DEG_TO_RAD(angle)     ((angle)*M_PI/180.0)
#define RAD_TO_DEG(angle)     ((angle)*180.0/M_PI)
#define FT_TO_METERS(ft)      ((ft)*0.3048)
#define METERS_TO_FT(meters)  ((meters)/0.3048)
#define SQUARED(value)        ((value)*(value))
#define CUBED(value)          ((value)*(value)*(value))

#define FRAME_RATE  25

enum EnInputType  { InputUnknown,  InputSqlite,  InputText  } ;
enum EnOutputType { OutputNone,    OutputSqlite, OutputMpeg } ;

bool redirectSql = false;

/*
 * Data structures
 * ---------------
 */


/*
 * Module data
 * -----------
 */

ClXPlaneControl         XPlaneControl;

int64_t                 ClSimTimer::lSimClockTicks;
int64_t                 ClSimTimer::lTicksPerSecond;
int64_t                 ClSimTimer::lTicksPerStep;
double                  ClSimTimer::fSimElapsedTime;

sqlite3               * pDB;
std::string             sSQL;
std::string             sTableName;

int                     iOutFile;

std::vector<ClSource_BMNavTxt *> txtSources;
std::vector<std::string> inputFiles;

EnInputType         enInputType = InputUnknown;

/*
 * Function prototypes
 * -------------------
 */

void XPlaneInit(ClSimState * pclSimState);
void XPlaneUpdate(ClSimState * pclSimState);
void XPlaneSendAircraftPosition(ClSimState* pclSimState, std::string prefix, int aircraftIndex);
void XPlaneInitAdditionalPlane(ClSimState* pclSimState, std::string prefix, int aircraftIndex);
uint32_t mkgmtime(struct tm * psuTmTime);
void FfmpegOpen(char * szOutFile);
void FfmpegClose();
int  FfmpegWrite(void* pUserData, uint8_t* pvDataBuffer, int iDataBufferSize);
void vUsage(void);

// ============================================================================

int main(int iArgc, char* aszArgv[])
{
  char                szInFile[256];      // Input file name
  char                szOutFile[256];     // Output file name
  EnOutputType        enOutputType = OutputNone;
  bool                bStatus;

  // Screen capture and ffmpeg stuff
  DXGIManager* pclDXGIManager = nullptr; // = new DXGIManager();
  size_t          iFrameBuffSize = 0;
  uint8_t* ubyFrameBuff = NULL;
  ClMpegEncoder   clMpegEncoder;
  unsigned int    ret;
  unsigned int    iFrameHeight, iFrameWidth;
  int64_t         iFrameCounter = 0;

  // Data Sources
  ClSource_BMNavDB* pSource_BMNavDB = nullptr;

  // Various simulation clocks and time
  ClSimTimer::lSimClockTicks = 0;
  ClSimTimer::lTicksPerSecond = 10000000;
  ClSimTimer::lTicksPerStep = 400000;     // 40 msec / 25 Hz
  ClSimTimer::fSimElapsedTime = 0.0;
  ClSimTimer      clSimTimer_40ms(400000);    // 40 msec / 25 Hz
  ClSimTimer      clSimTimer_100ms(1000000);  // 100 msec / 10 Hz
  ClSimTimer      clSimTimer_1S(10000000);    // 1 sec

  ClSimState      clSimState;
  double          fBluemaxTime;           // Bluemax data time (seconds)

  // Init some stuff
  szInFile[0] = '\0';
  szOutFile[0] = '\0';
  bStatus = false;

  fBluemaxTime = 0.0;
  clSimState.clear();

  // Process command line
  // --------------------

  for (int iArgIdx = 1; iArgIdx < iArgc; iArgIdx++) {

    switch (aszArgv[iArgIdx][0]) {

    case '-':
      switch (aszArgv[iArgIdx][1]) {

      case 'd':                   // Input database file
        if (enInputType != InputUnknown)
        {
          vUsage();
          return 1;
        }
        iArgIdx++;
        strcpy(szInFile, aszArgv[iArgIdx]);
        enInputType = InputSqlite;
        break;

      case 't':                   // Input text file
        if (enInputType != InputUnknown && enInputType != InputText)
        {
          vUsage();
          return 1;
        }
        iArgIdx++;
        strcpy(szInFile, aszArgv[iArgIdx]);
        enInputType = InputText;
        inputFiles.push_back(szInFile);
        break;

      case 'D':                    // Output database file
        if (enOutputType != OutputNone)
        {
          vUsage();
          return 1;
        }
        enOutputType = OutputSqlite;
        iArgIdx++;
        strcpy(szOutFile, aszArgv[iArgIdx]);
        break;

      case 'T':                    // Output database table name
        iArgIdx++;
        sTableName = aszArgv[iArgIdx];
        //            strcpy(szTableName, aszArgv[iArgIdx]);
        break;

      case 'M':                    // Output MPEG file
        if (enOutputType != OutputNone)
        {
          vUsage();
          return 1;
        }
        redirectSql = true;
        enOutputType = OutputMpeg;
        iArgIdx++;
        strcpy(szOutFile, aszArgv[iArgIdx]);
        break;

      default:
        vUsage();
        break;
      } /* end flag switch */
      break;

    } // end command line arg switch
  } // end for all arguments

  if (strlen(szInFile) == 0)
  {
    vUsage();
    return 1;
  }

  if (enOutputType != OutputNone)
  {
    // Initialize screen capture and ffmpeg
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    pclDXGIManager = new DXGIManager();
    pclDXGIManager->setup();

    RECT Dimensions = ((DXGIManager*)pclDXGIManager)->get_output_rect();
    iFrameHeight = Dimensions.bottom - Dimensions.top;
    iFrameWidth = Dimensions.right - Dimensions.left;

    clMpegEncoder.Open(&FfmpegWrite, &clSimState, iFrameHeight, iFrameWidth, FRAME_RATE);
  }

  // Init some more stuff
  clSimTimer_40ms.FromNow();
  clSimTimer_100ms.FromNow();
  clSimTimer_1S.FromNow();

  // Prepare inputs and outputs
  // --------------------------

  // Make data sources and open input files
  switch (enInputType)
  {
  case InputSqlite:
    pSource_BMNavDB = new ClSource_BMNavDB(&clSimState, "BM.");
    bStatus = pSource_BMNavDB->Open(szInFile);
    if (bStatus == false)
      return 1;
    pSource_BMNavDB->ReadNextLine();
    break;
  case InputText:
    for (int i = 0; i < inputFiles.size(); i++) {
      ClSource_BMNavTxt* txtsrc = new ClSource_BMNavTxt(&clSimState, "BM", i);
      txtSources.push_back(txtsrc);
      bStatus = txtsrc->Open(inputFiles[i]);
      if (bStatus == false)
        return 1;
      if (!txtsrc->ReadNextLine())
        return 1;
      if (!txtsrc->UpdateSimState(ClSimTimer::fSimElapsedTime))
        return 1;
    }
    break;
  } // end switch on input type

// Open output file
  switch (enOutputType)
  {
    //        int iStatus;
  case OutputSqlite:
    FfmpegOpen(szOutFile);
    break;

  case OutputMpeg:
    iOutFile = _open(szOutFile, _O_WRONLY | _O_CREAT | _O_BINARY, _S_IREAD | _S_IWRITE);
    break;

  default:
    break;
  } // end switch on output type

// Setup messages that will be written
// -----------------------------------

  XPlaneInit(&clSimState);
  Sleep(2000);

  // Start writing
  // -------------

  // Loop until an input is exhausted. Along the way various simulation
  // components check their notion of time against the current simulation
  // time and take appropriate action for this instant of time.

  bStatus = true;
  while (bStatus == true)
  {

    // Update input(s)
    // ---------------

    // Bluemax XLS input data
    while (clSimState.fState["BM.0.AC_TIME"] < fBluemaxTime)
    {

      switch (enInputType)
      {
      case InputSqlite: bStatus = pSource_BMNavDB->UpdateSimState(ClSimTimer::fSimElapsedTime);  break;
      case InputText:
      {
        bStatus = false;

        for (auto i = txtSources.begin(); i != txtSources.end(); i++) {
          if ((*i)->UpdateSimState(ClSimTimer::fSimElapsedTime)) {
            if (i == txtSources.begin())
              bStatus = true;
          }
        }
        break;
      }
      } // end switch on input type

  // Break out if input is exhausted
      if (bStatus == false)
        break;

      // Update XPlane state
      XPlaneUpdate(&clSimState);
      //printf("%.2f %.3f %.3f %.1f %3.0f %5.1f %5.1f\n", clSimState.fState["BM.0.actime"], clSimState.fState["BM.0.aclatd"], clSimState.fState["BM.0.aclond"], clSimState.fState["BM.0.acaltf"], clSimState.fState["BM.0.acktas"], clSimState.fState["BM.0.acthtad"], clSimState.fState["BM.0.acphid"]);
      printf("%.2f %.3f %.3f %.1f %3.0f %5.1f %5.1f\n", clSimState.fState["BM.0.AC_TIME"], clSimState.fState["BM.0.AC_LAT"], clSimState.fState["BM.0.AC_LON"], clSimState.fState["BM.0.AC_ALT"], clSimState.fState["BM.0.AC_TAS"], clSimState.fState["BM.0.AC_PITCH"], clSimState.fState["BM.0.AC_ROLL"]);
//Sleep(20);

    } // end while reading Bluemax XLS data

// Break out if input is exhausted
    if (bStatus == false)
      break;

    // 40 msec / 25 Hz events
    // ----------------------
    if (clSimTimer_40ms.Expired())
    {
      clSimTimer_40ms.FromPrev();
    } // end 40 msec / 25 Hz events

// 100 msec events
// ---------------
    if (clSimTimer_100ms.Expired())
    {
      clSimTimer_100ms.FromPrev();
    } // end 100 msec / 10 Hz events

// 1 Hz events
// -----------
    if (clSimTimer_1S.Expired())
    {
      clSimTimer_1S.FromPrev();

      //            printf("%f %f %f %f %f\n", clSimState.fState["BM.actime"], clSimState.fState["BM.aclatd"], clSimState.fState["BM.aclond"], clSimState.fState["BM.acaltf"], clSimState.fState["BM.acktas"]);
    } // end 1 Hz events

// Do a screen capture
// -------------------

    if (enOutputType != OutputNone)
    {

      /* make sure the frame data is writable */
      ret = av_frame_make_writable(clMpegEncoder.pAVFrame);
      if (ret < 0)
        exit(1);

      // Get screen image
      pclDXGIManager->get_output_data(&ubyFrameBuff, &iFrameBuffSize);
      //pclDXGIManager->save_to_bmp(ubyFrameBuff, iFrameBuffSize, iFrameWidth, iFrameHeight);

      /* Input screen frame buffer format is DXGI_FORMAT_B8G8R8A8_UNORM
         A four-component, 32-bit unsigned-normalized-integer format that supports
         8 bits for each color channel and 8-bit alpha.
      */
#if 0
      // Use with AV_PIX_FMT_RGB24
      int iSrcPitch = iFrameWidth * 4;
      int iDstPitch = clMpegEncoder.aiSrcLineSizes[0];    // Frame width * 3
      for (y = 0; y < iFrameHeight; y++)
      {
        for (x = 0; x < iFrameWidth; x++)
        {
          clMpegEncoder.pSrcData[0][(y * iDstPitch) + (x * 3) + 2] = ubyFrameBuff[(y * iSrcPitch) + (x * 4)];
          clMpegEncoder.pSrcData[0][(y * iDstPitch) + (x * 3) + 1] = ubyFrameBuff[(y * iSrcPitch) + (x * 4) + 1];
          clMpegEncoder.pSrcData[0][(y * iDstPitch) + (x * 3)    ] = ubyFrameBuff[(y * iSrcPitch) + (x * 4) + 2];
        }
      }
#else
      // Use with AV_PIX_FMT_BGR0
      if (ubyFrameBuff != NULL)
        memcpy(clMpegEncoder.pSrcData[0], ubyFrameBuff, iFrameBuffSize);
#endif

      clSimState.lState["BM.RowNum"] = iFrameCounter;
      clMpegEncoder.pAVFrame->pts = iFrameCounter;
      iFrameCounter++;

      /* encode the image */
      clMpegEncoder.Encode();
    } // end if recording video

// Update the various clocks
// -------------------------

    ClSimTimer::Tick();
    fBluemaxTime = ClSimTimer::fSimElapsedTime;
  } // end while reading until done

// Close files
  switch (enInputType)
  {
  case InputSqlite: pSource_BMNavDB->Close();  break;
  case InputText: 
    for (auto i = txtSources.begin(); i != txtSources.end(); i++)
      (*i)->Close();
    break;
  } // end switch on input type

  FfmpegClose();
  sqlite3_close(pDB);

  return 0;
}


// ----------------------------------------------------------------------------

// Send lat/lon/alt and heading/pitch/roll to XPlane

/*
DREF
sim/cockpit/electrical/HUD_brightness
sim/cockpit/electrical/instrument_brightness
sim/cockpit/electrical/cockpit_lights
sim/cockpit/electrical/night_vision_on
sim/cockpit/gyros/the_vac_ind_deg                Standby AI pitch
sim/cockpit/gyros/the_ind_deg3                   Main AI pitch
sim/cockpit/gyros/the_ind_elec_pilot_deg         Main AI pitch
sim/cockpit/gyros/psi_ind_degm3                  AI and HSI heading
sim/cockpit/gyros/psi_ind_elec_pilot_degm        AI and HSI heading
sim/cockpit/gyros/psi_ind_ahars_pilot_degm       External HUD heading
sim/cockpit/gyros/phi_ind_ahars_pilot_deg        External HUD roll
sim/cockpit/misc/compass_indicated               Magnetic Compass
sim/flightmodel/misc/h_ind                       Alitimeter
sim/flightmodel/position/indicated_airspeed      IAS
sim/flightmodel/position/vh_ind_fpm              VSI
sim/graphics/view/pilots_head_psi
sim/graphics/view/pilots_head_the
sim/flightmodel/failures/onground_all            1 = on the ground
sim/flightmodel/failures/onground_any

DATA
-> 4  Mach VVI G-load
-> 8  Joystick aileron/elevator/rudder
->11  Flight controls aileron/elevator/rudder
->14  Gear
  17  Roll / Pitch / Heading
->18  alpha beta 
  20  Lat / Lon / Alt
->25  Throttle Commanded 1 / 2
  35  Thrust
  37  RPM

CMND
sim/flight_controls/landing_gear_up
sim/flight_controls/landing_gear_down

*/
#define ALT_FUDGE       0

void XPlaneInit(ClSimState * pclSimState)
    {
    float                        afValue[8];

    for (int i=0; i<8; i++) afValue[i] = -999.0;

    switch (enInputType)
    {
    case InputSqlite:
      assert(1 == 0);
      break;
    case InputText:
      for (int i = 0; i < txtSources.size(); i++) {
        if (i == 0)
          XPlaneSendAircraftPosition(pclSimState, txtSources[i]->sPrefix, txtSources[i]->aircraftIndex);
        else
          XPlaneInitAdditionalPlane(pclSimState, txtSources[i]->sPrefix, txtSources[i]->aircraftIndex);
      }
      break;
    }

    XPlaneControl.SendDREF("sim/cockpit/electrical/HUD_brightness",        1.0);
    XPlaneControl.SendDREF("sim/cockpit/electrical/instrument_brightness", 1.0);

#if 0
    if (pclSimState->fState["BM.acgear"] > 50.0) pclSimState->insert("AC_geardown", true);
    else                                          pclSimState->insert("AC_geardown", false);
#else
    // Enable landing gear and put it down
    XPlaneControl.SendDREF("sim/flightmodel/failures/onground_all", 0.0);
    XPlaneControl.SendDREF("sim/flightmodel/failures/onground_any", 0.0);
    pclSimState->insert("AC_geardown", true);
    XPlaneControl.SendCMND("sim/flight_controls/landing_gear_down");

#endif
    }


void XPlaneUpdate(ClSimState* pclSimState)
{
  std::string prefix;
  float                        afValue[8];

  for (int i = 0; i < 8; i++) afValue[i] = -999.0;

  switch (enInputType)
  {
  case InputSqlite:
    assert(1 == 0);
    break;
  case InputText:
    prefix = txtSources[0]->sPrefix;
    for (auto i = txtSources.begin(); i != txtSources.end(); i++)
      XPlaneSendAircraftPosition(pclSimState, (*i)->sPrefix, (*i)->aircraftIndex);
    break;
  }

  // Altimeter / Airspeed / Mag compass
  XPlaneControl.SendDREF("sim/flightmodel/position/vh_ind_fpm", -(float)(pclSimState->fState[prefix + "AC_VEL_DOWN"] * 60.0));
  XPlaneControl.SendDREF("sim/cockpit/misc/compass_indicated", (float)pclSimState->fState[prefix + "AC_MAG_HDG"]);
  XPlaneControl.SendDREF("sim/flightmodel/misc/h_ind", (float)(pclSimState->fState[prefix + "AC_ALT"]));
  XPlaneControl.SendDREF("sim/flightmodel/position/indicated_airspeed", (float)pclSimState->fState[prefix + "AC_TAS"]);

  // Main AI and HSI
  XPlaneControl.SendDREF("sim/cockpit/gyros/psi_ind_elec_pilot_degm", (float)pclSimState->fState[prefix + "AC_TRUE_HDG"]);  // Heading
  XPlaneControl.SendDREF("sim/cockpit/gyros/phi_ind_elec_pilot_deg", (float)pclSimState->fState[prefix + "AC_ROLL"]);  // Roll
  XPlaneControl.SendDREF("sim/cockpit/gyros/the_ind_elec_pilot_deg", (float)pclSimState->fState[prefix + "AC_PITCH"]); // Pitch

  // Backup AI
  XPlaneControl.SendDREF("sim/cockpit/gyros/phi_vac_ind_deg", (float)pclSimState->fState[prefix + "AC_ROLL"]);  // Roll
  XPlaneControl.SendDREF("sim/cockpit/gyros/the_vac_ind_deg", (float)pclSimState->fState[prefix + "AC_PITCH"]); // Pitch

  // Angle of Attack
  afValue[0] = (float)pclSimState->fState[prefix + "AC_AOA"];
  XPlaneControl.SendDATA(18, afValue);

  // G meter
  afValue[0] = -999.0;
  afValue[4] = (float)pclSimState->fState[prefix + "GS"];
  XPlaneControl.SendDATA(4, afValue);

  // Throttle
  afValue[0] = (float)(pclSimState->fState[prefix + "AC_THROTTLE"] / 100.0);
  afValue[1] = (float)(pclSimState->fState[prefix + "AC_THROTTLE"] / 100.0);
  XPlaneControl.SendDATA(25, afValue);

  // External HUD
  XPlaneControl.SendDREF("sim/cockpit/gyros/psi_ind_ahars_pilot_degm", (float)pclSimState->fState[prefix + "AC_TRUE_HDG"]);  // Heading
  XPlaneControl.SendDREF("sim/cockpit/gyros/phi_ind_ahars_pilot_deg", (float)pclSimState->fState[prefix + "AC_ROLL"]);  // Roll
  XPlaneControl.SendDREF("sim/cockpit/gyros/the_ind_ahars_pilot_deg", (float)pclSimState->fState[prefix + "AC_PITCH"]); // Pitch

  // Landing gear
  if ((pclSimState->bState["AC_geardown"] == true) && (pclSimState->fState[prefix + "acgear"] < 40.0))
  {
    pclSimState->update("AC_geardown", false);
    XPlaneControl.SendCMND("sim/flight_controls/landing_gear_up");
    printf("CMD Gear Up\n");
  }
  else if ((pclSimState->bState["AC_geardown"] == false) && (pclSimState->fState[prefix + "acgear"] > 60.0))
  {
    pclSimState->update("AC_geardown", true);
    XPlaneControl.SendCMND("sim/flight_controls/landing_gear_down");
    printf("CMD Gear Down\n");
  }

  //XPlaneControl.SendDREF("", (float)pclSimState->fState[""]);
}

void XPlaneSendAircraftPosition(ClSimState* pclSimState, std::string prefix, int aircraftIndex) {
  ClXPlaneControl::SuPosition  suPos;

  suPos.fLat = pclSimState->fState[prefix + "AC_LAT"];
  suPos.fLon = pclSimState->fState[prefix + "AC_LON"];
  suPos.fElevFt = pclSimState->fState[prefix + "AC_ALT"] + ALT_FUDGE;
  suPos.fHeading = (float)pclSimState->fState[prefix + "AC_TRUE_HDG"];
  suPos.fPitch = (float)pclSimState->fState[prefix + "AC_PITCH"];
  suPos.fRoll = (float)pclSimState->fState[prefix + "AC_ROLL"];

  XPlaneControl.SendVEHX(suPos, aircraftIndex);
}

void XPlaneInitAdditionalPlane(ClSimState* pclSimState, std::string prefix, int aircraftIndex) {
  ClXPlaneControl::SuPosition  suPos;

  suPos.fLat = pclSimState->fState[prefix + "AC_LAT"];
  suPos.fLon = pclSimState->fState[prefix + "AC_LON"];
  suPos.fElevFt = pclSimState->fState[prefix + "AC_ALT"] + ALT_FUDGE;
  suPos.fHeading = (float)pclSimState->fState[prefix + "AC_TRUE_HDG"];
  suPos.fPitch = (float)pclSimState->fState[prefix + "AC_PITCH"];
  suPos.fRoll = (float)pclSimState->fState[prefix + "AC_ROLL"]; 

  XPlaneControl.SendACFN(aircraftIndex);
  XPlaneControl.SendPREL(suPos, aircraftIndex);
}


// ----------------------------------------------------------------------------

/* Return the equivalent in seconds past 12:00:00 a.m. Jan 1, 1970 GMT
   of the Greenwich Mean time and date in the exploded time structure `tm'.

   The standard mktime() has the annoying "feature" of assuming that the 
   time in the tm structure is local time, and that it has to be corrected 
   for local time zone.  In this library time is assumed to be UTC and UTC
   only.  To make sure no timezone correction is applied this time conversion
   routine was lifted from the standard C run time library source.  Interestingly
   enough, this routine was found in the source for mktime().

   This function does always put back normalized values into the `tm' struct,
   parameter, including the calculated numbers for `tm->tm_yday',
   `tm->tm_wday', and `tm->tm_isdst'.

   Returns -1 if the time in the `tm' parameter cannot be represented
   as valid `time_t' number. 
 */

// Number of leap years from 1970 to `y' (not including `y' itself).
#define nleap(y) (((y) - 1969) / 4 - ((y) - 1901) / 100 + ((y) - 1601) / 400)

// Nonzero if `y' is a leap year, else zero.
#define leap(y) (((y) % 4 == 0 && (y) % 100 != 0) || (y) % 400 == 0)

// Additional leapday in February of leap years.
#define leapday(m, y) ((m) == 1 && leap (y))

#define ADJUST_TM(tm_member, tm_carry, modulus) \
  if ((tm_member) < 0) { \
    tm_carry -= (1 - ((tm_member)+1) / (modulus)); \
    tm_member = (modulus-1) + (((tm_member)+1) % (modulus)); \
  } else if ((tm_member) >= (modulus)) { \
    tm_carry += (tm_member) / (modulus); \
    tm_member = (tm_member) % (modulus); \
  }

// Length of month `m' (0 .. 11)
#define monthlen(m, y) (ydays[(m)+1] - ydays[m] + leapday (m, y))


uint32_t mkgmtime(struct tm * psuTmTime)
    {

    // Accumulated number of days from 01-Jan up to start of current month.
    static short ydays[] =
    {
      0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365
    };

    int years, months, days, hours, minutes, seconds;

    years   = psuTmTime->tm_year + 1900;  // year - 1900 -> year
    months  = psuTmTime->tm_mon;          // 0..11
    days    = psuTmTime->tm_mday - 1;     // 1..31 -> 0..30
    hours   = psuTmTime->tm_hour;         // 0..23
    minutes = psuTmTime->tm_min;          // 0..59
    seconds = psuTmTime->tm_sec;          // 0..61 in ANSI wC.

    ADJUST_TM(seconds, minutes, 60)
    ADJUST_TM(minutes, hours,   60)
    ADJUST_TM(hours,   days,    24)
    ADJUST_TM(months,  years,   12)

    if (days < 0)
        do 
            {
            if (--months < 0) 
                {
                --years;
                months = 11;
                }
            days += monthlen(months, years);
            } while (days < 0);

    else
        while (days >= monthlen(months, years)) 
            {
            days -= monthlen(months, years);
            if (++months >= 12) 
                {
                ++years;
                months = 0;
                }
            } // end while

    // Restore adjusted values in tm structure
    psuTmTime->tm_year = years - 1900;
    psuTmTime->tm_mon  = months;
    psuTmTime->tm_mday = days + 1;
    psuTmTime->tm_hour = hours;
    psuTmTime->tm_min  = minutes;
    psuTmTime->tm_sec  = seconds;

    // Set `days' to the number of days into the year.
    days += ydays[months] + (months > 1 && leap (years));
    psuTmTime->tm_yday = days;

    // Now calculate `days' to the number of days since Jan 1, 1970.
    days = (unsigned)days + 365 * (unsigned)(years - 1970) +
           (unsigned)(nleap (years));
    psuTmTime->tm_wday = ((unsigned)days + 4) % 7; /* Jan 1, 1970 was Thursday. */
    psuTmTime->tm_isdst = 0;

    if (years < 1970)
        return (uint32_t)-1;

    return (uint32_t)(86400L * days  + 3600L * hours + 60L * minutes + seconds);
    }


// ----------------------------------------------------------------------------

void FfmpegOpen(char* szOutFile)
{
  if (!redirectSql) {
    int     iStatus;

    // Open SQL database
    iStatus = sqlite3_open(szOutFile, &pDB);
    assert(iStatus == SQLITE_OK);

    // Drop existing video table
    sSQL = "DROP TABLE IF EXISTS ";
    sSQL += sTableName;
    sSQL += ";";
    iStatus = sqlite3_exec(pDB, sSQL.c_str(), NULL, NULL, NULL);
    assert(iStatus == SQLITE_OK);

    // Create new video table
    sSQL = "CREATE TABLE ";
    sSQL += sTableName;
    sSQL += "(RowNum INT, VideoData BLOB);";
    iStatus = sqlite3_exec(pDB, sSQL.c_str(), NULL, NULL, NULL);
    assert(iStatus == SQLITE_OK);
  }
  else
    iOutFile = _open("..\\OutputData\\test.ts", _O_WRONLY | _O_CREAT | _O_BINARY, _S_IREAD | _S_IWRITE);
}



void FfmpegClose()
{
  if (!redirectSql) {
    int     iStatus;

    iStatus = sqlite3_close(pDB);
    assert(iStatus == SQLITE_OK);
  }
  else
    _close(iOutFile);
}



int FfmpegWrite(void* pUserData, uint8_t* pvDataBuffer, int iDataBufferSize)
{
  if (!redirectSql) {
    int             iStatus;
    std::string     sSQL;
    sqlite3_stmt* pSqlInsStmt;

    long            lNavRowNum = ((ClSimState*)pUserData)->lState["BM.RowNum"];

    sSQL = "INSERT INTO ";
    sSQL += sTableName;
    sSQL += " VALUES(?, ?);";

    iStatus = sqlite3_prepare_v2(pDB, sSQL.c_str(), sSQL.size(), &pSqlInsStmt, nullptr);
    assert(iStatus == SQLITE_OK);
    iStatus = sqlite3_bind_int(pSqlInsStmt, 1, lNavRowNum);
    assert(iStatus == SQLITE_OK);
    iStatus = sqlite3_bind_blob(pSqlInsStmt, 2, pvDataBuffer, iDataBufferSize, SQLITE_TRANSIENT);
    //   iStatus = sqlite3_bind_blob(pSqlInsStmt, 2, pvDataBuffer, iDataBufferSize, SQLITE_STATIC);
    assert(iStatus == SQLITE_OK);
    iStatus = sqlite3_step(pSqlInsStmt);
    assert(iStatus == SQLITE_DONE);
    iStatus = sqlite3_finalize(pSqlInsStmt);
    assert(iStatus == SQLITE_OK);
  }
  else
    _write(iOutFile, pvDataBuffer, iDataBufferSize);

  printf("FfmpegWrite wrote %6d bytes  ", iDataBufferSize);
  assert((iDataBufferSize % 188) == 0);
  return iDataBufferSize;
}
    

// ----------------------------------------------------------------------------

void vUsage(void)
{
  printf("\nBM6toXPlaneVideo  " __DATE__ " " __TIME__ "\n");
  printf("Drive XPlane with BlueMax nav data and record a video\n");
  printf("Usage: BM6toXPlaneVideo [flags]\n");
  printf("   -d filename  Input database file name    \n");
  printf("   -t filename  Input text file name        \n");
  printf("   -D filename  Output database file name   \n");
  printf("   -T tablename Output database table name  \n");
  printf("   -M filename  Output MPEG file name       \n");
}
