/* Source_CsvTxt - Open a CSV file and load that data into the simulation state line by line.
*/


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

ClSource_CsvTxt::ClSource_CsvTxt(ClSimState * pclSimState, std::string sPrefix)
    {
    this->pclSimState = pclSimState;
    this->sPrefix     = sPrefix;
    this->enInputType = this->InputNasaCsv;
    this->hCsvInput   = NULL;
    this->eof = false;
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


bool ClSource_CsvTxt::ReadLineToBuffer(char* buf, size_t bufLen, fpos_t* lastPosition)
{
  if (lastPosition != nullptr)
    fgetpos(hCsvInput, lastPosition);

  fgets(buf, bufLen, hCsvInput);
  if (feof(hCsvInput))
    return false;

  // Trim any CR or LF at the end
  for (int iLineCharIdx = strlen(buf) - 1; iLineCharIdx > 0; iLineCharIdx--)
    if ((buf[iLineCharIdx] == 10) || (buf[iLineCharIdx] == 13))
      buf[iLineCharIdx] = '\0';
    else
      break;

  return true;
}

bool ClSource_CsvTxt::HasNumericData(CSV_FIELDS &values) {
  bool foundNumber = false;
  for (auto i = values.begin(); i != values.end(); i++) {
    try {
      std::stof(*i);
      foundNumber = true;
    }
    catch (const std::invalid_argument& ia) {
    }
  }

  return foundNumber;
}

// ----------------------------------------------------------------------------

/// Open the CSV data file and read the header line. The first line must be a
/// header with variable names. The first column of data must be time.

bool ClSource_CsvTxt::Open(std::string sFilename)
{
  const size_t        maxLength = 2000;
  char                szLine[maxLength];   // Make sure this is big enough!
  bool                bCsvStatus;

  hCsvInput = fopen(sFilename.c_str(), "r");
  if (hCsvInput == NULL)
    return false;

  if (!ReadLineToBuffer(szLine, maxLength, nullptr))
    return false;

  // Parse the header line
  DataLabels.clear();
  bCsvStatus = ParseLine(szLine, DataLabels);
  if (!bCsvStatus || DataLabels.empty())
    return false;

  // Apply any field name mapping
  ApplyMapping();

  // Add prefix to data labels
  ApplyPrefix();

  // Look for column types within the next two lines
  CSV_FIELDS tmpFields;
  fpos_t lastLinePos;
  for (int i = 2; i > 0; i--)
  {
    if (!ReadLineToBuffer(szLine, maxLength, &lastLinePos))
      return false;

    tmpFields.clear();
    bCsvStatus = ParseLine(szLine, tmpFields);
    if (!bCsvStatus || tmpFields.empty())
      return false;

    if (HasNumericData(tmpFields)) {
      fsetpos(hCsvInput, &lastLinePos); // reset position before line
      break;
    }
    else
      DataTypes = tmpFields;
  }

  // Parse the first line's first field to determine time parsing format
  if (!ReadLineToBuffer(szLine, maxLength, &lastLinePos))
    return false;

  tmpFields.clear();
  bCsvStatus = ParseLine(szLine, tmpFields);
  if (!bCsvStatus || tmpFields.empty())
    return false;

  timeParser.Init(tmpFields.at(0));
  assert(timeParser.Valid());

  fsetpos(hCsvInput, &lastLinePos); // reset position before line

//    display_vector_contents(szLine, CsvFields);

  // Get the sim state variables ready
  Init();

  return true;

} // end Open()


// ----------------------------------------------------------------------------

// Read the header line, initialize the sim state, and otherwise get ready.

void ClSource_CsvTxt::Init()
    {
    // Step through all the header labels found
    for (VECTOR_ITR itLabel = DataLabels.begin(); itLabel != DataLabels.end(); ++itLabel)
        {
        // Insert an initial placeholder into SimState map
        // Note that it is assumed the data can be represented with a floating point. If this
        // isn't the case it needs to be fixed in a derived class.
        pclSimState->insert((*itLabel),-1.0);
        } // end for all header labels

    if (!sPrefix.empty())
      pclSimState->insertReady(sPrefix);

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
    const size_t        maxLength = 2000;
    char                szLine[maxLength];
    bool                bStatus;
    double              fDecodedTime;
    bool dataAvailable = false;

    // Get the next line
    bStatus = ReadLineToBuffer(szLine, maxLength);

    if (bStatus) {
      // Parse the input data line
      CsvMap.clear();
      bStatus = ParseLine(szLine, DataLabels, CsvMap);
      if (CsvMap.empty())
        bStatus = false;
    }
    else
      eof = true;

    if (bStatus) {
      // Decode the current data time
      bStatus = timeParser.Parse(CsvMap[DataLabels[0]], fDecodedTime);
      assert(bStatus == true);
      fRelTime = fDecodedTime - fStartTime;
      dataAvailable = true;
    }

    pclSimState->updateReady(this->sPrefix, dataAvailable);

    return dataAvailable;
    } // end ReadNextLine()


// ----------------------------------------------------------------------------

/// Copy the current simulation data into the current sim state at the 
/// appropriate time.
/// Return false when end of file.

bool ClSource_CsvTxt::UpdateSimState(double fSimElapsedTime)
{
  bool    bStatus;

  if (eof)
    return false;

  // Return if simulation time is less than current data time from this source
  if (fSimElapsedTime < fRelTime)
    return true;

  // Copy parsed data into the SimState
  for (CONST_MAP_ITR itCsvMap = CsvMap.begin(); itCsvMap != CsvMap.end(); ++itCsvMap)
  {
    if (itCsvMap->first == DataLabels[0])
      pclSimState->update(DataLabels[0], fRelTime);
    else
      pclSimState->update(itCsvMap->first, std::stod(itCsvMap->second));
  }

// Get the next line of data
  bStatus = ReadNextLine();

  return bStatus;
}


void ClSource_CsvTxt::SetMapping(ConfigMapping map) {
  this->mapping = map;
}

void ClSource_CsvTxt::ApplyMapping() {
  for (auto& [from, to] : this->mapping) {
    auto i = find(DataLabels.begin(), DataLabels.end(), from);
    if (i != DataLabels.end())
      (*i) = to;
  }
}

void ClSource_CsvTxt::ApplyPrefix() {
  for (auto i = DataLabels.begin(); i != DataLabels.end(); i++)
    (*i) = sPrefix + (*i);
}

bool ClSource_CsvTxt::ParseLine(char* szLine, CSV_FIELDS& fields)
{
  return CsvParser.parse_line(szLine, fields);
}

bool ClSource_CsvTxt::ParseLine(char* szLine, CSV_FIELDS& labels, KEY_VAL_FIELDS& fieldMap)
{
  return CsvParser.parse_line(szLine, labels, fieldMap);
}