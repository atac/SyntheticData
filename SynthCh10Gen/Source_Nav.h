#pragma once

#include "SimState.h"

class ClSource_Nav
    {
    public:
        ClSource_Nav(ClSimState * pclSimState, std::string sPrefix) {};
        ~ClSource_Nav() {};

    // Class variables
protected:

public:
    std::string         sPrefix;
    ClSimState        * pclSimState;

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
    virtual bool Open(std::string sFilename) = 0;
    virtual void Close()                     = 0;
    virtual bool ReadNextLine()              = 0;

    };

