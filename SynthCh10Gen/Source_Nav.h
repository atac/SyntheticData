/* Source_State.h - Base class for all simulation state input operations. 

This is a pure virtual class. Other specific types of state
input will derive from this class. But in the simulation engine during
the simulation run the UpdateSimState() method will be called for each
clock tick, giving each object derived from this class an opportunity
to add or update simulation state values at that particular instant
of time.
*/

#pragma once

#include "SimState.h"
#include "SimTimer.h"
#include "Config.h"
#include "TimeParser.h"

// https://github.com/rakeshgk/csv-parser
#include "csv_parser.hpp"

// Here are some conversions useful for anyone doing nav math
// ----------------------------------------------------------

// Heading to radians
#define HDG2RAD(heading)    ((90.0 - heading) * M_PI / 180.0l)

// Gs to Ft/Sec^2
#define G2FPS2(accel)       (accel * 32.17)

// Knots to Ft/Sec
#define KTS2FPS(speed)      (speed * 6076.0 / 3600.0)


// ----------------------------------------------------------------------------
// ClSource_Nav base class
// ----------------------------------------------------------------------------

class ClSource_Nav
{
public:
  ClSource_Nav() {};
  ~ClSource_Nav() {};

  // Class variables
protected:
  ConfigMapping mapping;
  TimeParser timeParser;

  bool eof;

public:
  std::string         sPrefix;        // Simulation state data label prefix to make it unique
  ClSimState*         pclSimState;    // Pointer to the simulation state 
  double              fStartTime;     // Data start clock time (time_t)
  double              fRelTime;       // Current data time offset (seconds) since fStartTime

  CSV_FIELDS DataLabels;
  CSV_FIELDS DataTypes;

  enum EnInputType { InputUnknown, InputBMSqlite, InputBMText, InputNasaCsv, InputNasaSqlite };

  EnInputType         enInputType;
  bool                bHavePosition;
  bool                bHaveAltitude;
  bool                bHaveAirspeed;
  bool                bHaveVelocity;
  bool                bHaveAcceleration;
  bool                bHaveTrueHeading;
  bool                bHaveMagHeading;
  bool                bHavePitchRoll;
  bool                bHaveAOA;
  bool                bHaveThrottle;

  // Methods
public:
  virtual bool Open(std::string sFilename)            = 0; // Open data source, init as needed, and read first line
  virtual void Close()                                = 0; // Clean up and close data source
  virtual bool ReadNextLine()                         = 0; // Advance source by one line
  virtual bool UpdateSimState(double fSimElapsedTime) = 0; // Advance source to current sim time, update state, and readnextline
  virtual void SetMapping(ConfigMapping map)          = 0; // Set a mapping between source and simstate labels


protected:
  virtual void ApplyMapping()                         = 0; // Apply the mapping to the instance's labels
  virtual void ApplyPrefix()                          = 0; // Apply the data source prefix to the instance's labels (do not call before mapping)
};

