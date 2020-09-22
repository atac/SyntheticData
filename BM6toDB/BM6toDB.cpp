// BM6toDB.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "SimState.h"
#include "Source_BMNavTxt.h"

#define SQLITE
//#define HDF5

#ifdef SQLITE
#include "sqlite3.h"
#endif

#ifdef HDF5
#include "hdf5.h"
#endif

#define TABLE_NAME_BLUEMAX      "BlueMax"

void vUsage(void);


// ----------------------------------------------------------------------------

int main(int iArgc, char * aszArgv[])
    {
    char                szInFile[256];     // Input file name
    char                szOutFile[256];    // Output file name
    bool                bStatus;

    // Data Sources
    ClSimState          clSimState;
    ClSource_BMNavTxt * pSource_BMNav;

    std::vector<std::string>::iterator itDataLabel;

    // Database
#ifdef SQLITE
    int                 iStatus;
    sqlite3           * pDB;
    std::string         sSQL;
#endif
#ifdef HDF5
      hid_t       hdfFileId;
      herr_t      hdfStatus;
#endif

    // Make data sources. No data name prefix in database.
    pSource_BMNav = new ClSource_BMNavTxt(&clSimState, "");

    // Process command line
    // --------------------
    szInFile[0]  = '\0';
    szOutFile[0] = '\0';

    if (iArgc < 2)
        {
        vUsage();
        return 1;
        }


    for (int iArgIdx=1; iArgIdx<iArgc; iArgIdx++) 
        {
        if (szInFile[0] == '\0') strcpy(szInFile, aszArgv[iArgIdx]);
        else                     strcpy(szOutFile,aszArgv[iArgIdx]);
        } // end for all arguments

    // Open input and output files
    // ---------------------------

    // Open Bluemax XLS file
    bStatus = pSource_BMNav->Open(szInFile);
    if (bStatus == false)
        return 1;

#if 0
    // Get the list of available data items
    // BM/actime BM/aclatd BM/aclond BM/acaltf BM/acktas BM/acvifps BM/acvxi 
    // BM/acvyi BM/acvzi BM/acaxi BM/acayi BM/acazi BM/acphid BM/acthtad BM/acpsid 
    // BM/acmagd BM/acaoad BM/acthro BM/acnzb BM/acvzi BM/acazb BM/acgear
    auto    itDataLabel = std::begin(pSource_BMNav->DataLabel);
    while (itDataLabel != std::end(pSource_BMNav->DataLabel))
        {
        std::cout << *itDataLabel << " ";
        itDataLabel++;
        } // end while listing data labels
    std::cout << "\n";
#endif

    // Open the output database file and init it
#ifdef SQLITE
//    strcpy(&(szOutFile[strlen(szOutFile)]), ".db");
    iStatus = sqlite3_open(szOutFile, &pDB);

    // Create the data table and index for BlueMax data
    std::string     sColumnNames;

    sSQL = "CREATE TABLE " TABLE_NAME_BLUEMAX "(RowNum INT PRIMARY KEY ASC, ";
    itDataLabel = std::begin(pSource_BMNav->DataLabel);
    while (itDataLabel != std::end(pSource_BMNav->DataLabel))
        {
        sSQL += *itDataLabel + " REAL";
        itDataLabel++;
        if (itDataLabel != std::end(pSource_BMNav->DataLabel))
            sSQL += ", ";
        else
            sSQL += ");";
        } // end while listing data labels

    std::cout << sSQL << "\n";

    iStatus = sqlite3_exec(pDB, sSQL.c_str(), NULL, NULL, NULL);
    if (iStatus != SQLITE_OK)
        printf("SQLite CREATE TABLE error - %s\n", sqlite3_errmsg(pDB));

#if 0
    sSQL = "CREATE INDEX MissionTimeOffset ON BlueMax(IDX, BM_actime);";
    iStatus = sqlite3_exec(pDB, sSQL.c_str(), NULL, NULL, NULL);
    if (iStatus != SQLITE_OK)
        printf("SQLite CREATE INDEX error - %s\n", sqlite3_errmsg(pDB));
#endif
#endif
#ifdef HDF5
    strcpy(&(szOutFile[strlen(szOutFile)]), ".h5");
    hdfFileId = H5Fcreate(szOutFile, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

#endif

    // Read data and write to database file
    // ------------------------------------

#ifdef SQLITE
    // Set pragmas for performance
    sSQL = "PRAGMA synchronous = OFF;";
    iStatus = sqlite3_exec(pDB, sSQL.c_str(), NULL, NULL, NULL);
    if (iStatus != SQLITE_OK)
        printf("SQLite PRAGMA error - %s\n", sqlite3_errmsg(pDB));

    sSQL = "PRAGMA journal_mode = WAL;";
    iStatus = sqlite3_exec(pDB, sSQL.c_str(), NULL, NULL, NULL);
    if (iStatus != SQLITE_OK)
        printf("SQLite PRAGMA error - %s\n", sqlite3_errmsg(pDB));
#endif

    // Loop on reading BlueMax data
    long            lRowIdx = 0;
    while (pSource_BMNav->ReadNextLine() != false)
        {
        // Loop on individual data labels
        itDataLabel = std::begin(pSource_BMNav->DataLabel);
#ifdef SQLITE
        sSQL = "INSERT INTO " TABLE_NAME_BLUEMAX " VALUES(" + std::to_string(lRowIdx) + ", ";
#endif
        while (itDataLabel != std::end(pSource_BMNav->DataLabel))
            {
#ifdef SQLITE
            sSQL += std::to_string(clSimState.fState[*itDataLabel]);
            itDataLabel++;
            if (itDataLabel != std::end(pSource_BMNav->DataLabel))
                sSQL += ", ";
            else
                sSQL += ");";
#endif
            } // end while listing data labels
        std::cout << lRowIdx << std::endl;
#ifdef SQLITE
        iStatus = sqlite3_exec(pDB, sSQL.c_str(), NULL, NULL, NULL);
        if (iStatus != SQLITE_OK)
            printf("SQLite INSERT error - %s\n", sqlite3_errmsg(pDB));
#endif

        lRowIdx++;
        } // end while reading BlueMax

    // Close data files and clean up
    // -----------------------------

#ifdef SQLITE
    sqlite3_close(pDB);
#endif

    pSource_BMNav->Close();

    return 0;
    } // end main()


// ----------------------------------------------------------------------------

void vUsage(void)
    {
    printf("\nBM6toDB  " __DATE__ " " __TIME__ "\n");
    printf("Write BlueMax6 text output to database file         \n");
    printf("Usage: BM6toDB <input filename> <output filename>   \n");
    printf("   <input filename>   Input BlueMax6 text file name \n");
    printf("   <output filename>  Output database file name     \n");
    }
