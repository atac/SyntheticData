
#include <cstdio>

#if defined(__GNUC__)
#define SOCKET            int
#define INVALID_SOCKET    -1
#define SOCKET_ERROR      -1
#define SOCKADDR          struct sockaddr
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#endif

#include "XPlaneControl.h"

/*
 * Macros and definitions
 * ----------------------
 */

#define FT_TO_METERS(ft)      ((ft)*0.3048)
#define METERS_TO_FT(meters)  ((meters)/0.3048)

#pragma pack(push,1)
struct SuPREL
{
  char    szCmd[5];       // PREL
  int     type_start;
  int     p;              // The index of the airplace you want to control
  char    szApt_ID[8];
  int     apt_rwy_idx;
  int     apt_rwy_dir;
  double  dat_lat;        // latitude, in degrees
  double  dat_lon;        // longitude, in degrees
  double  dat_ele;        // elevation above sea level, in meters
  double  veh_psi_true;   // heading, degrees true
  double  speed;
};

struct SuACFN
{
  char    szCmd[5];       // ACFN
  int p; // aircraft index
  char path[150];
  char pad[2];
  int liveryIndex;
};

    struct SuVEHX
        {
        char    szCmd[5];       // VEHX
        int     p;              // The index of the airplace you want to control. use 0 for the main airplane that you fly to drive the visuals.
        double  dat_lat;        // latitude, in degrees
        double  dat_lon;        // longitude, in degrees
        double  dat_ele;        // elevation above sea level, in meters
        float   veh_psi_true;   // heading, degrees true
        float   veh_the;        // pitch, degrees
        float   veh_phi;        // roll, degrees
        };

    struct SuCMND
        {
        char    szCmd[5];
        char    szPath[100];
        };

    struct SuDREF
        {
        char    szCmd[5];
        float   fValue;
        char    szPath[500];
        };

    struct SuDATA
        {
        char    szCmd[5];
        int     iIndex;         // Data index, the index into the list of variables you can output from the Data Output screen in X-Plane.
	    float   fValue[8];      // The up to 8 numbers you see in the data output screen associated with that selection.. many outputs do not use all 8, though.
        };
#pragma pack(pop)


ClXPlaneControl::ClXPlaneControl()
    {
#if defined(_MSC_VER) 
    int                     iResult;
    WORD                    wVersionRequested;
    WSADATA                 wsaData;
#endif

    // Init network stuff
#if defined(_MSC_VER) 
    // Initialize WinSock, request version 2.2
    wVersionRequested = MAKEWORD(2, 2);
    iResult = WSAStartup(wVersionRequested, &wsaData);

    if (iResult != 0)
        {
        printf("Unable to initialize Winsock 2.2\n");
        }
#endif

    //Create an address and socket
    suXPlaneAddr.sin_family      = AF_INET; 
    suXPlaneAddr.sin_port        = htons(49000); 
    suXPlaneAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);; 
    
    hXPlaneSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    }


ClXPlaneControl::~ClXPlaneControl()
    {
    }

void ClXPlaneControl::SendACFN(int aircraftIndex) {
  SuACFN  suACFN;
  memset(suACFN.path, 0, 150);
  memset(suACFN.pad, 0, 2);

  memcpy(suACFN.szCmd, "ACFN", 5);
  suACFN.p = aircraftIndex;
  memcpy(suACFN.path, "Aircraft\\Laminar Research\\McDonnell Douglas F-4 Phantom II\\F-4.acf", 66);
  suACFN.liveryIndex = 0;

  int iStatus = sendto(hXPlaneSocket, (char*)&suACFN, sizeof(SuACFN), 0, (struct sockaddr*)&suXPlaneAddr, sizeof(suXPlaneAddr));
  if (iStatus == -1)
    printf("sendto() error - %d\n", WSAGetLastError());
}

void ClXPlaneControl::SendPREL(SuPosition suACPosition, int aircraftIndex) {
  SuPREL  suPREL;
  memset(suPREL.szCmd, 0, 5);
  memset(suPREL.szApt_ID, 0, 8);

  memcpy(suPREL.szCmd, "PREL", 5);
  suPREL.p = aircraftIndex;
  suPREL.apt_rwy_dir = 0;
  suPREL.apt_rwy_idx = 0;
  suPREL.type_start = 6;
  suPREL.dat_lat = suACPosition.fLat;
  suPREL.dat_lon = suACPosition.fLon;
  suPREL.dat_ele = FT_TO_METERS(suACPosition.fElevFt);
  suPREL.veh_psi_true = suACPosition.fHeading;
  suPREL.speed = 0;

  int iStatus = sendto(hXPlaneSocket, (char*)&suPREL, sizeof(SuPREL), 0, (struct sockaddr*)&suXPlaneAddr, sizeof(suXPlaneAddr));
  if (iStatus == -1)
    printf("sendto() error - %d\n", WSAGetLastError());
}

void ClXPlaneControl::SendVEHX(SuPosition suACPosition, int aircraftIndex)
{
  SuVEHX  suVEHX;

  memcpy(suVEHX.szCmd, "VEHX", 5);
  suVEHX.p = aircraftIndex;
  suVEHX.dat_lat = suACPosition.fLat;
  suVEHX.dat_lon = suACPosition.fLon;
  suVEHX.dat_ele = FT_TO_METERS(suACPosition.fElevFt);
  suVEHX.veh_psi_true = suACPosition.fHeading;
  suVEHX.veh_the = suACPosition.fPitch;
  suVEHX.veh_phi = suACPosition.fRoll;

  int iStatus = sendto(hXPlaneSocket, (char*)&suVEHX, sizeof(SuVEHX), 0, (struct sockaddr*)&suXPlaneAddr, sizeof(suXPlaneAddr));
  if (iStatus == -1)
    printf("sendto() error - %d\n", WSAGetLastError());
}


void ClXPlaneControl::SendCMND(const char * szCmd)
    {
    SuCMND      suCMND;

    memcpy(suCMND.szCmd, "CMND", 5);
    strcpy(suCMND.szPath, szCmd);
    int     iCmndSize = 5 + strlen(szCmd) + 1;

    int iStatus = sendto(hXPlaneSocket, (char *)&suCMND, iCmndSize, 0, (struct sockaddr*)&suXPlaneAddr, sizeof(suXPlaneAddr));
    if (iStatus == -1)
        printf("sendto() error - %d\n",WSAGetLastError());
    }


void ClXPlaneControl::SendDREF(const char * szDrefPath, float fDrefValue)
    {
    SuDREF      suDREF;

    memcpy(suDREF.szCmd, "DREF", 5);
    memset(&suDREF.szPath, ' ', sizeof(suDREF.szPath));
    suDREF.fValue = fDrefValue;
    strcpy(suDREF.szPath, szDrefPath);

    int iStatus = sendto(hXPlaneSocket, (char *)&suDREF, sizeof(SuDREF), 0, (struct sockaddr*)&suXPlaneAddr, sizeof(suXPlaneAddr));
    if (iStatus == -1)
        printf("sendto() error - %d\n",WSAGetLastError());
    }


void ClXPlaneControl::SendDATA(int iIndex, float afValue[])
    {
    SuDATA      suDATA;

    memcpy(suDATA.szCmd, "DATA", 5);
    suDATA.iIndex    = iIndex;
    for (int i=0; i<8; i++) 
        suDATA.fValue[i] = afValue[i];

    int iStatus = sendto(hXPlaneSocket, (char *)&suDATA, sizeof(SuDATA), 0, (struct sockaddr*)&suXPlaneAddr, sizeof(suXPlaneAddr));
    if (iStatus == -1)
        printf("sendto() error - %d\n",WSAGetLastError());
    }
