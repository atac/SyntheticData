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
        ClSource_Nav(ClSimState * pclSimState, std::string sPrefix) {};
        ~ClSource_Nav() {};

    // Class variables
protected:

public:
    std::string         sPrefix;        // Simulation state data label prefix to make it unique
    ClSimState        * pclSimState;    // Pointer to the simulation state 
    double              fStartTime;     // Data start clock time (time_t)
    double              fRelTime;       // Current data time offset (seconds) since fStartTime

    enum EnInputType  { InputUnknown,  InputBMSqlite,  InputBMText, InputNasaCsv, InputNasaSqlite } ;

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
    virtual bool Open(std::string sFilename)            = 0;
    virtual void Close()                                = 0;
    virtual bool ReadNextLine()                         = 0;
    virtual bool UpdateSimState(double fSimElapsedTime) = 0;

    };

