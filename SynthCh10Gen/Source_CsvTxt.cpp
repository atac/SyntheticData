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

ClSource_CsvTxt::ClSource_CsvTxt(ClSimState* pclSimState, std::string sPrefix)
{
  this->pclSimState = pclSimState;
  this->sPrefix = sPrefix;
  this->enInputType = this->InputNasaCsv;
  this->hCsvInput = NULL;
  this->eof = false;

  this->fTimeShift = 0.0;
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

void display_vector_contents(const STR& input_line, const StringList& output_fields)
{
  auto it = output_fields.begin();
  int i = 0;

  for (; it != output_fields.end(); ++it)
  {
    std::cout << "Field [" << i++ << "] - " << *it << "\n";
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

bool ClSource_CsvTxt::HasNumericData(StringList &values) {
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
  this->fileSize = filesystem::file_size(sFilename.c_str());

  hCsvInput = fopen(sFilename.c_str(), "r");
  if (hCsvInput == NULL)
    return false;

  return Init();

} // end Open()


// ----------------------------------------------------------------------------

// Read the header line, initialize the sim state, and otherwise get ready.

bool ClSource_CsvTxt::Init()
{
  const size_t        maxLength = 2000;
  char                szLine[maxLength];   // Make sure this is big enough!
  bool                bCsvStatus;

  if (!ReadLineToBuffer(szLine, maxLength, nullptr))
    return false;

  // Parse the header line
  StringList fieldNames;
  bCsvStatus = ParseLine(szLine, fieldNames);
  if (!bCsvStatus || fieldNames.empty())
    return false;

  // Look for column types within the next two lines
  StringList fieldTypes;
  StringList tmpFields;
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
      fieldTypes = tmpFields;
  }

  if (!fieldTypes.empty()) {
    FieldTypeList types;
    for (string t : fieldTypes) {
      types.push_back(GetFieldType(t));
    }
    fields = FieldSet(fieldNames, types);
  }
  else
    fields = FieldSet(fieldNames);

  fields.applyMapping(this->mapping);
  fields.applyPrefix(this->sPrefix);


  // Step through all the header labels found
  for (auto f = fields.begin(); f != fields.end(); ++f)
  {
    // Insert an initial placeholder into SimState map
    // Note that it is assumed the data can be represented with a floating point. If this
    // isn't the case it needs to be fixed in a derived class.
    pclSimState->insert((f->getID()), -1.0);
  } // end for all header labels

  if (!sPrefix.empty())
    pclSimState->insertReady(sPrefix);


  if (!GetTimes())
    return false;

  ReadNextLine(); // prep the data for the first iteration

  return true;
}

bool ClSource_CsvTxt::GetTimes() {
  const size_t        maxLength = 2000;
  char                tmp[maxLength];
  char                szLine[maxLength];   // Make sure this is big enough!
  fpos_t startPos;

  // Parse the first line's first field to determine time parsing format
  if (!ReadLineToBuffer(szLine, maxLength, &startPos))
    return false;

  StringList tmpFields;
  bool status = ParseLine(szLine, tmpFields);
  if (!status || tmpFields.empty())
    return false;

  timeParser.Init(tmpFields.at(0));
  assert(timeParser.Valid());

  // parse start time
  if (!timeParser.Parse(tmpFields.at(0), fStartTime))
    return false;

  // Find time from last line
  int offset = 1024;
  if (offset > fileSize)
    offset = fileSize;

  bool foundTime = false;
  while (!foundTime) { // every iteration, go back farther from eof
    fseek(hCsvInput, fileSize - offset, SEEK_SET);

    // Read lines until eof
    // Last successful read is the last line
    int linesRead = 0;
    while (ReadLineToBuffer(tmp, maxLength))
    {
      memcpy(szLine, tmp, maxLength);
      linesRead++;
    }

    if (linesRead > 1) { // this guarantees last line read is not partial
      tmpFields.clear();
      bool status = ParseLine(szLine, tmpFields);
      if (status && !tmpFields.empty()) {
        if (timeParser.Parse(tmpFields[0], fEndTime))
          foundTime = true;
        else
          break; // failed to find time
      }
      else
        break; // failed to find time
    }
  }

  fsetpos(hCsvInput, &startPos);

  return true;
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
      bStatus = ParseLineToMap(szLine, fieldValueMap);
      if (fieldValueMap.empty())
        bStatus = false;
    }
    else
      eof = true;

    if (bStatus) {
      // Decode the current data time
      bStatus = timeParser.Parse(fieldValueMap[fields.begin()->getID()], fDecodedTime);
      assert(bStatus == true);
      fRelTime = fDecodedTime - fStartTime;
      dataAvailable = true;
    }

    return dataAvailable;
    } // end ReadNextLine()


// ----------------------------------------------------------------------------

/// Copy the current simulation data into the current sim state at the 
/// appropriate time.
/// Return false when end of file.

bool ClSource_CsvTxt::UpdateSimState(double fSimElapsedTime)
{
  bool bStatus = true;
  string timeFieldID = fields.begin()->getID();

  fSimElapsedTime -= fTimeShift; // apply shift by changing the apparent elapsed time

  while (fSimElapsedTime + TIME_COMPARE_MARGIN >= fRelTime)
  {
    if (eof)
      return false;

    // Copy current data row into the SimState
    for (CONST_MAP_ITR itValueMap = fieldValueMap.begin(); itValueMap != fieldValueMap.end(); ++itValueMap)
    {
      if (itValueMap->first == timeFieldID)
        pclSimState->update(timeFieldID, fRelTime);
      else
        pclSimState->update(itValueMap->first, std::stod(itValueMap->second));
    }

    // Get the next line of data
    bStatus = ReadNextLine();

    pclSimState->updateReady(this->sPrefix, bStatus);
  }

  return bStatus;
}


void ClSource_CsvTxt::SetMapping(ConfigMapping map) {
  this->mapping = map;
}

bool ClSource_CsvTxt::ParseLine(char* szLine, StringList& values)
{
  return CsvParser.parse_line(szLine, values);
}

bool ClSource_CsvTxt::ParseLineToMap(char* szLine, KEY_VAL_FIELDS& valueMap)
{
  valueMap.clear();
  return CsvParser.parse_line(szLine, fields.getFieldIDs(), valueMap);
}


FieldType ClSource_CsvTxt::GetFieldType(string typeString) {
  transform(typeString.begin(), typeString.end(), typeString.begin(), ::tolower);

  if (typeString == "integer")
    return FieldType::INTEGER_FIELD;
  else
    return FieldType::FLOAT_FIELD;
}