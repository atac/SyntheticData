#include <QDebug>
#include <QPixMap>
#include <QScreen>

#include "XPlaneControl_gui.h"
#include "XPlaneControl.h"


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


XPlaneControl_gui::XPlaneControl_gui(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.lineLat->setText("30.3817");
    ui.lineLon->setText("-86.455");
//    ui.lineLat->setText("47.4813");
//    ui.lineLon->setText("-122.2019");
    ui.lineAlt->setText("300.0");
    ui.lineHeading->setText("320.0");
    ui.linePitch->setText("0.0");
    ui.lineRoll->setText("0.0");

    ui.lineDataIndex->setText("1");
    ui.lineDataValue1->setText("-999");
    ui.lineDataValue2->setText("-999");
    ui.lineDataValue3->setText("-999");
    ui.lineDataValue4->setText("-999");

    QObject::connect(ui.pushSendVEHX, SIGNAL(clicked()), this, SLOT(pushSendVEHX_clicked()));
    QObject::connect(ui.pushSendCMND, SIGNAL(clicked()), this, SLOT(pushSendCMND_clicked()));
    QObject::connect(ui.pushSendDREF, SIGNAL(clicked()), this, SLOT(pushSendDREF_clicked()));
    QObject::connect(ui.pushSendDATA, SIGNAL(clicked()), this, SLOT(pushSendDATA_clicked()));

}

void XPlaneControl_gui::pushSendVEHX_clicked()
    {
    ClXPlaneControl::SuPosition     suPos;

    suPos.fLat     = ui.lineLat->text().toFloat();
    suPos.fLon     = ui.lineLon->text().toFloat();
    suPos.fElevFt  = ui.lineAlt->text().toFloat();
    suPos.fHeading = ui.lineHeading->text().toFloat();
    suPos.fRoll    = ui.lineRoll->text().toFloat();
    suPos.fPitch   = ui.linePitch->text().toFloat();

    clXPlaneControl.SendVEHX(suPos);
    }

void XPlaneControl_gui::pushSendCMND_clicked()
    {
    QByteArray  qCmnd     = ui.lineCmdPath->text().toLatin1();
    char      * szCommand = qCmnd.data();
    clXPlaneControl.SendCMND(szCommand);
    }

void XPlaneControl_gui::pushSendDREF_clicked()
    {
    QByteArray  qPath = ui.lineDrefPath->text().toLatin1();
    char     * szPath = qPath.data();
    float      fValue = ui.lineDrefValue->text().toFloat();
    clXPlaneControl.SendDREF(szPath, fValue);
    }

void XPlaneControl_gui::pushSendDATA_clicked()
    {
    float   afValue[8];

    afValue[0] = ui.lineDataValue1->text().toFloat();
    afValue[1] = ui.lineDataValue2->text().toFloat();
    afValue[2] = ui.lineDataValue3->text().toFloat();
    afValue[3] = ui.lineDataValue4->text().toFloat();
    for (int i=4; i<8; i++) afValue[i] = -999.0;
    clXPlaneControl.SendDATA(ui.lineDataIndex->text().toInt(), afValue);
    }
