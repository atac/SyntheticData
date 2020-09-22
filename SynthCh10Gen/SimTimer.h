#pragma once

#include <stdlib.h>
#include <cassert>
#include <string>       // std::string

class ClSimTimer
    {
    public:
        ClSimTimer(int64_t lTimeoutVal)  { this->lTimer = 0; this->lTimeoutVal = lTimeoutVal; }
        ~ClSimTimer() { }

    // Data
public:
    static int64_t  lTicksPerSecond;
    static int64_t  lTicksPerStep;
    static int64_t  lSimClockTicks;
    static double   fSimElapsedTime;

    int64_t     lTimer;
    int64_t     lTimeoutVal;

    // Methods
public:
    static void Tick()              { lSimClockTicks += lTicksPerStep; fSimElapsedTime = (double)lSimClockTicks/(double)lTicksPerSecond; }
    static void Tick(int64_t lStep) { lSimClockTicks += lStep; }

    bool Expired()  { return lSimClockTicks >= lTimer; }
    void FromNow()  { this->lTimer = lSimClockTicks + lTimeoutVal; }
    void FromPrev() { this->lTimer += lTimeoutVal; }
    };

