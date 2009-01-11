/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnPortabilityTest.cpp 20 2009-01-08 16:09:57Z adeguet1 $
  
  Author(s):  Anton Deguet
  Created on: 2003-08-09
  
  (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <iostream>
#include <string>

#include "cmnPortabilityTest.h"
#include <cmnPortabilityTestCMake.h>

using namespace std;

double cmnPortabilityTest::Zero = 0.0;

void cmnPortabilityTest::Print(void) {
    cout << endl;
    cout << "Cisst operating system code: " << CISST_OS << endl;
    cout << "Cisst operating system string: " << cmnOperatingSystemsStrings[CISST_OS] << endl;
    cout << "Cisst compiler code: " << CISST_COMPILER << endl;
    cout << "Cisst compiler string: " << cmnCompilersStrings[CISST_COMPILER] << endl;
    cout << "CMake system name: " << CISST_CMAKE_SYSTEM_NAME << endl;
    cout << "CMake c++ compiler: " << CISST_CMAKE_CXX_COMPILER << endl;
    cout << "CMake generator: " << CISST_CMAKE_GENERATOR << endl;
}


void cmnPortabilityTest::TestUndefined(void) {
    CPPUNIT_ASSERT(CISST_COMPILER != CISST_UNDEFINED);
    CPPUNIT_ASSERT(CISST_OS != CISST_UNDEFINED);
}


void cmnPortabilityTest::TestWithCMake(void) {
    // see also libs/code/cmnPortability.cpp
    std::string CMakeCompilerStrings[] = {"undefined",
                                          "c++",
                                          "cl",
                                          "cl",
                                          "CC",
                                          "CC",
                                          "icc",
	                                      "cl",
                                          "cl"};

    CPPUNIT_ASSERT(cmnOperatingSystemsStrings[CISST_OS] == CISST_CMAKE_SYSTEM_NAME);
    if (CISST_COMPILER == CISST_GCC) {
        CPPUNIT_ASSERT((std::string(CISST_CMAKE_CXX_COMPILER) == std::string("gcc"))
                       || (std::string(CISST_CMAKE_CXX_COMPILER) == std::string("c++"))
                       || (std::string(CISST_CMAKE_CXX_COMPILER) == std::string("g++"))
                       || (std::string(CISST_CMAKE_CXX_COMPILER) == std::string("g++-3.3"))
                       || (std::string(CISST_CMAKE_CXX_COMPILER) == std::string("g++-3.4"))
                       || (std::string(CISST_CMAKE_CXX_COMPILER) == std::string("g++-4.0"))
                       || (std::string(CISST_CMAKE_CXX_COMPILER) == std::string("g++-4.1"))
                       || (std::string(CISST_CMAKE_CXX_COMPILER) == std::string("g++-4.2")));
    } else {
        CPPUNIT_ASSERT(CISST_CMAKE_CXX_COMPILER == CMakeCompilerStrings[CISST_COMPILER]);
    }

    // to avoid the confusion with c++ and g++
    if (CISST_COMPILER == CISST_GCC) {
        CPPUNIT_ASSERT(std::string(CISST_CMAKE_COMPILER_IS_GNUCXX) == std::string("1"));
    } else {
        CPPUNIT_ASSERT(std::string(CISST_CMAKE_COMPILER_IS_GNUCXX) == std::string(""));
    }

    // compiler is cl for vc++6 and .net 7, check by generator
    if (CISST_OS == CISST_WINDOWS) {
        if (CISST_COMPILER == CISST_VCPP6) {
            CPPUNIT_ASSERT(std::string(CISST_CMAKE_GENERATOR) == std::string("Visual Studio 6"));
        }
        if (CISST_COMPILER == CISST_DOTNET7) {
            CPPUNIT_ASSERT(std::string(CISST_CMAKE_GENERATOR) == std::string("Visual Studio 7"));
        }
		if (CISST_COMPILER == CISST_DOTNET2003) {
            CPPUNIT_ASSERT(std::string(CISST_CMAKE_GENERATOR) == std::string("Visual Studio 7 .NET 2003"));
        }
        if (CISST_COMPILER == CISST_DOTNET2005) {
            CPPUNIT_ASSERT(std::string(CISST_CMAKE_GENERATOR) == std::string("Visual Studio 8 2005"));
        }
    }
}


void cmnPortabilityTest::TestCMN_ISNAN(void) {
    // a normal number
    double x = 20.0;
    CPPUNIT_ASSERT(!CMN_ISNAN(x));

    // inf/inf gives nan
    x /= Zero;
    double y = x;
    CPPUNIT_ASSERT(CMN_ISNAN(x / y));
}


void cmnPortabilityTest::TestCMN_ISFINITE(void) {
    // a normal number
    double x = 20.0;
    CPPUNIT_ASSERT(CMN_ISFINITE(x));

    // +inf
    x /= Zero;
    CPPUNIT_ASSERT(!CMN_ISFINITE(x));

    // -inf
    x = -20.0;
    x /= Zero;
    CPPUNIT_ASSERT(!CMN_ISFINITE(x));
}

