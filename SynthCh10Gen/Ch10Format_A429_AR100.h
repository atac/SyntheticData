#pragma once

// irig106lib
#include "config.h"
#include "i106_stdint.h"
#include "irig106ch10.h"
#include "i106_time.h"
#include "i106_decode_arinc429.h"

#include "Ch10Format_A429.h"

// ----------------------------------------------------------------------------
// ClCh10Format_ARINC429_AR100
// ----------------------------------------------------------------------------

class ClCh10Format_ARINC429_AR100 : public ClCh10Format_ARINC429
    {


public:
    ClCh10Format_ARINC429_AR100(int iBusNum, int iBusSpeed, int iEngineNum);
//    ~ClCh10Format_ARINC429_AR100();

    int     iEngineNum;

    char    szSimStateHash_Lbl41[50];   // N1.x
    char    szSimStateHash_Lbl42[50];   // N1C
    char    szSimStateHash_Lbl43[50];   // OIP.x
    char    szSimStateHash_Lbl44[50];   // N2.x
    char    szSimStateHash_Lbl45[50];   // EGT.x
    char    szSimStateHash_Lbl46[50];   // OIT.x
    char    szSimStateHash_Lbl47[50];   // FF.x

    // Data structure for ARINC-429 AR100 format data
#pragma pack(push,1)
    struct SuLabel_41                   // Synthetic Data ARINC-429 AR100 Label 41
        {
        uint32_t    uLabel      :  8;   // Octal label 41
        uint32_t    uSdiCode    :  2;   // SDI Code
        uint32_t                :  7;   // 
        uint32_t    uRpmN1Act   : 11;   // Engine Fan RPM N1 Actual
        uint32_t                :  1;   // 
        uint32_t    uSsmCode    :  2;   // SSM Code
        uint32_t    uParity     :  1;   // Odd Parity
        } * psuLabel_41;

    struct SuLabel_42                   // Synthetic Data ARINC-429 AR100 Label 42
        {
        uint32_t    uLabel      :  8;   // Octal label 42
        uint32_t    uSdiCode    :  2;   // SDI Code
        uint32_t                :  7;   // 
        uint32_t    uRpmN1Dem   : 11;   // Engine Fan RPM N1 Demand
        uint32_t                :  1;   // 
        uint32_t    uSsmCode    :  2;   // SSM Code
        uint32_t    uParity     :  1;   // Odd Parity
        } * psuLabel_42;

    struct SuLabel_43                   // Synthetic Data ARINC-429 AR100 Label 43
        {
        uint32_t    uLabel      :  8;   // Octal label 43
        uint32_t    uSdiCode    :  2;   // SDI Code
        uint32_t    uSenStatus  :  1;   // Sensor Data Status
        uint32_t    iEngOilPres : 18;   // Engine Oil Pressure
        uint32_t    uSsmCode    :  2;   // SSM Code
        uint32_t    uParity     :  1;   // Odd Parity
        } * psuLabel_43;

    struct SuLabel_44                   // Synthetic Data ARINC-429 AR100 Label 44
        {
        uint32_t    uLabel      :  8;   // Octal label 44
        uint32_t    uSdiCode    :  2;   // SDI Code
        uint32_t                :  7;   // 
        uint32_t    uRpmN2Act   : 11;   // Engine Turbine RPM N2
        uint32_t                :  1;   // 
        uint32_t    uSsmCode    :  2;   // SSM Code
        uint32_t    uParity     :  1;   // Odd Parity
        } * psuLabel_44;

    struct SuLabel_45                   // Synthetic Data ARINC-429 AR100 Label 45
        {
        uint32_t    uLabel      :  8;   // Octal label 45
        uint32_t    uSdiCode    :  2;   // SDI Code
        uint32_t                :  7;   // 
        uint32_t    uEGT        : 11;   // Exhaust Gas Temperature (EGT)
        uint32_t                :  1;   // 
        uint32_t    uSsmCode    :  2;   // SSM Code
        uint32_t    uParity     :  1;   // Odd Parity
        } * psuLabel_45;

    struct SuLabel_46                   // Synthetic Data ARINC-429 AR100 Label 46
        {
        uint32_t    uLabel      :  8;   // Octal label 46
        uint32_t    uSdiCode    :  2;   // SDI Code
        uint32_t                :  9;   // 
        uint32_t    iEngOilTemp : 10;   // Engine Oil Temperature C
        uint32_t    uSsmCode    :  2;   // SSM Code
        uint32_t    uParity     :  1;   // Odd Parity
        } * psuLabel_46;

    struct SuLabel_47                   // Synthetic Data ARINC-429 AR100 Label 47
        {
        uint32_t    uLabel      :  8;   // Octal label 47
        uint32_t    uSdiCode    :  2;   // SDI Code
        uint32_t                :  4;   // 
        uint32_t    uFuelFlow   : 14;   // Fuel Flow
        uint32_t                :  1;   // 
        uint32_t    uSsmCode    :  2;   // SSM Code
        uint32_t    uParity     :  1;   // Odd Parity
        } * psuLabel_47;
#pragma pack(pop)

    // Methods
public:
    void MakeMsg(ClSimState * pclSimState);
    std::string TMATS(ClTmatsIndexes & TmatsIndex, std::string sCDLN);

    };

