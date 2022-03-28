// BM6toKML.cpp
//

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
#include <vector>

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

#include "SimState.h"
#include "Source_BMNavTxt.h"
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

enum EnInputType  { InputUnknown,  InputSqlite,  InputText  } ;
enum EnOutputType { OutputNone,    OutputSqlite, OutputMpeg } ;

/*
 * Data structures
 * ---------------
 */

struct SuACData
    {
    double      fLat;
    double      fLon;
    double      fAlt;
    };

/*
 * Module data
 * -----------
 */

int64_t                 ClSimTimer::lSimClockTicks;
int64_t                 ClSimTimer::lTicksPerSecond;
int64_t                 ClSimTimer::lTicksPerStep;
double                  ClSimTimer::fSimElapsedTime;

/*
 * Function prototypes
 * -------------------
 */

uint32_t mkgmtime(struct tm * psuTmTime);
void vUsage(void);

// ============================================================================

int main(int iArgc, char * aszArgv[])
    {
    char                szInFile[256];      // Input file name
    char                szOutFile[256];     // Output file name
    FILE              * hOutFile;           // Output file handle
    int                 iStepTime;
    bool                bStatus;

    // Data Sources
    ClSource_BMNavTxt * pSource_BMNavTxt = nullptr;

    // Data Storage
    SuACData                        suACData;
    std::vector<SuACData>           listACData;

    // Various simulation clocks and time
    ClSimTimer::lSimClockTicks  =        0;
    ClSimTimer::lTicksPerSecond = 10000000;
    ClSimTimer::lTicksPerStep   =   400000;     // 40 msec / 25 Hz
    ClSimTimer::fSimElapsedTime =      0.0;
    ClSimTimer    * pclSimTimer_ReadDecimate;   // Timer to decimate incoming nav data

    ClSimState      clSimState;
    double          fBluemaxTime;           // Bluemax data time (seconds)

    // Init some stuff
    szInFile[0]         = '\0';
    szOutFile[0]        = '\0';
    bStatus             = false;

    fBluemaxTime        = 0.0;
    clSimState.clear();

    // Process command line
    // --------------------

    for (int iArgIdx=1; iArgIdx<iArgc; iArgIdx++) 
        {
        switch (aszArgv[iArgIdx][0]) 
          {
          case '-' :
            switch (aszArgv[iArgIdx][1]) {

              case 'd' :                   // Time step (seconds)
                iArgIdx++;
                iStepTime = std::stoi(aszArgv[iArgIdx]);
                break;

              default :
                break;
              } /* end flag switch */
            break;

          default :
            if (szInFile[0] == '\0') strcpy(szInFile, aszArgv[iArgIdx]);
            else                     strcpy(szOutFile,aszArgv[iArgIdx]);
            break;
          } // end command line arg switch
        } // end for all arguments

    if (strlen(szInFile)==0)
        {
        vUsage();
        return 1;
        }

    // Init some more stuff
    pclSimTimer_ReadDecimate = new ClSimTimer(iStepTime * ClSimTimer::lTicksPerSecond);
    //pclSimTimer_ReadDecimate->FromNow();

    // Prepare inputs and outputs
    // --------------------------

    // Make data sources and open input files
    pSource_BMNavTxt = new ClSource_BMNavTxt(&clSimState, "BM."); 
    bStatus = pSource_BMNavTxt->Open(szInFile);
    if (bStatus == false)
        return 1;
    pSource_BMNavTxt->ReadNextLine();



    // Read BlueMax
    // ------------

    // Loop until an input is exhausted. Along the way various simulation
    // components check their notion of time against the current simulation
    // time and take appropriate action for this instant of time.

    printf("Read BlueMax data...\n");
    bStatus = true;
    while (bStatus == true)
        {

        // Read BlueMax data until caught up on time
        while (clSimState.fState["BM.AC_TIME"] < fBluemaxTime)
            {
            bStatus = pSource_BMNavTxt->UpdateSimState(fBluemaxTime);

            // Break out if input is exhausted
            if (bStatus == false)
                break;

            } // end while reading Bluemax XLS data

        // Break out if input is exhausted
        if (bStatus == false)
            break;

        // Decimation timer event
        if (pclSimTimer_ReadDecimate->Expired())
            {
            // Store it
            suACData.fLat = clSimState.fState["BM.AC_LAT"];
            suACData.fLon = clSimState.fState["BM.AC_LON"];
            suACData.fAlt = FT_TO_METERS(clSimState.fState["BM.AC_ALT"]);
            listACData.push_back(suACData);

            pclSimTimer_ReadDecimate->FromPrev();
            } // end read decimation timer events

        // Update the various clocks
        ClSimTimer::Tick();
        fBluemaxTime = ClSimTimer::fSimElapsedTime;
        } // end while reading until done

    // Generate KML
    // ------------

    printf("Write KML data...\n");

    // Open output file
    if (strlen(szOutFile) != 0)
        {
        hOutFile = fopen(szOutFile,"w");
        if (hOutFile == NULL) 
            {
            fprintf(stderr, "Error opening output file\n");
            return 1;
            }
        }

    // No output file name so use stdout
    else
        hOutFile = stdout;

    // The XML is simple so I am not going to bother with an XML library... for now.
    fprintf(hOutFile, 
        "<kml xmlns=\"http://www.opengis.net/kml/2.2\"\n"
        "     xmlns:gx=\"http://www.google.com/kml/ext/2.2\">\n"
        "  <Document>\n"
        "\n"
        "    <Style id=\"OrangeLine\">\n"
        "      <LineStyle>\n"
        "        <color>ff008cff</color>\n"
        "        <width>1</width>\n"
        "      </LineStyle>\n"
        "    </Style>\n"
        "\n"
        "    <Style id=\"OrangeExtrude\">\n"
        "      <LineStyle>\n"
        "        <color>00008cff</color>\n"
        "        <width>1</width>\n"
        "      </LineStyle>\n"
        "      <PolyStyle>\n"
        "        <color>5f008cff</color>\n"
        "      </PolyStyle>\n"
        "    </Style>\n"
        "\n"
        //"    <Style id=\"EngineOutIcon\">\n"
        //"      <IconStyle>\n"
        //"        <Icon>\n"
        //"          <href>http://maps.google.com/mapfiles/kml/shapes/caution.png</href>\n"
        //"        </Icon>\n"
        //"      </IconStyle>\n"
        //"    </Style>\n"
        //"\n"
        //"    <Placemark>\n"
        //"      <name>IDLE Power</name>\n"
        //"      <styleUrl>#EngineOutIcon</styleUrl>\n"
        //"      <description>Simulated Engine Failure</description>\n"
        //"      <Point>\n"
        //"        <altitudeMode>absolute</altitudeMode>\n"
        //"        <coordinates>-86.66040945,30.8530724,185</coordinates>\n"
        //"      </Point>\n"
        //"    </Placemark>\n"
        //"\n"
        "    <Placemark>\n"
        "      <name>A-10 Echo Range</name>\n"
        "      <styleUrl>#OrangeLine</styleUrl>\n"
        //"      <LookAt>\n"
        //"        <longitude>-86.66544942</longitude>\n"
        //"        <latitude>30.85330141</latitude>\n"
        //"        <altitude>100</altitude>\n"
        //"        <heading>160</heading>\n"
        //"        <tilt>80</tilt>\n"
        //"        <range>2000</range>\n"
        //"      </LookAt> \n"
        "      <LineString>\n"
        "        <altitudeMode>absolute</altitudeMode>\n"
        "        <extrude>0</extrude>\n"
        "        <coordinates>\n");

    for (auto itACData : listACData)
        fprintf(hOutFile, "          %.6f,%.6f,%.1f\n", itACData.fLon, itACData.fLat, itACData.fAlt);

    fprintf(hOutFile, 
        "        </coordinates>\n"
        "      </LineString>\n"
        "    </Placemark>\n"
        "\n"
        "    <Placemark>\n"
        "      <name>A-10 Echo Range - Extrude</name>\n"
        "      <styleUrl>#OrangeExtrude</styleUrl>\n"
        "      <LineString>\n"
        "        <altitudeMode>absolute</altitudeMode>\n"
        "        <extrude>1</extrude>\n"
        "        <tessellate>1</tessellate>\n"
        "        <coordinates>\n");

    for (auto itACData : listACData)
        fprintf(hOutFile, "          %.6f,%.6f,%.1f\n", itACData.fLon, itACData.fLat, itACData.fAlt);

    fprintf(hOutFile, 
        "        </coordinates>\n"
        "      </LineString>\n"
        "    </Placemark>\n"
        "  </Document>\n"
        "</kml>");

    // Close files
    pSource_BMNavTxt->Close();

    printf("Done!\n");

    return 0;
    }


// ----------------------------------------------------------------------------

void vUsage(void)
    {
    printf("\nBM6toKML  " __DATE__ " " __TIME__ "\n");
    printf("Convert BlueMax output to KML for visualization in Google Earth\n");
    printf("Usage: BM6toKML [flags] InputFile OutputFile\n");
    printf("   -d num       Decimation count            \n");
    }
