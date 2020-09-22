#pragma once

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

/*
DREF
sim/cockpit/electrical/HUD_brightness
sim/cockpit/electrical/instrument_brightness
sim/cockpit/electrical/cockpit_lights
sim/cockpit/electrical/night_vision_on
sim/cockpit/gyros/the_vac_ind_deg                Standby AI pitch
sim/cockpit/gyros/phi_vac_ind_deg                Standby AI roll
sim/cockpit/gyros/the_ind_deg3                   Main AI pitch
sim/cockpit/gyros/the_ind_elec_pilot_deg         Main AI pitch
sim/cockpit/gyros/phi_ind_elec_pilot_deg         Main AI roll

sim/cockpit/gyros/psi_ind_degm3                  AI and HSI heading
sim/cockpit/gyros/psi_ind_elec_pilot_degm        AI and HSI heading
sim/cockpit/misc/compass_indicated               Magnetic Compass

sim/flightmodel/misc/h_ind                       Alitimeter
sim/flightmodel/position/indicated_airspeed      IAS
sim/flightmodel/position/vh_ind_fpm              VSI
sim/graphics/view/pilots_head_psi
sim/graphics/view/pilots_head_the

DATA
-> 4  Mach VVI G-load
-> 8  Joystick aileron/elevator/rudder
->11  Flight controls aileron/elevator/rudder
->14  Gear
  17  Roll / Pitch / Heading
->18  alpha beta 
  20  Lat / Lon / Alt
->25  Throttle Commanded 1 / 2
  35  Thrust
  37  RPM


CMND
sim/flight_controls/landing_gear_up
sim/flight_controls/landing_gear_down

*/


class ClXPlaneControl
    {

public:
    ClXPlaneControl();
    ~ClXPlaneControl();

protected:
    SOCKET                  hXPlaneSocket;
    struct sockaddr_in      suXPlaneAddr; 

public:
    struct SuPosition
        {
        double  fLat;       // latitude, in degrees
        double  fLon;       // longitude, in degrees
        double  fElevFt;    // elevation above sea level, in feet
        float   fHeading;   // heading, degrees true
        float   fPitch;     // pitch, degrees
        float   fRoll;      // roll, degrees
        };
    
    // Methods
public:
    void SendVEHX(SuPosition suACPosition);
    void SendCMND(const char * szCmd);
    void SendDREF(const char * szDrefPath, float fDrefValue);
    void SendDATA(int iIndex, float afValue[]);

    };

