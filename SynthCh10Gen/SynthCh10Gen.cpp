// SynthCh10Gen.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

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

#include "SimState.h"
#include "Source_BMNavTxt.h"
#include "Source_BMNavDB.h"
#include "Source_VideoDB.h"
#include "SimTimer.h"
#include "Ch10Format_1553.h"
#include "Ch10Format_1553_Nav.h"
#include "Ch10Writer_1553.h"
#include "Ch10Writer_Time.h"
#include "Ch10Writer_Video.h"
#include "Ch10Writer_Index.h"

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

enum EnInputType  { InputUnknown,  InputSqlite,  InputText  } ;

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

// Chapter 10 writers
ClCh10Writer_Time     * pCh10Writer_Time;
ClCh10Writer_1553     * pCh10Writer_1553;
ClCh10Writer_VideoF0  * pCh10Writer_Video_HUD;
ClCh10Writer_VideoF0  * pCh10Writer_Video_Cockpit;
ClCh10Writer_VideoF0  * pCh10Writer_Video_Chase;
ClCh10Writer_Index    * pCh10Writer_Index;

/*
 * Function prototypes
 * -------------------
 */

void SimClockToRel(int iI106Handle, double dSimTime, uint8_t abyRelTime[]);
void WriteTmats(int iI106Handle, double fCurrSimClockTime);
void vUsage(void);

// ============================================================================

int main(int iArgc, char * aszArgv[])
{
    char                szInFile[256];     // Input file name
    char                szOutFile[256];    // Output file name
    EnInputType         enInputType  = InputUnknown;
    bool                bStatus;

    // Data Sources
    ClSource_BMNavTxt * pSource_BMNavTxt;
    ClSource_BMNavDB  * pSource_BMNavDB;
    ClSource_VideoDB  * pSource_Video_HUD;
    ClSource_VideoDB  * pSource_Video_Cockpit;
    ClSource_VideoDB  * pSource_Video_Chase;

    int                 iI106Handle;

    // 1553 messages
//  ClCh10Format_1553_Nav   suNav_1Hz(RT_NAV, 1, 29, 32);
    ClCh10Format_1553_Nav   suNav_25Hz(RT_NAV, 1, 29, 32);

    unsigned long       ulBuffSize = 0L;
    unsigned char     * pvBuff  = NULL;

    EnI106Status        enStatus;
//  Irig106::SuTmatsInfo         suTmatsInfo;

    // Various simulation clocks and time
    ClSimTimer::lSimClockTicks  =        0;
    ClSimTimer::lTicksPerSecond = 10000000;
    ClSimTimer::lTicksPerStep   =   400000;     // 40 msec / 25 Hz
    ClSimTimer::fSimElapsedTime =      0.0;
    ClSimTimer      clSimTimer_40ms(400000);    // 40 msec / 25 Hz
    ClSimTimer      clSimTimer_100ms(1000000);  // 100 msec / 10 Hz
    ClSimTimer      clSimTimer_1S(10000000);    // 1 sec
    ClSimTimer      clSimTimer_6S(60000000);    // 6 sec (for node index packets)
    ClSimTimer      clSimTimer_60S(600000000);  // 60 sec (for root index packets)

    ClSimState      clSimState;
    double          fBluemaxTime;           // Bluemax data time (seconds)
    double          fStartSimClockTime;     // Starting simulation Date/Time
    double          fCurrSimClockTime;      // Current simulation Data/Time
    double          fNextPrintTime;         

    // Init some stuff
    szInFile[0]         = '\0';
    szOutFile[0]        = '\0';
    bStatus             = false;

    fBluemaxTime        = 0.0;
    fStartSimClockTime  = double(time(NULL));
    clSimState.clear();

    // Process command line
    // --------------------

  for (int iArgIdx=1; iArgIdx<iArgc; iArgIdx++) {

    switch (aszArgv[iArgIdx][0]) {

      case '-' :
        switch (aszArgv[iArgIdx][1]) {

          case 's' :                   // Data start date and time
            {
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

          case 'd' :                   // Input database file
            if (enInputType != InputUnknown)
                {
                vUsage();
                return 1;
                }
            iArgIdx++;
            strcpy(szInFile, aszArgv[iArgIdx]);
            enInputType = InputSqlite;
            break;

          case 't' :                   // Input text file
            if (enInputType != InputUnknown)
                {
                vUsage();
                return 1;
                }
            iArgIdx++;
            strcpy(szInFile, aszArgv[iArgIdx]);
            enInputType = InputText;
            break;

          default :
            break;
          } /* end flag switch */
        break;

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

    // Init some more stuff
    fCurrSimClockTime   = fStartSimClockTime;
    fNextPrintTime      =  0.0;
    clSimTimer_40ms.FromNow();
    clSimTimer_100ms.FromNow();
    clSimTimer_1S.FromNow();
    clSimTimer_6S.FromNow();
    clSimTimer_60S.FromNow();

    // Prepare inputs and outputs
    // --------------------------

    // Make data sources and open input files
    pSource_BMNavDB       = new ClSource_BMNavDB (&clSimState, "BM."); 
    pSource_BMNavTxt      = new ClSource_BMNavTxt(&clSimState, "BM."); 
    pSource_Video_HUD     = new ClSource_VideoDB();
    pSource_Video_Cockpit = new ClSource_VideoDB();
    pSource_Video_Chase   = new ClSource_VideoDB();

    switch (enInputType)
        {
        case InputSqlite : 
            bStatus = pSource_BMNavDB->Open(szInFile);
            if (bStatus == false)
                return 1;
            pSource_BMNavDB->ReadNextLine();
            break;
        case InputText   : 
            bStatus = pSource_BMNavTxt->Open(szInFile);
            if (bStatus == false)
                return 1;
            pSource_BMNavTxt->ReadNextLine();
            break;
        } // end switch on input type

    // Open video sources
    pSource_Video_HUD->Open(szInFile, "Video_HUD");
    pSource_Video_Cockpit->Open(szInFile, "Video_F4_Cockpit");
    pSource_Video_Chase->Open(szInFile, "Video_F4_Chase");

    // Make Chapter 10 writers
    pCh10Writer_Time          = new ClCh10Writer_Time();
    pCh10Writer_Video_HUD     = new ClCh10Writer_VideoF0();
    pCh10Writer_Video_Cockpit = new ClCh10Writer_VideoF0();
    pCh10Writer_Video_Chase   = new ClCh10Writer_VideoF0();
    pCh10Writer_1553          = new ClCh10Writer_1553();
    pCh10Writer_Index         = new ClCh10Writer_Index();

    // Open the output Ch 10 file and init it
    enStatus = enI106Ch10Open(&iI106Handle, szOutFile, I106_OVERWRITE);
    if (enStatus != I106_OK)
        {
        fprintf(stderr, "Error opening data file : Status = %d\n", enStatus);
        return 1;
        }

    // Setup messages that will be written
    // -----------------------------------

    // Setup the output channels
    pCh10Writer_Time->Init(iI106Handle, 1);
    pCh10Writer_Video_HUD->Init(iI106Handle, 10);
    pCh10Writer_Video_Cockpit->Init(iI106Handle, 11);
    pCh10Writer_Video_Chase->Init(iI106Handle, 12);
    pCh10Writer_1553->Init(iI106Handle, 30);
    pCh10Writer_Index->Init(iI106Handle, 0);

    // Get time setup
    pCh10Writer_Time->SetRelTime(ClSimTimer::lSimClockTicks, fStartSimClockTime);

    // Write initial TMATS and first time packet
    WriteTmats(iI106Handle, fCurrSimClockTime);
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

        // Bluemax input data
        while (clSimState.fState["BM.actime"] < fBluemaxTime)
            {
            switch (enInputType)
                {
                case InputSqlite : bStatus = pSource_BMNavDB->ReadNextLine();  break;
                case InputText   : bStatus = pSource_BMNavTxt->ReadNextLine(); break;
                } // end switch on input type

            // Break out if input is exhausted
            if (bStatus == false)
                break;

            } // end while reading Bluemax XLS data

        // Break out if input is exhausted
        if (bStatus == false)
            break;

        // Make BlueMax based nav message(s)
//      suNav_1Hz.MakeMsg(&clSimState);
        suNav_25Hz.MakeMsg(&clSimState);

        // Get current video data
        // Since video data and BlueMax data pace each other, when new BlueMax data
        // available then new video would also normally be available.
        pSource_Video_HUD->Read(clSimState.lState["BM.RowNum"]);
        if (pSource_Video_HUD->bVideoDataValid)
            pCh10Writer_Video_HUD->Write(&ClSimTimer::lSimClockTicks, pSource_Video_HUD->pachTSData, pSource_Video_HUD->iTSDataLength);

        pSource_Video_Cockpit->Read(clSimState.lState["BM.RowNum"]);
        if (pSource_Video_Cockpit->bVideoDataValid)
            pCh10Writer_Video_Cockpit->Write(&ClSimTimer::lSimClockTicks, pSource_Video_Cockpit->pachTSData, pSource_Video_Cockpit->iTSDataLength);

        pSource_Video_Chase->Read(clSimState.lState["BM.RowNum"]);
        if (pSource_Video_Chase->bVideoDataValid)
            pCh10Writer_Video_Chase->Write(&ClSimTimer::lSimClockTicks, pSource_Video_Chase->pachTSData, pSource_Video_Chase->iTSDataLength);

        // 40 msec / 25 Hz events
        // ----------------------
        if (clSimTimer_40ms.Expired())
            {
            clSimTimer_40ms.FromPrev();
            suNav_25Hz.SetRTC(&ClSimTimer::lSimClockTicks);
            pCh10Writer_1553->AppendMsg(&suNav_25Hz);
            } // end 40 msec / 25 Hz events

        // 100 msec events
        // ---------------
        if (clSimTimer_100ms.Expired())
            {
            clSimTimer_100ms.FromPrev();
            pCh10Writer_1553->Commit();
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

            printf("%f %f %f %f\n", clSimState.fState["BM.actime"], clSimState.fState["BM.aclatd"], clSimState.fState["BM.aclond"], clSimState.fState["BM.acktas"]);
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
        fBluemaxTime        = ClSimTimer::fSimElapsedTime;
        fCurrSimClockTime   = fStartSimClockTime + ClSimTimer::fSimElapsedTime;
        } // end while reading until done

    // Done, clean up
    // --------------

    // Close input sources
    switch (enInputType)
        {
        case InputSqlite : 
            pSource_BMNavDB->Close();
            break;
        case InputText   : 
            pSource_BMNavTxt->Close();
            break;
        } // end switch on input type

    // Write the final index root packet
    pCh10Writer_Index->WriteRootPacket();

    // Close outputs
    enI106Ch10Close(iI106Handle);

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

void WriteTmats(int iI106Handle, double fCurrSimClockTime)
    {
    SuI106Ch10Header    suI106Hdr;
    std::stringstream   ssTMATS;
    uint8_t           * pchDataBuff;
    uint32_t            ulDataBuffSize;
    SuTmats_ChanSpec  * psuTmats_ChanSpec;
    int                 iRSrcNum = 1;
    const int           iTotalRSrcs = 5;

    // Make a time string for the TMATS in the format 08-19-2014-17-33-59
    time_t        iCurrSimClockTime;
    char          szCurrSimClockTime[100];
    struct tm   * psuCurrSimClockTime;
    iCurrSimClockTime = (time_t)fCurrSimClockTime;
    psuCurrSimClockTime = gmtime(&iCurrSimClockTime);
    strftime(szCurrSimClockTime, sizeof(szCurrSimClockTime), "%m-%d-%Y-%H-%M-%S", psuCurrSimClockTime);

    ssTMATS.clear();
    ssTMATS <<
        "G\\PN:A10 SIM;\n"
        "G\\106:07;\n"
        "G\\DSI\\N:1;\n"
        "G\\DSI-1:DATASOURCE;\n"
        "G\\DST-1:STO;\n"
        "R-1\\ID:DATASOURCE;\n"
        "R-1\\RID:SynthCh10Gen;\n"
        "R-1\\NSB:0;\n"
        "R-1\\RI1:irig106.org;\n"
        "R-1\\RI2:SynthCh10Gen;\n"
        "R-1\\RI3:Y;\n";
    ssTMATS <<
        "R-1\\RI4:" << szCurrSimClockTime << ";\n";
    ssTMATS <<
        "R-1\\EV\\E:F;\n"
        "R-1\\IDX\\E:T;\n"
        "R-1\\N:" << iTotalRSrcs << ";\n";

    // Time section
    ssTMATS << pCh10Writer_Time->TMATS(1, iRSrcNum++);

    // Video section
    ssTMATS << pCh10Writer_Video_HUD->TMATS(1, iRSrcNum++, "VIDEO_HUD");
    ssTMATS << pCh10Writer_Video_Cockpit->TMATS(1, iRSrcNum++, "VIDEO_COCKPIT");
    ssTMATS << pCh10Writer_Video_Chase->TMATS(1, iRSrcNum++, "VIDEO_CHASE");

    // 1553 section
    ssTMATS << pCh10Writer_1553->TMATS(1, iRSrcNum++);

    assert(iRSrcNum-1 == iTotalRSrcs);

    // Form the TMATS header
    iHeaderInit(&suI106Hdr, 0, I106CH10_DTYPE_TMATS, I106CH10_PFLAGS_CHKSUM_NONE | I106CH10_PFLAGS_TIMEFMT_IRIG106, 0);
    suI106Hdr.ulDataLen = sizeof(SuTmats_ChanSpec) + ssTMATS.str().length();
    suI106Hdr.ubyHdrVer = 3;
    SimClockToRel(iI106Handle, fCurrSimClockTime, suI106Hdr.aubyRefTime);
//    memset(suI106Hdr.aubyRefTime, 0, 6);

    // Setup the TMATS data portion
    ulDataBuffSize = uCalcDataBuffReqSize(suI106Hdr.ulDataLen, I106CH10_PFLAGS_CHKSUM_NONE);
    pchDataBuff = (uint8_t *)malloc(ulDataBuffSize);
    memset(pchDataBuff, 0, ulDataBuffSize);
    psuTmats_ChanSpec = (SuTmats_ChanSpec *)pchDataBuff;
    psuTmats_ChanSpec->iCh10Ver = 7;
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
    printf("Convert a Bluemax simulation file to a Ch 10 1553 nav message file\n");
    printf("Usage: SynthCh10Gen  [flags] <output file>\n");
    printf("   -s m-d-y-h-m-s  Data start time   \n");
    printf("   -d filename     Input database file name     \n");
    printf("   -t filename     Input text file name         \n");
    printf("   <output file>   Output Ch 10 file name       \n");
    }
