
#include <cassert>
#include <cstdio>
#include <string>

#include "Source_BMNavTxt.h"

/*
actime      Time
aclatd      Latitude (degrees)
aclond      Longitude (degrees)
acaltf      Aircraft altitude above sea level (ft)
acktas      True Airspeed
acvifps     Total Velocity
acvxi       X Velocity
acvyi       Y Velocity
acvzi       Z Velocity
acaxi       Aircraft inertial x-coordinate acceleration (f/s^2)
acayi       Aircraft inertial y-coordinate acceleration (f/s^2)
acazi       Aircraft inertial z-coordinate acceleration (f/s^2)
acphid      Roll (degrees)
acthtad     Pitch (degrees)
acpsid      True Heading (degrees)
acmagd      Magnetic Heading (degrees)
acaoad      Angle of Attack
acthro      Throttle (%)
acnzb       Aircraft Body Normal Acceleration (f/s^2)
acazb       Aircraft body z-coordinate acceleration (f/s^2)
acgear      Aircraft landing gear position
*/

double fDmsToD(char * szDMS);

// ----------------------------------------------------------------------------
// Constructor / Destructor
// ----------------------------------------------------------------------------

ClSource_BMNavTxt::ClSource_BMNavTxt(ClSimState * pclSimState, std::string sPrefix) :
    ClSource_Nav(pclSimState, sPrefix)
    {
    this->pclSimState = pclSimState;
    this->sPrefix     = sPrefix;
    this->enInputType = this->InputBMText;
    }


// ----------------------------------------------------------------------------

ClSource_BMNavTxt::~ClSource_BMNavTxt()
    {
    Close();
    }


// ----------------------------------------------------------------------------
// Methods
// ----------------------------------------------------------------------------

/// Open the BlueMax data file

bool ClSource_BMNavTxt::Open(std::string sFilename)
    {
    char                szLine[1000];
    char                szTrimmedLabel[100];
    char              * szLabel;
    std::string         sDataLabelKey;
    int                 iTokens;

    hBMInput = fopen(sFilename.c_str(), "r");
    if (hBMInput == NULL)
        return false;

    // Get the first header line
    fgets(szLine, sizeof(szLine), hBMInput);

    // Make the list of data labels and populate data map
    // This is the current minimum list
    // UserVariable actime aclatd aclond acaltf acktas acvifps acvxi acvyi acvzi acaxi acayi acazi acphid acthtad acpsid acmagd
    DataLabel.clear();
    iTokens = 1;
    szLabel = strtok(szLine, "\t");
    while (iTokens == 1)
        {
        iTokens = sscanf(szLabel, "%s", szTrimmedLabel);
        szLabel = strtok(NULL, "\t");
        if (iTokens == 1)
            {
            // Replace certain labels with their standard label counterparts
            if      (strcmp(szTrimmedLabel, "actime" ) == 0) sDataLabelKey = sPrefix + "AC_TIME";
            else if (strcmp(szTrimmedLabel, "aclatd" ) == 0) sDataLabelKey = sPrefix + "AC_LAT";
            else if (strcmp(szTrimmedLabel, "aclond" ) == 0) sDataLabelKey = sPrefix + "AC_LON";
            else if (strcmp(szTrimmedLabel, "acaltf" ) == 0) sDataLabelKey = sPrefix + "AC_ALT";
            else if (strcmp(szTrimmedLabel, "acktas" ) == 0) sDataLabelKey = sPrefix + "AC_TAS";
            else if (strcmp(szTrimmedLabel, "acvxi"  ) == 0) sDataLabelKey = sPrefix + "AC_VEL_NORTH";
            else if (strcmp(szTrimmedLabel, "acvyi"  ) == 0) sDataLabelKey = sPrefix + "AC_VEL_EAST";
            else if (strcmp(szTrimmedLabel, "acvzi"  ) == 0) sDataLabelKey = sPrefix + "AC_VEL_DOWN";
            else if (strcmp(szTrimmedLabel, "acaxi"  ) == 0) sDataLabelKey = sPrefix + "AC_ACCEL_NORTH";
            else if (strcmp(szTrimmedLabel, "acayi"  ) == 0) sDataLabelKey = sPrefix + "AC_ACCEL_EAST";
            else if (strcmp(szTrimmedLabel, "acazi"  ) == 0) sDataLabelKey = sPrefix + "AC_ACCEL_DOWN";
            else if (strcmp(szTrimmedLabel, "acphid" ) == 0) sDataLabelKey = sPrefix + "AC_ROLL";
            else if (strcmp(szTrimmedLabel, "acthtad") == 0) sDataLabelKey = sPrefix + "AC_PITCH";
            else if (strcmp(szTrimmedLabel, "acpsid" ) == 0) sDataLabelKey = sPrefix + "AC_TRUE_HDG";
            else if (strcmp(szTrimmedLabel, "acmagd" ) == 0) sDataLabelKey = sPrefix + "AC_MAG_HDG";
            else if (strcmp(szTrimmedLabel, "acaoad" ) == 0) sDataLabelKey = sPrefix + "AC_AOA";
            else if (strcmp(szTrimmedLabel, "acthro" ) == 0) sDataLabelKey = sPrefix + "AC_THROTTLE";
            else                                             sDataLabelKey = sPrefix + szTrimmedLabel;

            // Insert the label into the list of column labels
            DataLabel.insert(DataLabel.end(), sDataLabelKey);
            pclSimState->insert(sDataLabelKey,-1.0);
            }
        } // end while tokenizing line

    return true;

    } // end Open()


// ----------------------------------------------------------------------------

/// Close the BlueMax data file

void ClSource_BMNavTxt::Close()
    {
    if (hBMInput != NULL)
        fclose(hBMInput);

    return;
    } // end Close()

// ----------------------------------------------------------------------------

/// Read the next line of BlueMax data

bool ClSource_BMNavTxt::ReadNextLine()
    {
//    int                 iItems;
//    char                szLine[2000];

    // Get the next line        
    fgets(szLine, sizeof(szLine), hBMInput);
    if (feof(hBMInput))
        return false;

    // Decode time
    // TODO

    return true;
    }



// ----------------------------------------------------------------------------

bool ClSource_BMNavTxt::UpdateSimState(double fSimElapsedTime)
    {
    bool    bStatus;

    // Return if simulation time is less than current data time from this source
    if (fSimElapsedTime < fRelTime)
        return false;

    // Tokenize the line and store values with the appropriate data label
    char  * szDataItem;
    double  fDataItem;
    int     iTokens = 1;
    auto    itDataLabel = std::begin(DataLabel);
    szDataItem = strtok(szLine, "\t");
    while ((iTokens == 1) && (itDataLabel != std::end(DataLabel)))
        {
        iTokens = sscanf(szDataItem, "%lf", &fDataItem);
        szDataItem = strtok(NULL, "\t");
        if (iTokens == 1)
            {
            pclSimState->update(*itDataLabel, fDataItem);
            itDataLabel++;
            }
        } // end while tokenizing line

    // Get the next line of data
    bStatus = ReadNextLine();

    return bStatus;
    } // end UpdateSimState()

// ----------------------------------------------------------------------------

// Convert DMS string to degrees float
// 035:41:45.24:N

double fDmsToD(char * szDMS)
    {
    int     iItems;
    int     iDegrees;
    int     iMinutes;
    float   fSeconds;
    char    chNSEW;
    double  fDegrees;

    iItems = sscanf(szDMS, "%d:%d:%f:%c", &iDegrees, &iMinutes, &fSeconds, &chNSEW);
    fDegrees = iDegrees + iMinutes / 60.0 + fSeconds / 3600.0;
    if ((chNSEW == 'W') || (chNSEW == 'S'))
        fDegrees = -fDegrees;

    return fDegrees;
    }