
#include <cassert>
#include <cmath>
#include <chrono>
#include <cstdio>
#include <string>

#include "Source_NasaNavTxt.h"

const double M_PI = 3.141592653589793238463;    // value of pi

/*
NASA Variables

    Name       Units      Rate  Points  Description
    1107       <units>    0.25  1640    SYNC WORD FOR SUBFRAME 1
    2670       <units>    0.25  1640    SYNC WORD FOR SUBFRAME 2
    5107       <units>    0.25  1640    SYNC WORD FOR SUBFRAME 3
    6670       <units>    0.25  1640    SYNC WORD FOR SUBFRAME 4
*   FPAC       G          16    104960  FLIGHT PATH ACCELERATION
*   BLAC       G          16    104960  BODY LONGITUDINAL ACCELERATION
*   CTAC       G          16    104960  CROSS TRACK ACCELERATION
*   TH         DEG        4     26240   TRUE HEADING LSP
*   MH         DEG        4     26240   MAGNETIC HEADING LSP
    EGT.1      DEG        4     26240   EXHAUST GAS TEMPERATURE 1
    EGT.2      DEG        4     26240   EXHAUST GAS TEMPERATURE 2
    EGT.3      DEG        4     26240   EXHAUST GAS TEMPERATURE 3
    EGT.4      DEG        4     26240   EXHAUST GAS TEMPERATURE 4
*   IVV        FT/MIN     16    104960  INERTIAL VERTICAL SPEED LSP
*   GS         KNOTS      4     26240   GROUND SPEED LSP
*   TRK        DEG        4     26240   TRACK ANGLE TRUE LSP
    TRKM       DEG        4     26240   TRACK ANGLE MAG LSP
    DA         DEG        4     26240   DRIFT ANGLE
    POVT                  1     6560    PYLON OVERHEAT ALL ENGINES
    WS         KNOTS      4     26240   WIND SPEED
    MW                    1     6560    MASTER WARNING
    DFGS                  1     6560    DFGS 1&2 MASTER
    WD         DEG        4     26240   WIND DIRECTION TRUE
*   ALT        FEET       4     26240   PRESSURE ALTITUDE LSP
    NSQT                  4     26240   SQUAT SWITCH NOSE MAIN GEAR
    RALT       FEET       8     52480   RADIO ALTITUDE LSP
    ALTR       FT/MIN     4     26240   ALTITUDE RATE
    FQTY.1     LBS        1     6560    FUEL QUANTITY TANK 1 LSB
    OIT.1      DEG        1     6560    OIL TEMPERATURE 1
    OIT.2      DEG        1     6560    OIL TEMPERATURE 2
    AOA1       DEG        4     26240   ANGLE OF ATTACK 1
    AOA2       DEG        4     26240   ANGLE OF ATTACK 2
*   PTCH       DEG        8     52480   PITCH ANGLE LSP
    FF.1       LBS/HR     4     26240   FUEL FLOW 1
    PSA        MB         2     13120   AVARAGE STATIC PRESSURE LSP
    FF.2       LBS/HR     4     26240   FUEL FLOW 2
    FF.3       LBS/HR     4     26240   FUEL FLOW 3
*   ROLL       DEG        8     52480   ROLL ANGLE LSP
    FF.4       LBS/HR     4     26240   FUEL FLOW 4
    N1.1       %RPM       4     26240   FAN SPEED 1 LSP
    N1.2       %RPM       4     26240   FAN SPEED 2 LSP
    MACH       MACH       4     26240   MACH LSP
    CAS        KNOTS      4     26240   COMPUTED AIRSPEED LSP
    APFD                  1     6560    AP FD STATUS
    PH                    1     6560    FLIGHT PHASE FROM ACMS
    CASM       KNOTS      4     26240   MAX ALLOWABLE AIRSPEED
*   TAS        KNOTS      4     26240   TRUE AIRSPEED LSP
*   VRTG       G          8     52480   VERTICAL ACCELERATION
*   LATG       G          4     26240   LATERAL ACCELERATION
    PI         MB         2     13120   IMPACT PRESSURE LSP
    PS         IN         2     13120   STATIC PRESSURE LSP
    N1.3       %RPM       4     26240   FAN SPEED 3 LSP
    EVNT                  1     6560    EVENT MARKER
    MRK                   1     6560    MARKERS- INNER, MIDDLE, OUTER
    VIB.1      IN/SEC     4     26240   ENGINE VIBRATION 1
    PT         MB         2     13120   TOTAL PRESSURE LSP
    VHF1                  1     6560    VHF KEYING #1
    VHF2                  1     6560    VHF KEYING #2
*   LGDN                  1     6560    GEARS L&R DOWN LOCKED
*   LGUP                  1     6560    GEARS L&R UP LOCKED
    VIB.2      IN/SEC     4     26240   ENGINE VIBRATION 2
    VHF3                  1     6560    VHF KEYING #3
    PUSH                  1     6560    STICK PUSHER
    SHKR                  2     13120   STICK SHAKER
    MSQT.2                2     13120   SQUAT SWITCH RIGHT MAIN GEAR
    VIB.3      IN/SEC     4     26240   ENGINE VIBRATION 3
*   LONG       G          4     26240   LONGITUDINAL ACCELERATION
*   PLA.1      DEG        4     26240   POWER LEVER ANGLE 1
    N1.4       %RPM       4     26240   FAN SPEED 4 LSP
    HYDY                  1     6560    LOW HYDRAULIC PRESSURE YELLOW
    HYDG                  1     6560    LOW HYDRAULIC PRESSURE GREEN
    SMOK                  1     6560    SMOKE WARNING
    CALT                  1     6560    CABIN HIGH ALTITUDE
    VIB.4      IN/SEC     4     26240   ENGINE VIBRATION 4
    PLA.2      DEG        4     26240   POWER LEVER ANGLE 2
    PLA.3      DEG        4     26240   POWER LEVER ANGLE 3
    PLA.4      DEG        4     26240   POWER LEVER ANGLE 4
*   GMT.HOUR   Hour       2     13120   GREENWICH MEAN TIME (HOUR)
*   GMT.MINUTE Minute     2     13120   GREENWICH MEAN TIME (MINUTE)
*   GMT.SEC    Second     2     13120   GREENWICH MEAN TIME (SECOND)
    ACMT                  1     6560    ACMS TIMING USED T1HZ
    FQTY.2     LSB        1     6560    FUEL QUANTITY TANK 2 LSB
    OIT.3      DEG        1     6560    OIL TEMPERATURE 3
    OIT.4      DEG        1     6560    OIL TEMPERATURE 4
*   DATE.YEAR  Year       0.25  1640    Date (Year)
*   DATE.MONTH Month      0.25  1640    Date (Month)
*   DATE.DAY   Day        0.25  1640    Date (Day)
    DVER.1                0.25  1640    DATABASE ID VERSION CHAR 1
    ACID                  0.25  1640    AIRCRAFT NUMBER
    BLV                   1     6560    BLEED AIR ALL VALVES
    EAI                   1     6560    ENGINE ANTICE ALL POSITIONS
    PACK                  1     6560    PACK AIR CONDITIONING ALL
    AOAI       DEG        4     26240   INDICATED ANGLE OF ATTACK
*   AOAC       DEG        4     26240   CORRECTED ANGLE OF ATTACK
    BAL1       FEET       4     26240   BARO CORRECT ALTITUDE LSP
    APUF                  2     13120   APU FIRE WARNING
    TOCW                  2     13120   TAKEOFF CONF WARNING
    BAL2       FEET       4     26240   BARO CORRECT ALTITUDE LSP
    WSHR                  1     6560    WINDSHEAR WARNING
*   WOW                   1     6560    WEIGHT ON WHEELS
    N2.1       %RPM       4     26240   CORE SPEED 1 LSP
    N2.2       %RPM       4     26240   CORE SPEED 2 LSP
    N2.3       %RPM       4     26240   CORE SPEED 3 LSP
    N2.4       %RPM       4     26240   CORE SPEED 4 LSP
    TAT        DEG        1     6560    TOTAL AIR TEMPERATURE
    SAT        DEG        1     6560    STATIC AIR TEMPERATURE
    N1T        %RPM       4     26240   N1 TARGET LSP
    N1C        %RPM       4     26240   N1 COMMAND LSP
*   RUDD       DEG        2     13120   RUDDER POSITION
    FQTY.3     LSB        1     6560    FUEL QUANTITY TANK 3 LSB
    OIP.1      PSI        1     6560    OIL PRESSURE 1
    OIP.2      PSI        1     6560    OIL PRESSURE 2
    FQTY.4     LSB        1     6560    FUEL QUANTITY TANK 4 LSB
    CRSS       DEG        1     6560    SELECTED COURSE
    HDGS       DEG        1     6560    SELECTED HEADING
    ALTS       FEET       1     6560    SELECTED ALTITUDE LSP
    SNAP                  1     6560    MANUAL SNAPSHOT SWITCH
    CASS       KNOTS      1     6560    SELECTED AIRSPEED
    N1CO                  1     6560    N1 COMPENSATION
    VSPS       FT/MIN     1     6560    SELECTED VERTICAL SPEED
    MNS        MMACH      1     6560    SELECTED MACH
    MSQT.1                2     13120   SQUAT SWITCH LEFT MAIN GEAR
    VMODE                 1     6560    VERTICAL ENGAGE MODES
    LMOD                  1     6560    LATERAL ENGAGE MODES
    A/T                   1     6560    THRUST AUTOMATIC ON
*   CCPC       COUNTS     2     13120   CONTROL COLUMN POSITION CAPT
    CCPF       COUNTS     2     13120   CONTROL COLUMN POSITION F/O
*   RUDP       COUNTS     2     13120   RUDDER PEDAL POSITION
*   CWPC       COUNTS     2     13120   CONTROL WHEEL POSITION CAPT
    CWPF       COUNTS     2     13120   CONTROL WHEEL POSITION F/O
    OIP.3      PSI        1     6560    OIL PRESSURE 3
    OIP.4      PSI        1     6560    OIL PRESSURE 4
    LOC        DDM        1     6560    LOCALIZER DEVIATION
    GLS        DDM        1     6560    GLIDESLOPE DEVIATION
*   LONP       DEG        1     6560    LONGITUDE POSITION LSP
    ABRK       DEG        1     6560    AIRBRAKE POSITION
*   AIL.1      DEG        1     6560    AILERON POSITION LH
*   AIL.2      DEG        1     6560    AILERON POSITION RH
    SPL.1      DEG        1     6560    ROLL SPOILER LEFT
    SPL.2      DEG        1     6560    ROLL SPOILER RIGHT
    ESN.4      UNITS      0.25  1640    ENGINE SERIAL NUMBER 4 LSP
    ECYC.1     HOURS      0.25  1640    ENGINE CYCLE 1 LSP
    ECYC.2     HOURS      0.25  1640    ENGINE CYCLE 2 LSP
*   ELEV.1     DEG        1     6560    ELEVATOR POSITION LEFT
*   ELEV.2     DEG        1     6560    ELEVATOR POSITION RIGHT
    FLAP       COUNTS     1     6560    T.E. FLAP POSITION
    PTRM       DEG        1     6560    PITCH TRIM POSITION
    HF1                   1     6560    HF KEYING #1
    HF2                   1     6560    HF KEYING #2
    SMKB                  1     6560    ANIMAL BAY SMOKE
    SPLY                  1     6560    SPOILER DEPLOY YELLOW
    SPLG                  1     6560    SPOILER DEPLOY GREEN
    FRMC                  0.25  1640    FRAME COUNTER
    DVER.2                0.25  1640    DATABASE ID VERSION CHAR 2
    ESN.3      UNITS      0.25  1640    ENGINE SERIAL NUMBER 3 LSP
    BPGR.1     PSI        1     6560    BRAKE PRESSURE LH GREEN
    BPGR.2     PSI        1     6560    BRAKE PRESSURE RH GREEN
    BPYR.1     PSI        1     6560    BRAKE PRESSURE LH YELLOW
    BPYR.2     PSI        1     6560    BRAKE PRESSURE RH YELLOW
    ECYC.3     HOURS      0.25  1640    ENGINE CYCLE 3 LSP
    ECYC.4     HOURS      0.25  1640    ENGINE CYCLE 4 LSP
    EHRS.1     HOURS      0.25  1640    ENGINE HOURS 1 LSP
    TCAS                  1     6560    TCAS LSP
    GPWS                  1     6560    GPWS 1-5
    TMAG                  1     6560    TRUE/MAG HEADING SELECT
    TAI                   1     6560    TAIL ANTICE ON
    WAI.1                 1     6560    INNER WING DEICE
    WAI.2                 1     6560    OUTER WING ANTICE
    DWPT                  1     6560    DISTANCE TO WAYPOINT LSP
    OIPL                  1     6560    LOW OIL PRESSURE ALL ENGINES
    FADF                  1     6560    FADEC FAIL ALL ENGINES
    FADS                  1     6560    FADEC STATUS ALL ENGINES
    EHRS.4     HOURS      0.25  1640    ENGINE HOURS 4 LSP
    EHRS.3     HOURS      0.25  1640    ENGINE HOURS 3 LSP
    EHRS.2     HOURS      0.25  1640    ENGINE HOURS 2 LSP
    TMODE                 1     6560    THRUST MODE
    ATEN                  1     6560    A/T ENGAGE STATUS
*   LATP       DEG        1     6560    LATITUDE POSITION LSP
    FIRE.1                1     6560    ENGINE FIRE #1
    FIRE.2                1     6560    ENGINE FIRE #2
    FIRE.3                1     6560    ENGINE FIRE #3
    FIRE.4                1     6560    ENGINE FIRE #4
    FGC3                  1     6560    DFGS STATUS 3
    ILSF                  1     6560    ILS FREQUENCY LSP
    ESN.1      UNITS      0.25  1640    ENGINE SERIAL NUMBER 1 LSP
    ESN.2      UNITS      0.25  1640    ENGINE SERIAL NUMBER 2 LSP

BlueMax Variables
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


bool ConvertNasaTime(std::string sNasaTime, double *fDecodedTime);

// ----------------------------------------------------------------------------
// Constructor / Destructor
// ----------------------------------------------------------------------------

ClSource_NasaNavTxt::ClSource_NasaNavTxt(ClSimState * pclSimState, std::string sPrefix)
    {
    this->pclSimState = pclSimState;
    this->sPrefix     = sPrefix;
    }


// ----------------------------------------------------------------------------

ClSource_NasaNavTxt::~ClSource_NasaNavTxt()
    {
    Close();
    }


// ----------------------------------------------------------------------------
// Methods
// ----------------------------------------------------------------------------

void display_vector_contents(const STR& input_line, const CSV_FIELDS& output_fields)
{
    CONST_VECTOR_ITR it = output_fields.begin();
    int i = 0;

    for( ; it != output_fields.end(); ++it)
    {
        std :: cout << "Field [" << i++ << "] - " << *it << "\n";
    }
}

void display_map_contents(const STR& input_line, const KEY_VAL_FIELDS& output_map)
{
    CONST_MAP_ITR it = output_map.begin();
    for (; it != output_map.end(); ++it)
    {
        std :: cout << "Key - " << it->first << " ,Value - " << it->second << "\n";
    }
}

/// Open the NASA data file

bool ClSource_NasaNavTxt::Open(std::string sFilename)
    {
    char                szLine[2000];

    std::string         sDataLabelKey;
    bool                bCsvStatus;

    hNasaInput = fopen(sFilename.c_str(), "r");
    if (hNasaInput == NULL)
        return false;

    // Get the first header line
    fgets(szLine, sizeof(szLine), hNasaInput);

    // Trim any CR or LF at the end
    for (int iLineCharIdx = strlen(szLine)-1; iLineCharIdx > 0; iLineCharIdx--)
        if ((szLine[iLineCharIdx] == 10) || (szLine[iLineCharIdx] == 13))
            szLine[iLineCharIdx] = '\0';
        else
            break;

    // Parse the header line
//    State.clear();
    CsvDataLabels.clear();
    bCsvStatus = CsvParser.parse_line(szLine, CsvDataLabels);
    assert(bCsvStatus == true);

//    display_vector_contents(szLine, CsvFields);

    // Step through all the header labels found
    for (VECTOR_ITR itLabel = CsvDataLabels.begin(); itLabel != CsvDataLabels.end(); ++itLabel)
        {
        // Replace known standard data fields (latitude, longitude, etc.) with their standard data labels
        if      (*itLabel == "LATP") *itLabel = "AC_LAT";
        else if (*itLabel == "LONP") *itLabel = "AC_LON";
        else if (*itLabel == "ALT")  *itLabel = "AC_ALT";
        else if (*itLabel == "TAS")  *itLabel = "AC_TAS";
        else if (*itLabel == "TH")   *itLabel = "AC_TRUE_HDG";
        else if (*itLabel == "MH")   *itLabel = "AC_MAG_HDR";
        else if (*itLabel == "PTCH") *itLabel = "AC_PITCH";
        else if (*itLabel == "ROLL") *itLabel = "AC_ROLL";
        else if (*itLabel == "AOAC") *itLabel = "AC_AOA";
        else if (*itLabel == "VRTG") *itLabel = "AC_ACCEL_DOWN";

        // Make the data label with the appropriate prefix
        sDataLabelKey = sPrefix + *itLabel;

        // Put header values into the parser fields map
//        CsvHeaders.push_back(sDataLabelKey);

        // Insert an initial placeholder into SimState map
        pclSimState->insert(sDataLabelKey,-1.0);

        }
//    display_vector_contents(szLine, CsvDataLabels);

    // Make t

    // Create additional SimState variables for required derived parameters
    pclSimState->insert(sPrefix + "AC_TIME", -1.0);
    pclSimState->insert(sPrefix + "AC_VEL_NORTH", -1.0);
    pclSimState->insert(sPrefix + "AC_VEL_EAST", -1.0);
    pclSimState->insert(sPrefix + "AC_VEL_DOWN", -1.0);
    pclSimState->insert(sPrefix + "AC_ACCEL_NORTH", -1.0);
    pclSimState->insert(sPrefix + "AC_ACCEL_EAST", -1.0);

    // Initialize variables
    fStartTime = -1.0;

    return true;

    } // end Open()


// ----------------------------------------------------------------------------

/// Close the NASA data file

void ClSource_NasaNavTxt::Close()
    {
    if (hNasaInput != NULL)
        fclose(hNasaInput);

    return;
    } // end Close()

// ----------------------------------------------------------------------------

// Heading to radians
#define HDG2RAD(heading)    ((90.0 - heading) * M_PI / 180.0l)

// Gs to Ft/Sec^2
#define G2FPS2(accel)       (accel * 32.17)

// Knots to Ft/Sec
#define KTS2FPS(speed)      (speed * 6076.0 / 3600.0)

/// Read the next line of NASA data

bool ClSource_NasaNavTxt::ReadNextLine()
    {
    char                szLine[2000];
    bool                bCsvStatus;
    KEY_VAL_FIELDS      CsvMap;

    // Get the next line        
    fgets(szLine, sizeof(szLine), hNasaInput);
    if (feof(hNasaInput))
        return false;

    // Trim any CR or LF at the end
    for (int iLineCharIdx = strlen(szLine)-1; iLineCharIdx > 0; iLineCharIdx--)
        if ((szLine[iLineCharIdx] == 10) || (szLine[iLineCharIdx] == 13))
            szLine[iLineCharIdx] = '\0';
        else
            break;

    // Parse the input data line
    bCsvStatus = CsvParser.parse_line(szLine, CsvDataLabels, CsvMap);
    assert(bCsvStatus == true);
//    display_map_contents(szLine, CsvMap);

    // Copy parsed data into the SimState variable. Yeah, I know copying large quantities
    // of data is bad for performance but that is how it is done for now. In the future I 
    // may just copy a subset of data fields but that has its own set of performance
    // implications.
    
    for (CONST_MAP_ITR itCsvMap = CsvMap.begin(); itCsvMap != CsvMap.end(); ++itCsvMap)
        {
        double  fDecodedVal;

        fDecodedVal = std::stod(itCsvMap->second);

        // Handle any special conversion cases
        if (itCsvMap->first == sPrefix + "DATE_TIME_ABS")
            {
            bool    bStatus;
            double  fDecodedTime;
            double  fRelTime;

            bStatus = ConvertNasaTime(itCsvMap->second, &fDecodedTime);
            assert(bStatus);
            pclSimState->update(sPrefix+"DATE_TIME_ABS", fDecodedTime);

            // Check for first time to set start time
            if (fStartTime < 0)
                fStartTime = fDecodedTime;

            // Make and store relative time
            fRelTime = fDecodedTime - fStartTime;
            pclSimState->update(sPrefix+"AC_TIME", fRelTime);
            }

        // Convert G's to fps^2
        else if (itCsvMap->first == sPrefix + "AC_ACCEL_DOWN")
            pclSimState->update(sPrefix+itCsvMap->first, G2FPS2(std::stod(itCsvMap->second)));

        // Default is to convert to a double and store it
        else
            // I wonder if the "sPrefix+" construct should be optimized. It seems like
            // a lot of string concatination.
            pclSimState->update(sPrefix+itCsvMap->first, std::stod(itCsvMap->second));
        }

    // After data is copied then calculate any necessary derived parameters
    pclSimState->update(sPrefix+"AC_VEL_NORTH",   (double)(KTS2FPS(pclSimState->fState["GS"]) * sin(HDG2RAD(pclSimState->fState["AC_TRUE_HDG"]))));
    pclSimState->update(sPrefix+"AC_VEL_EAST",    (double)(KTS2FPS(pclSimState->fState["GS"]) * cos(HDG2RAD(pclSimState->fState["AC_TRUE_HDG"]))));
    pclSimState->update(sPrefix+"AC_VEL_DOWN",    (double)(-1.0 * pclSimState->fState["IVV"] / 60.0));
    pclSimState->update(sPrefix+"AC_ACCEL_NORTH", (double)(G2FPS2(pclSimState->fState["FPAC"]) * sin(HDG2RAD(pclSimState->fState["AC_TRUE_HDG"]))));
    pclSimState->update(sPrefix+"AC_ACCEL_EAST",  (double)(G2FPS2(pclSimState->fState["FPAC"]) * cos(HDG2RAD(pclSimState->fState["AC_TRUE_HDG"]))));

    return true;
    }


// ----------------------------------------------------------------------------

bool ConvertNasaTime(std::string sNasaTime, double *fDecodedTime)
    {
    // 2004-02-02 06:30:24.000

    struct tm   suNasaTime;
    time_t      lNasaTime;
    double      fSecond;
    int         iTokens;

    // Decode the time string
    iTokens = sscanf(sNasaTime.c_str(), "%d-%d-%d %d:%d:%lf", 
            &suNasaTime.tm_year, &suNasaTime.tm_mon, &suNasaTime.tm_mday,
            &suNasaTime.tm_hour, &suNasaTime.tm_min, &fSecond);
    if (iTokens != 6)
        return false;

    // Fix up some tm fields
    suNasaTime.tm_year -= 1900;
    suNasaTime.tm_mon  -= 1;
    suNasaTime.tm_sec   = (int)fSecond;

    // Convert to a time_t
    lNasaTime = mktime(&suNasaTime);

    // Make a floating point representation
    *fDecodedTime = lNasaTime + (fSecond - suNasaTime.tm_sec);

    return true;
    }