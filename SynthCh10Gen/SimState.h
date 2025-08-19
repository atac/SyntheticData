#pragma once

#include <string>
#include <unordered_map>

class ClSimState
{
public:
  ClSimState() {};
  ~ClSimState() 
  {
    clear();
  };

public:
  typedef std::vector<uint8_t>* Blob;

  std::unordered_map<std::string, double>     fState;
  std::unordered_map<std::string, bool>       bState;
  std::unordered_map<std::string, long>       lState;
  std::unordered_map<std::string, Blob>       blobState;

  std::unordered_map<std::string, bool>       readyState;

  // Methods
public:
  void clear();
  void insert(std::string sKey, double fValue);
  void insert(std::string sKey, bool   bValue);
  void insert(std::string sKey, long   lValue);
  void insert(std::string sKey, Blob blob);

  void update(std::string sKey, double fValue);
  void update(std::string sKey, bool   bValue);
  void update(std::string sKey, long   lValue);
  void update(std::string sKey, Blob blob);

  void insertReady(std::string sSrcPrefix);
  void updateReady(std::string sSrcPrefix, bool bValue);


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
  readyState.clear();

  fState.clear();
  bState.clear();
  lState.clear();

  for (auto i = blobState.begin(); i != blobState.end(); i++)
  {
    if (i->second != nullptr) {
      delete i->second;
      i->second = nullptr;
    }
  }
  blobState.clear();
}

// double
inline void ClSimState::insert(std::string sKey, double fValue)
{
  fState.insert(std::pair(sKey, fValue));
}

inline void ClSimState::update(std::string sKey, double fValue)
{
  fState[sKey] = fValue;
}

// bool
inline void ClSimState::insert(std::string sKey, bool bValue)
{
  bState.insert(std::pair(sKey, bValue));
}

inline void ClSimState::update(std::string sKey, bool bValue)
{
  bState[sKey] = bValue;
}

// long
inline void ClSimState::insert(std::string sKey, long lValue)
{
  bState.insert(std::pair(sKey, lValue));
}

inline void ClSimState::update(std::string sKey, long lValue)
{
  lState[sKey] = lValue;
}

inline void ClSimState::insert(std::string sKey, Blob blob)
{
  blobState.insert(std::pair(sKey, blob));
}

inline void ClSimState::update(std::string sKey, Blob blob)
{
  Blob* orig = &blobState[sKey];
  Blob tmp = *orig;
  *orig = blob;
  if (tmp != nullptr) {
    delete tmp;
    tmp = nullptr;
  }
}

inline void ClSimState::insertReady(std::string sSrcPrefix)
{
  readyState.insert(std::pair(sSrcPrefix, false));
}

inline void ClSimState::updateReady(std::string sSrcPrefix, bool bValue)
{
  readyState[sSrcPrefix] = bValue;
}