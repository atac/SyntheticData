#pragma once

#include <string>
#include <unordered_map>

class ClSimState
    {
public:
    ClSimState() {};
    ~ClSimState() {};

    // Data
public:
    std::unordered_map<std::string, double>     fState;
    std::unordered_map<std::string, bool>       bState;
    std::unordered_map<std::string, long>       lState;

    // Methods
public:
    void clear();
    void insert(std::string sKey, double fValue);
    void update(std::string sKey, double fValue);
    void insert(std::string sKey, bool   bValue);
    void update(std::string sKey, bool   bValue);
    void insert(std::string sKey, long   lValue);
    void update(std::string sKey, long   lValue);

    };

// ----------------------------------------------------------------------------


inline void ClSimState::clear()
    {
    fState.clear();
    bState.clear();
    lState.clear();
    }

// double
inline void ClSimState::insert(std::string sKey, double fValue) 
    { 
    fState.insert(std::pair<std::string,double>(sKey, fValue)); 
    }

inline void ClSimState::update(std::string sKey, double fValue) 
    { 
    fState[sKey] = fValue; 
    }

// bool
inline void ClSimState::insert(std::string sKey, bool bValue) 
    { 
    bState.insert(std::pair<std::string,bool>(sKey, bValue)); 
    }

inline void ClSimState::update(std::string sKey, bool bValue) 
    { 
    bState[sKey] = bValue; 
    }

// long
inline void ClSimState::insert(std::string sKey, long lValue) 
    { 
    bState.insert(std::pair<std::string,long>(sKey, lValue)); 
    }

inline void ClSimState::update(std::string sKey, long lValue) 
    { 
    lState[sKey] = lValue; 
    }

