
#include <assert.h>
#include <memory>
#include <string>
#include <iomanip>
#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream
#include <math.h>       // pow()

#include <bitset>

#include "Common.h"
#include "Ch10Format_A429_AR100.h"

/*
--- KC-46 Example Data ---

TUI1
Subch   Data   Label Speed PErr FErr Gap Time  Computed IRIG Time
   0  7F304583 (301)  HI    0    0         0  076:00:16:28:018884
   0  E07DD943 (302)  HI    0    0      3601  076:00:16:28:019244
   0  7F4D05C3 (303)  HI    0    0      3600  076:00:16:28:019604
   0  FF538123 (304)  HI    0    0      3601  076:00:16:28:019964
   0  E00C09A3 (305)  HI    0    0      3599  076:00:16:28:020324
   0  FC000163 (306)  HI    0    0      3600  076:00:16:28:020684
   0  7F304583 (301)  HI    0    0      6999  076:00:16:28:021384
   0  607E5943 (302)  HI    0    0      3601  076:00:16:28:021744
   0  7F4D05C3 (303)  HI    0    0      3600  076:00:16:28:022104
   0  7F548123 (304)  HI    0    0      3601  076:00:16:28:022464
   0  600C89A3 (305)  HI    0    0      3599  076:00:16:28:022824
   0  7C800163 (306)  HI    0    0      3600  076:00:16:28:023184

--- KC-135 Example Data ---

AAR40-1-1
Subch   Data   Label Speed PErr FErr Gap Time  Computed IRIG Time
   1  E48801A6 (145)  HI    0    0         0  226:20:02:31:920700
   1  E45601A5 (245)  HI    0    0      3600  226:20:02:31:921060
   7  7FFA0151 (212)  LO    0    0       600  226:20:02:31:921120
   1  E44A01A7 (345)  HI    0    0      3200  226:20:02:31:921440
   5  A00002DE (173)  HI    0    0     12704  226:20:02:31:922710
   5  6071802B (324)  HI    0    0     11789  226:20:02:31:923889
   7  683B6133 (314)  LO    0    0      1108  226:20:02:31:924000
   4  33380D37 (354)  HI    0    0      1583  226:20:02:31:924158
   5  7FD140AB (325)  HI    0    0      1012  226:20:02:31:924259
   5  FFEE006B (326)  HI    0    0      3702  226:20:02:31:924629

*/

// ----------------------------------------------------------------------------
// ClCh10Format_ARINC429_AR100
// ----------------------------------------------------------------------------

ClCh10Format_ARINC429_AR100::ClCh10Format_ARINC429_AR100(int iBusNum, int iBusSpeed, int iEngineNum) :
        ClCh10Format_ARINC429()
    {

    // Total number of subchannels defined here
    uSubchannels = 1;
    this->iEngineNum = iEngineNum;

    // Make a default 429 message
    SuArinc429Msg   suDefault;
    memset(&suDefault, 0, sizeof(SuArinc429Msg));
    suDefault.suIPH.uBusNum   = iBusNum;
    suDefault.suIPH.uBusSpeed = iBusSpeed;
    suDefault.suData.uData    = 0;
    suDefault.suData.uLabel   = 0;
    suDefault.suData.uSDI     = 0;          // Channel A
    suDefault.suData.uSSM     = 3;          // Normal Operation

    // Resize the data vector to hold all the messages with default initial values
    aArinc429Msgs.resize(7, suDefault);

    // This makes it easier to debug
    psuEngineRpmN1Actual = (SuEngineRpmN1Actual *)(&aArinc429Msgs[0].suData);
    psuEngineRpmN1Demand = (SuEngineRpmN1Demand *)(&aArinc429Msgs[1].suData);
    psuEngineOilPres     = (SuEngineOilPres     *)(&aArinc429Msgs[2].suData);
    psuEngineRpmN2       = (SuEngineRpmN2       *)(&aArinc429Msgs[3].suData);
    psuEngineEGT         = (SuEngineEGT         *)(&aArinc429Msgs[4].suData);
    psuEngineOilTemp     = (SuEngineOilTemp     *)(&aArinc429Msgs[5].suData);
    psuFuelFlow          = (SuFuelFlow          *)(&aArinc429Msgs[6].suData);

    // Set the per item data fields
    aArinc429Msgs[0].suData.uLabel = ReverseLabel(0041);  // Engine RPM Actual
    aArinc429Msgs[1].suData.uLabel = ReverseLabel(0042);  // 42 octal reversed
    aArinc429Msgs[2].suData.uLabel = ReverseLabel(0043);  // 43 octal reversed
    aArinc429Msgs[3].suData.uLabel = ReverseLabel(0044);  // 44 octal reversed
    aArinc429Msgs[4].suData.uLabel = ReverseLabel(0045);  // 45 octal reversed
    aArinc429Msgs[5].suData.uLabel = ReverseLabel(0046);  // 46 octal reversed
    aArinc429Msgs[6].suData.uLabel = ReverseLabel(0047);  // 47 octal reversed

    // Default hash labels to use
#ifdef COMPILE_NASA
    std::snprintf(szSimStateHash_EngineRpmN1Actual, 50, "N1_%d",  iEngineNum);
    std::snprintf(szSimStateHash_EngineRpmN1Demand, 50, "N1C");
    std::snprintf(szSimStateHash_EngineOilPres,     50, "OIP_%d", iEngineNum);
    std::snprintf(szSimStateHash_EngineRpmN2,       50, "N2_%d",  iEngineNum);
    std::snprintf(szSimStateHash_EngineEGT,         50, "EGT_%d", iEngineNum);
    std::snprintf(szSimStateHash_EngineOilTemp,     50, "OIT_%d", iEngineNum);
    std::snprintf(szSimStateHash_FuelFlow,          50, "FF_%d",  iEngineNum);
#endif
    }

// ----------------------------------------------------------------------------

void ClCh10Format_ARINC429_AR100::MakeMsg(ClSimState * pclSimState)
    {
    // 0.1 msec message spacing

    // Engine Fan RPM (N1 Actual)
    aArinc429Msgs[0].suIPH.uGapTime                                 = 0;
    ((SuEngineRpmN1Actual *)(&aArinc429Msgs[0].suData))->uRpmN1Act  = (uint32_t)(pclSimState->fState[szSimStateHash_EngineRpmN1Actual] / (64.0 * (1.0/1024.0)));
    aArinc429Msgs[0].suData.uParity                                 = CalcParity(&aArinc429Msgs[0].suData);

    // Engine Fan RPM (N1 Demand)
    aArinc429Msgs[1].suIPH.uGapTime                                 = 1000;
    ((SuEngineRpmN1Demand *)(&aArinc429Msgs[1].suData))->uRpmN1Dem  = (uint32_t)(pclSimState->fState[szSimStateHash_EngineRpmN1Demand] / (64.0 * (1.0/1024.0)));
    aArinc429Msgs[1].suData.uParity                                 = CalcParity(&aArinc429Msgs[1].suData);

    // Engine Oil Pressure
    aArinc429Msgs[2].suIPH.uGapTime                                 = 1000;
    ((SuEngineOilPres *)(&aArinc429Msgs[2].suData))->uSenStatus     = 1;
    ((SuEngineOilPres *)(&aArinc429Msgs[2].suData))->iEngOilPres    = (uint32_t)(pclSimState->fState[szSimStateHash_EngineOilPres] / (64.0 * (1.0/65536.0)));
    aArinc429Msgs[2].suData.uParity                                 = CalcParity(&aArinc429Msgs[2].suData);

    // Engine Turbine RPM (N2)
    aArinc429Msgs[3].suIPH.uGapTime                                 = 1000;
    ((SuEngineRpmN2 *)(&aArinc429Msgs[3].suData))->uRpmN2Act        = (uint32_t)(pclSimState->fState[szSimStateHash_EngineRpmN2] / (64.0 * (1.0/1024.0)));
    aArinc429Msgs[3].suData.uParity                                 = CalcParity(&aArinc429Msgs[3].suData);

    // Exhaust Gas Temperature (EGT)
    aArinc429Msgs[4].suIPH.uGapTime                                 = 1000;
    ((SuEngineEGT *)(&aArinc429Msgs[4].suData))->uEGT               = (uint32_t)(pclSimState->fState[szSimStateHash_EngineEGT]);
    aArinc429Msgs[4].suData.uParity                                 = CalcParity(&aArinc429Msgs[4].suData);

    // Engine Oil Temperature
    aArinc429Msgs[5].suIPH.uGapTime                                 = 1000;
    ((SuEngineOilTemp *)(&aArinc429Msgs[5].suData))->iEngOilTemp    = (uint32_t)(pclSimState->fState[szSimStateHash_EngineOilTemp] / (128.0 * (1.0/256.0)));
    aArinc429Msgs[5].suData.uParity                                 = CalcParity(&aArinc429Msgs[5].suData);

    // Fuel Flow
    aArinc429Msgs[6].suIPH.uGapTime                                 = 1000;
    ((SuFuelFlow *)(&aArinc429Msgs[6].suData))->uFuelFlow           = (uint32_t)(pclSimState->fState[szSimStateHash_FuelFlow]);
    aArinc429Msgs[6].suData.uParity                                 = CalcParity(&aArinc429Msgs[6].suData);

    }

// ----------------------------------------------------------------------------

std::string ClCh10Format_ARINC429_AR100::TMATS(ClTmatsIndexes & TmatsIndex, std::string sCDLN)
    {
    std::stringstream   ssTMATS;
    int                 iMsgNum = 1;
    unsigned int        uLabel;
    std::stringstream   ssMessageName;
    std::stringstream   ssMeasurandName;

    // Define bus messages
    ssTMATS <<
        "B-" << TmatsIndex.iBIndex << "\\NMS\\N-1:7;\n";   // 7 message definitions

    // Message - Engine Fan RPM N1 Actual
    // ----------------------------------
    uLabel = ReverseLabel(aArinc429Msgs[0].suData.uLabel);
    ssMessageName.str("");
    ssMessageName << "ENG" << iEngineNum  << "_LBL" << std::oct << uLabel;
    ssTMATS <<
        "B-" << TmatsIndex.iBIndex << "\\MID-1-1:" << iMsgNum++ << ";\n"
        "B-" << TmatsIndex.iBIndex << "\\MNA-1-1:" << ssMessageName.str() << ";\n"
        "B-" << TmatsIndex.iBIndex << "\\LBL-1-1:" << std::bitset<8>(uLabel) << ";\n"
        "B-" << TmatsIndex.iBIndex << "\\SDI-1-1:ALL;\n"
        "B-" << TmatsIndex.iBIndex << "\\MN\\N-1-1:1;\n";

    // Measureand
    ssMeasurandName.str("");
    ssMeasurandName << "N1_RPM_ACTUAL_ENG" << iEngineNum;
    ssTMATS <<
        "B-" << TmatsIndex.iBIndex << "\\MN-1-1-1:" << ssMeasurandName.str() << ";\n" // Link to C
        "B-" << TmatsIndex.iBIndex << "\\NML\\N-1-1-1:1;\n"
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-1-1-1:1;\n"
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-1-1-1:00001111111111100000000000000000;\n";

    ssTMATS <<
        "C-" << TmatsIndex.iCIndex << "\\DCN:" << ssMeasurandName.str() << ";\n"  // Link from B
        "C-" << TmatsIndex.iCIndex << "\\MN1:Engine Fan RPM (N1 Actual) (Engine " << iEngineNum  << ");\n"
        "C-" << TmatsIndex.iCIndex << "\\MN3:% RPM;\n"
        "C-" << TmatsIndex.iCIndex << "\\MN4:OTH;\n"
        "C-" << TmatsIndex.iCIndex << "\\BFM:UNS;\n"
        "C-" << TmatsIndex.iCIndex << "\\DCT:COE;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO\\N:1;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO:0.0;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO-1:0.0625;\n";
    TmatsIndex.iCIndex++;

    // Message - Engine Fan RPM N1 Demand
    // ----------------------------------
    uLabel = ReverseLabel(aArinc429Msgs[1].suData.uLabel);
    ssMessageName.str("");
    ssMessageName << "ENG" << iEngineNum  << "_LBL" << std::oct << uLabel;
    ssTMATS <<
        "B-" << TmatsIndex.iBIndex << "\\MID-1-2:" << iMsgNum++ << ";\n"
        "B-" << TmatsIndex.iBIndex << "\\MNA-1-2:" << ssMessageName.str() << ";\n"
        "B-" << TmatsIndex.iBIndex << "\\LBL-1-2:" << std::bitset<8>(uLabel) << ";\n"
        "B-" << TmatsIndex.iBIndex << "\\SDI-1-2:ALL;\n"
        "B-" << TmatsIndex.iBIndex << "\\MN\\N-1-2:1;\n";

    // Measureand
    ssMeasurandName.str("");
    ssMeasurandName << "N1_RPM_DEMAND_ENG" << iEngineNum;
    ssTMATS <<
        "B-" << TmatsIndex.iBIndex << "\\MN-1-2-1:" << ssMeasurandName.str() << ";\n" // Link to C
        "B-" << TmatsIndex.iBIndex << "\\NML\\N-1-2-1:1;\n"
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-2-1-1:1;\n"
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-2-1-1:00001111111111100000000000000000;\n";

    ssTMATS <<
        "C-" << TmatsIndex.iCIndex << "\\DCN:" << ssMeasurandName.str() << ";\n"  // Link from B
        "C-" << TmatsIndex.iCIndex << "\\MN1:Engine Fan RPM (N1 Actual) (Engine " << iEngineNum  << ");\n"
        "C-" << TmatsIndex.iCIndex << "\\MN3:% RPM;\n"
        "C-" << TmatsIndex.iCIndex << "\\MN4:OTH;\n"
        "C-" << TmatsIndex.iCIndex << "\\BFM:UNS;\n"
        "C-" << TmatsIndex.iCIndex << "\\DCT:COE;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO\\N:1;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO:0.0;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO-1:0.0625;\n";
    TmatsIndex.iCIndex++;

    // Message - Engine Oil Pressure
    // -----------------------------
    uLabel = ReverseLabel(aArinc429Msgs[2].suData.uLabel);
    ssMessageName.str("");
    ssMessageName << "ENG" << iEngineNum  << "_LBL" << std::oct << uLabel;
    ssTMATS <<
        "B-" << TmatsIndex.iBIndex << "\\MID-1-3:" << iMsgNum++ << ";\n"
        "B-" << TmatsIndex.iBIndex << "\\MNA-1-3:" << ssMessageName.str() << ";\n"
        "B-" << TmatsIndex.iBIndex << "\\LBL-1-3:" << std::bitset<8>(uLabel) << ";\n"
        "B-" << TmatsIndex.iBIndex << "\\SDI-1-3:ALL;\n"
        "B-" << TmatsIndex.iBIndex << "\\MN\\N-1-3:2;\n";

    // Measureand
    ssMeasurandName.str("");
    ssMeasurandName << "OIL_PRESSURE_ENG" << iEngineNum;
    ssTMATS <<
        "B-" << TmatsIndex.iBIndex << "\\MN-1-3-1:" << ssMeasurandName.str() << ";\n" // Link to C
        "B-" << TmatsIndex.iBIndex << "\\NML\\N-1-3-1:1;\n"
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-3-1-1:1;\n"
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-3-1-1:00011111111111111111100000000000;\n";

    ssTMATS <<
        "C-" << TmatsIndex.iCIndex << "\\DCN:" << ssMeasurandName.str() << ";\n"  // Link from B
        "C-" << TmatsIndex.iCIndex << "\\MN1:Oil Pressure (Engine " << iEngineNum  << ");\n"
        "C-" << TmatsIndex.iCIndex << "\\MN3:PSI;\n"
        "C-" << TmatsIndex.iCIndex << "\\BFM:UNS;\n"
        "C-" << TmatsIndex.iCIndex << "\\DCT:COE;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO\\N:1;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO:0.0;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO-1:0.000976563;\n";
    TmatsIndex.iCIndex++;

    // Measureand
    ssMeasurandName.str("");
    ssMeasurandName << "OIL_PRESSURE_STATUS_ENG" << iEngineNum;
    ssTMATS <<
        "B-" << TmatsIndex.iBIndex << "\\MN-1-3-2:" << ssMeasurandName.str() << ";\n" // Link to C
        "B-" << TmatsIndex.iBIndex << "\\NML\\N-1-3-2:1;\n"
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-3-2-1:1;\n"
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-3-2-1:00000000000000000000010000000000;\n";

    ssTMATS <<
        "C-" << TmatsIndex.iCIndex << "\\DCN:" << ssMeasurandName.str() << ";\n"  // Link from B
        "C-" << TmatsIndex.iCIndex << "\\MN1:Oil Pressure Sensor Status (Engine " << iEngineNum  << ");\n"
        "C-" << TmatsIndex.iCIndex << "\\BFM:UNS;\n"
        "C-" << TmatsIndex.iCIndex << "\\DCT:COE;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO\\N:1;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO:0.0;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO-1:1.0;\n";
    TmatsIndex.iCIndex++;

    // Message - Engine Turbine RPM N2
    // -------------------------------
    uLabel = ReverseLabel(aArinc429Msgs[3].suData.uLabel);
    ssMessageName.str("");
    ssMessageName << "ENG" << iEngineNum  << "_LBL" << std::oct << uLabel;
    ssTMATS <<
        "B-" << TmatsIndex.iBIndex << "\\MID-1-4:" << iMsgNum++ << ";\n"
        "B-" << TmatsIndex.iBIndex << "\\MNA-1-4:" << ssMessageName.str() << ";\n"
        "B-" << TmatsIndex.iBIndex << "\\LBL-1-4:" << std::bitset<8>(uLabel) << ";\n"
        "B-" << TmatsIndex.iBIndex << "\\SDI-1-4:ALL;\n"
        "B-" << TmatsIndex.iBIndex << "\\MN\\N-1-4:1;\n";

    // Measureand
    ssMeasurandName.str("");
    ssMeasurandName << "N2_RPM_ENG" << iEngineNum;
    ssTMATS <<
        "B-" << TmatsIndex.iBIndex << "\\MN-1-4-1:" << ssMeasurandName.str() << ";\n" // Link to C
        "B-" << TmatsIndex.iBIndex << "\\NML\\N-1-4-1:1;\n"
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-4-1-1:1;\n"
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-4-1-1:00001111111111100000000000000000;\n";

    ssTMATS <<
        "C-" << TmatsIndex.iCIndex << "\\DCN:" << ssMeasurandName.str() << ";\n"  // Link from B
        "C-" << TmatsIndex.iCIndex << "\\MN1:Engine Fan RPM N2 Actual (Engine " << iEngineNum  << ");\n"
        "C-" << TmatsIndex.iCIndex << "\\MN3:% RPM;\n"
        "C-" << TmatsIndex.iCIndex << "\\MN4:OTH;\n"
        "C-" << TmatsIndex.iCIndex << "\\BFM:UNS;\n"
        "C-" << TmatsIndex.iCIndex << "\\DCT:COE;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO\\N:1;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO:0.0;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO-1:0.0625;\n";
    TmatsIndex.iCIndex++;

    // Message - Exhaust Gas Temperature (EGT)
    // ---------------------------------------
    uLabel = ReverseLabel(aArinc429Msgs[4].suData.uLabel);
    ssMessageName.str("");
    ssMessageName << "ENG" << iEngineNum  << "_LBL" << std::oct << uLabel;
    ssTMATS <<
        "B-" << TmatsIndex.iBIndex << "\\MID-1-5:" << iMsgNum++ << ";\n"
        "B-" << TmatsIndex.iBIndex << "\\MNA-1-5:" << ssMessageName.str() << ";\n"
        "B-" << TmatsIndex.iBIndex << "\\LBL-1-5:" << std::bitset<8>(uLabel) << ";\n"
        "B-" << TmatsIndex.iBIndex << "\\SDI-1-5:ALL;\n"
        "B-" << TmatsIndex.iBIndex << "\\MN\\N-1-5:1;\n";

    // Measureand
    ssMeasurandName.str("");
    ssMeasurandName << "EGT_ENG" << iEngineNum;
    ssTMATS <<
        "B-" << TmatsIndex.iBIndex << "\\MN-1-5-1:" << ssMeasurandName.str() << ";\n" // Link to C
        "B-" << TmatsIndex.iBIndex << "\\NML\\N-1-5-1:1;\n"
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-5-1-1:1;\n"
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-5-1-1:00001111111111100000000000000000;\n";

    ssTMATS <<
        "C-" << TmatsIndex.iCIndex << "\\DCN:" << ssMeasurandName.str() << ";\n"  // Link from B
        "C-" << TmatsIndex.iCIndex << "\\MN1:Exhaust Gas Temperature (Engine " << iEngineNum  << ");\n"
        "C-" << TmatsIndex.iCIndex << "\\MN3:Degrees C;\n"
        "C-" << TmatsIndex.iCIndex << "\\MN4:OTH;\n"
        "C-" << TmatsIndex.iCIndex << "\\BFM:UNS;\n"
        "C-" << TmatsIndex.iCIndex << "\\DCT:COE;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO\\N:1;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO:0.0;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO-1:0.0625;\n";
    TmatsIndex.iCIndex++;

    // Message - Engine Oil Temperature C
    // ----------------------------------
    uLabel = ReverseLabel(aArinc429Msgs[5].suData.uLabel);
    ssMessageName.str("");
    ssMessageName << "ENG" << iEngineNum  << "_LBL" << std::oct << uLabel;
    ssTMATS <<
        "B-" << TmatsIndex.iBIndex << "\\MID-1-6:" << iMsgNum++ << ";\n"
        "B-" << TmatsIndex.iBIndex << "\\MNA-1-6:" << ssMessageName.str() << ";\n"
        "B-" << TmatsIndex.iBIndex << "\\LBL-1-6:" << std::bitset<8>(uLabel) << ";\n"
        "B-" << TmatsIndex.iBIndex << "\\SDI-1-6:ALL;\n"
        "B-" << TmatsIndex.iBIndex << "\\MN\\N-1-6:1;\n";

    // Measureand
    ssMeasurandName.str("");
    ssMeasurandName << "OIL_TEMPERATURE_ENG" << iEngineNum;
    ssTMATS <<
        "B-" << TmatsIndex.iBIndex << "\\MN-1-6-1:" << ssMeasurandName.str() << ";\n" // Link to C
        "B-" << TmatsIndex.iBIndex << "\\NML\\N-1-6-1:1;\n"
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-6-1-1:1;\n"
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-6-1-1:00011111111110000000000000000000;\n";

    ssTMATS <<
        "C-" << TmatsIndex.iCIndex << "\\DCN:" << ssMeasurandName.str() << ";\n"  // Link from B
        "C-" << TmatsIndex.iCIndex << "\\MN1:Oil Temperature (Engine " << iEngineNum  << ");\n"
        "C-" << TmatsIndex.iCIndex << "\\MN3:Degrees C;\n"
        "C-" << TmatsIndex.iCIndex << "\\MN4:OTH;\n"
        "C-" << TmatsIndex.iCIndex << "\\BFM:TWO;\n"
        "C-" << TmatsIndex.iCIndex << "\\DCT:COE;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO\\N:1;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO:0.0;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO-1:0.5;\n";
    TmatsIndex.iCIndex++;

    // Message - Fuel Flow
    // -------------------
    uLabel = ReverseLabel(aArinc429Msgs[6].suData.uLabel);
    ssMessageName.str("");
    ssMessageName << "ENG" << iEngineNum  << "_LBL" << std::oct << uLabel;
    ssTMATS <<
        "B-" << TmatsIndex.iBIndex << "\\MID-1-7:" << iMsgNum++ << ";\n"
        "B-" << TmatsIndex.iBIndex << "\\MNA-1-7:" << ssMessageName.str() << ";\n"
        "B-" << TmatsIndex.iBIndex << "\\LBL-1-7:" << std::bitset<8>(uLabel) << ";\n"
        "B-" << TmatsIndex.iBIndex << "\\SDI-1-7:ALL;\n"
        "B-" << TmatsIndex.iBIndex << "\\MN\\N-1-7:1;\n";

    // Measureand
    ssMeasurandName.str("");
    ssMeasurandName << "FUEL_FLOW_ENG" << iEngineNum;
    ssTMATS <<
        "B-" << TmatsIndex.iBIndex << "\\MN-1-7-1:" << ssMeasurandName.str() << ";\n" // Link to C
        "B-" << TmatsIndex.iBIndex << "\\NML\\N-1-7-1:1;\n"
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-7-1-1:1;\n"
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-7-1-1:00001111111111111100000000000000;\n";

    ssTMATS <<
        "C-" << TmatsIndex.iCIndex << "\\DCN:" << ssMeasurandName.str() << ";\n"  // Link from B
        "C-" << TmatsIndex.iCIndex << "\\MN1:Fuel Flow (Engine " << iEngineNum  << ");\n"
        "C-" << TmatsIndex.iCIndex << "\\MN3:Pounds Per Hour;\n"
        "C-" << TmatsIndex.iCIndex << "\\MN4:OTH;\n"
        "C-" << TmatsIndex.iCIndex << "\\BFM:UNS;\n"
        "C-" << TmatsIndex.iCIndex << "\\DCT:COE;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO\\N:1;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO:0.0;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO-1:0.00012207;\n";
    TmatsIndex.iCIndex++;

    assert(iMsgNum == 8);

    return ssTMATS.str();
    }

