/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2009-08-10

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "serverTask.h"

CMN_IMPLEMENT_SERVICES(serverTask);


serverTask::serverTask(const std::string & taskName):
    mtsDevice(taskName),
    VoidCounter(0),
    ReadValue(0),
    QualifiedReadValue(0)
{
    mtsProvidedInterface * provided = AddProvidedInterface("Provided");
    if (provided) {
        provided->AddCommandVoid(&serverTask::Void, this, "Void");
        provided->AddCommandWrite(&serverTask::Write, this, "Write");
        provided->AddCommandRead(&serverTask::Read, this, "Read");
        provided->AddCommandQualifiedRead(&serverTask::QualifiedRead, this, "QualifiedRead");
        provided->AddEventVoid(EventVoid, "EventVoid");
        provided->AddEventWrite(EventWrite, "EventWrite", mtsInt());
    }

    ServerWindow.setupUi(&MainWindow);
    MainWindow.setWindowTitle("Server");
    MainWindow.setFixedSize(500, 205);
    MainWindow.move(100, 350);
    MainWindow.show();

    QObject::connect(this, SIGNAL(VoidQtSignal(int)),
                     ServerWindow.VoidValue, SLOT(setNum(int)));    
    QObject::connect(this, SIGNAL(WriteQtSignal(int)),
                     ServerWindow.WriteValue, SLOT(setNum(int)));
    QObject::connect(ServerWindow.ReadSlider, SIGNAL(valueChanged(int)),
                     this, SLOT(SetReadValue(int)));
    QObject::connect(ServerWindow.QualifiedReadSlider, SIGNAL(valueChanged(int)),
                     this, SLOT(SetQualifiedReadValue(int)));
}


void serverTask::Void(void)
{
    VoidCounter++;
    emit VoidQtSignal(VoidCounter);
}


void serverTask::Write(const mtsInt & data)
{
    emit WriteQtSignal(data.Data);
}


void serverTask::Read(mtsInt & placeHolder) const
{
    placeHolder.Data = ReadValue;
}


void serverTask::QualifiedRead(const mtsInt & data, mtsInt & placeHolder) const
{
    placeHolder.Data = data.Data + QualifiedReadValue;
}


void serverTask::SetReadValue(int newValue)
{
    ReadValue = newValue;
}


void serverTask::SetQualifiedReadValue(int newValue)
{
    QualifiedReadValue = newValue;
}
