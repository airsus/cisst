/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2010-03-19

  (C) Copyright 2010-2014 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstCommon/cmnPortability.h>

#include <cisstMultiTask/mtsInterfaceCommon.h>

// If not using MingW, compile as a single file.
#if (CISST_OS != CISST_WINDOWS) || (CISST_COMPILER != CISST_GCC)
#define MTS_CLASS_SERVICES_PART1
#define MTS_CLASS_SERVICES_PART2
#define MTS_CLASS_SERVICES_PART3
#endif

#ifdef MTS_CLASS_SERVICES_PART1

#include <cisstMultiTask/mtsCollectorEvent.h>
CMN_IMPLEMENT_SERVICES_DERIVED(mtsCollectorEvent, mtsCollectorBase)   // derives from mtsTaskFromSignal

#include <cisstMultiTask/mtsCollectorState.h>
CMN_IMPLEMENT_SERVICES_DERIVED(mtsCollectorState, mtsCollectorBase)   // derives from mtsTaskFromSignal

#include <cisstMultiTask/mtsCollectorFactory.h>
CMN_IMPLEMENT_SERVICES(mtsCollectorFactory)

#include <cisstMultiTask/mtsComponent.h>
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsComponentConstructorNameAndInt)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsComponentConstructorNameAndUInt)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsComponentConstructorNameAndLong)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsComponentConstructorNameAndULong)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsComponentConstructorNameAndDouble)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsComponentConstructorNameAndString)
CMN_IMPLEMENT_SERVICES(mtsComponent)

#include <cisstMultiTask/mtsComponentAddLatency.h>
CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG(mtsComponentAddLatency, mtsTaskPeriodic, mtsTaskPeriodicConstructorArg)

#include <cisstMultiTask/mtsInterfaceProvided.h>
CMN_IMPLEMENT_SERVICES(mtsInterfaceProvided)

#include <cisstMultiTask/mtsInterfaceRequired.h>
CMN_IMPLEMENT_SERVICES(mtsInterfaceRequired)

#include <cisstMultiTask/mtsStateIndex.h>
CMN_IMPLEMENT_SERVICES(mtsStateIndex)

#include <cisstMultiTask/mtsStateTable.h>
CMN_IMPLEMENT_SERVICES(mtsStateTable)
CMN_IMPLEMENT_SERVICES(mtsStateTableIndexRange)

#include <cisstMultiTask/mtsTask.h>
CMN_IMPLEMENT_SERVICES_DERIVED(mtsTask, mtsComponent)

#include <cisstMultiTask/mtsTaskContinuous.h>
CMN_IMPLEMENT_SERVICES_DERIVED(mtsTaskContinuous, mtsTask)
CMN_IMPLEMENT_SERVICES_DERIVED(mtsTaskMain, mtsTaskContinuous)

#include <cisstMultiTask/mtsTaskFromCallback.h>
CMN_IMPLEMENT_SERVICES_DERIVED(mtsTaskFromCallback, mtsTask)
CMN_IMPLEMENT_SERVICES(mtsTaskFromCallbackAdapter)

#include <cisstMultiTask/mtsTaskFromSignal.h>
CMN_IMPLEMENT_SERVICES_DERIVED(mtsTaskFromSignal, mtsTaskContinuous)

#include <cisstMultiTask/mtsTaskPeriodic.h>
CMN_IMPLEMENT_SERVICES_DERIVED(mtsTaskPeriodic, mtsTaskContinuous)

#endif  // MTS_CLASS_SERVICES_PART1

#ifdef MTS_CLASS_SERVICES_PART2

#include <cisstMultiTask/mtsFixedSizeVectorTypes.h>
#define MTS_FIXED_SIZE_VECTOR_IMPLEMENT(ElementType)   \
CMN_IMPLEMENT_SERVICES_TEMPLATED(mts##ElementType##1); \
CMN_IMPLEMENT_SERVICES_TEMPLATED(mts##ElementType##2); \
CMN_IMPLEMENT_SERVICES_TEMPLATED(mts##ElementType##3); \
CMN_IMPLEMENT_SERVICES_TEMPLATED(mts##ElementType##4); \
CMN_IMPLEMENT_SERVICES_TEMPLATED(mts##ElementType##5); \
CMN_IMPLEMENT_SERVICES_TEMPLATED(mts##ElementType##6)
MTS_FIXED_SIZE_VECTOR_IMPLEMENT(Double)
MTS_FIXED_SIZE_VECTOR_IMPLEMENT(Float)
MTS_FIXED_SIZE_VECTOR_IMPLEMENT(Int)
MTS_FIXED_SIZE_VECTOR_IMPLEMENT(UInt)
MTS_FIXED_SIZE_VECTOR_IMPLEMENT(Char)
MTS_FIXED_SIZE_VECTOR_IMPLEMENT(UChar)
MTS_FIXED_SIZE_VECTOR_IMPLEMENT(Bool)

#include <cisstMultiTask/mtsGenericObjectProxy.h>
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDouble)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsFloat)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsLong)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsLongLong)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsULong)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsInt)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsUInt)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsShort)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsUShort)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsChar)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsUChar)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsBool)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsStdString)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsStdStringVecProxy)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsStdDoubleVecProxy)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsStdCharVecProxy)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsStdVct3VecProxy)

#endif  // MTS_CLASS_SERVICES_PART2

#ifdef MTS_CLASS_SERVICES_PART3
#include <cisstMultiTask/mtsGenericObjectProxy.h>

CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVct1)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVct2)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVct3)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVct4)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVct5)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVct6)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVct7)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVct8)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVct9)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctFloat1)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctFloat2)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctFloat3)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctFloat4)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctFloat5)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctFloat6)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctFloat7)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctFloat8)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctFloat9)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctLong1)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctLong2)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctLong3)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctLong4)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctLong5)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctLong6)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctLong7)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctLong8)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctLong9)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctULong1)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctULong2)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctULong3)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctULong4)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctULong5)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctULong6)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctULong7)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctULong8)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctULong9)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctInt1)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctInt2)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctInt3)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctInt4)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctInt5)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctInt6)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctInt7)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctInt8)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctInt9)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUInt1)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUInt2)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUInt3)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUInt4)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUInt5)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUInt6)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUInt7)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUInt8)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUInt9)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctShort1)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctShort2)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctShort3)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctShort4)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctShort5)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctShort6)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctShort7)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctShort8)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctShort9)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUShort1)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUShort2)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUShort3)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUShort4)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUShort5)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUShort6)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUShort7)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUShort8)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUShort9)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctChar1)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctChar2)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctChar3)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctChar4)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctChar5)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctChar6)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctChar7)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctChar8)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctChar9)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUChar1)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUChar2)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUChar3)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUChar4)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUChar5)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUChar6)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUChar7)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUChar8)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUChar9)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctBool1)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctBool2)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctBool3)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctBool4)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctBool5)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctBool6)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctBool7)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctBool8)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctBool9)

CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVct2x2)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVct3x3)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVct4x4)

CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctDoubleVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctFloatVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctIntVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUIntVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctCharVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUCharVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctBoolVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctShortVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctUShortVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctLongVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctULongVec)

CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctDoubleMat)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctFloatMat)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctIntMat)

#endif // MTS_CLASS_SERVICES_PART3

#ifdef MTS_CLASS_SERVICES_PART2

CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctMatRot3)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctFrm3)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsVctFrm4x4)

#include <cisstMultiTask/mtsTransformationTypes.h>
CMN_IMPLEMENT_SERVICES(mtsDoubleQuat)
CMN_IMPLEMENT_SERVICES(mtsFloatQuat)
CMN_IMPLEMENT_SERVICES(mtsDoubleQuatRot3)
CMN_IMPLEMENT_SERVICES(mtsFloatQuatRot3)
CMN_IMPLEMENT_SERVICES(mtsDoubleAxAnRot3)
CMN_IMPLEMENT_SERVICES(mtsFloatAxAnRot3)
CMN_IMPLEMENT_SERVICES(mtsDoubleRodRot3)
CMN_IMPLEMENT_SERVICES(mtsFloatRodRot3)
CMN_IMPLEMENT_SERVICES(mtsDoubleMatRot3)
CMN_IMPLEMENT_SERVICES(mtsFloatMatRot3)
CMN_IMPLEMENT_SERVICES(mtsDoubleQuatFrm3)
CMN_IMPLEMENT_SERVICES(mtsFloatQuatFrm3)
CMN_IMPLEMENT_SERVICES(mtsDoubleMatFrm3)
CMN_IMPLEMENT_SERVICES(mtsFloatMatFrm3)
CMN_IMPLEMENT_SERVICES(mtsDoubleFrm4x4)
CMN_IMPLEMENT_SERVICES(mtsFloatFrm4x4)
CMN_IMPLEMENT_SERVICES(mtsDoubleAnRot2)
CMN_IMPLEMENT_SERVICES(mtsFloatAnRot2)
CMN_IMPLEMENT_SERVICES(mtsDoubleMatRot2)
CMN_IMPLEMENT_SERVICES(mtsFloatMatRot2)

#include <cisstMultiTask/mtsVector.h>
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDoubleVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsFloatVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsLongVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsULongVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsIntVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsUIntVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsShortVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsUShortVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsCharVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsUCharVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsBoolVec)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsStdStringVec)

#include <cisstMultiTask/mtsMatrix.h>
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDoubleMat)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsFloatMat)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsLongMat)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsULongMat)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsIntMat)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsUIntMat)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsShortMat)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsUShortMat)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsCharMat)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsUCharMat)
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsBoolMat)

#endif  // MTS_CLASS_SERVICES_PART2

#ifdef MTS_CLASS_SERVICES_PART1

#include <cisstMultiTask/mtsManagerGlobal.h>
CMN_IMPLEMENT_SERVICES(mtsManagerGlobal)
CMN_IMPLEMENT_SERVICES(mtsManagerGlobalInterface)

#include <cisstMultiTask/mtsManagerLocal.h>
CMN_IMPLEMENT_SERVICES(mtsManagerLocal)
CMN_IMPLEMENT_SERVICES(mtsManagerLocalInterface)

#include <cisstMultiTask/mtsInterfaceCommon.h>
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsInterfaceProvidedDescriptionProxy);
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsInterfaceRequiredDescriptionProxy);

#include <cisstMultiTask/mtsComponentState.h>
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsComponentStateProxy);

#include <cisstMultiTask/mtsExecutionResult.h>
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsExecutionResultProxy);

#include <cisstMultiTask/mtsParameterTypes.h>
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDescriptionComponentProxy);

CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDescriptionComponentClassProxy);
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDescriptionComponentClassVecProxy);

CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDescriptionInterfaceProxy);

CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDescriptionConnectionProxy);
CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDescriptionConnectionVecProxy);

CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsComponentStatusControlProxy);

CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsComponentStateChangeProxy);

CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsEndUserInterfaceArgProxy);

CMN_IMPLEMENT_SERVICES(mtsEventHandlerList);

CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsDescriptionLoadLibraryProxy);

CMN_IMPLEMENT_SERVICES(mtsLogMessage);

CMN_IMPLEMENT_SERVICES_TEMPLATED(mtsMessageProxy);

#include <cisstMultiTask/mtsComponentViewer.h>
CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG(mtsComponentViewer, mtsTaskFromSignal, std::string)

#include <cisstMultiTask/mtsSocketProxyClient.h>
CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG(mtsSocketProxyClient, mtsTaskContinuous, mtsSocketProxyClientConstructorArg)

#include <cisstMultiTask/mtsSocketProxyServer.h>
CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG(mtsSocketProxyServer, mtsTaskContinuous, mtsSocketProxyServerConstructorArg)

/* ICE dependent classes */
#include <cisstMultiTask/mtsConfig.h>
#if CISST_MTS_HAS_ICE

#include "mtsComponentInterfaceProxyClient.h"
CMN_IMPLEMENT_SERVICES(mtsComponentInterfaceProxyClient)

#include "mtsComponentInterfaceProxyServer.h"
CMN_IMPLEMENT_SERVICES(mtsComponentInterfaceProxyServer)

#include "mtsComponentProxy.h"
CMN_IMPLEMENT_SERVICES_DERIVED(mtsComponentProxy, mtsComponent)

#include "mtsManagerProxyClient.h"
CMN_IMPLEMENT_SERVICES(mtsManagerProxyClient)

#include "mtsManagerProxyServer.h"
CMN_IMPLEMENT_SERVICES(mtsManagerProxyServer)

#endif // CISST_MTS_HAS_ICE

#endif  // MTS_CLASS_SERVICES_PART1
