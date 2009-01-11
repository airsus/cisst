/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctQuaternionBase.cpp,v 1.3 2007/04/26 19:33:57 anton Exp $
  
  Author(s):	Anton Deguet
  Created on:	2005-10-08

  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctQuaternionBase.h>

template<>
vctQuaternionBase<vctFixedSizeVector<double, 4> >::vctQuaternionBase()
{}

template<>
vctQuaternionBase<vctFixedSizeVector<float, 4> >::vctQuaternionBase()
{}

template<>
vctQuaternionBase<vctDynamicVector<double> >::vctQuaternionBase()
{
    SetSize(SIZE);
}

template<>
vctQuaternionBase<vctDynamicVector<float> >::vctQuaternionBase()
{
    SetSize(SIZE);
}

