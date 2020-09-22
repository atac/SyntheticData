#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_XPlaneControl.h"

#include "XPlaneControl.h"


class XPlaneControl_gui : public QMainWindow
{
    Q_OBJECT

public:
    XPlaneControl_gui(QWidget *parent = Q_NULLPTR);


    ClXPlaneControl     clXPlaneControl;

private:
    Ui::XPlaneControlClass          ui;

public slots: 
    void pushSendVEHX_clicked();
    void pushSendCMND_clicked();
    void pushSendDREF_clicked();
    void pushSendDATA_clicked();
};
