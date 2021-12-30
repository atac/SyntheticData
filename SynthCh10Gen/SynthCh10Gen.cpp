/* SynthCh10Gen.cpp - This program reads various data sources and generates 
    Ch 10 data files.

This program is a framework for reading various data sources and generating
valid IRIG 106 Ch 10 data files. There are three main types of objects in this
software.

1) Data Sources - These objects read state data. Currently there are software
modules for reading CSV files, BlueMax formatted files (tab delimited), and 
data from a SQLite database.

2) Data Formatters - These object take simulation state data at an instant and
make valid Ch 10 formatted messages.

3) Data Writers - These objects take the appropriately formatted data and write
it to a Ch 10 channel in a data file.

This simulation engine is structured to simulate one recorder in a single aircraft.
Everything is paced from a single primary navigation data source. This nav data
source determines simulation start and stop times as well as the aircraft position
and attitude as it moves along the simulation. The data file time will be determined
by the time stamps of the primary nav data source. Simulation time, however, is a relative
time from the beginning of the primary nav data sources. Simulation time starts at 0.0 and
increases at the defined tick rate.

*/


#include <cstdio>
#include <cassert>
#include <ctime>
#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream
#include <string.h>
#include <stdlib.h>
#include <math.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#endif

#include "config.h"
#include "i106_stdint.h"
#include "irig106ch10.h"

#include "i106_time.h"
#include "i106_index.h"
#include "i106_decode_time.h"
#include "i106_decode_1553f1.h"
#include "i106_decode_tmats.h"
#include "i106_decode_index.h"

#include "Common.h"
#include "SimState.h"

// Sim data sources
#include "Source_Nav.h"

#ifdef COMPILE_BLUEMAX6
#include "Source_BMNavTxt.h"
#include "Source_BMNavDB.h"
#include "Source_VideoDB.h"
#endif
#ifdef COMPILE_NASA
#include "Source_NasaNavTxt.h"
#endif

#include "SimTimer.h"

// Sim data Ch 10 message formatters
#include "Ch10Format_1553.h"
#include "Ch10Format_1553_Nav.h"
#include "Ch10Format_A429.h"
#include "Ch10Format_A429_AR100.h"

// Sim data Ch 10 packet writers
#include "Ch10Writer.h"
#include "Ch10Writer_1553.h"
#include "Ch10Writer_Time.h"
#ifdef COMPILE_BLUEMAX6
#include "Ch10Writer_Video.h"
#endif
#include "Ch10Writer_Index.h"
#include "Ch10Writer_PCM.h"
#include "Ch10Writer_A429.h"

using namespace Irig106;

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

// 1553 Parameters
#define RT_NAV          6
#define RT_CONTROLLER  27

/*
 * Data structures
 * ---------------
 */


/*
 * Module data
 * -----------
 */

int64_t                 ClSimTimer::lSimClockTicks;
int64_t                 ClSimTimer::lTicksPerSecond;
int64_t                 ClSimTimer::lTicksPerStep;
double                  ClSimTimer::fSimElapsedTime;

ClTmatsIndexes          TmatsIndex;

// Ch 10 data formatters
// ClCh10Format_1553_Nav      * p1553Fmt_Nav_1Hz;
ClCh10Format_1553_Nav       * p1553Fmt_Nav_25Hz;
ClCh10Format_PCM_SynthFmt1  * pPCM_SynthFmt1;
ClCh10Format_ARINC429_AR100 * pA429_AR100_1;
ClCh10Format_ARINC429_AR100 * pA429_AR100_2;

// Chapter 10 writers
ClCh10Writer_Time           * pCh10Writer_Time;
ClCh10Writer_1553           * pCh10Writer_1553;
#ifdef COMPILE_BLUEMAX6
ClCh10Writer_VideoF0        * pCh10Writer_Video_HUD;
ClCh10Writer_VideoF0        * pCh10Writer_Video_Cockpit;
ClCh10Writer_VideoF0        * pCh10Writer_Video_Chase;
#endif
ClCh10Writer_Index          * pCh10Writer_Index;
ClCh10Writer_PCM            * pCh10Writer_PCM;
ClCh10Writer_A429           * pCh10Writer_A429_1;
ClCh10Writer_A429           * pCh10Writer_A429_2;

/*
 * Function prototypes
 * -------------------
 */

void SimClockToRel(int iI106Handle, double dSimTime, uint8_t abyRelTime[]);
void WriteTmats(int iI106Handle, std::string sProgramName, double fCurrSimClockTime);
void vUsage(void);

// ============================================================================

int main(int iArgc, char * aszArgv[])
{
    char                    szInFile[256];     // Input file name
    char                    szOutFile[256];    // Output file name
    bool                    bStatus;
    bool                    bVerbose;

    // Data Sources
    ClSource_Nav          * pSource_Nav = nullptr;
#ifdef COMPILE_BLUEMAX6
    std::string             sProgramName = "Synth Data BlueMax";
    ClSource_BMNavTxt     * pSource_BMNavTxt = nullptr;
    ClSource_BMNavDB      * pSource_BMNavDB  = nullptr;
    ClSource_VideoDB      * pSource_Video_HUD = nullptr;
    ClSource_VideoDB      * pSource_Video_Cockpit = nullptr;
    ClSource_VideoDB      * pSource_Video_Chase = nullptr;
#endif
#ifdef COMPILE_NASA
    std::string             sProgramName = "Synth Data NASA";
    ClSource_NasaNavTxt   * pSource_NasaNavTxt = nullptr;
#endif

    int                     iI106Handle;
    unsigned long           ulBuffSize = 0L;
    unsigned char         * pvBuff  = NULL;
    EnI106Status            enStatus;

    // Various simulation clocks and time
    ClSimTimer::lSimClockTicks  =        0;
    ClSimTimer::lTicksPerSecond = 10000000;     // 10 MHz, same as IRIG RTC
    ClSimTimer::lTicksPerStep   =   100000;     // 10 msec / 100 Hz
    ClSimTimer::fSimElapsedTime =      0.0;
    ClSimTimer      clSimTimer_10ms(100000);    // 10 msec / 100 Hz
    ClSimTimer      clSimTimer_40ms(400000);    // 40 msec / 25 Hz
    ClSimTimer      clSimTimer_100ms(1000000);  // 100 msec / 10 Hz
    ClSimTimer      clSimTimer_1S(10000000);    // 1 sec
    ClSimTimer      clSimTimer_6S(60000000);    // 6 sec (for node index packets)
    ClSimTimer      clSimTimer_60S(600000000);  // 60 sec (for root index packets)

    ClSimState      clSimState;                 // This holds all the simulation state data
    double          fNavSrcTime;            // Nav source data time (seconds)
    double          fStartSimClockTime;     // Starting simulation Date/Time
    double          fCurrSimClockTime;      // Current simulation Data/Time
    double          fNextPrintTime;         

    // Init some stuff
    szInFile[0]         = '\0';
    szOutFile[0]        = '\0';
    bStatus             = false;
    bVerbose            = false;
    pSource_Nav         = NULL;

    fNavSrcTime         =  0.0;
    fStartSimClockTime  = -1.0;
    clSimState.clear();

    // Process command line
    // --------------------

  for (int iArgIdx=1; iArgIdx<iArgc; iArgIdx++) {

    switch (aszArgv[iArgIdx][0]) {

      case '-' :
        switch (aszArgv[iArgIdx][1]) {

          case 'v' :
            bVerbose = true;
            break;

          case 'p' :                    // Data set name
            iArgIdx++;                  
            sProgramName = aszArgv[iArgIdx];
            break;

          case 's' :                    // Data start date and time. For now only useful
            {                           // with BlueMax data.
            iArgIdx++;
            struct tm   suStartTime;
            int iArgs = sscanf(aszArgv[iArgIdx],"%d-%d-%d-%d-%d-%d",
                    &suStartTime.tm_mon,  &suStartTime.tm_mday, &suStartTime.tm_year, 
                    &suStartTime.tm_hour, &suStartTime.tm_min,  &suStartTime.tm_sec);
            if (iArgs == 6)
                {
                suStartTime.tm_mon  -= 1;
                suStartTime.tm_year -= 1900;
                fStartSimClockTime = (double)mkgmtime(&suStartTime);
                }
            else
                {
                vUsage();
                return 1;
                }
            }
            break;

#ifdef COMPILE_BLUEMAX6
          case 'b' :                   // Input BlueMax database file
            if (pSource_Nav != NULL)
                {
                vUsage();
                return 1;
                }
            iArgIdx++;
            strcpy(szInFile, aszArgv[iArgIdx]);
            pSource_BMNavDB = new ClSource_BMNavDB (&clSimState, ""); 
            pSource_Nav = dynamic_cast<ClSource_Nav *>(pSource_BMNavDB);
            break;
#endif

#ifdef COMPILE_BLUEMAX6
          case 'B' :                   // Input BlueMax text file
            if (pSource_Nav != NULL)
                {
                vUsage();
                return 1;
                }
            iArgIdx++;
            strcpy(szInFile, aszArgv[iArgIdx]);
            pSource_BMNavTxt = new ClSource_BMNavTxt(&clSimState, ""); 
            pSource_Nav = dynamic_cast<ClSource_Nav *>(pSource_BMNavTxt);
            break;
#endif

#ifdef COMPILE_NASA
          case 'N' :                   // Input NASA text file
            if (pSource_Nav != NULL)
                {
                vUsage();
                return 1;
                }
            iArgIdx++;
            strcpy(szInFile, aszArgv[iArgIdx]);
            pSource_NasaNavTxt = new ClSource_NasaNavTxt(&clSimState, ""); 
            pSource_Nav = dynamic_cast<ClSource_Nav *>(pSource_NasaNavTxt);
            break;
#endif

          default :
            break;
          } // end flag switch
        break; // end of case '-'

      default :
        strcpy(szOutFile, aszArgv[iArgIdx]);
        break;

      } // end command line arg switch
    } // end for all arguments

    if ((strlen(szInFile)==0) || (strlen(szOutFile)==0))
        {
        vUsage();
        return 1;
        }

    // Prepare inputs and outputs
    // --------------------------

    // Setup video for various source types
#ifdef COMPILE_BLUEMAX6
        pSource_Video_HUD     = new ClSource_VideoDB();
        pSource_Video_Cockpit = new ClSource_VideoDB();
        pSource_Video_Chase   = new ClSource_VideoDB();
#endif

    bStatus = pSource_Nav->Open(szInFile);
    if (bStatus == false)
        return 1;

    // If need start time and the nav source is NASA then use the time stamp
    // of the first line of NASA data.
    if (fStartSimClockTime < 0.0)
        {
#ifdef COMPILE_NASA
        if (pSource_Nav->enInputType == ClSource_Nav::InputNasaCsv)
            fStartSimClockTime = dynamic_cast<ClSource_NasaNavTxt *>(pSource_Nav)->fStartTime;
        else
            fStartSimClockTime  = double(time(NULL));
#endif
#ifdef COMPILE_BLUEMAX6
        fStartSimClockTime  = double(time(NULL));
#endif
        } // end if start time needs to be set to something

    // Open video sources
#ifdef COMPILE_BLUEMAX6
    pSource_Video_HUD->Open(szInFile, "Video_HUD");
    pSource_Video_Cockpit->Open(szInFile, "Video_F4_Cockpit");
    pSource_Video_Chase->Open(szInFile, "Video_F4_Chase");
#endif

    // Data formatters
//  p1553Fmt_Nav_1Hz  = new ClCh10Format_1553_Nav(RT_NAV, 1, 29, 32);
    p1553Fmt_Nav_25Hz = new ClCh10Format_1553_Nav(RT_NAV, 1, 29, 32);
    pPCM_SynthFmt1    = new ClCh10Format_PCM_SynthFmt1(100);    // 100 Hz rate
    pA429_AR100_1     = new ClCh10Format_ARINC429_AR100(0, ARINC429_BUS_SPEED_LOW, 1);
    pA429_AR100_2     = new ClCh10Format_ARINC429_AR100(0, ARINC429_BUS_SPEED_LOW, 2);

    // Make Chapter 10 writers
    pCh10Writer_Time          = new ClCh10Writer_Time();
#ifdef COMPILE_BLUEMAX6
    pCh10Writer_Video_HUD     = new ClCh10Writer_VideoF0();
    pCh10Writer_Video_Cockpit = new ClCh10Writer_VideoF0();
    pCh10Writer_Video_Chase   = new ClCh10Writer_VideoF0();
#endif
    pCh10Writer_1553          = new ClCh10Writer_1553();
    pCh10Writer_Index         = new ClCh10Writer_Index();
    pCh10Writer_PCM           = new ClCh10Writer_PCM();
    pCh10Writer_A429_1        = new ClCh10Writer_A429();
    pCh10Writer_A429_2        = new ClCh10Writer_A429();

    // Open the output Ch 10 file and init it
    enStatus = enI106Ch10Open(&iI106Handle, szOutFile, I106_OVERWRITE);
    if (enStatus != I106_OK)
        {
        fprintf(stderr, "Error opening data file : Status = %d\n", enStatus);
        return 1;
        }

    // Init clock and other stuff
    // --------------------------

    fCurrSimClockTime   = fStartSimClockTime;
    fNextPrintTime      =  0.0;
    clSimTimer_10ms.FromNow();
    clSimTimer_40ms.FromNow();
    clSimTimer_100ms.FromNow();
    clSimTimer_1S.FromNow();
    clSimTimer_6S.FromNow();
    clSimTimer_60S.FromNow();

    // Setup messages that will be written
    // -----------------------------------

    // Setup the output channels
    pCh10Writer_Index->Init(iI106Handle, 0);
    pCh10Writer_Time->Init(iI106Handle, 1);
#ifdef COMPILE_BLUEMAX6
    pCh10Writer_Video_HUD->Init(iI106Handle, 10);
    pCh10Writer_Video_Cockpit->Init(iI106Handle, 11);
    pCh10Writer_Video_Chase->Init(iI106Handle, 12);
#endif
    pCh10Writer_PCM->Init(iI106Handle, 20);
    pCh10Writer_1553->Init(iI106Handle, 30);
    pCh10Writer_A429_1->Init(iI106Handle, 40);
    pCh10Writer_A429_2->Init(iI106Handle, 41);

    // This is a bit of a hack. Moving forward all writers will need a list of
    // formatters that will provide data, if for no other reason than to generate
    // meaningful TMATS. For now provide the PCM writer a reference to the PCM
    // formatter object. In the future this will be generalized for all writers
    // and formatters.
    pCh10Writer_PCM->pSynthPcmFmt1 = pPCM_SynthFmt1;

    // Get time setup
    pCh10Writer_Time->SetRelTime(ClSimTimer::lSimClockTicks, fStartSimClockTime);
    
    // Write initial TMATS and first time packet
    WriteTmats(iI106Handle, sProgramName, fCurrSimClockTime);
    pCh10Writer_Time->Write(fCurrSimClockTime);

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

        // Read the primary nav data source
        bStatus = pSource_Nav->UpdateSimState(fNavSrcTime);

        // Break out if input is exhausted
        if (bStatus == false)
            break;

        // Read any other input data sources
        // TODO

        // Make nav message(s)
//      suNav_1Hz.MakeMsg(&clSimState);

        // Get current video data
#ifdef COMPILE_BLUEMAX6
        // Since video data and BlueMax data pace each other, when new BlueMax data
        // available then new video would also normally be available.
        pSource_Video_HUD->Read(clSimState.lState["RowNum"]);
        if (pSource_Video_HUD->bVideoDataValid)
            pCh10Writer_Video_HUD->Write(&ClSimTimer::lSimClockTicks, pSource_Video_HUD->pachTSData, pSource_Video_HUD->iTSDataLength);

        pSource_Video_Cockpit->Read(clSimState.lState["RowNum"]);
        if (pSource_Video_Cockpit->bVideoDataValid)
            pCh10Writer_Video_Cockpit->Write(&ClSimTimer::lSimClockTicks, pSource_Video_Cockpit->pachTSData, pSource_Video_Cockpit->iTSDataLength);

        pSource_Video_Chase->Read(clSimState.lState["RowNum"]);
        if (pSource_Video_Chase->bVideoDataValid)
            pCh10Writer_Video_Chase->Write(&ClSimTimer::lSimClockTicks, pSource_Video_Chase->pachTSData, pSource_Video_Chase->iTSDataLength);
#endif
        // 10 msec / 100 Hz events
        // ----------------------
        if (clSimTimer_10ms.Expired())
            {
            clSimTimer_10ms.FromPrev();

            // PCM
            pPCM_SynthFmt1->MakeMsg(&clSimState);
            pPCM_SynthFmt1->SetRTC(&ClSimTimer::lSimClockTicks);
            pCh10Writer_PCM->AppendMsg(pPCM_SynthFmt1);

            // ARINC 429
            pA429_AR100_1->MakeMsg(&clSimState);
            pA429_AR100_1->SetRTC(&ClSimTimer::lSimClockTicks);
            pCh10Writer_A429_1->AppendMsg(pA429_AR100_1);

            pA429_AR100_2->MakeMsg(&clSimState);
            pA429_AR100_2->SetRTC(&ClSimTimer::lSimClockTicks);
            pCh10Writer_A429_2->AppendMsg(pA429_AR100_2);

            } // end 40 msec / 25 Hz events

        // 40 msec / 25 Hz events
        // ----------------------
        if (clSimTimer_40ms.Expired())
            {
            clSimTimer_40ms.FromPrev();
            p1553Fmt_Nav_25Hz->MakeMsg(&clSimState);
            p1553Fmt_Nav_25Hz->SetRTC(&ClSimTimer::lSimClockTicks);
            pCh10Writer_1553->AppendMsg(p1553Fmt_Nav_25Hz);
            } // end 40 msec / 25 Hz events

        // 100 msec events
        // ---------------
        if (clSimTimer_100ms.Expired())
            {
            clSimTimer_100ms.FromPrev();
            pCh10Writer_1553->Commit();
            pCh10Writer_PCM->Commit();
            pCh10Writer_A429_1->Commit();
            pCh10Writer_A429_2->Commit();
        } // end 100 msec / 10 Hz events

        // 1 Hz events
        // -----------
        if (clSimTimer_1S.Expired())
            {
            clSimTimer_1S.FromPrev();

            // Time packet
            pCh10Writer_Time->Write(fCurrSimClockTime);

            // Index the time packet
            pCh10Writer_Index->AppendNodeIndex(&(pCh10Writer_Time->suWritePktTimeF1.suCh10Header));

            // Low rate 1553 nav
//            suNav_1Hz.SetRTC(&ClSimTimer::lSimClockTicks);
//            pCh10Writer_1553->AppendMsg(&suNav_1Hz);

            if (bVerbose)
                printf("%f %f %f %f\n", clSimState.fState["AC_TIME"], clSimState.fState["AC_LAT"], clSimState.fState["AC_LON"], clSimState.fState["AC_TAS"]);
            } // end 1 Hz events

        // 6 second events
        // ---------------
        if (clSimTimer_6S.Expired())
            {
            clSimTimer_6S.FromPrev();

            // Write index node packet
            pCh10Writer_Index->WriteNodePacket();

            } // end 6 second events

        // 60 second events
        // ----------------
        if (clSimTimer_60S.Expired())
            {
            clSimTimer_60S.FromPrev();

            // Write index root packet
            pCh10Writer_Index->WriteRootPacket();


            } // end 6 second events

        // Update the various clocks
        ClSimTimer::Tick();
        fNavSrcTime         = ClSimTimer::fSimElapsedTime;
        fCurrSimClockTime   = fStartSimClockTime + ClSimTimer::fSimElapsedTime;
        } // end while reading until done

    // Done, clean up
    // --------------

    // Close input sources
#ifdef COMPILE_BLUEMAX6
    switch (pSource_Nav->enInputType)
        {
        case ClSource_Nav::InputBMSqlite : 
            // Make the compiler happy
            if (pSource_BMNavDB != nullptr)
                pSource_BMNavDB->Close();
            break;
        case ClSource_Nav::InputBMText   : 
            // Make the compiler happy
            if (pSource_BMNavTxt != nullptr)
                pSource_BMNavTxt->Close();
            break;
        } // end switch on input type
#else
    pSource_Nav->Close();
#endif

    // Write the final index root packet
    pCh10Writer_Index->WriteRootPacket();

    // Close outputs
    enI106Ch10Close(iI106Handle);

    printf("%s Done!\n", szInFile);

    return 0;
}


// ----------------------------------------------------------------------------

void SimClockToRel(int iI106Handle, double dSimTime, uint8_t abyRelTime[])
    {
    SuIrig106Time   suIrigTime;
                        
    suIrigTime.enFmt  = I106_DATEFMT_DMY;
    suIrigTime.ulSecs = (unsigned long)dSimTime;
    suIrigTime.ulFrac = (unsigned long)((dSimTime - (unsigned long)dSimTime) * 9999999.9);
    enI106_Irig2RelTime(iI106Handle, &suIrigTime, abyRelTime);

    return;
    }



// ============================================================================
// TMATS write routines
// ============================================================================

void WriteTmats(int iI106Handle, std::string sProgramName, double fCurrSimClockTime)
    {
    SuI106Ch10Header    suI106Hdr;
    std::stringstream   ssTMATS;
    uint8_t           * pchDataBuff;
    uint32_t            ulDataBuffSize;
    SuTmats_ChanSpec  * psuTmats_ChanSpec;

    // HMMMM.... NEED TO AUTOMATE THIS VALUE
    // The way to do this is to make a list of registered data sources. The 
    // number here would be the number of sources in the list. Later, step
    // through the sources in the list to initialize them.
#if defined(COMPILE_BLUEMAX6)
    const int           iTotalRSrcs = 5;
#elif defined(COMPILE_NASA)
    const int           iTotalRSrcs = 5;
#else
    const int           iTotalRSrcs = 0;
#endif

    // Make current time string
    time_t        iCurrTime;
    char          szCurrTime[100];
    struct tm   * psuCurrTime;
    iCurrTime   = time(NULL);
    psuCurrTime = gmtime(&iCurrTime);
    strftime(szCurrTime, sizeof(szCurrTime), "%m-%d-%Y-%H-%M-%S", psuCurrTime);

    // Make a time string for the TMATS in the format 08-19-2014-17-33-59
    time_t        iCurrSimClockTime;
    char          szCurrSimClockTime[100];
    struct tm   * psuCurrSimClockTime;
    iCurrSimClockTime   = (time_t)fCurrSimClockTime;
    psuCurrSimClockTime = gmtime(&iCurrSimClockTime);
    strftime(szCurrSimClockTime, sizeof(szCurrSimClockTime), "%m-%d-%Y-%H-%M-%S", psuCurrSimClockTime);

    ssTMATS.clear();
    ssTMATS <<
        "COMMENT:**********************************************************************;\n"
        "COMMENT: Synthetic data file created with SynthCh10Gen on " << szCurrTime << ";\n"
        "COMMENT: See https://github.com/atac/SyntheticData for details;\n"
        "COMMENT:**********************************************************************;\n"
        "COMMENT:                           G Group                                    ;\n"
        "COMMENT:**********************************************************************;\n";
    ssTMATS <<
        "G\\PN:" << sProgramName << ";\n";
    ssTMATS <<
        "G\\106:" CH10_VER_TMATS ";\n"
        "G\\DSI\\N:1;\n"
        "G\\SC:UNCLASSIFIED;\n"
        "G\\DSI-1:DATASOURCE;\n"
        "G\\DST-1:STO;\n"
        "G\\DSC-1:UNCLASSIFIED;\n";

    ssTMATS <<
        "COMMENT:**********************************************************************;\n"
        "COMMENT:                           R Group                                    ;\n"
        "COMMENT:**********************************************************************;\n"
        "R-1\\ID:DATASOURCE;\n"
        "R-1\\RID:SynthCh10Gen;\n"
        "R-1\\NSB:0;\n"
        "R-1\\RI1:Avionics Test and Analysis Corp (www.avtest.com);\n"
        "R-1\\RI2:SynthCh10Gen;\n"
        "R-1\\RI3:Y;\n";
    ssTMATS <<
        "R-1\\RI4:" << szCurrSimClockTime << ";\n";
    ssTMATS <<
        "R-1\\RI6:N;\n"
        "R-1\\CRE:Y;\n"
        "R-1\\RSS:C;\n"
        "R-1\\RML:I;\n"
        "R-1\\ERBS:AUTO;\n"
        "R-1\\EV\\E:F;\n"
        "R-1\\IDX\\E:T;\n"
        "R-1\\N:" << iTotalRSrcs << ";\n";

    // Time section
    ssTMATS << pCh10Writer_Time->TMATS(TmatsIndex);
    TmatsIndex.iRSrcNum++;

    // Video section
#ifdef COMPILE_BLUEMAX6
    ssTMATS << pCh10Writer_Video_HUD->TMATS(TmatsIndex, "VIDEO_HUD");
    TmatsIndex.iRSrcNum++;
    ssTMATS << pCh10Writer_Video_Cockpit->TMATS(TmatsIndex, "VIDEO_COCKPIT");
    TmatsIndex.iRSrcNum++;
    ssTMATS << pCh10Writer_Video_Chase->TMATS(TmatsIndex, "VIDEO_CHASE");
    TmatsIndex.iRSrcNum++;
#endif

    // 1553 R section, then linked B and C sections
    ssTMATS << pCh10Writer_1553->TMATS(TmatsIndex, pCh10Writer_1553->sCDLN, "1553-Nav");
    ssTMATS << p1553Fmt_Nav_25Hz->TMATS(TmatsIndex);
    TmatsIndex.iBIndex++;
    TmatsIndex.iRSrcNum++;

    // PCM R and P sections
    ssTMATS << pCh10Writer_PCM->TMATS(TmatsIndex);
    TmatsIndex.iRSrcNum++;

    // ARINC 429 R section
    ssTMATS << pCh10Writer_A429_1->TMATS(TmatsIndex, "A429InChan1", 1, "A429-Engine1");
    ssTMATS << pA429_AR100_1->TMATS(TmatsIndex, "A429InChan1");
    TmatsIndex.iBIndex++;
    TmatsIndex.iRSrcNum++;

    ssTMATS << pCh10Writer_A429_2->TMATS(TmatsIndex, "A429InChan2", 1, "A429-Engine2");
    ssTMATS << pA429_AR100_1->TMATS(TmatsIndex, "A429InChan2");
    TmatsIndex.iBIndex++;
    TmatsIndex.iRSrcNum++;

    assert(TmatsIndex.iRSrcNum-1 == iTotalRSrcs);

    // Form the TMATS packet header
    iHeaderInit(&suI106Hdr, 0, I106CH10_DTYPE_TMATS, I106CH10_PFLAGS_CHKSUM_NONE | I106CH10_PFLAGS_TIMEFMT_IRIG106, 0);
    suI106Hdr.ulDataLen = sizeof(SuTmats_ChanSpec) + ssTMATS.str().length();
    suI106Hdr.ubyHdrVer = CH10_VER_HDR_TMATS;
    SimClockToRel(iI106Handle, fCurrSimClockTime, suI106Hdr.aubyRefTime);
//    memset(suI106Hdr.aubyRefTime, 0, 6);

    // Setup the TMATS packet data portion
    ulDataBuffSize = uCalcDataBuffReqSize(suI106Hdr.ulDataLen, I106CH10_PFLAGS_CHKSUM_NONE);
    pchDataBuff = (uint8_t *)malloc(ulDataBuffSize);
    memset(pchDataBuff, 0, ulDataBuffSize);
    psuTmats_ChanSpec = (SuTmats_ChanSpec *)pchDataBuff;
    psuTmats_ChanSpec->iCh10Ver = CH10_VER_RECORDER;
    memcpy(&pchDataBuff[4], ssTMATS.str().c_str(), ssTMATS.str().length());
    uAddDataFillerChecksum(&suI106Hdr, pchDataBuff);
    suI106Hdr.uChecksum = uCalcHeaderChecksum(&suI106Hdr);

    enI106Ch10WriteMsg(iI106Handle, &suI106Hdr, pchDataBuff);

    free(pchDataBuff);
    }


// ----------------------------------------------------------------------------

#if 0
// Function to check if the key is present or not 
bool check_key(map<int, int> m, int key) 
    { 
    // Key is not present 
    if (m.find(key) == m.end()) 
        return falser; 
  
    return true;
    } 
#endif

// ----------------------------------------------------------------------------

void vUsage(void)
    {
    printf("\nSynthCh10Gen  " __DATE__ " " __TIME__ "\n");
#if     defined(COMPILE_BLUEMAX6)
    printf("Convert a Bluemax simulation file to a Ch 10 data file\n");
#elif defined(COMPILE_NASA)
    printf("Convert a NASA simulation file to a Ch 10 data file\n");
#endif
    printf("Usage: SynthCh10Gen  [flags] <output file>              \n");
    printf("   -v              Verbose                              \n");
    printf("   -p              Data set name (Program Name in TMATS)\n");
    printf("   -s m-d-y-h-m-s  Set or override data start time      \n");

#if defined(COMPILE_BLUEMAX6)
    printf("   -b filename     Input BlueMax database file name     \n");
    printf("   -B filename     Input BlueMax text data file name    \n");
#endif

#if defined(COMPILE_NASA)
    printf("   -N filename     Input NASA CSV data file name        \n");
#endif
    printf("   <output file>   Output Ch 10 file name               \n");
    }
