/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2008 

  (C) Copyright 2006-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlBufferImage_h
#define _svlBufferImage_h

#include <cisstOSAbstraction/osaThreadSignal.h>
#include <cisstOSAbstraction/osaCriticalSection.h>
#include <cisstStereoVision/svlStreamDefs.h>


class CISST_EXPORT svlBufferImage
{
public:
    svlBufferImage(unsigned int width, unsigned int height);
    ~svlBufferImage();

    unsigned int GetWidth();
    unsigned int GetHeight();
    unsigned int GetDataSize();

    unsigned char* GetPushBuffer();
    unsigned char* GetPushBuffer(unsigned int& size);
    void Push();
    bool Push(unsigned char* buffer, unsigned int size, bool topdown);
#if (CISST_SVL_HAS_OPENCV == ON)
    bool PushIplImage(IplImage* image);
#endif // CISST_SVL_HAS_OPENCV

    svlImageRGB* Pull(bool waitfornew, double timeout = 5.0);
#if (CISST_SVL_HAS_OPENCV == ON)
    IplImage* PullIplImage(bool waitfornew, double timeout = 5.0);
#endif // CISST_SVL_HAS_OPENCV

private:
    svlBufferImage() {}

#if (CISST_OS == CISST_WINDOWS)
    LONG Next, Latest, Locked;
#else
    unsigned int Next, Latest, Locked;
#endif
    int InitializationCounter;
    osaThreadSignal NewFrameEvent;
    svlImageRGB Buffer[3];
#if (CISST_SVL_HAS_OPENCV == ON)
    IplImage* OCVImage[3];
#endif // CISST_SVL_HAS_OPENCV

#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS)
    osaCriticalSection CS;
#endif

    bool TopDownCopy(unsigned char *targetbuffer, unsigned char *sourcebuffer);
};


#endif // _svlBufferImage_h
