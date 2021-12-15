/* Common.h - A place to put some common definitions shared by many modules

*/
#pragma once

#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream

#define F_TO_C(TempF)   ((TempF-32) / 1.8) 

// TMATS Generating Macros
// -----------------------

// 1553 Measurand Definitions

#define B_MEASURAND_1WORD(meas_name, mess_index, meas_index, word_pos, bit_mask)                                        \
    ssTMATS <<                                                                                                          \
        "B-" << TmatsIndex.iBIndex << "\\MN-1-"     << mess_index << "-" << meas_index << ":" << #meas_name << ";\n"    \
        "B-" << TmatsIndex.iBIndex << "\\MT-1-"     << mess_index << "-" << meas_index << ":D;\n"                       \
        "B-" << TmatsIndex.iBIndex << "\\NML\\N-1-" << mess_index << "-" << meas_index << ":1;\n"                       \
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-"    << mess_index << "-" << meas_index << "-1:" << word_pos << ";\n"    \
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-"    << mess_index << "-" << meas_index << "-1:" << #bit_mask << ";\n"   \
        "B-" << TmatsIndex.iBIndex << "\\MTO-1-"    << mess_index << "-" << meas_index << "-1:MSB;\n";                  \
        meas_index++;

// Note: word_pos_1 is the MSW
#define B_MEASURAND_2WORD(meas_name, mess_index, meas_index, word_pos_1, bit_mask_1, word_pos_2, bit_mask_2)            \
    ssTMATS <<                                                                                                          \
        "B-" << TmatsIndex.iBIndex << "\\MT-1-"     << mess_index << "-" << meas_index << ":D;\n"                       \
        "B-" << TmatsIndex.iBIndex << "\\NML\\N-1-" << mess_index << "-" << meas_index << ":2;\n"                       \
        "B-" << TmatsIndex.iBIndex << "\\MN-1-"     << mess_index << "-" << meas_index << ":" << #meas_name << ";\n"    \
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-"    << mess_index << "-" << meas_index << "-1:" << word_pos_1 << ";\n"  \
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-"    << mess_index << "-" << meas_index << "-1:" << #bit_mask_1 << ";\n" \
        "B-" << TmatsIndex.iBIndex << "\\MTO-1-"    << mess_index << "-" << meas_index << "-1:MSB;\n"                   \
        "B-" << TmatsIndex.iBIndex << "\\MFP-1-"    << mess_index << "-" << meas_index << "-1:1;\n"                     \
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-"    << mess_index << "-" << meas_index << "-2:" << word_pos_2 << ";\n"  \
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-"    << mess_index << "-" << meas_index << "-2:" << #bit_mask_2 << ";\n" \
        "B-" << TmatsIndex.iBIndex << "\\MTO-1-"    << mess_index << "-" << meas_index << "-2:MSB;\n"                   \
        "B-" << TmatsIndex.iBIndex << "\\MFP-1-"    << mess_index << "-" << meas_index << "-2:2;\n";                    \
        meas_index++;

// PCM Measurand Definitions

#define D_MEASURAND_1WORD(meas_name, meas_index, word_pos, bit_mask)                                \
    ssTMATS <<                                                                                      \
        "D-" << TmatsIndex.iDIndex << "\\MN-1-" << meas_index << ":" << #meas_name << ";\n"         \
        "D-" << TmatsIndex.iDIndex << "\\LT-1-" << meas_index << ":WDFR;\n"                         \
        "D-" << TmatsIndex.iDIndex << "\\MML\\N-1-" << meas_index << ":1;\n"                        \
        /* Measurand XXX Location 1; */                                                             \
        "D-" << TmatsIndex.iDIndex << "\\MNF\\N-1-" << meas_index << "-1:1;\n"                      \
        /* Measurand XXX Location 1 Word 1; */                                                      \
        "D-" << TmatsIndex.iDIndex << "\\WP-1-" << meas_index << "-1-1:" << word_pos << ";\n"       \
        "D-" << TmatsIndex.iDIndex << "\\WFM-1-" << meas_index << "-1-1:" << #bit_mask << ";\n"     \
        "D-" << TmatsIndex.iDIndex << "\\WFP-1-" << meas_index << "-1-1:1;\n";                      \
        meas_index++;

#define C_CONVERSION_NONE(meas_name, conv_name, units, bin_fmt)   \
    ssTMATS <<                                                              \
        "C-" << TmatsIndex.iCIndex << "\\DCN:" << #meas_name << ";\n"       \
        "C-" << TmatsIndex.iCIndex << "\\MN1:" << #conv_name << ";\n"       \
        "C-" << TmatsIndex.iCIndex << "\\MN3:" << #units << ";\n"           \
        "C-" << TmatsIndex.iCIndex << "\\BFM:" << #bin_fmt << ";\n"         \
        "C-" << TmatsIndex.iCIndex << "\\DCT:NON;\n";                       \
    TmatsIndex.iCIndex++;

#define C_CONVERSION_OFFSET_SCALE(meas_name, conv_name, units, bin_fmt, offset, scale)   \
    ssTMATS <<                                                              \
        "C-" << TmatsIndex.iCIndex << "\\DCN:" << #meas_name << ";\n"       \
        "C-" << TmatsIndex.iCIndex << "\\MN1:" << #conv_name << ";\n"       \
        "C-" << TmatsIndex.iCIndex << "\\MN3:" << #units << ";\n"           \
        "C-" << TmatsIndex.iCIndex << "\\BFM:" << #bin_fmt << ";\n"         \
        "C-" << TmatsIndex.iCIndex << "\\DCT:COE;\n"                        \
        "C-" << TmatsIndex.iCIndex << "\\CO\\N:1;\n"                        \
        "C-" << TmatsIndex.iCIndex << "\\CO:" << (offset) << ";\n"          \
        "C-" << TmatsIndex.iCIndex << "\\CO-1:" << uppercase << setprecision(12) << (scale) << ";\n";  \
    TmatsIndex.iCIndex++;


#define CENTER_COMMENT(stream, length, comment)                             \
    {                                                                       \
    int         iFillLen = (length-strlen(comment))/2;                      \
                                                stream << "COMMENT:";       \
    for (int iIdx = 0; iIdx < iFillLen; iIdx++) stream << " ";              \
                                                stream << comment;          \
    for (int iIdx = 0; iIdx < iFillLen; iIdx++) stream << " ";              \
    if (((length-strlen(comment)) %2) != 0)     stream << " ";              \
                                                stream << ";\n";            \
    }


// ============================================================================
// Class ClTmatsIndexes
// ============================================================================

// Index counters for various TMATS record fields

class ClTmatsIndexes
    {
public:
    int                 iRIndex  = 1;
    int                 iRSrcNum = 1;
    int                 iBIndex  = 1;
    int                 iCIndex  = 1;
    int                 iPIndex  = 1;
    int                 iDIndex  = 1;

    };


// ============================================================================
// Utilities
// ============================================================================

#if 0
std::string sCenter(std::string sInput, int iWidth) 
//void sCenter(std::string sInput, int iWidth) 
    { 
    std::stringstream ss, ssSpaces;
    int iPadSize;

    // Make padding
    iPadSize = iWidth - sInput.size();
    for (int i=0; i < iPadSize/2; ++i)
        ssSpaces << " ";

    // Make centered string
    ss << ssSpaces.str() << sInput << ssSpaces.str();

    // if pad odd #, add 1 more space
    if(iPadSize>0 && iPadSize%2!=0)                               
        ss << " ";

    return ss.str();
    }
#endif
