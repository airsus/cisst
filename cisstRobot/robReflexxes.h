/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet, Zerui Wang
  Created on: 2016-02-22

  (C) Copyright 2016 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#ifndef _robReflexxes_h
#define _robReflexxes_h

#include <ReflexxesAPI.h>
#include <RMLPositionFlags.h>
#include <RMLPositionInputParameters.h>
#include <RMLPositionOutputParameters.h>

#include <cisstVector/vctDynamicVectorTypes.h>

// Always include last
#include <cisstRobot/robExport.h>

/*!
  \brief Trajectory generator based on Reflexxes Type II Library

  Details can be found in www.reflexxes.com.

  \ingroup cisstRobot
*/
class CISST_EXPORT robReflexxes {

public:
    typedef enum {Reflexxes_NONE, Reflexxes_TIME, Reflexxes_PHASE} SynchronizationType;
    
    RMLPositionFlags Flags; //need to be hided

protected:
    bool mIsSet;                          /*!< To ensure we don't evaluate if the parameters are not set */
    size_t mDimension;                    /*!< Number of degrees of freedom */
    SynchronizationType mSynchronization; /*!< synchronization type we can set*/

    ReflexxesAPI *RML;                    /*!< */
    RMLPositionInputParameters *IP;       /*!< */
    RMLPositionOutputParameters *OP;      /*!< */
    int mResultValue;                     /*!< */
    double mTime;                         /*!< */

    vctDoubleVec mCurrentAcceleration;    /*!< */

public:
    robReflexxes(void);
    robReflexxes(const vctDoubleVec & MaxVelocity,
                 const vctDoubleVec & MaxAcceleration,
                 const double CycleTime,
                 const SynchronizationType synchronization = Reflexxes_NONE);
    robReflexxes(const vctDoubleVec & MaxVelocity,
                 const vctDoubleVec & MaxAcceleration,
                 const vctDoubleVec & MaxJerk,
                 const double CycleTime,
                 const SynchronizationType synchronization = Reflexxes_NONE);

    ~robReflexxes();

    void Init(void);

    const int & ResultValue(void) const;

    /*! Returns expected time left after last Evaluate call. */ 
    double Duration(void) const;
    
    double getTime(void) const;
    void setTime(const double);
    
    /*!  \brief Set target positions and velocities as well as desired
      maximum velocities and accelerations. All vectors must match the
      target position size.

      \param maximum velocities
      \param maximum acceleration
    */
    void Set(const vctDoubleVec & MaxVelocity,
             const vctDoubleVec & MaxAcceleration,
             const double CycleTime,
             const SynchronizationType synchronization = Reflexxes_NONE);

    void Set(const vctDoubleVec & MaxVelocity,
             const vctDoubleVec & MaxAcceleration,
             const vctDoubleVec & MaxJerk,
             const double CycleTime,
             const SynchronizationType synchronization = Reflexxes_NONE);

    /*
      \param current position
      \param current velocity
      \param target position
      \param target velocities
    */
    void Evaluate(vctDoubleVec & CurrentPosition,
                  vctDoubleVec & CurrentVelocity,
                  const vctDoubleVec & TargetPosition,
                  const vctDoubleVec & TargetVelocity);
};

#endif // _robReflexxes_h
