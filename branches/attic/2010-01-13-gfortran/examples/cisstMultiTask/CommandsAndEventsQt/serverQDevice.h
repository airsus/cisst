/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet, Ali Uneri
  Created on: 2009-10-26

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _serverQDevice_h
#define _serverQDevice_h

#include <cisstMultiTask.h>

#include <QMainWindow>
#include <QObject>

#include "ui_serverQWidget.h"


class serverQDevice: public QObject, public mtsDevice
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

 public:
    serverQDevice(const std::string & taskName);
    ~serverQDevice(void) {};

    void Configure(const std::string & CMN_UNUSED(filename)) {};

 protected:
    void Void(void);
    void Write(const mtsInt & data);
    void Read(mtsInt & placeHolder) const;
    void QualifiedRead(const mtsInt & data, mtsInt & placeHolder) const;
    void SendButtonClickEvent() {
        EventVoid();
    }

    mtsFunctionVoid EventVoid;
    mtsFunctionWrite EventWrite;

    int VoidCounter;
    int ReadValue;
    int QualifiedReadValue;

    QMainWindow MainWindow;
    QWidget CentralWidget;
    Ui::ServerWidget ServerWidget;

 signals:
    void VoidQSignal(int voidCounter);
    void WriteQSignal(int newValue);

 public slots:
    void ReadQSlot(int newValue);
    void QualifiedReadQSlot(int newValue);
    void EventVoidQSlot(void);
    void EventWriteQSlot(int newValue);
};

CMN_DECLARE_SERVICES_INSTANTIATION(serverQDevice);

#endif  // _serverQDevice_h