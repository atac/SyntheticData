#pragma once

#include <string>
#include <unordered_map>

class ClSimState
{
public:
  ClSimState() {};
  ~ClSimState() 
  {
    for (auto i = blobState.begin(); i != blobState.end(); i++)
      if (i->second != nullptr) {
        delete i->second;
        i->second = nullptr;
      }
  };

  // Data
public:
  std::unordered_map<std::string, double>     fState;
  std::unordered_map<std::string, bool>       bState;
  std::unordered_map<std::string, long>       lState;
  std::unordered_map<std::string, std::vector<uint8_t>*> blobState;

  // Methods
public:
  void clear();
  void insert(std::string sKey, double fValue);
  void update(std::string sKey, double fValue);
  void insert(std::string sKey, bool   bValue);
  void update(std::string sKey, bool   bValue);
  void insert(std::string sKey, long   lValue);
  void update(std::string sKey, long   lValue);
  void insert(std::string sKey, std::vector<uint8_t>* blob);
  void update(std::string sKey, std::vector<uint8_t>* blob);

  void SetSimClockTime(double* currTime);
  double GetCurrSimClockTime();

private:
  double* currentTime;
};

// ----------------------------------------------------------------------------

inline void ClSimState::SetSimClockTime(double* currTime) {
  currentTime = currTime;
}

inline double ClSimState::GetCurrSimClockTime() {
  return *currentTime;
}


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

inline void ClSimState::insert(std::string sKey, std::vector<uint8_t>* blob)
{
  blobState.insert(std::pair<std::string, std::vector<uint8_t>*>(sKey, blob));
}

inline void ClSimState::update(std::string sKey, std::vector<uint8_t>* blob)
{
  std::vector<uint8_t>* tmp = blobState[sKey];
  blobState[sKey] = blob;
  if (tmp != nullptr) {
    delete tmp;
    tmp = nullptr;
  }
}

