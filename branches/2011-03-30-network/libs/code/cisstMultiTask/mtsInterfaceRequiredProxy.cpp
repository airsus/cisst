/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Peter Kazanzides, Anton Deguet
  Created on: 2008-11-13

  (C) Copyright 2008-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "mtsInterfaceRequiredProxy.h"

CMN_IMPLEMENT_SERVICES(mtsInterfaceRequiredProxy);

mtsInterfaceRequiredProxy::mtsInterfaceRequiredProxy(const std::string & interfaceName,
                                                     mtsComponent * component,
                                                     mtsMailBox * mailBox,
                                                     mtsRequiredType required):
    mtsInterfaceRequired(interfaceName, component, mailBox, required),
    LastFunction(0)
{
}


mtsInterfaceRequiredProxy::~mtsInterfaceRequiredProxy()
{
}


void mtsInterfaceRequiredProxy::SetLastFunction(mtsFunctionBase * lastFunction)
{
    CMN_ASSERT(this->LastFunction == 0); // this should have been reset to 0 when previous command is executed
    this->LastFunction = lastFunction;
    CMN_ASSERT(this->LastFunction != 0);
}


void mtsInterfaceRequiredProxy::ResetLastFunction(void)
{
    this->LastFunction = 0;
 }


mtsFunctionBase * mtsInterfaceRequiredProxy::GetLastFunction(void) const
{
    return this->LastFunction;
}
