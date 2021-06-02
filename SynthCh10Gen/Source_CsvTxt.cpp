
#include <cassert>
#include <cmath>
#include <chrono>
#include <cstdio>
#include <string>

#include "Source_Nav.h"
#include "Source_CsvTxt.h"

// ----------------------------------------------------------------------------
// Constructor / Destructor
// ----------------------------------------------------------------------------

ClSource_CsvTxt::ClSource_CsvTxt(ClSimState * pclSimState, std::string sPrefix) :
    ClSource_Nav(pclSimState, sPrefix)
    {
    this->pclSimState = pclSimState;
    this->sPrefix     = sPrefix;
    this->enInputType = this->InputNasaCsv;
    this->hCsvInput   = NULL;
    }


// ----------------------------------------------------------------------------

ClSource_CsvTxt::~ClSource_CsvTxt()
    {
    Close();
    }


// ----------------------------------------------------------------------------
// Methods
// ----------------------------------------------------------------------------

// These are useful for debugging

void display_vector_contents(const STR& input_line, const CSV_FIELDS& output_fields)
    {
    CONST_VECTOR_ITR it = output_fields.begin();
    int i = 0;

    for ( ; it != output_fields.end(); ++it)
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


// ----------------------------------------------------------------------------

/// Open the CSV data file and read the header line. The first line must be a
/// header with variable names. The first column of data must be DATE_TIME.

bool ClSource_CsvTxt::Open(std::string sFilename)
    {
    char                szLine[2000];   // Make sure this is big enough!
    bool                bCsvStatus;

    hCsvInput = fopen(sFilename.c_str(), "r");
    if (hCsvInput == NULL)
        return false;

    // Get the first header line
    fgets(szLine, sizeof(szLine), hCsvInput);

    // Trim any CR or LF at the end
    for (int iLineCharIdx = strlen(szLine)-1; iLineCharIdx > 0; iLineCharIdx--)
        if ((szLine[iLineCharIdx] == 10) || (szLine[iLineCharIdx] == 13))
            szLine[iLineCharIdx] = '\0';
        else
            break;

    // Parse the header line
    CsvDataLabels.clear();
    bCsvStatus = CsvParser.parse_line(szLine, CsvDataLabels);
    assert(bCsvStatus == true);

//    display_vector_contents(szLine, CsvFields);

    // Get the sim state variables ready
    Init();

    return true;

    } // end Open()


// ----------------------------------------------------------------------------

// Read the header line, initialize the sim state, and otherwise get ready.

void ClSource_CsvTxt::Init()
    {
    std::string         sDataLabelKey;

    // Step through all the header labels found
    for (VECTOR_ITR itLabel = CsvDataLabels.begin(); itLabel != CsvDataLabels.end(); ++itLabel)
        {
        // Make the data label with the appropriate prefix
        sDataLabelKey = sPrefix + *itLabel;

        // Insert an initial placeholder into SimState map
        // Note that it is assumed the data can be represented with a floating point. If this
        // isn't the case it needs to be fixed in a derived class.
        pclSimState->insert(sDataLabelKey,-1.0);
        } // end for all header labels

    // Get the first line of data and figure out the start time
    fStartTime = 0.0;
    ReadNextLine();
    fStartTime = fRelTime;

    // Since we are at the beginning of the data file reset the relative time to 0.0
    fRelTime = 0.0;

    return;
    }


// ----------------------------------------------------------------------------

/// Close the CSV data file

void ClSource_CsvTxt::Close()
    {
    if (hCsvInput != NULL)
        fclose(hCsvInput);

    hCsvInput = NULL;
    return;
    } // end Close()


// ----------------------------------------------------------------------------

/// Read the next line of CSV data

bool ClSource_CsvTxt::ReadNextLine()
    {
    char                szLine[2000];
    bool                bCsvStatus;
    bool                bStatus;
    double              fDecodedTime;

    // Get the next line        
    fgets(szLine, sizeof(szLine), hCsvInput);
    if (feof(hCsvInput))
        return false;

    // Trim any CR or LF at the end
    for (int iLineCharIdx = strlen(szLine)-1; iLineCharIdx > 0; iLineCharIdx--)
        if ((szLine[iLineCharIdx] == 10) || (szLine[iLineCharIdx] == 13))
            szLine[iLineCharIdx] = '\0';
        else
            break;

    // Parse the input data line
    CsvMap.clear();
    bCsvStatus = CsvParser.parse_line(szLine, CsvDataLabels, CsvMap);
    assert(bCsvStatus == true);

//    display_map_contents(szLine, CsvMap);

    // Decode the current data time
    bStatus = ConvertTime(CsvMap[sPrefix + "DATE_TIME"], &fDecodedTime);
    assert(bStatus == true);
    fRelTime = fDecodedTime - fStartTime;

    return true;
    } // end ReadNextLine()


// ----------------------------------------------------------------------------

/// Copy the current simulation data into the current sim state at the 
/// appropriate time.
/// Return false when end of file.

bool ClSource_CsvTxt::UpdateSimState(double fSimElapsedTime)
    {
    bool    bStatus;

    // Return if simulation time is less than current data time from this source
    if (fSimElapsedTime < fRelTime)
        return true;

    // Copy parsed data into the SimState variable.
    for (CONST_MAP_ITR itCsvMap = CsvMap.begin(); itCsvMap != CsvMap.end(); ++itCsvMap)
        {
        // I wonder if the "sPrefix+" construct should be optimized. It seems like
        // a lot of string concatination.

        // Handle any special conversion cases
        if (itCsvMap->first == sPrefix + "DATE_TIME")
            {
            // Relative time has already been calculated so store it
            pclSimState->update(sPrefix+"AC_TIME", fRelTime);
            }

        // Default is to convert to a double and store it
        else
            {
//            double fDecodedVal = std::stod(itCsvMap->second);
            pclSimState->update(sPrefix+itCsvMap->first, std::stod(itCsvMap->second));
            } // end if default copy
        } // end for all CSV labeled data

    // Get the next line of data
    bStatus = ReadNextLine();

    return bStatus;
    }


// ----------------------------------------------------------------------------

// Convert time string to a time value. Time must be in the form
//   2001-01-12 09:15:46.140
// The output time value is a floating point representation of a time_t.

bool ClSource_CsvTxt::ConvertTime(std::string sTime, double *fDecodedTime)
    {
    struct tm   suNasaTime;
    time_t      lNasaTime;
    double      fSecond;
    int         iTokens;

    // Decode the time string
    iTokens = sscanf(sTime.c_str(), "%d-%d-%d %d:%d:%lf", 
            &suNasaTime.tm_year, &suNasaTime.tm_mon, &suNasaTime.tm_mday,
            &suNasaTime.tm_hour, &suNasaTime.tm_min, &fSecond);
    if (iTokens != 6)
        return false;

    // Fix up some tm fields
    suNasaTime.tm_year -= 1900;
    suNasaTime.tm_mon  -= 1;
    suNasaTime.tm_sec   = (int)fSecond;

    // Convert to a time_t
    lNasaTime = _mkgmtime(&suNasaTime);

    // Make a floating point representation
    *fDecodedTime = lNasaTime + (fSecond - suNasaTime.tm_sec);

    return true;
    }